#include "RichardsonTransformation.h"
#include <cmath>
using namespace std;

double Richardson::richardson() const {
  double sum=0;
  const unsigned int & MPP=size, M=MPP-1;
  const unsigned int     N=q.size()-M;
  //
  // You need M+1 terms labelled to M.
  //
  for (unsigned int k=0;k<MPP;k++) {
    sum  += pow(-1,M+k)*q[N-1+k]*pow(N+k,M)/tgamma(k+1)/tgamma(M-k+1);
  }
  return sum;
}


void  Richardson::addPoint(double val) {
  q.push_back(val);
  if (q.size()>=size) {
    refinement=richardson();
  }
}


Richardson::Richardson(unsigned int size):size(size){
}

Query<double>  Richardson::getRefinement() const {
  return refinement;
}
