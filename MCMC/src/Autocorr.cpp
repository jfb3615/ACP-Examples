#include "Autocorr.h"

void Autocorr::addDataPoint(unsigned int t) {

  if (t%T) return;  // Sample every T points. 
  
  size_t NVARS{followedVariables.size()};
  if (!locked) {
    if (NVARS==0) throw std::runtime_error ("The Autocorr is not following any variables");

    xixj=Eigen::MatrixXd::Zero(NVARS,NVARS);
    xixi=Eigen::MatrixXd::Zero(NVARS,NVARS);
    xjxj=Eigen::MatrixXd::Zero(NVARS,NVARS);
    xi=Eigen::VectorXd::Zero(NVARS);
    xj=Eigen::VectorXd::Zero(NVARS);
    X=Eigen::VectorXd::Zero(NVARS);

    locked=true;
  }
  else {
    N++;
  }

  Eigen::VectorXd Y=Eigen::VectorXd::Zero(NVARS);
  for (size_t i=0;i<NVARS;i++) {
    Y(i)=*followedVariables[i];
  }

  if (N>1) {

    
    // now X is the old set, Y is the new set.
    
    xi += Y;
    xixi += Y*Y.transpose();
    xj += X;
    xjxj+=X*X.transpose();
    xixj+=X*Y.transpose();
  }  
  
  // Get ready for next round. 
  X=Y;

}

void Autocorr::compute() {

  // 
  //double sum{0.0};
  //for (unsigned int i=0;i<xixj.rows();i++) {
  //  sum+=harvest(i);
  //}
  //result=sum/xixj.rows();

  double sij2=xixj.trace()/double(N-1)-(xi*xj.transpose()).trace()/double(std::pow(N-1,2));
  double sii2=xixi.trace()/double(N-1)-(xi*xi.transpose()).trace()/double(std::pow(N-1,2));
  double sjj2=xjxj.trace()/double(N-1)-(xj*xj.transpose()).trace()/double(std::pow(N-1,2));


  result =  sij2/sqrt(sii2*sjj2);	
}

double Autocorr::harvest() const {
  return result;
}
double Autocorr::harvest(unsigned int i) const {

  double sij2=xixj(i,i)/double(N-1)-xi(i)*xj(i)/double(std::pow(N-1,2));
  double sii2=xixi(i,i)/double(N-1)-xi(i)*xi(i)/double(std::pow(N-1,2));
  double sjj2=xjxj(i,i)/double(N-1)-xj(i)*xj(i)/double(std::pow(N-1,2));
  
  return sij2/sqrt(sii2*sjj2);	
}

  
