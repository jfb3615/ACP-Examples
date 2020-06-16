#include <iostream>
#include <sstream>
#include <random>


int main(int argc, char ** argv) {
  
  const std::string usage = std::string(argv[0]) + " -npow NPOW -npoints NPOINTS [-a]";

  // No Arguments?  Print usage and exit:
  if (argc==1) {
    std::cout << usage << std::endl;
    return 0;
  }
  //
  // Arguments?  Parse the command line and calculate PI
  //
  bool altFormat=false;
  int NPOW=0,NPOINTS=0;
  for (int argi=1;argi<argc;argi++) {
    if (std::string(argv[argi])=="-npow") {
      argi++;
      if (argi<argc) {
	std::istringstream stream(argv[argi]);
	if ((stream >> NPOW)) {
	}
	else {
	  std::cout << "Parse error" << std::endl;
	}
      }
      else {
	std::cout << "Parse error" << std::endl;
      }
    }
    else if (std::string(argv[argi]) =="-npoints") {
      argi++;
      if (argi<argc) {
	std::istringstream stream(argv[argi]);
	if ((stream >> NPOINTS)) {
	}
	else {
	  std::cout << "Parse error" << std::endl;
	}
      }
      else {
	std::cout << "Parse error" << std::endl;
      }   
    }
    else if (std::string(argv[argi])=="-a") {
      altFormat=true;
    }
    else {
      std::cout << usage << std::endl;
      return 1;
    }      
  }

  //
  // Random number generation:
  //
  std::random_device dev;
  std::mt19937 engine(dev());
  std::uniform_real_distribution<double> flat;

  //
  // Integrate
  //
  double sum=0.0,sumSq=0.0;
  for (int i=0;i<NPOINTS;i++) {
    double x = flat(engine);
    double y = pow(x,NPOW);
    sum   += y;
    sumSq += y*y;
  }
  double mean            = sum/NPOINTS;
  double squareOfTheMean = mean*mean;
  double meanOfTheSquare = sumSq/NPOINTS;
  double variance        = meanOfTheSquare-squareOfTheMean;
  double error           = sqrt(variance/NPOINTS);

  if (altFormat) {
    std::cout << "Deviation from unity = " << ((NPOW+1)*mean-1.0) << "+-" << (NPOW+1)*error  
	      << " (" << ((NPOW+1)*mean - 1.0)/(NPOW+1)/error << ")" <<  std::endl;
  }
  else {
    std::cout << "Int x^" << NPOW << " dx from [0,1] = " << mean << "+-" << error << std::endl;
  }
  return 0;	
}
