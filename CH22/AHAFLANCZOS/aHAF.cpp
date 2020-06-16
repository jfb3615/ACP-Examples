/* aHAFF.cpp 
   g++ -I/usr/local/include/eigen3 -o aHAF aHAF.cpp -O3
*/

#include <iostream>
#include <cstdlib>
#include <cmath>
#include <fstream>
#include <iomanip>
#include <Eigen/Eigenvalues> 

using namespace std;
using namespace Eigen;

int *pl = NULL;
int *rl = NULL;
unsigned long int rsn;
double *phi = NULL;
double *phiOld = NULL;
double *phiOlder = NULL;
double *Hphi = NULL;
double *A;
double *B;
MatrixXd Lanczos;
SelfAdjointEigenSolver<MatrixXd> es;
int L,Sztot;
int maxlen;
double g, oldnorm, norm;
ofstream ofs,ofs2;

void initialize(int);
void exit();
int Sz(unsigned long int);
int spin(unsigned long int, int);
int INV(unsigned long int);
unsigned long int flip(unsigned long int,int);
bool flippable(unsigned long int,int);

void initialize(int opt) {
  A = new double[200];
  B = new double[200];
  pl = new int [L];           // watch zero start !
  for (int i=0;i<L;i++) {
    pl[i] = i+1;
  }
  pl[L-1] = 0;
  // allocate memory
  unsigned long int size=1;   // L!/(L/2)!(L/2)!
  int L2 = L/2;               // L must be even
  int j=1;
  for (int i=L2+1;i<=L;i++) {
    size = size*i/j;
    j += 1;
  }
  cout << "     " <<  endl;
  double d;
  cout << "double: " << sizeof(d) << endl;
  cout << "int: " << sizeof(j) << endl;
  cout << "ulint: " << sizeof(size) << endl;
  cout << "basis size for Sztot=0: " << size << endl;
  if (!(phi  = new double[size+1] )) {   // rsn starts from 1 !!
   cout << "out of memory" << endl;
   exit(1);
  }
  if (!(phiOld  = new double[size+1] )) {
   cout << "out of memory" << endl;
   exit(1);
  }
  if (!(phiOlder  = new double[size+1] )) {
   cout << "out of memory" << endl;
   exit(1);
  }
  if (!(Hphi  = new double[size+1] )) {
   cout << "out of memory" << endl;
   exit(1);
  }
  if (!(rl = new int[size+1] )) {
   cout << "out of memory" << endl;
   exit(1);
  }
  // set up relabeled state codes
  unsigned long int maxnum = 1 << L;   // a dirty trick to get 2^L (cpp does not support pow(int,int)!!
  cout << " 2^L = " << maxnum << endl;
  rsn = 1;
  for (unsigned long int sn=0;sn<maxnum;sn++) {
    if (Sz(sn) == Sztot) {
      rl[rsn] = sn;
      rsn++;
    }
  }
  maxlen = rsn - 1;
  cout << " basis size for Sztot= " << Sztot << " : " << maxlen << endl;
  // initial state
  if (opt == 1){                      // initial state = neel1 + neel2
    int neel1 = maxnum/3;
    int neel2 = 2*neel1;
    for (rsn=1;rsn<=maxlen;rsn++) {
       phi[rsn] = 0.0;
    }  
    phi[INV(neel1)] = 1.0/sqrt(2.0);
    phi[INV(neel2)] = 1.0/sqrt(2.0);
    oldnorm = 1.0;
    if (L == 4) {
      cout << "neel1 " << neel1 << " inv " << INV(neel1) << endl;
      cout << "neel2 " << neel2 << " inv " << INV(neel2) << endl;
    }
  } else {                          // random start
    int seed;
    cout << " enter the random seed " << endl;
    cin >> seed;
    srand(seed);
    oldnorm = 0.0;
    for (rsn=1;rsn<=maxlen;rsn++) {
     phi[rsn] = 2.0*( (double) rand()/RAND_MAX - 0.5);
     oldnorm = oldnorm + phi[rsn]*phi[rsn];
    }
  }
  // file miscellany
  ofs2.open("aHAFab.dat");
  ofs.open("aHAFout.dat");
  ofs << "# aHAF.cpp" << endl;
  ofs << "# L = " << L <<  endl;
  ofs << "# g = " << g << endl;
  ofs << "# Sztot = " << Sztot << endl;
  if (opt == 1) {
   ofs << "# Neel start " << endl;
  } else {
   ofs << "# random start " << endl;
  }
}


void exit() {
  delete [] phi;
  delete [] phiOld;
  delete [] phiOlder;
  delete [] Hphi;
  delete [] rl;
  delete [] pl;
  delete [] A;
  delete [] B;
  ofs.close();
  ofs2.close();
}

int INV(unsigned long int sn) {
  // find the state index, rsn, for a state, sn=rl(rsn)
  unsigned long int mid;
  unsigned long int low = 1;
  unsigned long int high = maxlen;
  st:  mid = (low + high)/2;
  if (sn ==  rl[mid]) return mid;
  if (sn <  rl[mid]) {
    high = mid - 1;
  } else {
    low = mid + 1;
  }
  goto st;
}

