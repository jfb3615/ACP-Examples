
/*
 * Modern C++ : Variadic templates
 *
 */

#include <iostream>



// VARIADIC TEMPLATES

// base version, with single argument
template <typename T> T add(T a) {
  std::cout << __PRETTY_FUNCTION__ << "\n";
  return a;
}
// The recursive version, with multiple arguments
template <typename T, typename... ARGS> T add(T a, ARGS... args) {
  std::cout << __PRETTY_FUNCTION__ << "\n";
  return a + add(args...);
}



int main(int argc, char *argv[]) {

  using namespace std;


  // sum of 5 integers
  int iSum = add(1, 2, 3, 4, 5);

  // sum of 3 doubles
  double dSum = add(1.5, 2.3, 3.5);

  // sum of 2 std::string
  std::string a = "c++";
  std::string b = "11";
  std::string c = add(a, b);
  // output
  std::cout << "out: " << iSum << ", " << dSum << ", " << c << std::endl;

  return 0;
}
