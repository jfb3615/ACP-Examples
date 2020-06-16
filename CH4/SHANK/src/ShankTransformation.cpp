#include "ShankTransformation.h"
using namespace std;

double Shank::shank(queue<double> q) {
  double s0=q.front(); q.pop();
  double s1=q.front(); q.pop();
  double s2=q.front(); q.pop();
  return (s0*s2-s1*s1)/(s0+s2-2.0*s1);
}


void  Shank::addPoint(double val) {
  Query<double> v=val;
  for (size_t s=0;s<q.size();s++) {
    if (v.isValid()) {
      q[s].push(v);
      if (q[s].size()==4) q[s].pop();
      if (q[s].size()==3) {
	v=shank(q[s]);
      }
    }
  }
}


Shank::Shank(unsigned int size){
  q.resize(size);
}

Query<double>  Shank::getRefinement(int i) const {
  if (!q[i].empty()) return q[i].back();
  return 0;
}
