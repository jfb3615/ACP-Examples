#include "Circuit/Circuit.h"
#include "CircuitIntegrator.h"
#include "QatGenericFunctions/AbsFunction.h"
#include "DefaultCircuitStepper.h"
#include <set>
#include <memory>
#include <vector>

namespace circuit {


  class CircuitIntegrator::CircuitFunction : public Genfun::AbsFunction {
    
    FUNCTION_OBJECT_DEF(CircuitFunction)
    
    public:
    
    // Constructor
    CircuitFunction(std::shared_ptr<CircuitIntegrator::CircuitData>  & data, unsigned int index);
    
    // Destructor
    virtual ~CircuitFunction();
    
    // Copy constructor
    CircuitFunction(const CircuitFunction &right);
    
    // Retreive function value
    virtual double operator ()(double argument) const;
    virtual double operator ()(const Genfun::Argument & a) const {return operator() (a[0]);}
    
  private:
    
    // It is illegal to assign a CircuitFunction
    const CircuitFunction & operator=(const CircuitFunction &right);
    
    // The shared data:
    std::shared_ptr<CircuitData>       _data{};
    const  unsigned int                _index;
    
  };
  

  void CircuitIntegrator::CircuitData::lock() {
  if (!_locked) {
    unsigned int size = _diffEqn.size();
    for (size_t i=0;i<size;i++) {
      if (!(_diffEqn[i]->dimensionality()==size)) throw std::runtime_error("Runtime error in CircuitIntegrator");
    }
    _locked=true;
  }
}

  FUNCTION_OBJECT_IMP(CircuitIntegrator::CircuitFunction)

  CircuitIntegrator::CircuitFunction::CircuitFunction(std::shared_ptr<CircuitData> & data, unsigned int index)
  :_data(data),
  _index(index)
  {
  }

  CircuitIntegrator::CircuitFunction::~CircuitFunction() 
  {
  }

  CircuitIntegrator::CircuitFunction::CircuitFunction(const CircuitFunction & right)
    :AbsFunction(right),
     _data(right._data),
     _index(right._index)
  {
  }


  double CircuitIntegrator::CircuitFunction::operator() (double t) const {
    if (t<0) return 0;
    if (!_data->_locked) _data->lock();
    
    
    // If the cache is empty, make an entry for t=0;
    size_t nvar = _data->_startingValues.size();
    if (_data->_fx.empty()) {
      CircuitData::Data d(nvar);
      d.time=0;
      Genfun::Argument arg(nvar);
      for (size_t f=0;f<nvar;f++) {
	d.variable[f]=_data->_startingValues[f];
	arg[f]=d.variable[f];
      }
      _data->_fx.insert(d);
    }
    
    if (t==0) return (*_data->_fx.begin()).variable[_index];
    
    CircuitData::Data dt(nvar);
    dt.time=t;
    std::set<CircuitData::Data>::iterator l =_data->_fx.lower_bound(dt);
    
    // We did find an exact value (measure 0), just return it. 
    if (l!=_data->_fx.end() && (*l).time==t) {
      return (*l).variable[_index];
    }
    
    else {
      std::set<CircuitData::Data>::iterator u =_data->_fx.upper_bound(dt);
      
      while (u==_data->_fx.end()) {
	u--;
	CircuitData::Data newData(nvar);;
	_data->_stepper->step(_data,*u, newData, 0);
	_data->_fx.insert(l,newData);
	if (newData.time==t) return newData.variable[_index];
	u = _data->_fx.upper_bound(dt);
      }
      u--;
      _data->_stepper->step(_data,*u, dt, t);
      return dt.variable[_index];
    }
  }
  
  CircuitIntegrator::CircuitIntegrator(const Circuit & circuit) {
    size_t N=countLoops(circuit);
    std::cout << "HELLO from circuit integrator......" << std::endl;
    std::cout << "Creating the data cache............" << std::endl;
    _data=std::make_unique<CircuitData>();
    std::cout << "Initializing the starting values..." << std::endl;
    _data->_startingValues=Eigen::VectorXd::Zero(3*N);
    std::cout << "Creating the stepper..............." << std::endl;
    _data->_stepper=std::make_unique<DefaultCircuitStepper>(circuit); 
    for (size_t i=0;i<3*N;i++) {
      _fcn.emplace_back(new CircuitFunction(_data,i));
    }
  }

  int  CircuitIntegrator::getNumIndependentCurrents() const {
    return _fcn.size();
  }

  const Genfun::AbsFunction *CircuitIntegrator::getIndependentCurrent(int i) const {
    return _fcn[i];
  }

}

