
/*
 * return parameters, different argument types
 *
 */

#include <iostream>


template <typename T> T getMin(T a, T b) {
  if (a < b)
    return a;
  return b;
}

template <typename Ta, typename Tb> Ta getMin(Ta a, Tb b) {
  if (a < b)
    return a;
  return b;
}



int main(int , char * *) {

 // this will use the first function template, that uses two arguments of the same type
 std::cout << "getMin(2.8, 1.5): " << getMin( 2.8, 1.5 ) << std::endl; // --> 1.5

 // this will use the second function template, that uses two arguments of different type
 std::cout << "getMin(2, 7.2): " << getMin( 2, 7.2 ) << std::endl; // --> 2

 return 0;
}
