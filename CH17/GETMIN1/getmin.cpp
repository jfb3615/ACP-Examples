
/*
 * return parameters, different argument types
 *
 */

#include <iostream>
#include <typeinfo>


int getMin(int a, int b) { std::cout << "[int,int] : "; return a < b ? a : b; };

template <typename T> T getMin(T a, T b) {
  std::cout << " [template<" << typeid(T).name() << ">[template (T)] : ";
  if (a < b)
    return a;
  return b;
}

template <typename Ta, typename Tb> Ta getMin(Ta a, Tb b) {
  std::cout << " [template<" << typeid(Ta).name() << "," << typeid(Tb).name() << ">(Ta,Tb)] : ";
  if (a < b)
    return a;
  return b;
}

template <typename Ta, typename Tb, typename Tr> Ta getMin(Ta a, Tb b) {
  std::cout << " [template<" << typeid(Ta).name() << "," << typeid(Tb).name() << "," << typeid(Tr).name() << ">(Ta,Tb,Tr)] : ";
  Tr ret;
  if (a < b)
    ret = a;
  ret = b;
  return ret;
}



int main(int , char **) {

 // this will use the ordinary function, which uses two integer arguments
 std::cout << "getMin(5, 11): " << getMin( 5, 11 ) << std::endl;
 // these two calls will use the first function template, which uses two arguments of the same type
 std::cout << "getMin(2.9, 17.2): " << getMin( 2.9, 17.2 ) << std::endl;
 std::cout << "getMin(1E-29, 1E38): " << getMin( 1E-290, 1E308 ) << std::endl;
 // this will use the second function template, which uses two arguments of different type
 std::cout << "getMin(3, 1.2): " << getMin( 3, 1.2 ) << std::endl;

 // explicit type declaration for parameter subsititution
 std::cout << "getMin(2.9, 17.2): " << getMin( 2.9, 17.2 ) << std::endl;
 std::cout << "getMin<float,float>(2.9, 17.2): " << getMin<float,float>( 2.9, 17.2 ) << std::endl;
 std::cout << "getMin<float>(2.9, 17.2): " << getMin<float>( 2.9, 17.2 ) << std::endl;
 std::cout << "getMin<>(2, 5): " << getMin<>( 2, 5 ) << std::endl;
 std::cout << "getMin<>(2.9, 17.2): " << getMin<>( 2.9, 17.2 ) << std::endl;

 std::cout << "getMin<double,double,float>(2.9, 17.2): " << getMin<double,double,float>( 2.9, 17.2 ) << std::endl;

 return 0;
}
