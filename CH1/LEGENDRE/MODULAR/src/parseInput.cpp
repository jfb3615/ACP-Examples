#include "LegendreLib.h"

#include <string>
#include <sstream>
#include <iostream>


int parseInput(int argc, char *argv[]) {
  std::string usage = std::string ("usage: ") + argv[0] + " <l> ";

  // Parse the command line:
  if (argc != 2)  {
    std::cerr << usage << std::endl;
    exit(1);
  }
  
  int l{0};
  std::istringstream stream(argv[1]);
  if (!(stream >> l)) {
    std::cerr << usage << std::endl;
    exit(2);
  } else if (l < 0) {
    std::cerr << "l must be non-negative.\n";
    exit (3);
  }
  else {
    return l;
  }
  
}
