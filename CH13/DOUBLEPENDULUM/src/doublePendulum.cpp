#include "DoublePendulumWidget.h"
#include <QApplication>
#include <cstdlib>
#include <iostream>
#include <string>

int main (int argc, char **argv) {

  // Automatically generated:-------------------------:

  std::string usage= std::string("usage: ") + argv[0]; 
  if (argc!=1) {
    std::cout << usage << std::endl;
  }


 // Application and it's main window:
  QApplication     app(argc,argv);
  DoublePendulumWidget *window=new DoublePendulumWidget();
  window->show();
  app.exec();
  return 1;



  // -------------------------------------------------:
  return 1;

}

