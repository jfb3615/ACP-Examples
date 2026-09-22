#ifndef _RATIONAL_H_
#define _RATIONAL_H_
#include <cmath>
#include <limits>
class Rational {
  //
  // Here is an incomplete class representing rational
  // numbers.  It can be constructed from numerator and
  // denominator or with a double precision number. 
  //
 
 public:

  // Default Constructor.  Compiler Generated. 
  Rational()=default;
  
  // Construct from numerator and denominator;
  Rational(int num, int den):m_num(num),m_den(den){}
  
  // Construct from a double:
  Rational(double d):m_num(1),m_den(1){
    static int digits=std::numeric_limits<double>::digits;
    m_num=(d*=(1<<(digits-1)));
    m_den*=(1<<(digits-1));
  }
  
  Rational (const Rational & source): 
    m_num{source.m_num},
    m_den{source.m_den}
  {}
  
  // accessors:
  long int num() const { return m_num;}
  long int den() const { return m_den;}
  
  // compute the value as a double precision number:
  double value() const { return double(m_num)/double(m_den);}
  
  // cast to double 
  operator double() const { return value();}
  
private:

  long int m_num{0l};  // store the numerator
  long int m_den{1l};  // store the denominator
  
};

#endif
