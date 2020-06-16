/* 
   2D square XY model
   Metropolis algorithm with h=0
   correlation function G(r)
*/


#include <iostream>
#include <cstdlib> 
#include <cmath>
#include <fstream>

using namespace std;
int *pl, *mn;
int L,Ncorr,Nmeas,Ntherm,initOpt;
int Lmax;
double **spin;
double **G, **Gavg;
double kT, tpi;
double M,H,Mavg=0.0,Havg=0.0,Msq=0.0,Savg=0.0;
ofstream of1,of2,of3;


void initialize(int,int);
void MetropolisUpdate(int);
void measure(void);
void measureG(void);


void measureG(void) {
  for (int rx=0;rx<=Lmax;rx++) {
  for (int ry=0;ry<=Lmax;ry++) {
    G[rx][ry] = 0.0;
    for (int i=0;i<L;i++) {
    for (int j=0;j<L;j++) {
      int ip = i + rx;
      int jp = j + ry;
      if (ip >= L) ip = ip-L;   // implement PBCs
      if (jp >= L) jp = jp-L;
      G[rx][ry] += cos(spin[i][j] - spin[ip][jp]);
    }}
    G[rx][ry] /= L*L;
  }}
}


void measure(void) {
  M = 0.0;
  H = 0.0;
  for (int i=0;i<L;i++) {
  for (int j=0;j<L;j++) {
    M += cos(spin[i][j]);
    H -= ( cos(spin[i][j] - spin[pl[i]][j]) + cos(spin[i][j]-spin[i][pl[j]]) );
  }}    
  M /= (L*L);
  H /= (L*L);
}

void MetropolisUpdate (int N) {
 // single hit Metropolis
 for (int nn=0;nn<N;nn++) {
  for (int i=0;i<L;i++) {
  for (int j=0;j<L;j++) {
   double sp = tpi * (double) rand()/RAND_MAX;
   double s = spin[i][j];
   double dE = -cos(spin[i][pl[j]]-sp) - cos(spin[pl[i]][j]-sp) -
                cos(spin[i][mn[j]]-sp) - cos(spin[mn[i]][j]-sp) + 
                cos(spin[i][pl[j]]-s) +  cos(spin[pl[i]][j]-s) + 
                cos(spin[i][mn[j]]-s) +  cos(spin[mn[i]][j]-s);
   double r = (double) rand()/RAND_MAX;
   if (r < min(1.0,exp(-dE/kT))) {
     spin[i][j] = sp;
   }
  }}
 }
}

void initialize(int L,int initOpt) {
  // set up spin and cluster matrices and periodic boundary conditions
  spin = new double* [L];
  G = new double* [L];
  Gavg = new double* [L];
  pl = new int [L];
  mn = new int [L];
  for (int i=0;i<L;i++) {
    spin[i] = new double [L];
    G[i] = new double [L];
    Gavg[i] = new double [L];
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
     spin[i][j] = 0.0;
     Gavg[i][j] = 0.0;
    }}
  } else {
  // hot start 
    for (int i=0;i<L;i++) {
    for (int j=0;j<L;j++) {
      spin[i][j] = tpi * (double) rand()/RAND_MAX ;
     Gavg[i][j] = 0.0;
    }}
  }
}


// --------------------------------------------------  

int main() {

  cout << " enter kT, L, Ntherm, Ncorr, Nmeas " << endl;
  cin >> kT >> L >> Ntherm >> Ncorr >> Nmeas;
  cout << " enter 1 for a hot start " << endl;
  cin >> initOpt;

  // file miscellany
  of1.open("XY.dat");
  of1 << "# xy.cpp" << endl;
  of1 << "# L,Ntherm,Ncorr,Nmeas: " << L << " " << Ntherm << " " << Ncorr << " " << Nmeas << endl;
  of2.open("XYf.dat");
  of2 << "# xy.cpp " << endl;
  of2 << "# L,Ntherm,Ncorr,Nmeas: " << L << " " << Ntherm << " " << Ncorr << " " << Nmeas << endl;
  of2 << "# x  y dx dy " << endl;
  of3.open("XYg.dat");
  of3 << "# xy.cpp " << endl;
  of3 << "# L,Ntherm,Ncorr,Nmeas: " << L << " " << Ntherm << " " << Ncorr << " " << Nmeas << endl;
  of3 << "# x y G(x,y) " << endl;
 
  tpi = 8.0*atan(1.0); 
  Lmax = min(L/2,16);

  initialize(L,initOpt);
  MetropolisUpdate(Ntherm);

  for (int mes=0;mes<Nmeas;mes++) {
   MetropolisUpdate(Ncorr);
   measure();                    
   Savg += M;
   Mavg += abs(M);
   Msq += M*M;
   Havg += H;
   of1 << mes << " " << M << " " <<  H << endl;
   measureG();
   for(int rx=0;rx<=Lmax;rx++) {
   for(int ry=0;ry<=Lmax;ry++) {
    Gavg[rx][ry] += G[rx][ry];
   }}
  } // end of measurements

  Mavg /= Nmeas;
  Savg /= Nmeas;
  Havg /= Nmeas;
  Msq /= Nmeas;
  double Msig = sqrt(abs(Msq-Mavg*Mavg))/sqrt(Nmeas-1);
  cout << " M = " << Savg << endl;
  cout << " |M| = " << Mavg << " +/- " << Msig <<  endl;
  cout << " E = " << Havg << endl;

  // to plot vector fields in gnuplot: plot 'fn' u 1:2:3:4 with vectors head filled lt 2
  // where 1:2:3:4 = x:y:dx:dy
  for (int i=0;i<L;i++){
  for (int j=0;j<L;j++){
   of2 << i << " " << j << " " << 0.7*cos(spin[i][j]) << " " << 0.7*sin(spin[i][j]) << endl;
  }}

  for(int rx=0;rx<=Lmax;rx++) {
  for(int ry=0;ry<=Lmax;ry++) {
   Gavg[rx][ry] /= Nmeas;
   of3 << rx << " " << ry << " " << Gavg[rx][ry] << endl;
  }}

  cout << " sweep, M, E in XY.dat " << endl;
  cout << " final vector field in XYf.dat" << endl;
  cout << " x y G(x,y) in XYg.dat " << endl;
  return 0;
}
