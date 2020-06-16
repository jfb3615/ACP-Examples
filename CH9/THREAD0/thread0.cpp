// thread example
#include <iostream>       // std::cout
#include <thread>         // std::thread
#include <cmath>
#include <mutex> 
#include <random> 
#include <vector> 


void foo(int seed, double * result) 
{
  std::mt19937 engine(seed);
  double sum=0;
  for (int i=1;i<10000;i++) sum += engine();
  *result=sum;
}


int main() 
{

  size_t NPROC=std::thread::hardware_concurrency();

  std::vector<std::thread> tVector;
  std::vector<double>       rVector(NPROC);
  
  for (size_t i=0;i<NPROC;i++) {
    tVector.push_back(std::thread(foo,i,&rVector[i]));
  }
  

  for (size_t i=0;i<NPROC;i++) {
    tVector[i].join();
    std::cout << "Result " << i << " :" << rVector[i] << std::endl;
  }


  return 0;
}
