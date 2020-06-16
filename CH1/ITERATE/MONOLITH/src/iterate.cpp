#include <iostream> // for std::cout, & cetera 
#include <sstream>  // for istringstream 
#include <cstdlib>  // for exit
#include <unistd.h> // for sleep.  
//  
// Define a data structure  
// 
struct Control {   
  int iterations;   
  int seconds; 
}; 
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
// 
// finalize: 
// 
void finalize(Control *control) {   
   delete control; 
}
// 
// execute: 
// 
void execute(Control *control) {   
  if (control) {     
    for (int i=0;i<control->iterations;i++) {       
      sleep(control->seconds);       
      std::cout << i << " sleeping for " << control->seconds << " seconds" << std::endl;        
    }
  }  
}
//
//
//
int main(int argc, char ** argv) {   
  Control *control = initialize(argc, argv);   
  execute(control);   
  finalize(control); 
  return 0;
}
