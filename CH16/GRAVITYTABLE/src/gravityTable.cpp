#include "QatDataAnalysis/HistogramManager.h"
#include "QatDataAnalysis/Table.h"
#include <cstdlib>
#include <iostream>
#include <sstream>
#include <string>
#include <random>
int main (int argc, char **argv) {

  // Automatically generated:-------------------------:

  std::string usage= std::string("usage: ") + argv[0]; 
  if (argc!=1) {
    std::cout << usage << std::endl;
    exit(0);
  }
  
  unsigned int seed=0;
  std::mt19937 engine(seed);
  std::normal_distribution<double> n;
  double        g=9.8;
  double       v0=-32.1;
  double       x0=10.0;
  double       sigmaV=9.0;
  double       sigmaX=4.0;

  Table table("Gravity");
  unsigned int i=0;
  for (double t=0;t<10;t+=1.0) {
    double v = v0+g*t;
    double x = x0 + v0*t + 1/2.*g*t*t;
    v += sigmaV*n(engine);
    x += sigmaX*n(engine);
    table.add("I", i++);
    table.add("Time", t);
    table.add("Velocity", v);
    table.add("Position", x);
    table.add("SigmaX", sigmaX);
    table.add("SigmaV", sigmaV);
    table.capture();
  }
  table.print();
  // -------------------------------------------------:
  return 1;

}

