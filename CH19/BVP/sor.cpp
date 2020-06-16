/*
       sor.cpp


       SOR Chebycheff acceleration and checkerboard updating
  
       work on a square with BCs bottom=0 left=0 right=1 top=1
       optimal omega should work in this case

*/

#include <iostream>
#include <fstream>
#include <cmath>

using namespace std;
double *V = NULL;
int N;

int main (void) {
  int it;
  double omega,dV;
  double tol;
  ofstream ofs,ofs2;
  ofs.open("sor.dat");
  ofs2.open("sor-V.dat");
  cout << " enter number of grid points, tol " << endl;
  cin >> N >> tol;
  double rJsq = pow(cos(M_PI/N),2);
  double V[1000][1000];
  // initial field
  for (int i=0;i<=N;i++) {
  for (int j=0;j<=N;j++) {
    V[i][j] = 0.0;
  }}
  // bc
  for (int i=0;i<=N;i++) {
     V[N][i] = 1.0;
     V[i][N] = 1.0;
  }
  // iterate
  omega = 1.0;
  it = 0;
  dV = 100000.;
  while (dV > tol) {
    
    if (it == 1) omega = 1.0/(1.0-rJsq/2.0);
    if (it > 1) omega = 1.0/(1.0-rJsq*omega/4.0);
    dV = 0.0;
  
    for (int i=1;i<N;i++) {  // leave boundaries untouched
    for (int j=1;j<N;j++) {
      if ((i+j)%2 == it%2) { // checkerboard
        double xi = -V[i][j] + ( V[i+1][j] + V[i-1][j] + V[i][j+1] + V[i][j-1] )/4.0;
        xi *= omega;
        dV += abs(xi);
        V[i][j] += xi;
      }
    }}
    dV /= (N*N);
    ofs << it << " " << dV << endl;
    it++;
  } // iteration loop
  cout << " iterations: " << --it << endl;

  // final result
  for (int i=0;i<=N;i++) {
    for (int j=0;j<=N;j++) {
      double x = i/ (double) N;
      double y = j/ (double) N;
      ofs2 << x << " " << y << " " << V[i][j] << endl;
    }
    ofs2 << "   " << endl;
  }
  cout << " errors in sor.dat " << endl;
  cout << " x, y, V in sor-V.dat " << endl;
  cout << " NB: these are 1/2 iterations! " << endl;
  ofs.close();
  ofs2.close();
  return 0;
}
