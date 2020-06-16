/* 
   2D square lattice q-state Potts  model
   Wolff algorithm  (requires h=0!)
*/


#include <iostream>
#include <cstdlib> 
#include <cmath>
#include <fstream>
#include <complex>

using namespace std;
int **spin, **cluster, *Istack, *Jstack;
int *pl, *mn;
complex <double> *Cmap;
int L,Ncorr,Nmeas,Ntherm,initOpt,stackp,q;
double Wprob;
double h=0.0, kT;
double M,H,Mavg,Havg=0.0,Msq=0.0;
ofstream of2;

int delta(int,int);
void initialize(int,int);
void wolffUpdate(int);
void growC (int, int, int, int);
void measure(void);

void measure(void) {
  complex <double> S(0.0,0.0);
  H = 0.0;
  for (int i=0;i<L;i++) {
  for (int j=0;j<L;j++) {
    S += Cmap[spin[i][j]]; 
    H += delta(spin[i][j],spin[pl[i]][j]) + delta(spin[i][j],spin[i][pl[j]]);
  }}    
  M = abs(S)/(L*L);
  H /= (L*L);
}

int delta(int x, int y) {
 int d = 0;
 if (x == y) d = 1;
 return d;
}

void wolffUpdate (int N) {
  int i, x, y, seed;
  for (int nn=0;nn<N;nn++) {
    // zero cluster spin markers
    for (x = 0; x < L; x++) {
    for (y = 0; y < L; y++) {
      cluster[x][y] = 0;
    }}
    // initialize stack
    stackp = -1;
    // choose a random spin
    x = (int) (L* (double) rand()/RAND_MAX);
    y = (int) (L* (double) rand()/RAND_MAX);
    // grow cluster 
    seed = spin[x][y];  // save the seed spin
    // choose the new state != seed
    int qnew = seed;
    while (qnew == seed) {
      qnew = 1 + (int) ( q* (double) rand()/RAND_MAX );
    }
    spin[x][y] = qnew;         // flip the seed spin
    cluster[x][y] = 1;
    stackp++;                  // push the seed spin onto the stack
    Istack[stackp] = x;
    Jstack[stackp] = y;
    while (stackp > -1) {
      x = Istack[stackp];
      y = Jstack[stackp];
      stackp--;
      // try add each neighbor to cluster
      growC(pl[x], y, seed, qnew);
      growC(mn[x], y, seed, qnew);
      growC(x, pl[y], seed, qnew);
      growC(x, mn[y], seed, qnew);
    }
  }
}

void growC (int x, int y, int seed, int qnew) {
  if ((cluster[x][y] == 0) && (spin[x][y] == seed))  // not in the cluster and equal to seed
    if ((double) rand()/RAND_MAX < Wprob) {
     spin[x][y] = qnew;                                      // flip spin
     cluster[x][y] = 1;                                      // place in cluster
     stackp++;                                               // and in the stack
     Istack[stackp] = x;
     Jstack[stackp] = y;
    }
}


void initialize(int L,int initOpt) {
  // set up spin and cluster matrices and periodic boundary conditions
  spin = new int* [L];
  cluster = new int* [L];
  Istack = new int [L*L];
  Jstack = new int [L*L];
  pl = new int [L];
  mn = new int [L];
  Cmap = new complex<double> [q+1];
  for (int i=0;i<L;i++) {
    spin[i] = new int [L];
    cluster[i] = new int [L];
    pl[i] = i+1;
    mn[i] = i-1;
  }
  pl[L-1] = 0;
  mn[0] = L-1;

  // set up complex spin values
  complex <double> I(0.0,1.0);
  double pi = 4.0*atan(1.0);
  for (int s=0;s<q;s++) {
    Cmap[s+1] = exp(I*2.0*pi* (double) s/(double) q);
  }

  // initialize lattice 
  srand(time(0));
  if (initOpt != 1) {
    // cold start 
    for (int i=0;i<L;i++) {
    for (int j=0;j<L;j++) {
     spin[i][j] = 1;
    }}
  } else {
  // hot start 
    for (int i=0;i<L;i++) {
    for (int j=0;j<L;j++) {
      spin[i][j] = 1 + (int) ( q * (double) rand()/RAND_MAX );
      if (spin[i][j] > q) cout << " hit 1 in rand " << endl;
      if (spin[i][j] < 1) cout << " 0 hit in rand " << endl;
    }}
  }

  // file miscellany
  of2.open("potts.dat");
  of2 << "# potts.cpp" << endl;
  of2 << "# q,L,Ntherm,Ncorr,Nmeas: " <<q << " " << L << " " << Ntherm << " " << Ncorr << " " << Nmeas << endl;
  of2 << "# kT   Mavg   Havg " << endl;
}

// --------------------------------------------------  

int main() {
  int nT;
  double Tmin,Tmax;
  cout << " enter q " << endl;
  cin >> q;
  cout << " enter  L, Ntherm, Ncorr, Nmeas " << endl;
  cin >> L >> Ntherm >> Ncorr >> Nmeas;
  cout << " enter Tmin, Tmax, nT " << endl;
  cin >> Tmin >> Tmax >> nT;
  cout << " enter 1 for a hot start " << endl;
  cin >> initOpt;

  initialize(L,initOpt);
  kT = Tmin;
  Wprob = 1.0 - exp(-2.0/kT);
  wolffUpdate(Ntherm);

  for (int t=0;t<=nT;t++) {
    Mavg = 0.0;
    Havg = 0.0;
    kT = Tmin + (Tmax-Tmin)*t/nT;
    Wprob = 1.0 - exp(-2.0/kT);
    for (int mes=0;mes<Nmeas;mes++) {
      wolffUpdate(Ncorr);
      measure();                          // compute M and H
      Mavg += M;
      Msq += M*M;
      Havg += H;
    } // end of measurements
    Mavg /= Nmeas;
    Havg /= Nmeas;
    of2 << kT << " " << Mavg << " " << Havg << endl;
    double Msig = sqrt(abs(Msq/Nmeas-Mavg*Mavg))/sqrt(Nmeas-1);
  } // end of kT loop

  cout << " kT, Mavg, Havg in potts.dat" << endl;
  of2.close();
  return 0;
}
