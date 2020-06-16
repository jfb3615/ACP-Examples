/*
  crank-nicolson soln to the time-dep S eqn

  2D - simple ADI / with V treated differently

  compile with -std=c++11 !
  
*/

#include <vector>
#include <iostream>
#include <fstream>
#include <cmath>
#include <complex>

using namespace std;

double a;   // potl radius
double V0;  // potl height
int Nx;

void triDiag(const vector <complex <double> > &d, const complex <double> u, const complex <double> l, const std::vector <complex <double> > &r, std::vector <complex <double> > &v) {
  std::vector <complex <double> > gamma (v);
  complex <double> beta = d[0];
  v[0] = r[0]/beta;
  for (int i=1;i<v.size();i++) {
    gamma[i] = u/beta;
    beta = d[i] - l*gamma[i];
    if (beta == 0.0) exit(0);
    v[i] = (r[i]-l*v[i-1])/beta;
  }
  for (int i=v.size()-2;i>=0;i--) v[i] -= gamma[i+1]*v[i+1];
  return;
}

int IJ(int i, int j) {
  // index [0:2Nx]^2
  return (2*Nx+1)*i+j;
}

double V(double x, double y) {
  double v = 0.0;
  if (sqrt(x*x+y*y) <= a) v = V0;
  return  v;
}

int main (void) {
   int T;
   double dx,h,k0x,k0y;
   double alpha = 1.0;
   double m = 1.0;
   const complex <double> I (0.0,1.0);
   ofstream ofs,ofs2,ofs3;
   ofs.open("cn2.dat");
   ofs2.open("cn3.dat");
   ofs3.open("cn2.xyz");

   cout << " enter Nx, h, T, k0x, k0y" << endl;
   cin >> Nx >> h >> T >> k0x >> k0y;
   dx = 10.0/Nx;
   ofs3 << (2*Nx+1)*(2*Nx+1) << endl;
   cout << " enter a, V0 " << endl;
   cin >> a >> V0;

   vector <complex <double> > psi ((2*Nx+1)*(2*Nx+1),0.0);
   vector <complex <double> > psiNew ((2*Nx+1)*(2*Nx+1),0.0);
   vector <complex <double> > psip (2*Nx+1,0.0);
   vector <complex <double> > r (2*Nx+1,0.0);
   vector <complex <double> > d (2*Nx+1,0.0);
   complex <double> del (0.0,h/2.0);
   double dm = 2.0*m*dx*dx;
 
   // psi(x,y,0)
   double Psq = 0.0;
   for (int i=0;i<=2*Nx;i++) {
   for (int j=0;j<=2*Nx;j++) {
     double x = dx*(i-Nx); 
     double y = dx*(j-Nx);
     double X0 = 4.0;
     double x1 = x+X0;
     double y1 = y+X0;
     psi[IJ(i,j)] = exp(-(x1*x1+y1*y1)/(4.0*alpha))*exp(I*x1*k0x+I*y1*k0y)/sqrt(2.0*alpha*M_PI);
     ofs << x << " " << y << " " << real(psi[IJ(i,j)]) << " " << imag(psi[IJ(i,j)]) << endl;
     Psq += real(psi[IJ(i,j)]*conj(psi[IJ(i,j)]));
   }
   ofs << "  " << endl;
   }
   cout << " int |psi|^2 dx dy = " << dx*dx*Psq << endl;

   // evolve
   for (int t=0;t<=T;t++) {
     // x step for psi^{n+1/2}
     for (int j=0;j<=2*Nx;j++) { // solve for all j
      // form RHS(j)
      for (int i=1;i<2*Nx;i++) {
        double x = dx*(i-Nx);
        r[i] = (1.0 - 2.0*del/dm)*psi[IJ(i,j)] + del/dm*psi[IJ(i+1,j)] + del/dm*psi[IJ(i-1,j)];
      }
      // throw in some pbcs
      r[0] = (1.0 - 2.0*del/dm)*psi[IJ(0,j)] + del/dm*psi[IJ(1,j)] + del/dm*psi[IJ(2*Nx,j)];
      r[2*Nx] = (1.0 - 2.0*del/dm)*psi[IJ(2*Nx,j)] + del/dm*psi[IJ(2*Nx-1,j)] + del/dm*psi[IJ(0,j)];
      // form diagonal(j)
      for (int i=0;i<=2*Nx;i++) d[i] = 1.0 + 2.0*del/dm;
      // and go
      triDiag(d,-del/dm,-del/dm,r,psip);
      for (int i=1;i<2*Nx;i++) {
        psiNew[IJ(i,j)] = psip[i];
      }
     } // end of the j loop
     for (int i=0;i<=2*Nx;i++) {
     for (int j=0;j<=2*Nx;j++) {
       psi[IJ(i,j)] = psiNew[IJ(i,j)];
     }}
     // y step for psi^{n+1} 
     for (int i=0;i<=2*Nx;i++) { // solve for all i
      // form RHS(i)
      for (int j=1;j<2*Nx;j++) {
        double x = dx*(j-Nx);
        r[j] = (1.0 - 2.0*del/dm - del*V(dx*(i-Nx),dx*(j-Nx)))*psi[IJ(i,j)] + del/dm*psi[IJ(i,j+1)] + del/dm*psi[IJ(i,j-1)];
      }
      // throw in some pbcs
      r[0] = (1.0 - 2.0*del/dm)*psi[IJ(i,0)] + del/dm*psi[IJ(i,1)] + del/dm*psi[IJ(i,2*Nx)];
      r[2*Nx] = (1.0 - 2.0*del/dm)*psi[IJ(i,2*Nx)] + del/dm*psi[IJ(i,2*Nx-1)] + del/dm*psi[IJ(i,0)];
      // form diagonal(i)
      for (int j=0;j<=2*Nx;j++) d[j] = 1.0 + 2.0*del/dm + del*V(dx*(i-Nx),dx*(j-Nx));
      // and go
      triDiag(d,-del/dm,-del/dm,r,psip);
      for (int j=1;j<2*Nx;j++) {
        psiNew[IJ(i,j)] = psip[j];
      }
     } // end of the i loop
     double Psq = 0.0;
     for (int i=0;i<=2*Nx;i++) {
     for (int j=0;j<=2*Nx;j++) {
       psi[IJ(i,j)] = psiNew[IJ(i,j)];
       Psq += real(psi[IJ(i,j)]*conj(psi[IJ(i,j)]));
       ofs3 << "argon " << dx*(i-Nx) << " " << dx*(j-Nx) << " " << 20*sqrt(real(psi[IJ(i,j)]*conj(psi[IJ(i,j)]))) << endl;
       //fs3 << "argon " << dx*(i-Nx) << " " << dx*(j-Nx) << " " << 20*real(psi[IJ(i,j)]) << endl;
     }}
     Psq *= dx*dx;
     cout << t << " " << Psq << endl;
   } // end of t loop
   
   for (int i=0;i<=2*Nx;i++) {
   for (int j=0;j<=2*Nx;j++) {
     double x = dx*(i-Nx); 
     double y = dx*(j-Nx);
     ofs2 << x << " " << y << " " << real(psi[IJ(i,j)]) << " " << imag(psi[IJ(i,j)]) << endl;
   }
   ofs2 << "  " << endl;
   }
   ofs.close();
   ofs2.close();
   cout << " x,y, Re psi, Im psi init in cn2.dat "  << endl;
   cout << " x,y, Re psi, Im psi fin in cn3.dat "  << endl;
   return 0;
}
