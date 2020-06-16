#ifndef _ITERATE_H_ 
#define _ITERATE_H_ 

// Data structure controlling the iteration loop: 
struct Control {      
  int iterations;      
  int seconds;  
}; 

// Initialize.  Parse the command line: 
Control *initialize(int argc, char ** argv);

// Execute the iteration loop: 
void execute(Control *control);

// Finalize.  Clean up the memory: 
void finalize(Control *control);

#endif

