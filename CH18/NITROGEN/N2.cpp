/*

   N2.cpp  Ryckaert method for dumbbell N2

   this time with standard Verlet 

molecule index  m:0->N_mol-1)
atom index      a:(0,1)
particle index  i:(0->2N_mol-1)

*/


#include <iostream>
#include <fstream>
#include <cmath>
#include <random>
#include <vector>

using namespace std;
typedef mt19937_64 MersenneTwister;
double d;     // N2 diameter
double mass;  // atomic mass (equal mas case)
double eps;   // potential strength in units of Ar LJ potl
int Nmol;     // number of molecules
int Nmol3;    // Nmol^1/3 for indexing in cube
int Np;       // 2 Nmol
double L;


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
 
  void setV(ThreeVect v) {
    x=v.x; y=v.y; z=v.z;
  }

  double magSq() {
    return  x*x + y*y + z*z;
  }

 double operator*(const ThreeVect & other);

 ThreeVect operator+(const ThreeVect & other);

 ThreeVect operator-(const ThreeVect & other);

 ThreeVect operator^(const ThreeVect & other);

 friend std::ostream &operator<<(std::ostream &out, ThreeVect v0) {
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


ThreeVect *rold = NULL;
ThreeVect *r = NULL;
ThreeVect *rnew = NULL;
ThreeVect *rCM = NULL;
ThreeVect *v = NULL;
ThreeVect *f = NULL;

int IM(int m, int a) {
  // indexing for m=molecule (0,N^3) and a=atom (0,1)
  return 2*m + a;
}

int IJ(int i, int j) {
  // indexing for (0:Np)^2 
  return i*Np+j;
}


int I(int m1, int m2, int m3) {
  // indexing for 3d lattice (0:Nmol3-1)^3
  return  (m1*Nmol3+m2)*Nmol3 + m3;
}

double rsqMin(int i, int j) {
  ThreeVect r1,r2;
  r1 = r[i];
  r2 = r[j];
  if (abs(r1.x-r2.x) > L/2.0) r2.x += copysign(L,r1.x-r2.x);
  if (abs(r1.y-r2.y) > L/2.0) r2.y += copysign(L,r1.y-r2.y);
  if (abs(r1.z-r2.z) > L/2.0) r2.z += copysign(L,r1.z-r2.z);
  return (r1-r2).magSq();
}

ThreeVect rhatMin(int i, int j) {
  // really \vec{r} not \hat{r}
  ThreeVect r1,r2;
  r1 = r[i];
  r2 = r[j];
  if (abs(r1.x-r2.x) > L/2.0) r2.x += copysign(L,r1.x-r2.x);
  if (abs(r1.y-r2.y) > L/2.0) r2.y += copysign(L,r1.y-r2.y);
  if (abs(r1.z-r2.z) > L/2.0) r2.z += copysign(L,r1.z-r2.z);
  ThreeVect rh = r1-r2;
  return rh;
}

void getF() {
  // obtain F
  for (int m=0;m<Nmol;m++) {
  for (int a=0;a<2;a++) {
    int i1 = IM(m,a);
    f[IJ(i1,i1)].setV(0.0,0.0,0.0);
    for (int n=m+1;n<Nmol;n++) {
    for (int b=0;b<2;b++) {
      int i2 = IM(n,b);
      f[IJ(i1,i2)].setV(0.0,0.0,0.0);
      f[IJ(i2,i2)].setV(0.0,0.0,0.0);
      double rsq = rsqMin(i1,i2);
      double r6 = rsq*rsq*rsq;
      ThreeVect rhat = rhatMin(i1,i2);
      double ft = eps*24.0*(2.0/(r6*r6) - 1.0/r6)/rsq;
      f[IJ(i1,i2)] = ft*rhat;
      f[IJ(i2,i1)] = (-1.0)*f[IJ(i1,i2)];
    }}
  }}
  return;
}

ThreeVect getA(int i) {
  // obtain acceleration for particle i
  ThreeVect a;
  for (int j=0;j<Np;j++) {
    a = a + f[IJ(i,j)];
  }
  return a/mass;
}

void getBC() {
  // it doesn't matter how one defines wrapping, as long as the volume is L^3
  // we'll use the CM coords
  for (int m=0;m<Nmol;m++) {
    if (rCM[m].x > L) {
      rnew[IM(m,0)].x -=  L;
      rnew[IM(m,1)].x -=  L;
      r[IM(m,0)].x -=  L;
      r[IM(m,1)].x -=  L;
    }
    if (rCM[m].x < 0) {
      rnew[IM(m,0)].x +=  L;
      rnew[IM(m,1)].x +=  L;
      r[IM(m,0)].x +=  L;
      r[IM(m,1)].x +=  L;
    }
    if (rCM[m].y > L) {
      rnew[IM(m,0)].y -=  L;
      rnew[IM(m,1)].y -=  L;
      r[IM(m,0)].y -=  L;
      r[IM(m,1)].y -=  L;
    }
    if (rCM[m].y < 0) {
      rnew[IM(m,0)].y +=  L;
      rnew[IM(m,1)].y +=  L;
      r[IM(m,0)].y +=  L;
      r[IM(m,1)].y +=  L;
    }
    if (rCM[m].z > L) {
      rnew[IM(m,0)].z -=  L;
      rnew[IM(m,1)].z -=  L;
      r[IM(m,0)].z -=  L;
      r[IM(m,1)].z -=  L;
    }
    if (rCM[m].z < 0) {
      rnew[IM(m,0)].z +=  L;
      rnew[IM(m,1)].z +=  L;
      r[IM(m,0)].z +=  L;
      r[IM(m,1)].z +=  L;
    }
  } // end of molecule loop
}

double temp() {
  // equipartition theorem: per molecule we have
  // sum_{a=0,1} 1/2 m v^2 = 5/2 kT  -->
  // sum_{m,a} 1/2 m_m v_{m,a}^2 = 5/2 Nmol kT
  double kin=0.0;
  for (int i=0;i<Np;i++) {
    kin += v[i].magSq();
  }
  return mass*kin/(5.0*Nmol);
}

double LJ(double rsq) {
  // unitless Lenard-Jones potential as a function of r^2
  // scale by eps (in units of Ar epsilon)
  // m=1 eps=1 sigma=1  t=1.8 ps
  // normally V = 4 eps ( (sig/r)^12 - (sig/r)^6)  eps/k = 119.8K sig= 2.405 A
  double r6 = rsq*rsq*rsq;
  return eps*4.0*(1.0/(r6*r6) - 1.0/r6);
}

double U() {
  double u = 0.0;
  for (int m=0;m<Nmol;m++) {
  for (int a=0;a<2;a++) {
    int i1 = IM(m,a);
    for (int n=m+1;n<Nmol;n++) {
    for (int b=0;b<2;b++) {
      int i2 = IM(n,b);
      u += LJ(rsqMin(i1,i2));
    }}
  }}
  return u/Np;
}

double energy() {
  double e = 0.0;
  for (int i=0;i<Np;i++) {
   e += v[i].magSq();
  }
  e = mass*e/2.0;
  for (int m=0;m<Nmol;m++) {
  for (int a=0;a<2;a++) {
    int i1 = IM(m,a);
    for (int n=m+1;n<Nmol;n++) {
    for (int b=0;b<2;b++) {
      int i2 = IM(n,b);
      e += LJ(rsqMin(i1,i2));
    }}
  }}
  return e/Np;
}


int main(void) {
  uniform_real_distribution<double> Dist(-1.0,1.0);
  uniform_real_distribution<double> thDist(0.0,M_PI); 
  uniform_real_distribution<double> phDist(0.0,2.0*M_PI); 
  double mu,v0,dt,temp0,tol;
  int Ttherm,Tlambda;
  ofstream ofs,ofs2,ofs3;
  ofs.open("N2.dat");
  ofs2.open("N2b.dat");
  ofs3.open("N2c.dat");
  ofs << "# N2.cpp " << endl;
  cout << " input N (number of molecules = N^3), box size, molecule diameter, atomic mass " << endl;
  cin >> Nmol3 >> L >> d >> mass;
  mu = mass/2.0;
  Nmol = Nmol3*Nmol3*Nmol3;
  Np = 2*Nmol;
  cout << " enter temp, dt, Ttherm, Tlambda, eps " << endl;
  cin >> temp0 >> dt >> Ttherm >> Tlambda >> eps;
  cout << " enter tolerance for Ryckaert iteration " << endl;
  cin >> tol;
  v0 = sqrt(5.0*temp0/(2.0*mass));
  double r0 = L/Nmol3;
  cout << " enter seed " << endl;
  int seed;
  cin >> seed;
  MersenneTwister mt(seed);

  rold = new ThreeVect[Np];
  r = new ThreeVect[Np];
  rnew = new ThreeVect[Np];
  rCM = new ThreeVect[Nmol];
  v = new ThreeVect[Np];
  f = new ThreeVect[Np*Np];
  
  for (int i=0;i<Nmol3;i++) {
  for (int j=0;j<Nmol3;j++) {
  for (int k=0;k<Nmol3;k++) {
    // initial molecular CM
    ThreeVect RCM(r0*(i + 0.5 + Dist(mt)/10.0),
                  r0*(j + 0.5 + Dist(mt)/10.0),
                  r0*(k + 0.5 + Dist(mt)/10.0));
    // initial d
    double th = thDist(mt);
    double ph = phDist(mt);
    ThreeVect dd(d*sin(th)*cos(ph),d*sin(th)*sin(ph),d*cos(th));
    // set atoms
    int m = I(i,j,k);
    r[IM(m,0)].setV(RCM+dd/2.0);
    r[IM(m,1)].setV(RCM-dd/2.0);
    ofs << "0 " << r[IM(m,0)] << " " << r[IM(m,1)] << endl;
  }}}

  // try something simple for now
  for (int i=0;i<Np;i++) {
    rold[i] = r[i];
    v[i].setV(0.0,0.0,0.0);
  }
  
  cout << " initial temp: " << 0.4*mass*v0*v0 << " " << temp() << endl;
  cout << " density: " << pow(Nmol3/L,3) << endl;

  int tLam=1;
  for (int t=0;t<Ttherm;t++) {
    getF();
    // standard Verlet
    for (int i=0;i<Np;i++) {
      ThreeVect a = getA(i);
      if (dt*dt*sqrt(a.magSq()) > r0) cout << " trouble " << endl;
      rnew[i] = 2.0*r[i]  - rold[i] + dt*dt*a;
    }
    // temperature rescaling.
    tLam++;
    if (tLam == Tlambda) {
      double lambda = sqrt(temp0/temp());
       cout << temp0 << " " << temp() << endl;
      for (int i=0;i<Np;i++) rnew[i] = lambda*rnew[i] - (lambda-1.0)*rold[i];
      tLam = 1;
    }
    // impose Lagrangian constraints
    for (int m=0;m<Nmol;m++) {
      ThreeVect Dr;
      Dr.setV(r[IM(m,0)]-r[IM(m,1)]);
      ThreeVect r0 = rnew[IM(m,0)];
      ThreeVect r1 = rnew[IM(m,1)];
      ThreeVect Drold=r0-r1;  // rold(m,0)-rold(m,1); starting value is rnew from Verlet
      double lambda = 1000.0;
      int it=0;
      while (abs(lambda) > tol) {
        double den = Drold*Dr;
        den = den*dt*dt*4.0/mu;
        lambda = (Drold.magSq() - d*d)/den;
        ThreeVect rn0 = r0 - 2.0*dt*dt*lambda/mass*Dr;
        ThreeVect rn1 = r1 + 2.0*dt*dt*lambda/mass*Dr;
        r0 = rn0;
        r1 = rn1; 
        Drold = r0-r1;
        it++;
        if (it > 100) {
          cout << " iteration trouble " << lambda << " " << m << " " << t << endl;
          abort();
        }
      }
      rnew[IM(m,0)] = r0;
      rnew[IM(m,1)] = r1;
    }
    // BCs
    for (int m=0;m<Nmol;m++) {
      rCM[m] = 0.5*(rnew[IM(m,0)]+rnew[IM(m,1)]);
    }
    getBC();
    // and update
    for (int i=0;i<Np;i++) {
      v[i] = (rnew[i] - rold[i])/(2.0*dt);
      rold[i] = r[i];
      r[i] = rnew[i];
    }
    ofs2 << t << " " << r[IM(1,0)] << " " << r[IM(1,1)] << " " << (r[IM(1,0)]-r[IM(1,1)]).magSq() << endl;
    for (int m=0;m<Nmol;m++) {
      ofs << t << " "  << r[IM(m,0)] << " " << r[IM(m,1)] << endl;
    }
    ofs3 << t << " " << temp() << " " << U() << " " << energy() << endl;
  } //   end of  thermalization loop

  cout << " t, r(1), r(2) in N2.dat " << endl;
  cout << " t, r(1), r(2) for a molecule 1 in N2b.dat " << endl;
  cout << " t, T, U, E in N2c.dat " << endl;

  delete [] rold;
  delete [] r;
  delete [] rnew;
  delete [] rCM;
  delete [] v;
  delete [] f;
  ofs.close();
  ofs2.close();
  ofs3.close();
  return 0;
}
