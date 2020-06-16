#include "mpi.h"
#include <iostream>
#include <unistd.h>

// Complile with: mpicxx -o hw2 hw2.cpp

int main(int argc, char *argv[]) {
 using namespace std;
 int numtasks, taskid, len;
 char hostname[MPI_MAX_PROCESSOR_NAME];

 MPI_Init(&argc,&argv);                      // initialize MPI
 MPI_Comm_size(MPI_COMM_WORLD,&numtasks);    // get number of tasks
 MPI_Comm_rank(MPI_COMM_WORLD,&taskid);      // get my rank
 MPI_Get_processor_name(hostname, &len);

 cout << " Hello World from task " << taskid << endl;
 cout << " hostname: " << hostname << " number of tasks: " << numtasks << endl;
 MPI_Finalize();
}

