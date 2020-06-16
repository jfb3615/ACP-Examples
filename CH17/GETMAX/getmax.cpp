
/*
 * Function templates overloading
 *
 */

#include <iostream>
#include <typeinfo>


int getMax(int a, int b) {
  std::cout << "[int,int] : ";
  return a > b ? a : b;
}

template <typename T> T getMax(T a, T b) {
  std::cout << " [template<" << typeid(T).name() << ">[template (T)] : ";
  if (a > b)
    return a;
  return b;
}




int main(int argc, char *argv[]) {

 // this will use the ordinary function, which uses two integer arguments
 std::cout << "getMax(5, 11): " << getMax( 5, 11 ) << std::endl;

 // these two calls will use the function template, which uses two arguments of the same type
 std::cout << "getMax(1.5, 2.2): " << getMax<>( 1.5, 2.2 ) << std::endl;

 // ...but this will not use the template,
 // because only the ordinary function accepts two different arguments,
 // when their types can be converted into 'int', giving a WRONG result!
 std::cout << "getMax(1, 1.5): " << getMax( 1, 1.5 ) << std::endl; // wrong result!!

 // the line below, if uncommented, will give a compilation error,
 // because the compiler cannot find a match with the available function definitions
 //
 //std::cout << "getMax<>(1, 1.5): " << getMax<>( 1, 1.5 ) << std::endl;

 // ...to fix the above problem, we can specify the type
 // this will tell the compiler to convert both the arguments to a double
 // before looking for a matching candidate, which will be the template
 std::cout << "getMax<double>(1, 1.5): " << getMax<double>( 1, 1.5 ) << std::endl;

 return 0;
}
