#include "ZOO/Whale.h"
#include "ZOO/Fish.h"
#include "ZOO/Rabbit.h"
#include <vector>
void htd(const std::vector<Animal *> aVect) {
  for (int i=0;i<aVect.size();i++) {
    aVect[i]->haveTypicalDay();
  }
}


int main() {


  Rabbit a3("jacqueline");
  Whale  a4("nathan");
  
  std::vector<Animal*> aVect={&a3,&a4};
  htd(aVect);
  


  return 0;

}
