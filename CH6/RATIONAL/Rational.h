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
  // Construct from numerator and denominator;
 Rational(int num, int den):_num(num),_den(den){}
  
  // Construct from a double:
 Rational(double d):_num(1),_den(1){
    static int digits=std::numeric_limits<double>::digits;
    _num=(d*=(1<<(digits-1)));
    _den*=(1<<(digits-1));
  }

  Rational (const Rational & source) {
    _num=source._num;
    _den =source._den;
  }
  // accessors:
  long int num() const { return _num;}
  long int den() const { return _den;}

  // compute the value as a double precision number:
  double value() const { return double(_num)/double(_den);}

  // cast to double 
  operator double() const { return value();}
  
 private:

  long int _num;  // store the numerator
  long int _den;  // store the denominator

};

#endif
