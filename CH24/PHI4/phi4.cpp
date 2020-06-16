/*  phi4.cpp

  (2+1)D phi^4 theory

  g++ -std=c++11 -pthread -o fn fn.cpp

*/

#include <cstdlib>
#include <iostream>
#include <fstream>
#include <random>
#include <cmath>
#include <thread>
#include <vector>
#include <list>

using namespace std;

typedef mt19937_64 MersenneTwister;
unsigned int nthreads = thread::hardware_concurrency();
ofstream ofs;

list<double>phiSqvalues;
vector<thread> th;
vector<MersenneTwister> MT;
double phi[63][63][63][20]; // 5.5M entries => 44 MB
double del,kappa,g,alpha;
double * phiAvg = NULL;
double * phiSqAvg = NULL;
double * MAvg = NULL;
double * phiBar = NULL;
double * phiSqBar = NULL;
double * MBar = NULL;
double * naBar = NULL;
double * autoCorr = NULL;

int *pl = NULL;
int *mn = NULL;
int *PL = NULL;  
int L,start_op,upop,nXvalues;
int Ntherm,Ncorr,Nmeas,NOR;

void getAutoCorrs(double X) {
 // collect info in X for autocorrelations
 if (phiSqvalues.size() == nXvalues) {// we’ve filled the list
   autoCorr[0] += X*X;
   list<double>::const_iterator it = phiSqvalues.begin(); 
   for (int i=1; i<=nXvalues; i++) {
      autoCorr[i] +=  *it++ * X;
   }
   phiSqvalues.pop_back();
 }
 phiSqvalues.push_front(X);
}

double phiSq(int L, double msq) {
  // compute <phi^2(x)> for g=0
  double Msum = 0.0;
  for (int n1=0;n1<L;n1++) {
  for (int n2=0;n2<L;n2++) {
  for (int n3=0;n3<L;n3++) {
    double k1 = 2.0*M_PI*n1/L;
    double k2 = 2.0*M_PI*n2/L;
    double k3 = 2.0*M_PI*n3/L;
    Msum += 1.0/(msq + 4.0*(pow(sin(k1/2.0),2) + pow(sin(k2/2.0),2) + pow(sin(k3/2.0),2)));
  }}}
  double kap = 2.0/(msq + 6.0);   // general case is kappa = (2.0-4.0*g)/(m*m + 6.0)
  return Msum/(kap*L*L*L);
}

void globalInit() {
  MAvg = new double[nthreads];
  phiAvg = new double[nthreads];
  phiSqAvg = new double[nthreads];
  phiBar = new double[nthreads];
  phiSqBar = new double[nthreads];
  MBar = new double[nthreads];
  naBar = new double[nthreads];
  autoCorr = new double[nXvalues+1];
  for (int i=0;i<Nmeas;i++) {
   phiBar[i] = 0.0;
   phiSqBar[i] = 0.0;
   MBar[i] = 0.0;
   naBar[i] = 0.0;
  }
  pl = new int[L];            // pbcs
  mn = new int[L];
  for (int i=0;i<L;i++) {
    pl[i] = i+1;
    mn[i] = i-1;
  }
  pl[L-1] = 0;
  mn[0] = L-1;
  PL = new int[L];            // add L/2 to x,y,t
  for (int i=0;i<L;i++) {
    PL[i] = (i+L/2)%L;
  }
  ofs.open("phi4N.dat");
  ofs << "# phi4N.cpp " << endl;
  ofs << "# L= " << L << " " << " kappa= " << kappa << " g = " << g << " del = " << del << endl;
  ofs << "# alpha = " << alpha << endl;
  ofs << "# Ntherm = " << Ntherm << " Ncorr = " << Ncorr << " Nmeas = " << Nmeas << " N_OR = " << NOR <<endl;
}

void initialize(int th_id) {
  if (start_op == 0) {
    for (int x=0; x<L; x++) {
    for (int y=0; y<L; y++) {
    for (int t=0; t<L; t++) {
      phi[t][y][x][th_id] = 1.0;
    }}}
  } else if (start_op == 1) {
    for (int x=0; x<L; x++) {
    for (int y=0; y<L; y++) {
    for (int t=0; t<L; t++) {
      phi[t][y][x][th_id] = 0.0;
    }}}
  } else if (start_op == 2) {
    for (int x=0; x<L; x++) {
    for (int y=0; y<L; y++) {
    for (int t=0; t<L; t++) {
      uniform_real_distribution<double> udist(-del/2.0,del/2.0);
      phi[t][y][x][th_id] = udist(MT[th_id]);
    }}}
  } else if (start_op == 3) {
    for (int x=0; x<L; x++) {
    for (int y=0; y<L; y++) {
    for (int t=0; t<L; t++) {
      uniform_real_distribution<double> udist;
      phi[t][y][x][th_id] = 1.0;
      if (udist(MT[th_id]) < 0.5) phi[t][y][x][th_id] = -1.0;
    }}}
  }
  return;
}

