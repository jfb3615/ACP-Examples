/*

disk.cpp

Leap frog Verlet simulation of a disk galaxy a la Peebles and Ostriker


Peebles and Ostriker have a halo w/ a disk G=1 m=1, N=300 D=2, dt=.001 eps=0.05,

*/

#include <iostream>
#include <fstream>
#include <cmath>
#include <random>
#include <vector>

using namespace std;
typedef mt19937_64 MersenneTwister;
int N;      // number of point masses
double R;   // galaxy radius in units of 60 kly
double J;   // galaxy angular momentum in units of 10^67 Js
double M;   // galaxy mass in units of 10^12 M_sol
double eps; // force cutoff in units of r0 = 
double SMBH; // ratio of supermassive black hole mass to m=1


class ThreeVect {

public:

  double x;
  double y;
  double z;

  ThreeVect () {
    x=0.0; y=0.0; z=0.0;
  };
  
  ThreeVect (double x0, double y0, double z0) {
    x=x0; y=y0; z=z0;
  };

  void setV(double x0, double y0, double z0) {
    x=x0; y=y0; z=z0;
  }

  double magSq() {
    return  x*x + y*y + z*z;
  }
 
  double getR() {
    return sqrt(x*x+y*y+z*z);
  }

  double getTheta() {
    return acos(z/sqrt(x*x+y*y+z*z));
  }

  double getPhi() {
    return atan2(y,x);
  }

 double operator*(const ThreeVect & other);

 ThreeVect operator+(const ThreeVect & other);

 ThreeVect operator-(const ThreeVect & other);

 ThreeVect operator^(const ThreeVect & other);

 friend std::ostream &operator<<(std::ostream &out, ThreeVect v0) {
  //out << "(" << v0.x << ", " << v0.y << ", " << v0.z << ")";
  out << v0.x << " " << v0.y << " " << v0.z;
  return out;
 }

}; // end of class ThreeVect

ThreeVect ThreeVect::operator-(const ThreeVect & other) {
  return ThreeVect(x-other.x,y-other.y,z-other.z);
}

ThreeVect ThreeVect::operator+(const ThreeVect & other) {
  return ThreeVect(x+other.x,y+other.y,z+other.z);
}

ThreeVect ThreeVect::operator^(const ThreeVect & other) {
  // cross product
  double x0 = y*other.z - z*other.y;
  double y0 = z*other.x - x*other.z;
  double z0 = x*other.y - y*other.x;
  return ThreeVect(x0,y0,z0);
}

double ThreeVect::operator*(const ThreeVect & other) {
  // dot product
  return x*other.x + y*other.y + z*other.z;
}

ThreeVect operator*(const double & lhs, const ThreeVect & rhs) {
  ThreeVect v0 = ThreeVect(lhs*rhs.x,lhs*rhs.y,lhs*rhs.z);
  return v0;
}

ThreeVect operator*(const ThreeVect & lhs, const double & rhs) {
  ThreeVect v0 = ThreeVect(rhs*lhs.x,rhs*lhs.y,rhs*lhs.z);
  return v0;
}

ThreeVect operator/(const ThreeVect & lhs, const double & rhs) {
  ThreeVect v0 = ThreeVect(lhs.x/rhs,lhs.y/rhs,lhs.z/rhs);
  return v0;
}

// --------------------------------------------------------------------


ThreeVect *r = NULL;
ThreeVect *rnew = NULL;
ThreeVect *v = NULL;
ThreeVect *vnew = NULL;
ThreeVect *f = NULL;


int IJ(int i, int j) {
  // indexing for (0:N)^2 
  return i*N+j;
}


void getF() {
  // obtain F 
  int i = 0;  // our supermassive particle!
    f[IJ(i,i)].setV(0.0,0.0,0.0);
    for (int j=1;j<N;j++) {
      double rsq = (r[i]-r[j]).magSq();
      f[IJ(i,j)] = SMBH*(-1.0)*(r[i]-r[j])/pow(rsq+eps*eps,1.5);
      f[IJ(j,i)] = (-1.0)*f[IJ(i,j)];
    } 
  for (int i=1;i<N;i++) {
    f[IJ(i,i)].setV(0.0,0.0,0.0);
    for (int j=i+1;j<N;j++) {
      double rsq = (r[i]-r[j]).magSq();
      f[IJ(i,j)] = (-1.0)*(r[i]-r[j])/pow(rsq+eps*eps,1.5);
      f[IJ(j,i)] = (-1.0)*f[IJ(i,j)];
    } 
  }
  return;
}

ThreeVect getA(int i) {
  // obtain acceleration for particle i (m=1)
  ThreeVect a;
  for (int j=0;j<N;j++) {
    a = a + f[IJ(i,j)];
  } 
  if (i == 0) return a/SMBH;
  return a;
}

