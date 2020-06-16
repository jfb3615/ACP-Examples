#include <iostream>
#include <cmath>
#include <random>
#include <thread>
#include <vector>

/*

SU(2) gauge theory 

class SU2 to implement SU2 gauge elements
class Markov for independent threaded Markov chains

Latt[x][y][z][t][mu = 0,1,2,3]

*/

using namespace std;
typedef mt19937_64 MersenneTwister;
unsigned int nthreads = thread::hardware_concurrency();


class SU2 {
/*
   a  = a0 + i sigma * vec a
   a.mag() = |a|
   a.project() = a/|a|
   a.inv() = a^{-1}          implement += and *=?
   a.pinv() = (a/|a|)^{-1}
   a.real() = 2 a0
*/

public:
  double a0;
  double a1;
  double a2;
  double a3;

 SU2() : a0(1.0),a1(0.0),a2(0.0),a3(0.0) {
 };

 SU2(double A0, double A1, double A2, double A3) : a0(A0),a1(A1),a2(A2),a3(A3) {
 };


 SU2 project() {
  double rho = sqrt(a0*a0+a1*a1+a2*a2+a3*a3);
  a0 /= rho;
  a1 /= rho;
  a2 /= rho;
  a3 /= rho;
  SU2 b(a0,a1,a2,a3);
  return b;
 }

 double real() {
   return 2.0*a0;
 }
 
 double mag() {
  return sqrt(a0*a0 + a1*a1 + a2*a2 + a3*a3);
 }

 SU2 inv() {
   SU2 b(a0,-a1,-a2,-a3);
   return b;
 }

 SU2 pinv() {
   double rho = sqrt(a0*a0+a1*a1+a2*a2+a3*a3);
   SU2 b(a0/rho,-a1/rho,-a2/rho,-a3/rho);
   return b;
 }

 SU2 operator+(const SU2 & other);

 SU2 operator*(const SU2 & other);

 friend std::ostream &operator<<(std::ostream &out, SU2 c) {
  out << "(" << c.a0 << ", " << c.a1 << ", " << c.a2 << ", " << c.a3 <<")";
  return out;
 }
};  // end of class SU2

SU2 SU2::operator+(const SU2 & other) {
  double A0 = a0 + other.a0;
  double A1 = a1 + other.a1;
  double A2 = a2 + other.a2;
  double A3 = a3 + other.a3;
  return SU2(A0,A1,A2,A3);
}

SU2 SU2::operator*(const SU2 & other) {
   double c0 = a0*other.a0 - a1*other.a1 - a2*other.a2 - a3*other.a3;
   double c1 = a0*other.a1 + other.a0*a1 - a2*other.a3 + a3*other.a2;
   double c2 = a0*other.a2 + other.a0*a2 + a1*other.a3 - a3*other.a1;
   double c3 = a0*other.a3 + other.a0*a3 - a1*other.a2 + a2*other.a1;
  return SU2(c0,c1,c2,c3);
}


class Markov {
  private:
  MersenneTwister mt;
  SU2 *Latt = NULL;
  int *pl   = NULL;  
  int *mn   = NULL;
  int m_L;                      
  unsigned long int m_seed;
  double m_beta;
  struct observables {
    double plaq;
    double poly;
  } o;

  int I(int i1, int i2, int i3, int i4, int mu) {  
    // indexing for 4d lattice 0:L-1 and mu 0:3
    return  (((i1*m_L+i2)*m_L + i3)*m_L + i4)*4 + mu;
  }

   void initialise(int m_start_op) {
    Latt = new SU2[m_L*m_L*m_L*m_L*4];  // cold start & create
    if (m_start_op == 1) {              // hot start
      uniform_real_distribution<double> uDist(-1.0,1.0);
      for (int i=0;i<m_L*m_L*m_L*m_L*4;i++) {
        again: double A0 = uDist(mt);
        double A1 = uDist(mt);
        double A2 = uDist(mt);
        double A3 = uDist(mt);
        double AA = sqrt(A0*A0 + A1*A1 + A2*A2 + A3*A3); 
        if (AA > 1.0) goto again;
        SU2 r(A0/AA,A1/AA,A2/AA,A3/AA);
        Latt[i] = r;
      }
    }

    // boundary conditions 
    pl = new int[m_L];  
    mn = new int[m_L];
    for (int i=0;i<m_L;i++) {
      pl[i] = i+1;
      mn[i] = i-1;
    }
    pl[m_L-1] = 0;
    mn[0] = m_L-1;
    return; 
   } // end of initialise

