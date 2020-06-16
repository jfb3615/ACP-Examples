/*

   loop in tau1
   Feynman Hellmann method
   remember the guided random walk algorithm
   applied to the 1d SHO m=1, omega=1, phi0 ~ exp(-x*x/2) E_n = (n+1/2)

*/

#include <iostream>
#include <fstream>
#include <cstdlib>
#include <random>
#include <cmath>

using namespace std;

double gp(double, double);
double gpp(double, double);

double V(double x) {
 return x*x/2.0;
}

double gp(double g, double x) {
  // g(x) = g*x*x/4   exact guidance at g=1
  return g*x/2.0;
}

double gpp(double g, double x) {
  return g/2.0;
}

int main() {
  double tau1,tau2,g,dtau;
  int NRW, Nstats,seed;
  ofstream ofs;
  cout << " enter NRW, dtau, g, Nstats " << endl;
  cin >> NRW >> dtau >> g >> Nstats;
  cout << " enter a random seed " << endl;
  cin >> seed;
  ofs.open("grw-sho.dat");
  ofs << "# grw-sho.cpp" << endl;
  ofs << "# NRW, dtau, tau1, tau2, g, Nstats: " << NRW << " " <<  dtau 
      << " " <<  tau1 << " " << tau2 << " " <<  g << " " <<  Nstats << endl;
  ofs << "# seed = " << seed << endl;
  mt19937_64 e(seed);                              // initiate a Mersene twistor engine
  uniform_real_distribution<double> uDist(0,1);
  double dx = sqrt(dtau/1.0);                      // ie dx^2/dt = 1/m
  cout << " dx = " << dx << endl;
 
  for (int tt=1;tt<20;tt++) {
   tau1 = 0.4*tt;
   tau2 = tau1+3.0;
 
  int T1 = tau1/dtau;
  int T2 = tau2/dtau;
  double E0avg=0.0;
  double E0sq = 0.0;
  double FHavg[9] = {0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0};
  double FHsq[9] =  {0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0};
  for (int ns=0; ns < Nstats; ns++) {
   double w1=0.0,w2=0.0;
   double FHsum1[9] = {0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0};
   double FHsum2[9] = {0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0};
   for (int  nr=0; nr < NRW; nr++) {
     double x = 0.0;
     double a = 0.0;
     double FHx[9] = {0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0};
     for (int t=0; t<T1; t++) {  
      if ( uDist(e) < 0.5 - dx*gp(g,x) )  {
        x += dx;
      } else {
        x -= dx;
      }
      a += V(x) + gpp(g,x) - 2.0*gp(g,x)*gp(g,x);
      for (int j=2;j<=8;j=j+2) FHx[j] += pow(x,j);
     }  // end of tau1 loop
     double ee = exp(-dtau*a);
     w1 += ee;
     for (int j=2;j<=8;j=j+2) FHsum1[j] += ee*dtau*FHx[j];

     for (int t=T1; t<T2; t++) {  
      if ( uDist(e) < 0.5 - dx*gp(g,x) )  {
        x += dx;
      } else {
        x -= dx;
      }
      a += V(x) + gpp(g,x) - 2.0*gp(g,x)*gp(g,x);
      for (int j=2;j<=8;j=j+2) FHx[j] += pow(x,j);
     }  // end of tau2 loop
     ee = exp(-dtau*a);
     w2 += ee;
     for (int j=2;j<=8;j=j+2) FHsum2[j] += ee*dtau*FHx[j];
   }  // end of NRW loop
   double E0 = log(w1/w2)/(tau2-tau1);
   E0avg += E0;
   E0sq += E0*E0;
   for (int j=2;j<=8;j=j+2) {
     FHsum1[j] /= w1;
     FHsum2[j] /= w2;
     double vev  = (FHsum2[j]-FHsum1[j])/(tau2-tau1);
     FHavg[j] += vev;
     FHsq[j]  += vev*vev;
   }
  } // end of Nstats loop
  E0avg /= Nstats;
  E0sq /= Nstats;
  E0sq = sqrt(abs(E0sq - E0avg*E0avg))/sqrt(Nstats-1);
  cout << "<E0> = " << E0avg << " +/- " << E0sq << endl;
  ofs << tau1 << " " << E0avg << " " << E0sq << " ";
  for (int j=2;j<=8;j=j+2) {
    FHavg[j] /= Nstats;
    FHsq[j] /= Nstats;
    FHsq[j] = sqrt(abs(FHsq[j] - FHavg[j]*FHavg[j]))/sqrt(Nstats-1);
    cout << "<x^"<< j << "> = " << FHavg[j] << " +/- " << FHsq[j] << endl;
    ofs << FHavg[j] << " " << FHsq[j] << " ";
  }
  ofs << endl;
  }  // end of tau1/tt loop
  cout << "[0.5   0.75   1.875   6.5625]" << endl;
  cout << " output in grw-sho.dat " << endl;
  return 0;
}
