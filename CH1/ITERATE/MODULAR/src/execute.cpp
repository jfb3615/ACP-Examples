#include "iterate.h"
#include <iostream>
#include <unistd.h>
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
