/* 
   aHAF-grw.cpp
   guided discrete random walks
   evaluate <w>(tau) and <wO>(tau) and store
   evaluate the stag mag squared  Feynman-Hellman method
*/

#include <cstdlib>
#include <cmath>
#include <fstream>
#include <iostream>
#include <iomanip>
#include <random>

using namespace std;

int *pl = NULL;
int *mn = NULL;
int *spin = NULL;
int *NSS = NULL;                   //array to hold NSS(del E=-1,0,1)
double *wavg = NULL;
double *wwavg = NULL;
double *wOavg = NULL;
double *wwOavg = NULL;
double *eeff = NULL;
double *Oeff = NULL;
int L,skip;
double g,A,B;

int Nz();
void jackknife(double, double, double, int, int);

void jackknife(double obs[], double &avg, double &err, int Nlo, int Nhi) {
 // single elimination jackknife  (just the usual when there is no correlation)
 avg = 0.0;
 err = 0.0;
 int N = Nhi-Nlo+1;
 for (int i=Nlo;i<=Nhi;i++) {
  avg += obs[i];
  err += obs[i]*obs[i];
 }
 avg /= N;
 err = err/N - avg*avg;
 err = sqrt(err/(N-1));
 return;
}

int Nz() { 
  int S=0;
  int sgn = 1;
  for (int i=0;i<L;i++) {
    S += sgn*spin[i];
    sgn *= -1;
  }
  return S;
}

