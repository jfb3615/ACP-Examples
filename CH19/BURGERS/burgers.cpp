/*
       burgers.cpp

       upwind differenced Burgers eqn w/ nonlinear trick


*/

#include <iostream>
#include <fstream>
#include <cmath>

using namespace std;


int main (void) {
  int N,T,it1,it2;
  double R,D,h,del;
  ofstream ofs,ofs2;
  ofs.open("burgers.dat");
  ofs2.open("burgers-V.dat");
  cout << " enter h [0.05], del [0.05], box size [10], T [100], D [0.01]  " << endl;
  cin >> h >> del >> R >> T >> D;
  cout << "enter 1/h, 2/h " << endl;
  cin >> it1 >> it2;
  N = R/del;

  double V[4000];
  double Vnew[4000];
  double Vold[4000];
  
  // initial field
  for (int i=0;i<N+1;i++) {
    double x = del*i;
    V[i] = exp(-(x-3.0)*(x-3.0));  // pos so upwards difference and motion to the right
    Vold[i] = V[i];  
  }

  // iterate
  for (int it=0;it<T;it++) {
    for (int i=1;i<N;i++) {  // leave boundaries untouched (assume V-> 0 there)
      Vnew[i] =  V[i] - h/del*Vold[i]*(V[i] - V[i-1]) + 
                 h*D/(del*del)*(V[i+1]-2.0*V[i] + V[i-1]);
    }
    for (int i=1;i<N;i++) {
      Vold[i] = V[i];
      V[i] = Vnew[i];
      ofs << it*h << " " << del*i << " " << V[i] << endl;
    }
    if ((it == 0) || (it == it1) || (it == it2)) {
      for (int i=0;i<N+1;i++) {
        ofs2 << it*h << " " << del*i << " " << V[i] << endl;
      }
    }
    ofs << "  " << endl;
  }
  
  cout << "t  x, V in burgers-V.dat " << endl;
  ofs.close();
  ofs2.close();
  return 0;
}
