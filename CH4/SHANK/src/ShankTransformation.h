#ifndef _Shank_h_
#define _Shank_h_
#include "QatDataAnalysis/Query.h"
#include <queue>
#include <vector>

// Do a shank transformation on the series.:

class Shank {
public:
  
  // Constructor, taking the number of refinements
  Shank(unsigned int size);

  // Add a point in the series:
  void  addPoint(double val);
  
  // Gets the i^the refinement.  i=0: no refinement.  i=1 first. i=2 second. etc.
  Query<double>  getRefinement(int i) const;
  
  
private:
  
  // A vector of queues. Each one will ever hold
  // up to four values.  
  std::vector<std::queue<double>>          q;

  // Here is an actual computation:
  static double shank(std::queue<double> q);

};
#endif