double U() { 
  // total potential energy |U|  G=1 m=1
  double u = 0.0;
  for (int i=0;i<N;i++) {
    for (int j=i+1;j<N;j++) {
      double rsq = (r[i]-r[j]).magSq();
      u += 1.0/sqrt(rsq + eps*eps);
    }
  }
  return u;
}

double kin() {
  // total kinetic energy 
  double t = 0.0;
  for (int i=0;i<N;i++) {
    t += v[i].magSq();
  } 
  return t/2.0;
}


int main(void) {
  uniform_real_distribution<double> uDist(0.0,1.0);
  
  int T,opt,Vopt;
  int numEjected=0;
  double v0,rho,Tstar,dt;
  ofstream ofs,ofs2,ofs3,ofs4,ofs5;
  ofs.open("disk.dat");
  ofs2.open("disk2.dat");
  ofs3.open("disk.xyz");
  ofs4.open("disk-hist.dat");
  ofs5.open("disk-tu.dat");

  cout << " input number of point masses " << endl;
  cin >> N;
  ofs3 << N << endl;
  cout << " units are G=1, m=1 " << endl;
  cout << " enter the mass of a supermassive BH wrt m=1 " << endl;
  cin >> SMBH;
  cout << " input galaxy radius  in units of R_Milky Way [60 kly]" << endl;
  cin >> R;
  cout << " enter mass distn 0=1/r^2 sphere; 1=uniform sphere; 2=disk 1/r " << endl;
  cin >> opt;
  cout << " enter vel profile -1=const; 0=1/r^2 sphere; 1=uniform sphere " << endl;
  cin >> Vopt;
  if (Vopt == -1) {
    cout << " v0 " << endl;
    cin >> v0;
  }
  cout << " enter force cutoff in units of .... " << endl;
  cin >> eps;
  cout << " enter dt, number of steps " << endl;
  cin >> dt >> T;
  ofs << "# disk.cpp " << endl;
  ofs << "# N = " << N << " R = " << R << endl;
  ofs << "# dt = " << dt << " no steps = " << T << " epsilon = " << eps << endl;
  ofs << "# mass opt = " << opt << " " << " v opt = " << Vopt << endl;
  cout << " enter seed " << endl;
  int seed;
  cin >> seed;
  cout << " enter number of bins " << endl;
  int Nbins;
  cin >> Nbins;
  MersenneTwister mt(seed);

  vector<double> Ghist (Nbins);
  vector<double> Vhist (Nbins);
  vector<double> Nhist (Nbins);
  double xhi = 10*R;
  double dx = xhi/Nbins;

  r = new ThreeVect[N];
  rnew = new ThreeVect[N];
  v = new ThreeVect[N];
  vnew = new ThreeVect[N];
  f = new ThreeVect[N*N];

  // initialize galaxy mass distn
  if (opt == 0) {         // sphere 1/r^2  gives n(r)= const, rho(r)=1/r^2
     rho = 3.0*N/(4.0*M_PI*R*R*R); // m=1
     for (int i=0;i<N;i++) {
       double rr = uDist(mt)*R;        
       double th = uDist(mt)*M_PI;
       double ph = uDist(mt)*2.0*M_PI;
       r[i].setV(rr*cos(ph)*sin(th),rr*sin(ph)*sin(th),rr*cos(th));
     }
  } else if (opt == 1) {  // sphere const
      rho = 3.0*N/(4.0*M_PI*R*R*R); // m=1
      for (int i=0;i<N;i++) {
        gen: double x = 2.0*(uDist(mt)-0.5);
        double y = 2.0*(uDist(mt)-0.5);
        double z = 2.0*(uDist(mt)-0.5);
        if (x*x+y*y+z*z > 1.0) goto gen;
        r[i].setV(R*x,R*y,R*z);
      }
  } else if (opt == 2) {  // disk 1/r
     rho = N/(M_PI*R*R*R/10.0); // m=1
     for (int i=0;i<N;i++) {
       double rr = uDist(mt)*R;
       double th = uDist(mt)*2.0*M_PI;
       double ww = (uDist(mt)-0.5)*R/5.0;
       r[i].setV(rr*cos(th),rr*sin(th),ww);
     } 
  }
  Tstar = 1.0/sqrt(rho);        // G=1
  cout << " rho = " << rho << " T* = " << Tstar << endl;
  if (dt > Tstar/10.0) cout << " you should probably reduce dt " << endl;
  // initialise velocities 
  ThreeVect vtot;
  if (Vopt == -1 ) {       // constant v
    for (int i=0;i<N;i++) {
      double ph = r[i].getPhi();
      v[i].setV(-v0*sin(ph),v0*cos(ph),0.0); // CCW in xy plane  
      vtot = vtot + v[i];
    }
  } else if (Vopt == 0) {  // 1/r^2 sphere 
    double v00 = sqrt(N/R);  // really this rho0 r0^2 normalized to N w/ G=1
    for (int i=0;i<N;i++) {
      double ph = r[i].getPhi();
      v[i].setV(-v00*sin(ph),v00*cos(ph),0.0); // CCW in xy plane  
      vtot = vtot + v[i];
    }
  } else if (Vopt == 1) {  // constant sphere => v^2(r) =  4 pi G rho0/3 r^2
    double  v00 = sqrt(4.0*M_PI*rho/3.0);
    for (int i=0;i<N;i++) {
      v0 = v00*r[i].getR();
      double ph = r[i].getPhi();
      v[i].setV(-v0*sin(ph),v0*cos(ph),0.0); // CCW in xy plane  
      vtot = vtot + v[i];
    }
  }
  if (SMBH != 1.0) {  // place SMBH at the orgin and at rest
    r[0].setV(0.0,0.0,0.0);
    v[0].setV(0.0,0.0,0.0);
  }
  // set CM Vtot to zero
  vtot = vtot/N;
  cout << " vtot = " << vtot << endl;
  for (int i=0;i<N;i++) {
    v[i] = v[i] - vtot;
    ofs << "-1 " << r[i] << " " << v[i] << endl;
  }


  // RUN
  for (int t=0;t<T;t++) {
    getF();
    // leap frog Verlet
    for (int i=0;i<N;i++) {
      ThreeVect a = getA(i);
      vnew[i] = v[i] + dt*a;
      rnew[i] = r[i] + dt*vnew[i];
    } 
    // and update
    for (int i=0;i<N;i++) {
      if ((rnew[i].magSq() > 6.0*R*R) && (r[i].magSq() < 6.0*R*R)) {
        numEjected++;
        cout << " particle " << i << " ejected " << " at time: " << t*dt << " (total: " << numEjected <<")"<< endl;
      }
      if ((rnew[i].magSq() < 6.0*R*R) && (r[i].magSq() > 6.0*R*R)) {
        numEjected--;
        cout << " particle " << i << " re-entered " << " at time: " << t*dt << endl;
      }
      r[i] = rnew[i];
      v[i] = vnew[i];
      ofs3 << "argon " << r[i] << endl;
      ofs << t << " "  << r[i] << endl;
    }
    ofs2 << t << " " << r[0] << " " << v[0] << endl;
    // collect pair correlation data
    for (int i=0;i<N;i++) {
    for (int j=i+1;j<N;j++) {
      int b = sqrt((r[i]-r[j]).magSq())*Nbins/(xhi+dx);
      if (b>Nbins-1) b = Nbins-1;
      Ghist[b] = Ghist[b] + 1.0;
    }}
    // collect velocity profile/rotation curve and number profile data
    double Tmean = 0.0;
    vector<double> VThist (Nbins);
    for (int i=0;i<N;i++) {
      int b = sqrt(r[i].magSq())*Nbins/(xhi+dx);
      if (b>Nbins-1) b = Nbins-1;  
      Vhist[b] = Vhist[b] + sqrt(v[i].magSq());
      Nhist[b] = Nhist[b] + 1.0;
      double ph = r[i].getPhi();
      ThreeVect vphi(-sin(ph),cos(ph),0.0); 
      double vp = v[i]*vphi;
      VThist[b] = VThist[b] + vp;
      Tmean += vp*vp;   // component of v along phi (squared) NOT ostriker+peebles defn!
    }
    double TOP = 0.0;
    for (int b=0;b<Nbins;b++) { 
      if (Nhist[b] > 0.0) TOP += VThist[b]*VThist[b]/Nhist[b];
    }
    ofs5 << t << " " <<  0.5*Tmean << " " << 0.5*TOP << " " << kin() << " " << U() << endl;
  } // end of the time loop 
  for (int i=0;i<N;i++) ofs << r[i] << " " << v[i] << endl;

  double xlo = dx/2.0;
  for (int i=0;i<Nbins;i++) {
    double x = xlo+i*dx;
    Ghist[i] *= 2.0*R*R*R/(N*(N-1));
    Ghist[i] /= 4.0*M_PI*x*x*dx;
    Ghist[i] /= T;
    Vhist[i] = Vhist[i]/Nhist[i];
    Nhist[i] /= T;
    ofs4 << x << " " << Ghist[i] << " " << Vhist[i] << " " << Nhist[i] << endl;
  }

  delete [] r;
  delete [] rnew;
  delete [] v;
  delete [] vnew;
  delete [] f;
  ofs.close();
  ofs2.close();
  ofs3.close();
  ofs4.close();
  ofs5.close();
  cout << " t r in disk.dat " << endl;
  cout << " t r v for particle 0 in disk2.dat " << endl;
  cout << " VMD file in disk.xyz" << endl; 
  cout << " r g(r) v(r) n(r) in disk-hist.dat " << endl;
  cout << " N = int n(r) dr (as opposed to int rho(r) r dr)" << endl;
  cout << " t Tmean1 Tmean(O&P) K |U| in disk-tu.dat " << endl;
  return 0;
}
