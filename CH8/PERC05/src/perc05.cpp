#include "Percolator.h"
#include "QatDataAnalysis/OptParse.h"
#include "QatDataAnalysis/Hist1D.h"
#include <cstdlib>
#include <iostream>
#include <string>
#include <cstdio>
#include <thread>
#include <memory>
// This program generates a sequence consisting of multiple (nominally 1000)
// configuration each at probabilities p. It determines how many of these
// have percolated and writes out the data in the following format:
//
//    p nSuccess nFailure
//
// to the standard output.  This process is multithreaded.  It detects the
// number of processors on a unix (or mac) system and then opens up to that
// number of independent configurations, simultaneously, repeating the operation
// each time the batch of configurations finishes until the entire job is
// done. 

using namespace std;
typedef unique_ptr<thread> ThreadPtr;
typedef unique_ptr<Percolator> PercolatorPtr;

// The threader contains the percolator and a status flag (percolates)
// which indicates upon completion of compute method whether the cluster
// has percolated or not.
struct Threader {
  PercolatorPtr percolator; // Has a percolator!
  bool percolates;
};

//
// Runs in parallel:
//
void compute (Threader * threader) {
  threader->percolator->next();
  threader->percolator->cluster();
  threader->percolates=threader->percolator->percolates();

}


int main (int argc, char **argv) {

  //
  // Parse the command line
  //
  string usage= string("usage: ") + argv[0] +
    "[PMIN=val/def=0.55] [PMAX=val/dev=0.65] [NP=val/def=100] [NPER=val/def=1000] [L=val/def=100]"; 

  //
  // Parse the command line:
  // 
  NumericInput input;
  double PMIN=0.55, PMAX=0.65;
  unsigned int NP=100, NPER=1000,L=100;
  
  input.declare("PMIN", "Minimum range in p",   PMIN);
  input.declare("PMAX", "Maximum range in p",   PMAX);
  input.declare("NP",   "Number of points in P", NP);
  input.declare("NPER", "Number of trials per point", NPER);
  input.declare("L", "Lattice size", L);
  try {
    input.optParse(argc, argv);
  }
  catch (const exception & e)  {
    cerr << usage << endl;
    cerr << input.usage() << endl;
    exit(1);
  }
  if (argc>1 && std::string(argv[1])=="-?") {
    std::cout << usage << std::endl;
    std::cout << input.usage() << std::endl;
    exit(0);
  }
  PMIN = input.getByName("PMIN");
  PMAX = input.getByName("PMAX");
  NP   = (unsigned int ) (0.5 + input.getByName("NP"));
  NPER = (unsigned int ) (0.5 + input.getByName("NPER"));
  L    = (unsigned int ) (0.5 + input.getByName("L"));
  //
  // Determine the number of CPUS on this machine:
  //
  unsigned int NPROCESSORS=4;
  FILE *cpu=popen("cat /proc/cpuinfo | grep processor | wc -l","r");
  if (!fscanf(cpu,"%u",&NPROCESSORS)) {
    std::cerr << "Warning, cannnot detect number of processors, using just 1" << std::endl;
  }
  pclose(cpu);

  // Start the threads
  double dp=(PMAX-PMIN)/NP;
  for (unsigned int i=0; i<NP; i++) {
    double p = PMIN+i*dp + dp/2.0;
    unsigned int NJOBS=NPER;
    unsigned int success=0,failure=0;
    while (NJOBS>0) {
      unsigned int N = min(NJOBS,NPROCESSORS);
      vector<Threader>  threader(N);
      vector<ThreadPtr> threadVector(N);
      for (unsigned int j=0;j<N;j++) threader[j].percolator= PercolatorPtr(new Percolator(L,L,p));
      for (unsigned int j=0;j<N;j++) threadVector[j]=ThreadPtr(new thread(compute,&threader[j]));
      for (unsigned int j=0;j<N;j++) threadVector[j]->join();
      for (unsigned int j=0;j<N;j++)  {
	if (threader[j].percolator->percolates()) {
	  success++;
	}
	else {
	  failure++;
	}
      }
      NJOBS-=N;
    }
    cout << p << " " << success << " " << success+failure  << endl;
  }
  
  if (argc!=1) {
    cout << usage << endl;
  }

  // -------------------------------------------------:
  return 1;

}

