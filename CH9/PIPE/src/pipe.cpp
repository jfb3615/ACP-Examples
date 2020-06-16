#include <cstdio>
#include <cstdlib>
#include <string>
#include <iostream>
#include <signal.h>
#include <unistd.h>
//
// Pipe: a simple program to pass some data (a string) from a child
// to a parent.  The parent prints it to the terminal. 
//
int main() {

  // Don't wait for child to finish.
  signal(SIGCHLD,SIG_IGN);

  // Open a pipe:
  int pipefd[2];
  pipe(pipefd);
  
  // For subprocess
  pid_t child = fork();

  
  if (child == 0) {    // Child writes string to the pipe:
    close(pipefd[0]);  // Close unused read end
    
    std::string blurb="Ceci n'est pas une pipe";
    size_t bsize=blurb.size();
    
    write(pipefd[1], &bsize, sizeof(bsize));         // write the data size
    write(pipefd[1], blurb.c_str(), blurb.size());   // write the date
    close(pipefd[1]);                                // close the write end
    
  } else {             // Parent reads string from pipe

    close(pipefd[1]);  // Close unused write end

    size_t bsize;      
    read(pipefd[0], &bsize, sizeof(bsize)); // Read the string size

    char buf[bsize];                        // Allocate space
    read(pipefd[0], buf, bsize);            // Read the string
   
    std::cout << buf << std::endl;          // Print the string
    close(pipefd[0]);                       // Close the read end

  }
}
