/* 
   2D square lattice Ising  model
   Wolff algorithm  (requires h=0!)
   Heat Bath
   autocorrelation
*/


#include <iostream>
#include <cstdlib> 
#include <cmath>
#include <fstream>
#include <list>

using namespace std;
int **spin, **cluster, *Istack, *Jstack;
int *pl, *mn;
int L,Ncorr,Nmeas,Ntherm,initOpt,stackp,algOpt;
int nMvalues;
double *autoCorr;
double prob[9];
double Wprob;
double h=0.0, kT;
double M,H,Mavg=0.0,Havg=0.0,Savg=0.0;
list<double> Mvalues;  
ofstream outputFile,of2;

void initialize(int,int);
void HBUpdate(int);
void setHBProbs(double,double);
void wolffUpdate(int);
void growC (int, int, int);
void measure(void);
void getAutoCorrs(double);


void measure(void) {
  M = 0.0;
  H = 0.0;
  for (int i=0;i<L;i++) {
  for (int j=0;j<L;j++) {
    M += spin[i][j];
    H += spin[i][j]*(spin[pl[i]][j] + spin[i][pl[j]]);
  }}    
  M /= (L*L);
}

void getAutoCorrs(double X) {
  if (Mvalues.size() == nMvalues) {   // we've filled the list
    autoCorr[0] += X*X;
    list<double>::const_iterator  it = Mvalues.begin();
    for (int i=1; i<=nMvalues; i++) {
      autoCorr[i] +=  *it++ * X;
    }
    Mvalues.pop_back();              // pop the oldest M
  }
  Mvalues.push_front(X);             // push the new M
}


void wolffUpdate (int N) {
  int i, x, y, cluster_spin;
  for (int nn=0;nn<N;nn++) {
    // zero cluster spin markers
    for (x = 0; x < L; x++) {
    for (y = 0; y < L; y++) {
      cluster[x][y] = 0;
    }}
    // initialize stack
    stackp = -1;
    // choose a random spin
    x = (int) (L*rand()/RAND_MAX);
    y = (int) (L*rand()/RAND_MAX);
    // grow cluster 
    cluster_spin = spin[x][y];  // save the seed spin
    spin[x][y] = -spin[x][y];   // flip the seed spin
    cluster[x][y] = 1;
    stackp++;                  // push the seed spin onto the stack
    Istack[stackp] = x;
    Jstack[stackp] = y;
    while (stackp > -1) {
      x = Istack[stackp];
      y = Jstack[stackp];
      stackp--;
      // try add each neighbor to cluster
      growC(pl[x], y, cluster_spin);
      growC(mn[x], y, cluster_spin);
      growC(x, pl[y], cluster_spin);
      growC(x, mn[y], cluster_spin);
    }
  }
}

void growC (int x, int y, int cluster_spin) {
  if ((cluster[x][y] == 0) && (spin[x][y] == cluster_spin))  // not in the cluster and parallel to seed
    if ((double) rand()/RAND_MAX < Wprob) {
     spin[x][y] = -spin[x][y];                               // flip spin
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
  autoCorr = new double [nMvalues+1];
  for (int i=0;i<L;i++) {
    spin[i] = new int [L];
    cluster[i] = new int [L];
    pl[i] = i+1;
    mn[i] = i-1;
  }
  pl[L-1] = 0;
  mn[0] = L-1;
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
      spin[i][j] = (double) rand()/RAND_MAX > 0.5 ? +1 : -1;
    }}
  }
  // file miscellany
  outputFile.open("isingW.dat");
  outputFile << "# wolff.cpp" << endl;
  outputFile << "# L,Ntherm,Ncorr,Nmeas: " << L << " " << Ntherm << " " << Ncorr << " " << Nmeas << endl;
  outputFile << "# kT = " << kT << endl;
  outputFile << "#     kT    |M|    dM    Mex     susc(|M|)  " << endl;
  of2.open("isingCorr.dat");
  of2 << "# wolff.cpp" << endl;
  of2 << "# t  C(t) " << endl;
}

void setHBProbs(double h, double kT) {
  // prob[neighbours](sigma=1)  we have added 4 to the index
  prob[8] = exp(-(h-4.0)/kT)/(exp(-(h-4.0)/kT)+exp((h-4.0)/kT));
  prob[6] = exp(-(h-2.0)/kT)/(exp(-(h-2.0)/kT)+exp((h-2.0)/kT));
  prob[4] = exp(-h/kT)/(exp(-h/kT)+exp(h/kT));
  prob[2] = exp(-(h+2.0)/kT)/(exp(-(h+2.0)/kT)+exp((h+2.0)/kT));
  prob[0] = exp(-(h+4.0)/kT)/(exp(-(h+4.0)/kT)+exp((h+4.0)/kT));
}
  
void HBUpdate(int Ntherm) {
  for (int sw=0;sw<Ntherm;sw++) { 
    for (int i=0;i<L;i++) {
    for (int j=0;j<L;j++) {
      int neighbours = spin[pl[i]][j]+spin[mn[i]][j]+
                       spin[i][pl[j]]+spin[i][mn[j]] + 4;
      spin[i][j] = -1;
      double rn = (double) rand()/RAND_MAX;
      if (rn < prob[neighbours]) {
         spin[i][j] = 1;
      }
    }}    
  } // end of sweep loop
} 

// --------------------------------------------------  

int main() {
  cout << " enter kT, L, Ntherm, Ncorr, Nmeas " << endl;
  cin >> kT >> L >> Ntherm >> Ncorr >> Nmeas;
  cout << " enter the number of autocorrelations to compute "<<endl;
  cin >> nMvalues;
  cout << " enter 1 for a hot start " << endl;
  cin >> initOpt;
  cout << " enter 1 for heat bath, 0 for Wolff " << endl;
  cin >> algOpt;

  Wprob = 1.0 - exp(-2.0/kT);
  initialize(L,initOpt);
  if (algOpt == 1) {
    setHBProbs(h,kT);
    HBUpdate(Ntherm);
  } else {
    wolffUpdate(Ntherm);
  }


 for (int mes=0;mes<Nmeas;mes++) {
   if (algOpt == 1) {
     HBUpdate(Ncorr);
   } else {
     wolffUpdate(Ncorr);
   }
   measure();                          // compute M and H
   Savg += M;
   Mavg += abs(M);
   Havg += H;
   getAutoCorrs(abs(M));               // update the autocorrelations
  } // end of measurements

  Savg /= Nmeas;
  Mavg /= Nmeas;
  Havg /= Nmeas;
  double Msq = autoCorr[0]/(Nmeas - nMvalues);
  double Msig = sqrt(abs(Msq-Mavg*Mavg))/sqrt(Nmeas-1);
  double susc = L*L*(Msq - Mavg*Mavg);

  cout << " |M| = " << Mavg << " +/- " << Msig << " susc(|M|) = "  << susc << endl;
  cout << " exact M = " << pow((1.0 - pow(sinh(2.0/kT),-4)),1.0/8.0) << endl << endl;

  double tau = 0.0;
  Msig = Msq - Mavg*Mavg;
  of2 << "0 1.0 " << endl;
  for (int i=1; i<=nMvalues; i++) {
    autoCorr[i] = (autoCorr[i]/(Nmeas-nMvalues) - Mavg*Mavg)/Msig;
    of2 << i << " " << autoCorr[i] << endl;
    tau += autoCorr[i];
  }
  of2.close();
  cout << " integrated autocorrelation time = " << tau << endl;

  cout << " t, C(t) autocorrelations in isingCorr.dat " << endl;
  return 0;
}
