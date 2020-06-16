/*
       cn.cpp

       use Crank-Nicolson to solve the 1d advection eqn

 d/dt rho = -v d/dx rho
 with  soln rho=f(x-vt), take f(x)= pulse of square pulse
 x = (0,5)
 t = (0,1)

 take v=1


*/

#include <vector>
#include <iostream>
#include <fstream>
#include <cmath>

using namespace std;

void triDiag(const double d, const double u, const double l, const std::vector <double> &r, std::vector <double> & v, const int offset) {
  std::vector <double> gamma (v);
  double beta = d;
  v[offset] = r[offset]/beta;
  for (int i=1+offset;i<v.size()-offset;i++) {
    gamma[i] = u/beta;
    beta = d - l*gamma[i];
    if (beta == 0.0) exit(0);
    v[i] = (r[i]-l*v[i-1])/beta;
  }
  for (int i=v.size()-2-offset;i>=offset;i--) v[i] -= gamma[i+1]*v[i+1];
  return;
}

int main (void) {
  double h,del;
  int IC,Nh,Ndel,T;
  ofstream ofs0,ofs1,ofs2;
  ofs0.open("cna.dat");
  ofs1.open("cnb.dat");
  ofs2.open("cnc.dat");
  cout << " enter Nh, Ndelta, T, 0 for pulse, 1 for square " << endl;
  cin >> Nh >> Ndel >> T >> IC;
  h = 1.0/Nh; del = 5.0/Ndel;
  double d = h/(4.0*del); // v=1
  cout << " h/delta = " << h/del << " d = " << d << endl;
  vector <double> r (Ndel+1,0.0);
  vector <double> rho (Ndel+1,0.0);
  vector <double> rhoNew (Ndel+1,0.0);

  // initial condition
  for (int i=0;i<=Ndel;i++) {
    double x = i*del;
    if (IC == 0) {
      rho[i] = exp(-100.0*(x-1.0)*(x-1.0));
    } else {
      rho[i] = 0.0;
      if (abs(x-1.0) < 0.25) rho[i] = 1.0;
    }
    ofs0 <<  del*i << " " << rho[i] << endl;
  }

  // evolve
  for (int t=0;t<=T;t++) {
    // form RHS
    r[0] = rho[0] - d*rho[1] + d*rho[Ndel];
    r[Ndel] = d*rho[Ndel-1] + rho[Ndel] - d*rho[0];
     r[0] = 0.0;
     r[Ndel] = 0.0;
    for (int i=1;i<Ndel;i++) r[i] = d*rho[i-1] + rho[i] - d*rho[i+1];
    //solve for rho'
    triDiag(1.0,d,-d,r,rhoNew,0);
    rhoNew[0] = 0.0;
    rhoNew[Ndel] = 0.0;
    for (int i=0;i<=Ndel;i++) {
      rho[i] = rhoNew[i];
      if (t == T/2) ofs1 << del*i << " " << rho[i] << endl;
      if (t == T)   ofs2 << del*i << " " << rho[i] << endl;
    }
  }

  ofs0.close();
  ofs1.close();
  ofs2.close();
  cout << " rho(x,0) in cna.dat" << endl;
  cout << " rho(x,T/2) in cnb.dat" << endl;
  cout << " rho(x,T) in cnc.dat" << endl;
  return 0;
}
