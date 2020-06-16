#include <iostream>
#include <random>
#include <thread>
#include <vector>

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
  for (int i=0;i<100000000;i++) {
    sum+= engine();
  }
  thingToBeHarvested=sum;
}

double  Generator::harvest() const {
  return thingToBeHarvested;
}



int main() {

  std::vector<Generator *> generator;
  std::vector<std::thread> tVector;

  size_t NPROC=std::thread::hardware_concurrency();
  for (size_t i=0;i<NPROC;i++) {
    generator.push_back(new Generator(i));
  }

  for (size_t i=0;i<NPROC;i++) { 
    //generator[i]->compute();
    tVector.push_back(std::thread(&Generator::compute, generator[i]));
  }  

  

  for (size_t i=0;i<NPROC;i++) { 
    tVector[i].join();
    double x = generator[i]->harvest();
    std::cout << "Generated  " << x << std::endl;
  }  
  return 0;
}
