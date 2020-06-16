#include <iostream>
#include <fstream>
#include <cmath>
#include <complex>
#include <iomanip>
#include <Eigen/Dense>
#include "GaussLegendreEigen.cpp"

/*

single channel non-rel T-matrix solver

g++ -I/usr/local/include/eigen3 ...

*/

using namespace std;
using namespace Eigen;

typedef std::complex<double> Complex;
static const Complex I(0.0,1.0);
double V0,r0,mu;

double deltaExact(double k) {
  // S-wave square well phase shift
  // should be a pos del for attractive potl
  double kp = sqrt(k*k+abs(V0)*2.0*mu);
  return (-k*r0+atan(k*tan(kp*r0)/kp))*180.0/M_PI;
}


Complex SqWell(double k, double p, int ell) {
  // V = V0*theta(r-r0) S-wave only
  double SqReal;
  if (k != p) {
    SqReal = -2.0*M_PI*V0/(k*p)*(sin((k+p)*r0)/(k+p) - 
                             sin((k-p)*r0)/(k-p));
  } else {
    SqReal = -2.0*M_PI*V0/(k*k)*(sin(2.0*k*r0)/(2.0*k)-r0);
  }
  return Complex(SqReal,0.0);
}

int main () {
  ofstream ofs;
  ofs.open("T.dat");
  double m1,m2;
  int Ngrid,ell;
  Complex D;

  cout << " enter m1, m2, ell " << endl;
  cin >> m1 >> m2 >> ell;
  cout << " enter V0 (<0 for attractive), r0 " << endl;
  cin >> V0 >> r0;
  cout << " enter Ngrid " << endl;
  cin >> Ngrid;
  
  mu = m1*m2/(m1+m2);
  VectorXd k(Ngrid+1),w(Ngrid);
  VectorXcd V(Ngrid+1),T(Ngrid+1);
  MatrixXcd M(Ngrid+1,Ngrid+1);

  GaussLegendreEigen(k,w,Ngrid,0.0,M_PI_2);   // create GL grid & weights
  for (int i=0;i<Ngrid;i++) {                                 // map grid
    w(i) = w(i)/pow(cos(k(i)),2);
    k(i) = tan(k(i));
  }
  //  test this
  double s=0.0;
  for (int i=0;i<Ngrid;i++) {
   s += w(i)*k(i)*k(i)*exp(-k(i)*k(i));
  }
  s *= 4/sqrt(M_PI);
  cout << " test integral " << setprecision(20) << s << endl;

  for (int ik=1;ik<50;ik++) {            // loop in scattering momentum
    double k0 = ik*0.04;
    k(Ngrid) = k0;                       // add  k0 to the grid
    for (int a=0;a<=Ngrid;a++) {
       V(a) = SqWell(k(a),k0,ell);
    }
    for (int a=0;a<=Ngrid;a++) {
    for (int b=0;b<=Ngrid;b++) {
      if (b == Ngrid) {
        double Dr = 0.0;
        for (int i=0;i<Ngrid;i++)
          Dr -= w(i)*k0*k0/(k0*k0-k(i)*k(i)); 
          D = Complex(Dr,-M_PI*k0/2.0); 
      } else {
          D = Complex(w(b)*k(b)*k(b)/(k0*k0-k(b)*k(b)),0.0); 
      }
      M(a,b) = -mu/(M_PI*M_PI)*SqWell(k(a),k(b),ell)*D;
      if (a == b) M(a,b) = Complex(1.0,0.0) + M(a,b);
    }} // end of a/b loops
    T = M.inverse()*V;                        // invert to obtain  T(k,k0)
    Complex Tonshell = T(Ngrid);
    double delta1 = atan(Tonshell.imag()/Tonshell.real())*180.0/M_PI;
    double delta2 = asin(-mu*k0*Tonshell.real()/M_PI)*90.0/M_PI;
    ofs << k0 << " " << delta1 << " " << delta2 << " " << deltaExact(k0) << endl;
  } // end of k0 loop 
  cout << " k, delta1 , delta2, delta_exact in T.dat " << endl;
  ofs.close();
  return 0;
}
