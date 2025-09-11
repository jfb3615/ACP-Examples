#ifndef _Interpolator_h_
#define _Interpolator_h_ 
#include <vector>
#include <stdexcept>
#include <memory>
template <typename T> class Times;
template <typename T> class Plus;
template <typename T> class Interpolator;

template<typename T>
class TFInterface {
public:

  virtual T operator ()(double argument) const=0;

  virtual TFInterface<T> *clone() const =0;

  virtual ~TFInterface()=0;
};

template<typename T>
class GenInterpolator:public TFInterface<T> {
public:

  GenInterpolator();

  GenInterpolator(const Interpolator<T> & interp);
  
  GenInterpolator(const TFInterface<T> &op);

  GenInterpolator(const GenInterpolator<T> &op);
  
  virtual T operator ()(double argument) const;

  virtual GenInterpolator<T> *clone() const;

  virtual ~GenInterpolator();

  GenInterpolator<T> & operator=(const GenInterpolator<T> & right);


private:

  std::shared_ptr<const TFInterface<T>> _op{nullptr};
};



template<typename T> 
class Interpolator:public TFInterface<T> {
  
public:
  
  // Constructor
  Interpolator();
  
  // Copy constructor
  Interpolator(const Interpolator &right);
  
  // Destructor:
  virtual ~Interpolator();
  
  // Retreive function value
  virtual T operator ()(double argument) const;
  
  // Add a new point to the set.
  void addPoint(double x, const T & y);
  
  // Get the range:
  void getRange(double & min, double & max) const;

  // Clone:
  virtual Interpolator *clone() const;

  // Number of points
  unsigned int getNumPoints() const;

  // Empty?
  bool empty() const;
  
private:
  
  // It is illegal to assig
  const Interpolator & operator=(const Interpolator &)=delete;
  
  std::vector<std::pair<double,T> > xPoints;
};


template <typename T>
class Times:public TFInterface<T>  {
  
  
public:
  
  using TFInterface<T>::operator();
  
  // Constructor
  Times(double constant, std::shared_ptr<const TFInterface<T> >arg);
  
  // Copy constructor
  Times(const Times &right);
  
  // Destructor
  virtual ~Times();
  
  // Retreive function value
  virtual T operator ()(double argument) const;

  // Clone:
  virtual Times<T> * clone() const;
  
  
private:
  
  // It is illegal to assign a Times
  const Times & operator=(const Times &right);
  
  double             _constant;
  std::shared_ptr<const TFInterface<T> >_arg;
};


template<typename T> 
Times<T> operator * (double c, const TFInterface<T> &op);

//==================================================


template <typename T>
class Plus:public TFInterface<T>  {
  
  public:
  
  using TFInterface<T>::operator();
  
  // Constructor
  Plus(std::shared_ptr<const TFInterface<T>>arg1, std::shared_ptr<const TFInterface<T> > arg2);
  
  // Copy constructor
  Plus(const Plus &right);
  
  // Destructor
  virtual ~Plus();
  
  // Retreive function value
  virtual T operator ()(double argument) const;

  // Clone:
  virtual Plus<T> * clone() const;
  
  
private:
  
  // It is illegal to assign a Plus
  const Plus & operator=(const Plus &right);
  
  std::shared_ptr<const TFInterface<T>>_arg1;
  std::shared_ptr<const TFInterface<T>>_arg2;
  

};

template <typename T>
class Minus:public TFInterface<T>  {
  
  
public:
  
  using TFInterface<T>::operator();
  
  // Constructor
  Minus(std::shared_ptr<TFInterface<T>>arg1, std::shared_ptr<const TFInterface<T> > arg2);
  
  // Copy constructor
  Minus(const Minus &right);
  
  // Destructor
  virtual ~Minus();
  
  // Retreive function value
  virtual T operator ()(double argument) const;

  // Clone:
  virtual Minus<T> * clone() const;
  
  
private:
  
  // It is illegal to assign a Minus
  const Minus & operator=(const Minus &right);
  
  std::shared_ptr<const TFInterface<T>>_arg1;
  std::shared_ptr<const TFInterface<T>>_arg2;
};


template<typename T> 
inline Minus<T> operator - (const TFInterface<T>&op1, const TFInterface<T> &op2);

template<typename T> 
inline Plus<T> operator + (const TFInterface<T>&op1, const TFInterface<T> &op2);

#include "Interpolator.icc"

#endif