   double polyLoop(int x, int y, int z) {
     // compute the Polyakov at location x,y,z,t=0
     SU2 a = Latt[I(x,y,z,0,3)];
     for (int t=1;t<m_L;t++) {
       SU2 b = a*Latt[I(x,y,z,t,3)];
       a = b;
     }
     return a.real();
   }

   double measurePolyLoop() {
     // spatially averaged Polyakov loop
     double sum = 0.0;
     for (int x=0;x<m_L;x++) {
     for (int y=0;y<m_L;y++) {
     for (int z=0;z<m_L;z++) {
       sum += polyLoop(x,y,z);
     }}}
     return sum/(m_L*m_L*m_L);
   }


   double measurePlaq () {
     // sum all plaquettes
     double p = 0.0;
     for (int x=0;x<m_L;x++) {
     for (int y=0;y<m_L;y++) {
     for (int z=0;z<m_L;z++) {
     for (int t=0;t<m_L;t++) {
     for (int mu=0;mu<3;mu++) {
     for (int nu=mu+1;nu<4;nu++) {
       p += plaquette(x,y,z,t,mu,nu);
     }}}}}}
     return p/(m_L*m_L*m_L*m_L*6);
   }


   double plaquette(int x, int y, int z, int t, int mu, int nu) {
     // compute the (mu nu) plaquette at R
     int R[4] = {x,y,z,t};
     int Rmu[4] = {x,y,z,t};
     Rmu[mu] = pl[R[mu]];
     int Rnu[4] = {x,y,z,t};
     Rnu[nu] = pl[R[nu]];
     SU2 plaq = Latt[I(x,y,z,t,mu)] * 
                Latt[I(Rmu[0],Rmu[1],Rmu[2],Rmu[3],nu)] *
                Latt[I(Rnu[0],Rnu[1],Rnu[2],Rnu[3],mu)].inv() * 
                Latt[I(x,y,z,t,nu)].inv();   
     return plaq.real(); 
   }


  SU2 staple(int x, int y, int z, int t, int mu) { 
    // compute the sum of staples wrt to link L(R,mu)
    SU2 st(0.0,0.0,0.0,0.0);
    int R[4] = {x,y,z,t};
    int Rmu[4] = {x,y,z,t};
    Rmu[mu] = pl[R[mu]];
    for (int nu=0;nu<4;nu++) {
      if (nu != mu) {
        vector<int> Rnu = {x,y,z,t};
        Rnu[nu] = pl[R[nu]];
        vector<int> Rnum = {x,y,z,t};
        Rnum[nu] = mn[R[nu]];
        vector<int> Rnumu(Rnum.begin(),Rnum.end());
        Rnumu[mu] = pl[Rnum[mu]];
        SU2 rt = Latt[I(Rmu[0],Rmu[1],Rmu[2],Rmu[3],nu)] *
                 Latt[I(Rnu[0],Rnu[1],Rnu[2],Rnu[3],mu)].inv() *        
                 Latt[I(x,y,z,t,nu)].inv();
        SU2 lt = Latt[I(Rnumu[0],Rnumu[1],Rnumu[2],Rnumu[3],nu)].inv() *
                 Latt[I(Rnum[0],Rnum[1],Rnum[2],Rnum[3],mu)].inv() *
                 Latt[I(Rnum[0],Rnum[1],Rnum[2],Rnum[3],nu)]; 
        st = st + rt + lt;
      }
    }
    return st;
  }

   void ORupdate(int N) {
     for (int sw=0;sw<N;sw++) {
       for (int x=0;x<m_L;x++) {
       for (int y=0;y<m_L;y++) {
       for (int z=0;z<m_L;z++) {
       for (int t=0;t<m_L;t++) {
       for (int mu=0;mu<4;mu++) {
         SU2 L = Latt[I(x,y,z,t,mu)];
         SU2 S = staple(x,y,z,t,mu);
         Latt[I(x,y,z,t,mu)] = S.pinv()*L.inv()*S.pinv();
       }}}}}
     }
   } // end of ORupdate