void measure(int th_id) {
  double phiA = 0.0;
  double phiSqA = 0.0;
  double MA = 0.0;
  for (int x=0; x<L; x++) {
  for (int y=0; y<L; y++) {
  for (int t=0; t<L; t++) {
    double p = phi[t][y][x][th_id];
    phiA += p;
    phiSqA += p*p;
    MA += (p*phi[PL[t]][PL[y]][PL[x]][th_id]);
  }}}
  phiAvg[th_id] = phiA/(L*L*L);
  phiSqAvg[th_id] = phiSqA/(L*L*L);
  MAvg[th_id] = MA/(L*L*L);
  return;
}

double HBupdate(int nup, int th_id) {
  // Heat Bath update
  uniform_real_distribution<double> udist(0.0,1.0);
  double no=0.0;
  for (int sw=0;sw<nup;sw++) {
    for (int x=0;x<L;x++) {
    for (int y=0;y<L;y++) {
    for (int t=0;t<L;t++) {
      double p = phi[t][y][x][th_id];
      double nn = phi[pl[t]][y][x][th_id] + phi[t][pl[y]][x][th_id] + phi[t][y][pl[x]][th_id] +
               phi[mn[t]][y][x][th_id] + phi[t][mn[y]][x][th_id] + phi[t][y][mn[x]][th_id];
      nn *= kappa;
      normal_distribution<double> Ndist(nn/alpha,1.0/sqrt(alpha));
      double phip = Ndist(MT[th_id]);
      double A = 1.0 + (alpha-2.0)/(4.0*g);
      double dd = g*(phip*phip-A)*(phip*phip-A);
      if (udist(MT[th_id]) < exp(-dd)) {               // implement the rest of the heat bath
        no = no + 1.0;
        phi[t][y][x][th_id] = phip;
      }
    }}}
  }
  return no/(nup*L*L*L);
}


void ORupdate(int nup, int th_id) {
  // semi-microscopic over-relaxation
  uniform_real_distribution<double> udist(0.0,1.0);
  for (int sw=0;sw<nup;sw++) { 
    for (int x=0;x<L;x++) {
    for (int y=0;y<L;y++) {
    for (int t=0;t<L;t++) {
      double p = phi[t][y][x][th_id];
      double nn = phi[pl[t]][y][x][th_id] + phi[t][pl[y]][x][th_id] + phi[t][y][pl[x]][th_id] +
               phi[mn[t]][y][x][th_id] + phi[t][mn[y]][x][th_id] + phi[t][y][mn[x]][th_id];
      double phip = kappa*nn - p; 
      double dd = -2.0*phip*phip+phip*phip*phip*phip+2.0*p*p-p*p*p*p;
      if (udist(MT[th_id]) < exp(-g*dd)){
        phi[t][y][x][th_id] = phip;
      }
    }}}
  }
}

double update(int nup, int th_id) {
  // metropolis update
  uniform_real_distribution<double> Ddist(-del/2.0,del/2.0);
  uniform_real_distribution<double> udist(0.0,1.0);
  double no=0.0;
  for (int sw=0;sw<nup;sw++) {
    for (int x=0;x<L;x++) {
    for (int y=0;y<L;y++) {
    for (int t=0;t<L;t++) {
      double p = phi[t][y][x][th_id];
      double delta = Ddist(MT[th_id]);
      double nn = phi[pl[t]][y][x][th_id] + phi[t][pl[y]][x][th_id] + phi[t][y][pl[x]][th_id] +
               phi[mn[t]][y][x][th_id] + phi[t][mn[y]][x][th_id] + phi[t][y][mn[x]][th_id];
      double dS = delta*(-kappa*nn + 4.0*g*p*p*p + 2.0*(1.0-2.0*g)*p + 
                 (6.0*g*p*p+1.0-2.0*g)*delta + 4.0*g*p*delta*delta + g*delta*delta*delta);
      if (udist(MT[th_id]) < exp(-dS)) {                   // accept update (always if dS<0)
        no = no + 1.0;
        phi[t][y][x][th_id] = p+delta;
      }
    }}}
  }
  return no/(nup*L*L*L);
}

void exit() {
  delete [] pl;
  delete [] mn;
  delete [] PL;
  delete [] phiAvg;
  delete [] phiSqAvg;
  delete [] MAvg;
  delete [] phiBar;
  delete [] phiSqBar;
  delete [] MBar;
  delete [] naBar;
  delete [] autoCorr;
  MT.clear();
  th.clear();
  phiSqvalues.clear();
  ofs.close();
  return;
}

