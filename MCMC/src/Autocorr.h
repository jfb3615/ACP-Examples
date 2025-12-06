#ifndef _AUTOCORR_H_
#define _AUTOCORR_H_
#include "Eigen/Dense"
#include <optional>
class Autocorr {

 public:
  
  Autocorr(unsigned int T);

  // Add a variable to follow to the calculation.  This is legal only until
  // the first data point is entered. 
  void         followVariable (const double * variable); 
  unsigned int getNumFollowedVariables() const;

  // The object becomes "locked" when the first data point is added. 
  void addDataPoint(unsigned int t);
  

  // this method provides an average autocorr.
  double harvest() const;

  // compute the result:
  void compute();
  
  // retreive the result. 
  double harvest(unsigned int i) const;

  // retrieve the time associated with this Autocorrelation calculator
  unsigned int getT() const;
  
 private:
  
  bool locked{false};
  unsigned int N{0};
  
  unsigned int T{0};                              // The time over which to check autocorr
  std::vector<const double *> followedVariables;  // List of variable to follow

  mutable Eigen::VectorXd xi;
  mutable Eigen::VectorXd xj;
  mutable Eigen::MatrixXd xixi;
  Eigen::MatrixXd xjxj;
  Eigen::MatrixXd xixj;
  Eigen::VectorXd X;
  double result{0};  
};
#include "Autocorr.tpp"
#endif