   void update(int N) {
     // Creutz heat bath
     uniform_real_distribution<double> unDist(0.0,1.0);
     uniform_real_distribution<double> omDist(-1.0,1.0);
     for (int sw=0;sw<N;sw++) {
       for (int x=0;x<m_L;x++) {
       for (int y=0;y<m_L;y++) {
       for (int z=0;z<m_L;z++) {
       for (int t=0;t<m_L;t++) {
       for (int mu=0;mu<4;mu++) {
         SU2 S = staple(x,y,z,t,mu);
         double bk = m_beta*S.mag();
         double ee = exp(bk);
         uniform_real_distribution<double> uDist(1.0/ee,ee);
         again: double u = uDist(mt);
         double wt = log(u)/bk;
         wt = wt*wt;
         wt = sqrt(1.0 - wt);
         if (unDist(mt) > wt) goto again;
         double a0 = log(u)/bk;
         again2: double a1 = omDist(mt);
         double a2 = omDist(mt);
         double a3 = omDist(mt);
         double aa = sqrt(a1*a1+a2*a2+a3*a3);
         if (aa > 1.0) goto again2;
         aa = sqrt(1.0-a0*a0)/aa;
         a1 *= aa;
         a2 *= aa;
         a3 *= aa;
         Latt[I(x,y,z,t,mu)] = SU2(a0,a1,a2,a3)*S.pinv(); 
       }}}}}
     }
     return;
   } // end of update


  public:

  observables getObservables () {
    return o;
  }


  void run (int start_op, int Ntherm, int Nmeas, int Ncorr, int NOR) {
    int m_start_op = start_op;
    int m_Ntherm   = Ntherm;
    int m_Nmeas    = Nmeas; 
    int m_Ncorr    = Ncorr;
    int m_NOR      = NOR;
    // initialise
    initialise(m_start_op);
    // thermalize
    update(m_Ntherm);
    ORupdate(m_NOR);
    // measure
    double polyAvg = 0.0;
    double plaqAvg = 0.0;
    for (int i=0;i<m_Nmeas;i++) {
      update(m_Ncorr);
      ORupdate(m_NOR);
      plaqAvg += measurePlaq();
      polyAvg += measurePolyLoop();
    }
    o.poly = polyAvg/m_Nmeas;
    o.plaq = plaqAvg/m_Nmeas;
  } // end of run


  Markov (unsigned long int seed, int L, double beta) 
         :mt(seed),m_L(L),m_beta(beta) {
  }
  
  ~Markov(void) {
    delete [] pl;
    delete [] mn;
    delete [] Latt;
  }

}; // end of Markov



int main(void) {
  vector<Markov> vecM;
  vector<thread> th;
  unsigned long int seed;
  int L, Ntherm, Nmeas, Ncorr, NOR, start_op;
  double beta;
  cout << " enter lattice size, beta, seed " << endl;
  cin >> L >> beta >> seed;
  cout << " enter Ntherm, Nmeas, Ncorr, NOR " << endl;
  cin >> Ntherm >> Nmeas >> Ncorr >> NOR;
  cout << " enter 1 for hot start, 0 for cold " << endl;
  cin >> start_op;

  for (int th_id=0;th_id<nthreads;th_id++) {
    vecM.push_back(Markov(seed+th_id,L,beta));
  }

  for (int th_id=0;th_id<nthreads;th_id++) {
    th.push_back(thread(&Markov::run,&vecM[th_id],start_op,Ntherm,Nmeas,Ncorr,NOR));
  }

  for (auto &t : th) {   // wait for threads to exit
    t.join();
  }

  double plaqA = 0.0;
  double plaqSig = 0.0;
  double polyA = 0.0;
  double polySig = 0.0;
  for (int th_id=0;th_id<nthreads;th_id++) {
    double v = vecM[th_id].getObservables().plaq;
    plaqA += v; plaqSig += v*v;
    cout << th_id << " plaq: " << v << "   poly: ";
    v = vecM[th_id].getObservables().poly;
    cout << v << endl;
    polyA += v, polySig += v*v;
  }
  plaqA /= nthreads;
  plaqSig /= nthreads;
  polyA /= nthreads;
  polySig /= nthreads;
  polySig = sqrt(polySig-polyA*polyA)/sqrt(nthreads-1);
  plaqSig = sqrt(plaqSig-plaqA*plaqA)/sqrt(nthreads-1);
  cout << " <plaq> = " << plaqA << " +/- " << plaqSig << endl;
  cout << " <poly> = " << polyA << " +/- " << polySig << endl;
  vecM.clear();
  th.clear();
  return 0; 
}