void run_th(int th_id) {
  initialize(th_id);
  double no = update(Ntherm-Ncorr,th_id);
  double na;
  for (int i=0;i<Nmeas;i++) {
    if (upop == 0) {
      na = update(Ncorr,th_id);
    } else {
      na = HBupdate(Ncorr,th_id);
    }
    ORupdate(NOR,th_id);  // run with Ntherm=1,Ncorr=1
    measure(th_id);
    naBar[th_id] += na;
    phiBar[th_id] += phiAvg[th_id];
    phiSqBar[th_id] += phiSqAvg[th_id];
    MBar[th_id] += MAvg[th_id];
    if (nthreads == 1) getAutoCorrs(phiSqAvg[0]);  // (don't bother threading this...)
  }
  phiBar[th_id] /= Nmeas;
  phiSqBar[th_id] /= Nmeas;
  MBar[th_id] /= Nmeas;
  naBar[th_id] /= Nmeas;
}

int main(void) {
  unsigned long int seed;        

  cout << " you have " << nthreads << " threads available" << endl;
  cout << " enter the number of threads to use " << endl;
  cin >> nthreads;
  cout << " enter seed " << endl;
  cin >> seed;
  redo:
  cout << " enter L, kappa, g " << endl;
  cin >> L >> kappa >> g;
  cout << " enter 0 for phi=1; 1 for phi=0; 2 phi=rand, 3 phi = +/- 1 " << endl;
  cin >> start_op;
  cout << " enter 0 for Metropolis, 1 for heat bath " << endl;
  cin >> upop;
  if ((upop == 1) && (g == 0.0)) {
     cout << " NO!" << endl;
     goto redo;
  }
  cout << " enter initial field width [2.0]" << endl;
  cin >> del;
  cout << " enter alpha [0.6] " << endl;
  cin >> alpha;
  cout << " enter Ntherm, Ncorr, Nmeas, NOR " << endl;
  cin >> Ntherm >> Ncorr >> Nmeas >> NOR;
  cout << " enter the max tau for autocorrelations " << endl;
  cin >> nXvalues;

  globalInit();

  double msq = (2.0-4.0*g)/kappa  - 6.0;
  cout << " msq = " << msq << " <phi^2>(g=0) = " << phiSq(L,msq) << endl;

  for (int th_id=0;th_id<nthreads;th_id++) {
    MT.push_back(MersenneTwister(seed+th_id));
  }
  for (int th_id=0;th_id<nthreads;th_id++) {
    th.push_back(thread(run_th,th_id));
  }
  for (auto &t : th) {   // wait for threads to exit
    t.join();
  }

  double phiB=0.0,phiBsq=0.0,phiSqB=0.0,phiSqBsq=0.0,MB=0.0,MBsq=0.0;
  double naB=0.0,naBsq=0.0;
  for (int th_id=0;th_id<nthreads;th_id++) {
   phiB += phiBar[th_id];
   phiBsq += phiBar[th_id]*phiBar[th_id];
   phiSqB += phiSqBar[th_id];
   phiSqBsq += phiSqBar[th_id]*phiSqBar[th_id];
   MB += MBar[th_id];
   MBsq += MBar[th_id]*MBar[th_id];
   naB += naBar[th_id];
   naBsq += naBar[th_id]*naBar[th_id];
  }
  phiB /= nthreads;
  phiBsq /= nthreads;
  phiSqB /= nthreads;
  phiSqBsq /= nthreads;
  MB /= nthreads;
  MBsq /= nthreads;
  naB /= nthreads;
  naBsq /= nthreads;
  phiBsq = sqrt(phiBsq - phiB*phiB)/sqrt(nthreads-1.0);
  double phiSig = sqrt(phiSqBsq - phiSqB*phiSqB)/sqrt(nthreads-1.0);
  MBsq = sqrt(MBsq - MB*MB)/sqrt(nthreads-1.0);
  naBsq = sqrt(naBsq - naB*naB)/sqrt(nthreads-1.0);
  cout << " <phi> = " << phiB << " +/- " << phiBsq << endl;
  cout << " <phi^2> = " << phiSqB << " +/- " << phiSig << endl;
  cout << " <M> = " << MB << " +/- " << MBsq << endl;
  cout << " acceptance = " << naB << " +/- " << naBsq << endl;
  if (nthreads == 1) {
    // autocorrelations
    phiSig = autoCorr[0]/(Nmeas-nXvalues)-phiSqB*phiSqB;
    double tau= 0.0;
    ofs << " 0.0 1.0 " << endl;
    for (int i=1; i<=nXvalues; i++) {
      autoCorr[i] = (autoCorr[i]/(Nmeas-nXvalues) - phiSqB*phiSqB)/phiSig;
      ofs << i << " " << autoCorr[i] << endl;
      tau += autoCorr[i];
    }
    cout << " integrated autocorrelation time = " << tau << endl;
  }
  cout << " autocorrelation in phi4N.dat (one thread only)" << endl;
  exit();
  return 0;
}
