#include "iterate.h"
#include <iostream>
#include <sstream>
// 
// Parse the command line: 
// 
Control *initialize (int argc, char ** argv) {
  Control *control=NULL;
  if (argc!=3) {
    std::cerr << "Usage: " << argv[0] << " iterations seconds" << std::endl;     
    exit(0);   
  }   
  else {     
    control =new Control;     
    control->iterations=0;     
    control->seconds=0;     
    {       
      std::istringstream stream(argv[1]);       
      if (!(stream >> control->iterations)) return control;
    }     
    {       
     std::istringstream stream(argv[2]);       
     if (!(stream >> control->seconds)) return control;     
    }   
  }   
  return control; 
}

