#include <Eigen/Dense>
#include <iostream>
#include <complex>
#include <string>
#include <sstream>
typedef std::complex<double> Complex;
int main(int argc, char **argv) {
  using namespace std;
  string usage = string("Usage: ") 
    + argv[0] 
    + " [-?] [-v potential] [-k wvnumber]";
   
  if (argc>1 && argv[1]==string("-?")) {
    cout << usage << endl;
    exit (0);
  }

  // default values
   double v=0.5;
   double k=0.2;
 
  try {
    // overwritten by command line:
    for (int i=1; i<argc;i+=2) {
      istringstream stream(argv[i+1]);
      if (string(argv[i])=="-v") stream >> v;
      if (string(argv[i])=="-k") stream >> k;
    }
  }
  catch (exception &) {
    cout << usage << endl;
    exit (0);
  }

  Complex I(0,1.0);
  Complex nk=k*sqrt(Complex(1-v));
  
  Eigen::VectorXcd Y(4);
  Y(0)= -exp(-I*k);
  Y(1)= 0;
  Y(2)= -I*k*exp(-I*k);
  Y(3)= 0;

  Eigen::MatrixXcd A(4,4);
  // First row:
  A(0,0)= exp(I*k)      ;
  A(0,1)=-exp(-I*nk)    ; 
  A(0,2)= -exp(I*nk)    ; 
  A(0,3)=0              ;

  // Second row:
  A(1,0)= 0             ;
  A(1,1)= exp(I*nk)     ; 
  A(1,2)= exp(-I*nk)    ; 
  A(1,3)=-exp(I*k)      ;
  
  // Third row:
  A(2,0)= -I*k*exp(I*k)  ;
  A(2,1)=-I*nk*exp(-I*nk); 
  A(2,2)= I*nk*exp(I*nk) ; 
  A(2,3)=0                 ;

  // Fourth row:
  A(3,0)= 0                ;
  A(3,1)=I*nk*exp(I*nk)  ; 
  A(3,2)=-I*nk*exp(-I*nk); 
  A(3,3)=-I*k*exp(I*k)   ;

  Eigen::MatrixXcd AInv= A.inverse();
  Eigen::VectorXcd BCDF=AInv*Y;
  Complex B=BCDF(0), F=BCDF(3);
  cout << "Complex coefficients" << endl;
  cout << BCDF << endl;
  cout << endl;
  cout << "Reflection coefficient  : " << norm(B)         << endl;
  cout << "Transmission coefficient: " << norm(F)         << endl;
  cout << "Sum:                      " << norm(B)+norm(F) << endl;

}
