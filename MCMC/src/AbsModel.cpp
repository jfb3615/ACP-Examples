#include "AbsModel.h"

// Constructor:
AbsModel::AbsModel(unsigned int nParticles, double mass):
  nParticles(nParticles),mass(mass),x(nParticles),v(nParticles)
{
}

