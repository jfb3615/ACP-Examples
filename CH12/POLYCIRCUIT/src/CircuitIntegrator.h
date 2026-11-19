#pragma once
#include "Circuit/CircuitElement.h"
#include "Circuit/Circuit.h"
#include "Eigen/Dense"
#include <memory>
#include <set>
namespace circuit {

   class CircuitIntegrator {

  public:

     // Some helper classes:
     class CircuitFunction;
     class CircuitData;
     class CircuitStepper;
     
     // Constructor
     CircuitIntegrator(const Circuit & circuit);
     
     // Destructor
     ~CircuitIntegrator()=default;
     
     // Move assignment
     CircuitIntegrator & operator=(CircuitIntegrator && right)=default;
     
     // Move constructor
     CircuitIntegrator(CircuitIntegrator && right)=default;
     
     // Get back a function. 
     int  getNumIndependentCurrents() const;
     const Genfun::AbsFunction *getIndependentCurrent(int i) const;
     const Genfun::AbsFunction *getCurrent(const CircuitElement *element) const;
     
   private:
     
     // It is illegal to assign an CircuitIntegrator
     const CircuitIntegrator & operator=(const CircuitIntegrator &right)=delete;
     
     // It is illegal to copy an CircuitIntegrator
     CircuitIntegrator(const CircuitIntegrator &right)=delete;
     
     // Here is the data, it belongs to the integrator and to the
     // functions, and is reference counted:
     std::shared_ptr<CircuitIntegrator::CircuitData>                  _data;
     
     // Here are the functions:
     std::vector<const CircuitIntegrator::CircuitFunction *>          _fcn;
     
     // Here is the stepper;
     std::shared_ptr<CircuitIntegrator::CircuitStepper>               _stepper;
     
  };

   class CircuitIntegrator::CircuitData  {
     
     
   public:
     
     // Information about solution at each mesh point.
     struct Data{
       
       std::vector<double>         variable;             // Solution
    
       double time;                                      // time
       
     Data(int size): variable(size), time(0) {} 
       bool operator <  (const Data & right) const { return time < right.time; }
       bool operator == (const Data & right) const { return time==right.time; } 
     };
     
     void lock();
     
     std::vector<std::unique_ptr<const Genfun::AbsFunction>>   _diffEqn{};
     Eigen::VectorXd                                           _startingValues{};
     std::set<Data >                                           _fx{};
     bool                                                      _locked{false};
     std::unique_ptr<const CircuitIntegrator::CircuitStepper>  _stepper{};
     
   private:
     
   };
   
   // An abstract base class for steppers:
   class CircuitIntegrator::CircuitStepper {
   public:
     
     virtual ~CircuitStepper()=default;
     virtual void step (const std::shared_ptr<CircuitIntegrator::CircuitData>  & data, 
			const CircuitIntegrator::CircuitData::Data             & sdata, 
			CircuitIntegrator::CircuitData::Data                   & ddata, 
			double timeLimit=0) const =0;
   };

}
