#include <limits>
#include <iostream>
#include <iomanip>
int main() {
  using ShortIntLimits    = std::numeric_limits<short int>;
  using IntLimits         = std::numeric_limits<int>;
  using LongIntLimits     = std::numeric_limits<long int>;
  using LongLongIntLimits = std::numeric_limits<long long int>;
  using FloatLimits       = std::numeric_limits<float>;
  using DoubleLimits      = std::numeric_limits<double>;
  using LongDoubleLimits  = std::numeric_limits<long double>;



  { short int d;      std::cout << "On this computer a short int has "      << sizeof(d)*8 << " bits." << std::endl;}
  { int d;            std::cout << "On this computer an int has "           << sizeof(d)*8 << " bits." << std::endl;}
  { long int d;       std::cout << "On this computer a long int has "       << sizeof(d)*8 << " bits." << std::endl;}
  { long long int d;  std::cout << "On this computer a long long int has "  << sizeof(d)*8 << " bits." << std::endl;}

  { float d;          std::cout << "On this computer a float has "          << sizeof(d)*8 << " bits." << std::endl;}
  { double d;         std::cout << "On this computer a double has "         << sizeof(d)*8 << " bits." << std::endl;}
  { long double d;    std::cout << "On this computer a long double has "    << sizeof(d)*8 << " bits." << std::endl;}

  std::cout << "On this computer: \n"; 

  std::cout << " A short int can have a maximum value of "     << ShortIntLimits::max()    << std::endl;
  std::cout << " An int can have a maximum value of "          << IntLimits::max()         << std::endl;
  std::cout << " A long int can have a maximum value of "      << LongIntLimits::max()     << std::endl;
  std::cout << " A long long int can have a maximum value of " << LongLongIntLimits::max() << std::endl;
  
  std::cout << " An float can have a maximum value of "        << FloatLimits::max()         << std::endl;
  std::cout << " A double can have a maximum value of "        << DoubleLimits::max()        << std::endl;
  std::cout << " A long double can have a maximum value of "   << LongDoubleLimits::max()    << std::endl;

  std::cout << " Eps float has a precision of "         << FloatLimits::epsilon()         << std::endl;
  std::cout << " Eps double has a precision of "        << DoubleLimits::epsilon()        << std::endl;
  std::cout << " Eps long double has a precision of "   << LongDoubleLimits::epsilon()    << std::endl;

  return 0;
}
