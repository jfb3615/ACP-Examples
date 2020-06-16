#include <iostream>
int main() {

  {
    int i=0XFA4;
    std::cout << i << std::endl;
  }

  {
    int i=0;
    i= 1 | (1<<2) | (1<<4);
    std::cout << i << std::endl;
  }

  {
    int i=55;
    for (int p=31;p>=0;p--) {
      std::cout << ((i & (1<<p))!=0 );
    }
    std::cout << std::endl;
  }

}
