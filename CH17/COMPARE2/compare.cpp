#include <iostream>
#include <complex>  // std::complex

#include <limits>   // std::numeric_limits --> guaranteed minimal limits
#include <climits>  // ULLONG_MAX --> actual limits

bool comparesGreater(int a, int b) {
  std::cout << " (non-templated function) " << std::endl;
  if (a > b)
   return true;
 return false;
}

template <typename T> bool comparesGreater(T a, T b) {
  std::cout << " ** templated function ** " << std::endl;
  if (a > b)
    return true;
  return false;
}

int main(int argc, char *argv[]) {

 using namespace std;


 cout << "1 (int) compares greater than 2 (int)? " << comparesGreater(1, 2) << endl; // for this call the compiler will choose the non-templated function
 cout << "1.5 (float) compares greater than 1.22 (float)? " << comparesGreater(1.5, 1.22) << endl; // for this call the compiler will choose the templated function
 cout << "'c' (char) compares greater than 'f' (char)? " << comparesGreater( 'c', 'f' ) << endl; // for this call the compiler will choose the templated function
 cout << "'c++' (string) compares greater than 'c' (string)? " << comparesGreater( string("c++"), string("c") ) << endl; // for this call the compiler will choose the templated function


 cout << "Checking numeric limits..." << endl;
 std::cout << "Maximum value for 'int' (<limits>): " << std::numeric_limits<int>::max() << '\n'; // guaranteed minimals
 std::cout << "Maximum value for 'unsigned long long int' (<limits>): " << std::numeric_limits<unsigned long long int>::max() << '\n'; // guaranteed minimals
 std::cout << "Maximum value for 'unsigned long long int' (<climits>): " << ULLONG_MAX << '\n'; // actual limit
 unsigned long long int dataA = 1.5*10E18;
 unsigned long long int dataB = 1*10E-15;
 cout << "1.5E19 (unsigned long long int) compares greater than 1E-15 (unsigned long long int)? " << comparesGreater( dataA, dataB ) << endl; // --> true


 std::complex<double> c1 = -1; // -1 + 0i
 std::complex<double> c2(1,2); // 1 + 2i
 std::cout << "c1, c2: " << c1 << c2 << endl;
 // if uncommented, the line below gives compilation error, because the std::complex class does not implement the comparison operator '>'
 // cout << "(-1,0) (complex) compares greater than (1,2) (complex)? " << comparesGreater(c1, c2) << endl; // for this call the compiler will choose the non-templated function!!

 // the call below will use the specialized non-templated function, and the result will not be correct.
 cout << "'a' (char) compares greater than 2.48 (float)? " << comparesGreater('a', 2.48) << endl; // for this call the compiler will choose the non-templated function!!

 return 0;
}
