#include "QatDataAnalysis/ClientSocket.h"
#include <stdexcept>
#include <iostream>
#include <string>
#include <sstream>
#include <stdexcept>

enum Command {GET_VERSION_INFO,
	      CALCULATE,
	      HANGUP_SERVER};

int main ( int argc, char * argv[] )
{
  using namespace std;
  //
  //
  //
  string usage = string ("usage") + argv[0] + "[-h host/localhost] [-p port/7000]"; 
  //
  //
  //
  string host          = "localhost";
  unsigned int port    = 70000;

  for (int i=1; i<argc;i++) {
    if (string(argv[i])=="-h") {
      i++;
      host=argv[i];
    }
    else if (string(argv[i])=="-p") {
      i++;
      istringstream stream(argv[i]);
      if (!(stream>>port)) {
	throw runtime_error("Cannot parse port");
      }
    }
    else {
      cerr << usage << endl;
      return 1;
    }
  }  
  try {
    
    ClientSocket clientSocket(host,port);
    unsigned int    version, concurrency, pid;
    clientSocket << GET_VERSION_INFO; // Sample size and version;
    clientSocket >> version;
    clientSocket >> concurrency;
    clientSocket >> pid;
    cout << "Client Connected to host "
	 << host
	 << " port "
	 << port
	 << " server version "
	 << version
	 << " hardware concurrency  "
	 << concurrency
	 << " pid " 
	 << pid
	 << endl;
    
    clientSocket << CALCULATE;
    clientSocket << HANGUP_SERVER;
  }
  catch (const exception & e ) {
    cout << "Exception caught: " << e.what() << endl;
  }
  
  return 0;
}
