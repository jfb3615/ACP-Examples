#include "QatDataAnalysis/ClientSocket.h"
#include "QatGenericFunctions/Variable.h"
#include "QatGenericFunctions/Parameter.h"
#include <iostream>
#include <string>
#include <sstream>
#include <string>
#include <iomanip>
#include <cmath>
#include <cstdlib>
#include <random>
int main ( int argc, char * argv[] )
{
  std::string usage=std::string(argv[0]) + " host [port]"; 
  if (argc<2) {
    std::cout << usage << std::endl;
    exit(0);
  }

  using namespace std;
  Genfun::Variable X;
  Genfun::GENFUNCTION F=1 - X + X*X - X*X*X;
  mt19937 engine;
  normal_distribution<double> gauss;

  // MAKE A PROFILE PLOT
  double error = 1.0;

  try {

      int port;
      {
	std::istringstream stream(argv[2]);
	if (!(stream >> port) ) {
	  std::cerr << "Error, cannot parse port (try ~40K range)" << std::endl;
	  exit (0);
	}
      }
      
      // CARRY OUT THE FIRST PSEUDO EXPERIMENT
      ClientSocket client_socket (argv[1], port );
      for (double t=-3.0; t<3; t += 0.3) {
	//	int i;
	//while (!(std::cin >> i)) sleep(4);
	std::cin.ignore();
	double v = F(t);
	double dv = error*gauss(engine);
	
	
	
	
	try {
	  client_socket << 0; 
	  client_socket << t;
	  client_socket << v;
	  client_socket << dv;
	  
	}
	catch (std::exception& e ) {
	  std::cout << "I/O exception was caught:" << e.what() << "\n";
	}
      }
      client_socket << 1;
  }
  catch (std::exception & e )
    {
      std::cout << "Connect exception was caught:" << e.what() << "\n";
    }
  return 0;
}
  
  
