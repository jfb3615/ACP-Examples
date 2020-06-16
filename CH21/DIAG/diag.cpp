/* 

 g++ -I/usr/local/include/eigen3 -o diag diag.cpp

*/


#include <iostream>
#include <Eigen/Eigenvalues> 
      
using namespace std;
using namespace Eigen;


double V(double x) {
  return x*x/2.0;
}

double E(int i, int ell) {
  return  2*i+ell+1.5;
}

int main() {
  int N,ell;
  double xmax;
  cout << " enter xmax, ell, N " << endl;
  cin >> xmax >> ell >> N;
  double dx = xmax/N;
  MatrixXd H = MatrixXd::Zero(N,N); // variable-size double matrix
  // fill in H
  for (int i=0;i<N;i++) {
    double x = i*dx + dx;
    H(i,i) = 1.0/(dx*dx) + ell*(ell+1.0)/(2.0*x*x) + V(x);
  }
  for (int i=0;i<N-1;i++) {
    double x = i*dx + dx;
    H(i,i+1) = -1.0/(2.0*dx*dx);
    H(i+1,i) = -1.0/(2.0*dx*dx);
  }
  SelfAdjointEigenSolver<MatrixXd> es(H);   
  VectorXd ev(N), r2(N);
  for (int i=0;i<N;i++) {   
    double x = dx*i + dx;
    r2(i) = x*x;
  }
  MatrixXd R2 = r2.asDiagonal();
  for (int i=0;i<10;i++) { 
    ev = es.eigenvectors().col(i).normalized();  // ith eigenvector
    ev /= sqrt(dx);                              // normalize
    double rsq   = dx * ev.transpose() *  R2 * ev;  // <r^2>
    cout << E(i,ell) << " " << es.eigenvalues()[i] << " " << (es.eigenvalues()[i]-E(i,ell))/E(i,ell) << " Rrms: " << sqrt(rsq) << endl;
  }

  return 0;
}
