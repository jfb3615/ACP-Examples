#ifndef _Richardson_h_
#define _Richardson_h_
#include "QatDataAnalysis/Query.h"
#include <vector>
// Do a richardson transformation on the series.:

class Richardson {
public:
  
  // Constructor, taking the number of refinements
  Richardson(unsigned int size);

  // Add a point in the series:
  void  addPoint(double val);
  
  // Gets the i^the refinement.  i=0: no refinement.  i=1 first. i=2 second. etc.
  Query<double>  getRefinement() const;
  
  
private:

  unsigned int                       size;
  std::vector<double>          q;
  Query<double>                 refinement;

  // Here is an actual computation:
  double richardson() const;

};
#endif