int main() {
  int seed, Sopt;
  int NRW;
  double Na;
  double dtau,tau1,tau2;
  cout << " enter L, g " << endl;
  cin >> L >> g;
  cout << " enter NRW, dtau, tau1, tau2 " << endl;
  cin >> NRW >> dtau >> tau1 >> tau2;
  cout << "enter the lag " << endl;
  cin >> skip;
  cout << " enter guidance paramters A,B " <<endl;
  cin >> A >> B;
  cout << " enter a random seed " << endl;
  cin >> seed;
  cout << " enter 1 for Neel start, 2 for dimer, 3 for iceburg " << endl;
  cin >> Sopt;
  if (dtau > 1./(L*g)) cout << "warning dtau should be less than " << 1./(L*g) << endl;
  int T1 = (int) tau1/dtau;
  int T2 = (int) tau2/dtau;
  mt19937_64 mt(seed);
  uniform_real_distribution<double> drand(0.0,1.0);
  wavg = new double[T2+1];
  wwavg = new double[T2+1];
  wOavg = new double[T2+1];
  wwOavg = new double[T2+1];
  eeff = new double[T2+1];
  Oeff = new double[T2+1];
  NSS = new int[3]; 
  spin = new int[L];
  pl = new int[L];           // watch zero start !
  mn = new int[L];
  for (int i=0;i<L;i++) {
    pl[i] = i+1;
    mn[i] = i-1;
  }
  pl[L-1] = 0;
  mn[0] = L-1;
  for (int ww=0;ww<=T2;ww++) {
   wavg[ww] = 0.0;
   wwavg[ww] = 0.0;
   wOavg[ww] = 0.0;
   wwOavg[ww] = 0.0;
  }
  for (int nr=0; nr<NRW; nr++) {
     int sgn=1;
     if (Sopt == 1) {
      for (int i=0; i<L; i++) {
       spin[i] = sgn;
       sgn *= -1;
      }
     } else if (Sopt == 2) {
      int sgn = 1;
      for (int i=0;i<L; i+=2) {
        spin[i] = sgn;
        spin[i+1] = sgn;
        sgn *= -1; 
      }
     } else {
      for (int i=0; i<L/2; i++) {
        spin[i] = 1;
        spin[i+L/2] = -1;
      }
     }
     double w = 0.0;
     double O = 0.0;
     for (int t=1; t<=T2; t++) {
      st: int i = (int) (drand(mt) * L);  // choose a random spin
      if (spin[i]*spin[pl[i]] < 0) {
       for (int delE=-1; delE <= 1; delE++) {   // evaluate NSS(delE)
        NSS[delE+1] = 0;                        // this is delE *if the flip is made*
       }
       Na = 0; 
       for (int j=0; j<L; j++) {
        if (spin[j]*spin[pl[j]] < 0) {
         int delE = -(spin[j]*spin[mn[j]] + spin[pl[j]]*spin[pl[pl[j]]])/2;
         NSS[delE+1]++;
         Na++;
        }
       }
       double cwalk = 0.0;
       for (int delE=-1; delE<=1; delE++) {
        cwalk += NSS[delE+1]*(0.5 + A*exp(-B*delE/A));
       }
       w += (double) L/4.0 - cwalk;
       wavg[t] += exp(-dtau*w);
       wwavg[t] += exp(-2.0*dtau*w);
       double tt = (double) Nz()/L;
       O += tt*tt;
       wOavg[t] += exp(-dtau*w)*dtau*O;
       wwOavg[t] += exp(-2.0*dtau*w)*dtau*dtau*O*O;
       int delE = -(spin[i]*spin[mn[i]] + spin[pl[i]]*spin[pl[pl[i]]])/2;
       double RSS =  A*exp(-B*delE/A); 
       double r = drand(mt);
       if (r < RSS*dtau*Na ) {
         spin[i] *= -1;
         spin[pl[i]] *= -1;
       }
      } else {
       goto st;
      }
     }  // end of tau loop
  }  // end of NRW loop
  
  ofstream ofs,ofs2,ofs3,ofs4,ofs5,ofs6,ofs7,ofs8;
  ofs.open("grwB.dat");
  ofs3.open("grwC.dat");
  ofs2.open("grwA.dat");
  ofs4.open("grwD.dat");
  ofs5.open("grwE.dat");
  ofs6.open("grwF.dat");
  ofs7.open("grwG.dat");
  ofs8.open("grwH.dat");
  ofs << "# aHAF-grw7b.cpp " << endl;
  ofs << "# L = " << L << " dtau = " << dtau << " NRW = " << NRW << endl;
  for (int i=0;i<=T2;i++) {
   wwavg[i] /= NRW;
   wavg[i] /= NRW;
   wOavg[i] /= NRW;
   wwOavg[i] /= NRW;
  }
  for (int i=1;i<T2;i++) {
   wwavg[i] = sqrt(wwavg[i]-wavg[i]*wavg[i])/sqrt(NRW-1);  // error estimate for each point
   wwOavg[i] = sqrt(wwOavg[i]-wOavg[i]*wOavg[i])/sqrt(NRW-1);
   ofs2 << i*dtau <<  " " << setprecision(12) << std::fixed << wavg[i] << " " << wwavg[i] << endl;
  }
  for (int i=1;i<T2;i++) {
   eeff[i] = log(wavg[i]/wavg[i+1])/(dtau*L);
   double err = (wwavg[i]/wavg[i] - wwavg[i+1]/wavg[i+1])/(dtau*L);
   Oeff[i] = (wOavg[i+1]/wavg[i+1] - wOavg[i]/wavg[i])/dtau; 
   ofs << i*dtau << " " <<  setprecision(12) << std::fixed << eeff[i] << " " << err << endl;
   ofs3 << i*dtau << " " << Oeff[i] << endl;
  }
  for (int i=1;i<T2-100;i+=100) {
   double t = log(wavg[i]/wavg[i+100])/(100.0*dtau*L);
   double err = (wwavg[i]/wavg[i] - wwavg[i+100]/wavg[i+100])/(100.0*dtau*L);
   ofs4 << i*dtau << " " << t << " " << err << endl;
   t = (wOavg[i+100]/wavg[i+100] - wOavg[i]/wavg[i])/(100*dtau);
   ofs8 << i*dtau << " " << t << endl;
  }
  double ebar;
  for (int ss=1;ss<=4;ss++) {
  ebar = 0.0;
  double devbar = 0.0;
  double errbar = 0.0;
  double Obar = 0.0;
  double Oerr = 0.0;
  int sk = skip*ss;
  cout << " skip = " << sk << endl;
  ofs5 << " skip = " << sk << endl;
  ofs6 << " skip = " << sk << endl;
  int n=0;
  for (int i=1;i<T2-sk;i+=sk) {
   double t = log(wavg[i]/wavg[i+sk])/(sk*dtau*L);
   double err = (wwavg[i]/wavg[i] - wwavg[i+sk]/wavg[i+sk])/(sk*dtau*L);
   if (i*dtau >= tau1) {
     ebar += t;
     devbar += err;
     errbar += t*t;
     n++;
   }
   ofs5 << i*dtau << " " << t << " " << err << endl;
   t = (wOavg[i+sk]/wavg[i+sk] - wOavg[i]/wavg[i])/(sk*dtau);
   err = (wwOavg[i+sk]/wavg[i+sk] - (wOavg[i+sk]*wwavg[i+sk])/(wavg[i+sk]*wavg[i+sk]) - wwOavg[i]/wavg[i] + (wOavg[i]*wwavg[i])/(wavg[i]*wavg[i]))/(sk*dtau);
   if (i*dtau >= tau1) {
    Obar += t;
    Oerr += t*t;
   }     
   ofs6 << i*dtau << " " << t << " " << err << endl;
  }
  ebar /= n;
  devbar /= n;
  errbar /= n;
  Obar /= n;
  Oerr /= n;
  errbar = sqrt(errbar-ebar*ebar)/sqrt(n-1);
  cout << " e(skip) = " << ebar << " +/- (avg dev) " << devbar << " +/- (err) " << errbar << endl;
  Oerr = sqrt(Oerr-Obar*Obar)/sqrt(n-1);
  cout << " Nz^2(skip) = " << Obar << " +/- " << Oerr << endl;
  cout << " number of data points: " << n << endl << endl;
  } // end of skip loop
  ebar=0.0;
  for (int d=0; d<T2-T1; d++) {
   ebar += eeff[d+T1];
  }
  ebar /= (T2-T1);
  for (int d=0; d<T2-T1; d++) {
   double gam = 0.0;
   for (int i=0; i<T2-T1-d; i++) {
    gam += (eeff[T1+i]-ebar)*(eeff[T1+i+d]- ebar);
   }
   gam /= (T2-T1-d);
   ofs7 << d << " " << gam << endl;
  }
 
  cout << " t, wavg, err in grwA.dat " << endl; 
  cout << " t, Eeff(1), err in grwB.dat " << endl;
  cout << " t, Eeff(100), err in grwD.dat " << endl;
  cout << " t, Eeff(skips), err in grwE.dat " << endl;
  cout << " t, Oeff(1) in grwC.dat " << endl;
  cout << " t, Oeff(100), err in grwH.dat " << endl;
  cout << " t, Oeff(skips), err  in grwF.dat " << endl;
  cout << " t, Gamma[t]  in grwG.dat " << endl << endl;
  ofs.close();
  ofs2.close();
  ofs3.close();
  ofs4.close();
  ofs5.close();
  ofs6.close();
  ofs7.close();
  ofs8.close();
  return 0;
}
