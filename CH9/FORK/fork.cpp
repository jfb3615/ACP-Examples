#include <iostream>
#include <random>
#include <vector>
#include <algorithm>

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


#include <unistd.h>
#include <sys/wait.h>
int main(int argc, char **argv) {

  std::random_device dev;
  int seed=dev();

  Generator generator(seed);

  std::vector<pid_t> children;

  for (int i=0;i<4;i++) {
    pid_t child=fork();
    if (child==0) { // I am the child
      generator.compute();
      double x = generator.harvest();
      return 0;
    }
    else {          // I am the parent
      children.push_back(child);
    }
  }
    
  while (!children.empty()) {
    int status;
    pid_t pid=wait(&status);
    auto element=std::find(children.begin(),children.end(), pid);
    if (element==children.end()) {
      throw std::runtime_error("Cannot find child process in list");
    }
    else {
      children.erase(element);
    }
    sleep(1);
  }

  return 0;
}
