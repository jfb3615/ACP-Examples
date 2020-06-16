
/*
 * Modern C++ : the 'auto' keyword
 *
 */

#include <iostream>

std::string getString() {
  std::string myStr = "C++11";
  return myStr;
}

auto getString_auto() {
  std::string myStr = std::string("C++14");
  return myStr;
}
auto getString_auto_auto(auto str) {
  return str;
}

int main(int argc, char *argv[]) {

  using namespace std;

  // C++98
  int iVar = 15;
  std::string strVar = getString();

  // C++11
  auto iVar_2 = 15;
  auto strVar_2 = getString();

  // C++14
  cout << getString_auto << " - " << getString_auto_auto("C++14 auto parameters list") << std::endl;


  return 0;
}
