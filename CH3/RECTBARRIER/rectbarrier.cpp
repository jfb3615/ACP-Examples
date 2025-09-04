#include <Eigen/Dense>
#include <iostream>
#include <complex>
#include <string>
#include <sstream>
int main(int argc, char **argv) {
  using namespace std;
  string usage = string("Usage: ") 
    + argv[0] 
    + " [-?] [-v potential] [-k wvnumber]";
   
  if (argc>1 && argv[1]==string("-?")) {
    cout << usage << endl;
    return 1;
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
    return 1;
  }

  complex I(0.0,1.0);
  complex nk=k*sqrt(complex(1.0-v)),Ik=I*k,Ink=I*nk;

  Eigen::VectorXcd Y{{-exp(-Ik)    },
		     {0.0           },
		     {-Ik*exp(-Ik)},
		     {0.0}          };

  Eigen::MatrixXcd A{{exp(Ik),     -exp(-Ink),   -exp(Ink),       0.0        },
		     {0       ,    exp(Ink),      exp(-Ink),     -exp(Ik)    },
		     {-Ik*exp(Ik),-Ink*exp(-Ink), Ink*exp(Ink),   0          },
		     {0          , Ink*exp(Ink), -Ink*exp(-Ink), -Ik*exp(Ik)}};

  Eigen::MatrixXcd AInv= A.inverse();
  Eigen::VectorXcd BCDF=AInv*Y;
  complex B=BCDF(0), F=BCDF(3);
  cout << "Complex coefficients" << endl;
  cout << BCDF << endl;
  cout << endl;
  cout << "Reflection coefficient  : " << norm(B)         << endl;
  cout << "Transmission coefficient: " << norm(F)         << endl;
  cout << "Sum:                      " << norm(B)+norm(F) << endl;
  return 0;
}
