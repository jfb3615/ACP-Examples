#include "Eigen/Dense"
#include <iostream>

// Compute the distance of closest approach of particle 2 
// to particle 1:
double distance(const Eigen::Vector3d & P1,
                const Eigen::Vector3d & P2,
                const Eigen::Vector3d & v1,
                const Eigen::Vector3d & v2) {

  return (P2-P1).cross((v2-v1).normalized()).norm();

}

// Compute the distance of closest approach of trajectory 2 
// to trajectory 1:
double trajSeparation(const Eigen::Vector3d & P1,
                      const Eigen::Vector3d & P2,
                      const Eigen::Vector3d & v1,
                      const Eigen::Vector3d & v2) {
  
    return fabs((P2-P1).dot(v2.cross(v1).normalized()));	

}

int main(int argc, char ** argv) {
  using namespace std;
  Eigen::Vector3d r1(1,3,4), r2(-2,0,6), p1(3,4,9), p2(3,6,-4);
  cout << "Trajectory 1 position: \n" << r1 << endl;
  cout << "Trajectory 1 velocity: \n" << p1 << endl;
  cout << "Trajectory 2 position: \n" << r2 << endl;
  cout << "Trajectory 3 velocity: \n" << p2 << endl;
  cout << endl;
  cout << "Distance at closest approach: " << distance(r1,r2,p1,p2) << endl;
  cout << "Trajectory separation       : " << trajSeparation(r1,r2,p1,p2) << endl;
}
