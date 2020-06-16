//
//
// Objective function for Helium Variational Calculation
//                                                       
//
#ifndef _HELIUMOBJECTIVE_H_
#define _HELIUMOBJECTIVE_H_

#include "QatGenericFunctions/Parameter.h"           
#include "QatGenericFunctions/Sigma.h"           
#include "QatDataModeling/ObjectiveFunction.h"
#include "Eigen/Dense"
#include <vector>
#include <sstream>
class HeliumObjective:public ObjectiveFunction {

public:

  typedef Genfun::Sigma Wavefunction;

  // Iterative computation.  The electron-electron interaction is computed
  // from the previous step. If there is no previous step, it is turned OFF,
  // and this can anchor the procedure...

  HeliumObjective(unsigned int N=4, const HeliumObjective *prevObjective=NULL);

  ~HeliumObjective ();

  const Genfun::Parameter * getAlpha(unsigned int i) const;

  const Genfun::Parameter * getC(unsigned int i) const;

  Genfun::Parameter * getAlpha(unsigned int i);

  Genfun::Parameter * getC(unsigned int i);
  
  unsigned int getNumComponents() const;

  double norm() const ;

  virtual double operator() () const;

  Wavefunction wavefunction() const; 

  void normalize();

  double energy() const;
 
private:
  
  unsigned int             _N;
  std::vector<Genfun::Parameter *>  alpha;
  std::vector<Genfun::Parameter *>  c;
  const HeliumObjective            *prevObjective;

  HeliumObjective(const HeliumObjective &);
  HeliumObjective & operator = (const HeliumObjective &);

};
#endif
