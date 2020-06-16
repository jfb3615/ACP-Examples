
#include <iostream>
#include <random>
class Generator {


public:

  // Constructor
  Generator(int seed);

  void compute();
  double harvest() const;
  
private:

  std::mt19937 engine;
  double thingToBeHarvested;
  
};

Generator::Generator(int seed):engine(seed),thingToBeHarvested(0.0) {
}

void Generator::compute() {
  double sum=0;
  for (int i=0;i<1000000000;i++) {
    sum+= engine();
  }
  thingToBeHarvested=sum;
}

double  Generator::harvest() const {
  return thingToBeHarvested;
}


#include "mpi.h"
int main(int argc, char **argv) {

  
  const unsigned int MASTER=0;
  int TASKID, NUMTASKS;
  MPI_Init(&argc, &argv);
  MPI_Comm_size(MPI_COMM_WORLD, &NUMTASKS);
  MPI_Comm_rank(MPI_COMM_WORLD, &TASKID);
  
  int seed=TASKID;

  Generator generator(seed);

  generator.compute();

  double x = generator.harvest();
  double sumx;
  int status=MPI_Reduce(&x, 
			&sumx,
			1,
			MPI_DOUBLE,
			MPI_SUM,
			MASTER,
			MPI_COMM_WORLD);

  std::cout << "Taskid " << TASKID << " Partial " << x << std::endl;

  if (TASKID==MASTER) {
    std::cout << "Total "  << sumx << std::endl;
  }

  MPI_Finalize();


  return 0;
}
