#include "CubicSplineXtrapolatorWidget.h"
#include <QApplication>
#include <cstdlib>
#include <iostream>
#include <string>
#include <stdexcept>
int main (int argc, char * * argv) {

  // Automatically generated:-------------------------:
  
  std::string usage= std::string("usage: ") + std::string(argv[0]) + std::string("[-p plugin]"); 

  std::string filename;
  for (int i=1; i<argc;i++) {
    if (std::string(argv[i])=="-p") {
      i++;
      if (i<argc) {
	filename=argv[i];
	std::copy (argv+i+1, argv+argc, argv+i-1);
	argc -=2;
	i    -=2;
       }
      else {
	throw std::runtime_error("Cannot parse plugin file" );
      }
    }
  }
  if (argc!=1) {
    std::cout << usage << std::endl;
  }
  

  // Application and it's main window:
  QApplication     app(argc,argv);
  CubicSplineXtrapolatorWidget *window=new CubicSplineXtrapolatorWidget(NULL, filename);
  window->show();
  app.exec();
  return 1;

}

