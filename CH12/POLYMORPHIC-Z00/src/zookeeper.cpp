#include "ZOO/Whale.h"
#include "ZOO/Fish.h"
#include "ZOO/Rabbit.h"
#include <vector>

void executeOneWeek(const std::vector<Animal *> aVect) {
  for (int i=0;i<aVect.size();i++) {
    aVect[i]->executeDailySequence();
  }
}


int main() {

  Rabbit a1("peter");
  Whale   a2("sarko");
  Rabbit a3("jacqueline");
  Whale  a4("nathan");
  
  std::vector<Animal*> aVect={&a3,&a4};
  executeOneWeek(aVect);

  return 0;

}
