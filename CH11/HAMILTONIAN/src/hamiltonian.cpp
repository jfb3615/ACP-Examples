#include "QatGenericFunctions/RungeKuttaClassicalSolver.h"
#include "QatGenericFunctions/PhaseSpace.h"
#include "QatGenericFunctions/Variable.h"
#include "QatGenericFunctions/Power.h"
#include <cmath>
#include <cstdlib>
#include "orbit.h"
#include "plot.h"

// --------------------------------------------------------------------------//
//                                                                           //
// Apply the Hamiltonian solver to central potentials..                      //
// The potentials are r^N (N>0) or -1 r^{-N} (N>0)                           //
//                                                                           //
// The solver works well for N>-2 but has problems for N<-2.                 //
//                                                                           //
// --------------------------------------------------------------------------//
using namespace Genfun;

int main (int argc, char **argv) {

  std::string usage=std::string("usage: ") + std::string(argv[0])+ " [N/def=+2 [-?]";
  if (argv[argc-1]==std::string("-?")) {
    std::cout << usage << std::endl;
    std::cout << "advice:  N> -2.0 " << std::endl;
    exit(0);
  }

  double N=argc==2 ? atof(argv[1]):2 ;         // r^N potential 

 
  Classical::PhaseSpace phaseSpace(2);         // Phase Space:
  const Classical::PhaseSpace::Component 
    & q=phaseSpace.coordinates(), 
    & p=phaseSpace.momenta();
 
  GENFUNCTION H = (p[0]*p[0]+p[1]*p[1])/2.0    // The Hamiltonian:
    +(N<0? -1.0:1.0) *Power(N/2.0)(q[0]*q[0]+q[1]*q[1]);     
  
  phaseSpace.start(q[0],0.0);                  // Starting value of q & p 
  phaseSpace.start(q[1],1.0);
  phaseSpace.start(p[0],1.0/sqrt(2.0));
  phaseSpace.start(p[1],1.0/sqrt(2.0));
  
  Classical::RungeKuttaSolver solver(H, phaseSpace);


  // Plot the solution:
  //----------------------------------------------------------------------
  QApplication     app(argc,argv);

  orbit(app, solver.equationOf(q[0]),solver.equationOf(q[1]));
  plot (app, solver.equationOf(q[0]),solver.equationOf(p[0]),"", "t", "x & px");
  plot (app, solver.equationOf(q[1]),solver.equationOf(p[1]),"", "t", "y & py");
  plot (app, solver.energy(), "Energy", "t", "E");

}

