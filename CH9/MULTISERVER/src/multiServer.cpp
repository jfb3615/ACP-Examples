#include "QatDataAnalysis/ServerSocket.h"
#include <stdexcept>
#include <sstream>
#include <thread>
#include <signal.h>

enum Command {GET_VERSION_INFO,
	      CALCULATE,
	      HANGUP_SERVER};


using namespace std;


// Version this server.  The client can then check for compatibility.
const unsigned int version=0;


int main(int argc, char ** argv ) {

  // Usage statement:
  string usage = string ("usage") + argv[0] + "[-p port]"; 

  // Default port is 70000
  unsigned int port  = 70000;

  // Parse the command line:
  for (int i=1; i<argc;i++) {
    if (string(argv[i])=="-p") {
      i++;
      istringstream stream(argv[i]);
      if (!(stream>>port)) {
	throw runtime_error("Cannot parse port");
      }
    }
    else {
      cerr << usage << endl;
    }
  }
  //
  // This call prevents child processes from living on as Zombies:
  //
  signal(SIGCHLD,SIG_IGN);
  //
  try {
    //
    // Create the socket:
    //
    ServerSocket *server = new ServerSocket(port);
    while (true) {

      cout << "Server ready, waiting.." << endl;
      ServerSocket newSocket;
      server->accept(newSocket);
      pid_t child = fork();
      
      if (child == 0) {


	delete server;
	server=NULL;

	while (true) {
	  //
	  // Read the command:
	  //
	  Command command;
	  newSocket >> command;
	  //
	  // Execute the command:
	  //
	  if (command==HANGUP_SERVER) {
	    std::cout << getpid() << " HANGUP" << std::endl;
	    break;
	  }
	  else if (command==GET_VERSION_INFO) {
	    std::cout << getpid() << " VERSION" << std::endl;
	    newSocket << version;
	    newSocket << std::thread::hardware_concurrency();
	    newSocket << getpid();
	  }
	  else if (command==CALCULATE) {
	    std::cout << getpid() << " CALCULATE" << std::endl;
	  }
	}
      }
      if (!server) break;
    }
  }
  catch (exception & e) {
    cerr << "Exception was caught: " << e.what() << "\n Exiting" << endl;
  }
  return 0;
}
