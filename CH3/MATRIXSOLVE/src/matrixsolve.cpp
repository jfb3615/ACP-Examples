#include <Eigen/Dense>
#include <iostream>
int main(int argc, char **argv) {
  using namespace std;
  
  // Solve the equation:
  // A*X = Y where:
  // A=  1.0   2.0
  //     4.0   9.0
  // 
  // Y= 1
  //    3

  Eigen::VectorXd Y{{1.0},{3.0}};
  Eigen::MatrixXd A{{1.0,2.0},{4.0,9.0}};

  cout << "Determinant = " << A.determinant() << endl;

  Eigen::MatrixXd AInv= A.inverse();
  cout << "Solution is \n" << AInv*Y << endl;

}
