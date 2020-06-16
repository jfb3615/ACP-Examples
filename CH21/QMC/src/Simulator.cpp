#include "Simulator.h"
#include <utility>
Simulator:: Simulator(GENFUNCTION U, double x, unsigned int NSTEPS):
  U(U), pos(NSTEPS),wgt(NSTEPS), engine(rndDevice())
{
  pos[0]=x;
  wgt[0]=1.0;
}
Simulator::~Simulator() {
}

void Simulator::simulate() {
  static const double scale=20;
  for (size_t i=0;i<pos.size()-1;i++) {
    double x0=pos[i];
    double w0=wgt[i];
    
    double x=flip(engine) ? x0+1.0/scale : x0-1.0/scale;
    double w=U(x/scale)+w0;
    pos[i+1]=x;
    wgt[i+1]=w;
  }
}

double Simulator::position(unsigned int i) const {
  return pos[i];
}

double Simulator::weight(unsigned int i) const {
  return wgt[i];
}

size_t Simulator::length() const {
  return pos.size();
}

std::random_device Simulator::rndDevice; 
