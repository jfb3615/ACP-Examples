#include <Eigen/Dense>
#include <iostream>
// Solves an eigenvalue problem, print out the solution plus some checks:

int main(int argc, char **argv) {

  Eigen::VectorXd Y(2);
  Y(0)= 1.0;
  Y(1)= 3.0;

  Eigen::MatrixXd A(2,2);
  A(0,0)= 1.0; A(0,1)=2.0;
  A(1,0)= 2.0; A(1,1)=9.0;

  Eigen::SelfAdjointEigenSolver<Eigen::MatrixXd> s(A);
  Eigen::VectorXd  val=s.eigenvalues();
  Eigen::MatrixXd  vec=s.eigenvectors();

  std::cout << "Eigenvalues:\n "  << val << std::endl;
  std::cout << std::endl;

  std::cout << "Eigenvectors A:\n " << vec << std::endl;
  std::cout << std::endl;

  std::cout << "Aᵀ⋅A:\n" << vec.transpose()*vec << std::endl;
  std::cout << std::endl;

  std::cout << "Reconstituted original matrix:\n" <<  vec*val.asDiagonal()*vec.transpose() << std::endl;
  std::cout << std::endl;



}
