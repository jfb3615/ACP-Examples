#include "ZOO/Animal.h"
#include <vector>
#include <libgen.h>
#include <dlfcn.h>
void typicalDayAtTheZoo( const std::vector<Animal *> & aVect) {
  for (int i=0;i<aVect.size();i++) {
    aVect[i]->haveTypicalDay();
  }
}

int main(int argc, char **argv) {

  std::vector<Animal *> aVect;


  std::string path=dirname(argv[0])+std::string("/../Animals/");
  for (int i=1;i<argc;i++) {
    std::string dso=path+argv[i]+".so";
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
    // 
    //
    Animal * animal = (Animal *) F();
    aVect.push_back(animal);

  }

  
  typicalDayAtTheZoo(aVect);

  return 0;

}
