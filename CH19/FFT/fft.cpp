/*
       fft.cpp

  nabla^2 u = exp(-x^2-y^2)  on (0,1)^2 w/ u=0 on BC

g++ -I/usr/local/include/eigen3

*/

#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>
#include <complex>
#include <Eigen/Dense>
#include <unsupported/Eigen/FFT>

using namespace std;

void sineTransform2D(Eigen::MatrixXd & Mk, const Eigen::MatrixXd & Mx) {
  // Mk_{nm} = sum_{ij=1}^{N-1} Mx_{ij} sin(in pi/N) sin (jm pi/N)
  int N = Mx.rows();
  int M = Mx.cols();
  Eigen::FFT<double> fft;
  Eigen::MatrixXd Mrow(N,M);
  std::vector<double> f(2*N,0.0);
  std::vector<std::complex<double> > fkk(2*N,0.0);
  // sine transform rows
  for (int j=0;j<M;j++) {
    for (int i=1;i<N;i++) {
      f[i] = Mx(i,j);
      f[2*N-i] = -Mx(i,j);
    }
    f[0] = 0.0;
    f[N] = 0.0;
    fft.fwd(fkk,f);
    for (int m=0;m<N;m++)  Mrow(m,j) = -1.0*imag(fkk[m])/2.0;
  }
  // sine transform columns
  f.resize(2*M);
  for (int m=0;m<N;m++) {
    for (int j=1;j<M;j++) {
      f[j] = Mrow(m,j);
      f[2*M-j] = -Mrow(m,j);
    }
    f[0] = 0.0;
    f[M] = 0.0;
    fft.fwd(fkk,f);
    for (int n=0;n<M;n++) Mk(m,n) = -1.0*imag(fkk[n])/2.0;
  }
  return;
}

int main (void) {
  ofstream ofs;
  ofs.open("fft.dat");
  int N;
  cout << "enter N " << endl;
  cin >> N ;
  double del = 1.0/(N+1);

  Eigen::MatrixXd rho(N+1,N+1);
  Eigen::MatrixXd rhoHat(N+1,N+1);
  Eigen::MatrixXd uHat(N+1,N+1);
  Eigen::MatrixXd u(N+1,N+1);

  for (int i=1;i<N+1;i++) {
  for (int j=1;j<N+1;j++) {
    double x = del*i;
    double y = del*j;
    rho(i,j) = exp(-(x*x+y*y)/0.1);
    // if (j == N) rho(i,j) = rh - vz/(dely*dely);
  }}
  sineTransform2D(rhoHat,rho);
  for (int n=1;n<N+1;n++) {
  for (int m=1;m<N+1;m++) {
    uHat(n,m) = rhoHat(n,m)/(2.0*(cos(n*M_PI/(N+1))-1.0)/(del*del) + 
                         2.0*(cos(m*M_PI/(N+1))-1.0)/(del*del)) ;
  }}
  // invert to obtain u(n,m)
  sineTransform2D(u,uHat);
  for (int i=1;i<N+1;i++) {
  for (int j=1;j<N+1;j++) {
    ofs << i*del << " " << j*del << " " << u(i,j)*4/((N+1)*(N+1)) << endl;
  }
  ofs << "   " << endl;
  } 

  cout << " x y u in fft.dat " << endl;
  ofs.close();
  return 0;
}
