#include "ZOO/Animal.h"
#include <vector>
#include <libgen.h>
#include <dlfcn.h>
void typicalDayAtTheZoo( const std::vector<Animal *> & aVect) {
  for (int i=0;i<aVect.size();i++) {
    aVect[i]->executeDailySequence();
  }
}

int main(int argc, char **argv) {

  // Here is a vector which we will load with animals
  std::vector<Animal *> aVect;

  // The name of the place to look for plugins:
  std::string path=dirname(argv[0])+std::string("/../Animals/");

  // The name of the animal is given on the command line:
  for (int i=1;i<argc;i++) {

    // We have arranged for the name of the dynamic shared
    // object (dso) to be encoded with the name of the animal
    // 
    std::string dso=path+argv[i]+".so";
    // In the dso, the name of the function is also encoded with
    // the name of the animal:  eg makeSloth()
    std::string createFunctionName = std::string("make")+argv[i];

    //
    // Loads the library:
    //
    void *handle = dlopen(dso.c_str(),RTLD_NOW);
    if (!handle) std::cout << dlerror() << std::endl;

    //
    // Gets the function
    //
    void *f = dlsym(handle,createFunctionName.c_str());
    if (!f) std::cerr << dlerror() << std::endl;
    
    typedef void * (*CreationMethod) ();
    CreationMethod F = (CreationMethod) f;
    
    //
    // Invoke the creation function, collect the animal.  
    //
    Animal * animal = (Animal *) F();
    aVect.push_back(animal);

  }

  // send messages to the polymorphic collection of animals. 
  typicalDayAtTheZoo(aVect);

  return 0;

}