int spin(unsigned long int sn, int bt) {
  // return the bt'th bit in sn converted to +-
  int S = -1;
  int k = sn & 1 << bt;     
  if (k != 0) S=1;
  return S;
}

int Sz(unsigned long int sn) {
 // compute Sztot for a state sn
 int S=0;
 for (int b=0;b<L;b++) {
  S += spin(sn,b);
 }
 return S;
} 

int T(unsigned long int rsn) {
  // compute H0 for a state rsn  (need to divide by 4)
  int S=0;
  unsigned long int sn = rl[rsn];
  for (int bt=0;bt<L;bt++) {
    S += spin(sn,bt)*spin(sn,pl[bt]);
  }
  return S;
}

unsigned long int flip(unsigned long int rsn, int i) {
 // flip bits i and i+1  using a nasty trick; return a relabeled statecode
 // will fail drastically if b(i) = b(i+1)
 unsigned long int sn = rl[rsn];
 sn ^= 1 << i;
 sn ^= 1 << pl[i];
 return INV(sn);
}

bool flippable(unsigned long int rsn, int bt) {
 // check if bit i != bit i+1
 bool f = false;
 int b0 = (rl[rsn] >> bt) & 1;
 int b1 = (rl[rsn] >> pl[bt]) & 1;
 if (b0 != b1) f = true;
 return f;
}

 

int main() {
  int opt;
  cout << " enter L, Sztot, g " << endl;
  cin >> L >> Sztot >> g;
  cout << " enter 1 for Neel start, 2 for random " << endl;
  cin >> opt;

  initialize(opt);

  // H{phi}
  for (rsn=1;rsn<=maxlen;rsn++) {
   Hphi[rsn] = T(rsn)*phi[rsn]/4.0;
  }
  for (rsn=1;rsn<=maxlen;rsn++) {
   for (int bt=0;bt<L;bt++) {
    if (flippable(rsn,bt)) {
     Hphi[flip(rsn,bt)] += g/2.0*phi[rsn];
    }
   }
  }

  double sum = 0.0;
  for (rsn=1;rsn<=maxlen;rsn++) {
    sum += phi[rsn]*Hphi[rsn];
    phiOld[rsn] = phi[rsn];
  }
  A[0] = sum/oldnorm;
  B[0] = 0.0;
  cout << " variational  e0 = " << A[0]/L << endl;
  ofs << "0 A: " << A[0] << endl;

  for (int loop=1;loop<=4*L;loop++) {    // Lanzcos loop  :::::::::::::::::::::::::::
   for (rsn=1;rsn<=maxlen;rsn++) {
     phi[rsn] = Hphi[rsn] - A[loop-1]*phiOld[rsn] - B[loop-1]*phiOlder[rsn];
   }
   // H{phi}
   for (rsn=1;rsn<=maxlen;rsn++) {
    Hphi[rsn] = T(rsn)*phi[rsn]/4.0;
   }
   for (rsn=1;rsn<=maxlen;rsn++) {
    for (int bt=0;bt<L;bt++) {
     if (flippable(rsn,bt)) {
      Hphi[flip(rsn,bt)] += g/2.0*phi[rsn];
     }
    }
   }
   double norm = 0.0;
   for (rsn=1;rsn<=maxlen;rsn++) {
     norm += phi[rsn]*phi[rsn];
   }
   double sum = 0.0;
   for (rsn=1;rsn<=maxlen;rsn++) {
     sum += phi[rsn]*Hphi[rsn];
   }
   A[loop] = sum/norm;
   B[loop] = norm/oldnorm;
   oldnorm = norm;
   for (rsn=1;rsn<=maxlen;rsn++) {
     phiOlder[rsn] = phiOld[rsn];
     phiOld[rsn] = phi[rsn] ;
   }
 
   ofs2 << loop << setprecision(16) << std::fixed << " A: " << A[loop] << " B: " << B[loop] << endl;
   // diagonalise
   Lanczos.resize(loop+1,loop+1);
   Lanczos.setZero();
   for (int i=0;i<=loop;i++) {
     Lanczos(i,i) = A[i];
   }
   for (int i=0;i<loop;i++) {
     Lanczos(i,i+1) = sqrt(B[i+1]);
     Lanczos(i+1,i) = Lanczos(i,i+1);
   }
   es.compute(Lanczos);
   double lam0 = es.eigenvalues()[0];
   double lam1 = es.eigenvalues()[1];
   ofs << loop << setprecision(12) << std::fixed << " " << lam0 << " " << lam1;
   if (loop > 3 ) {
    double lam2 = es.eigenvalues()[2];
    double lam3 = es.eigenvalues()[3];
    ofs << " " << lam2 << " " << lam3;
   }
   ofs << endl;
   cout << loop << setprecision(12) << std::fixed <<  "\t e0: " << lam0/L << "\t e1: " << lam1/L << "\t gap: " << lam1-lam0 << endl;
  }  // end of Lanczos iteration loop           :::::::::::::::::::::::::::::: 

  exit();
  cout << endl << " data in aHAFout.dat" << endl;
  cout << " A,B in aHAFab.dat" << endl << endl;
  return 0;
}
