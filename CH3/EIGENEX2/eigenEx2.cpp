#include <Eigen/Dense>
#include <iostream>
// Solves a set of linear equations. 

int main(int argc, char **argv) {
  Eigen::VectorXd Y{{1.},{3.}};
  Eigen::MatrixXd A{{1.,2.},{4.,9.}};
  Eigen::MatrixXd AInv=A.inverse();
  Eigen::VectorXd X=AInv*Y;

  std::cout << X << std::endl;
  return 0;
}
