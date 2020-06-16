#include "HeliumObjective.h"
#include "QatGenericFunctions/Variable.h"
#include "QatGenericFunctions/Exp.h"
using namespace Eigen;
using namespace std;
using namespace Genfun;
HeliumObjective::HeliumObjective(unsigned int N, const HeliumObjective *prevObjective):_N(N),alpha(N),c(N),prevObjective(prevObjective) {
  for (unsigned int i=0;i<_N;i++) {
    {
      ostringstream stream;
      stream << "Alpha" << i; 
      //                       name          default    min  max
      alpha[i] = new Parameter(stream.str(), 1/(i+1.0), 0,   10);
    }
    {
      ostringstream stream;
      stream << "C" << i; 
      //                       name          default    min  max
      c[i] = new Parameter(stream.str(), 1/(i+1.0), 0,   10);
    }
  }
}

HeliumObjective::~HeliumObjective () {
  for (unsigned int i=0;i<_N;i++) {
    delete alpha[i];
    delete c[i];
  }
}

const Parameter * HeliumObjective::getAlpha(unsigned int i) const  { 
  return alpha[i];
}    

const Parameter * HeliumObjective::getC(unsigned int i) const { 
  return c[i];
}    

Parameter * HeliumObjective::getAlpha(unsigned int i) { 
  return alpha[i];
}    

Parameter * HeliumObjective::getC(unsigned int i) { 
  return c[i];
}    

unsigned int HeliumObjective::getNumComponents() const {
  return _N;
}

void HeliumObjective::normalize() {
  double normFactor=norm();
  for (unsigned int i=0;i<_N;i++) {
    c[i]->setValue(c[i]->getValue()/normFactor);
  }
}

double HeliumObjective::norm() const {
  MatrixXd S(_N,_N);
  VectorXd C(_N);
  for (unsigned int i=0;i<_N;i++) {
    C(i) = c[i]->getValue();
      for (unsigned int j=0;j<_N;j++) {
	S(i,j) = pow(M_PI/(alpha[i]->getValue()+alpha[j]->getValue()), 3.0/2.0);
      }
  }
  return sqrt((C.transpose()*S*C)(0,0));
}

double HeliumObjective::operator() () const {
  MatrixXd S(_N,_N), T(_N,_N), A(_N,_N), F=MatrixXd::Zero(_N,_N),h=MatrixXd::Zero(_N,_N);
  VectorXd C(_N);
  for (unsigned int i=0;i<_N;i++) {
    C(i) = c[i]->getValue();
    for (unsigned int j=0;j<_N;j++) {
      S(i,j) = pow(M_PI/(alpha[i]->getValue()+alpha[j]->getValue()), 3.0/2.0);
      T(i,j) = 3.0*pow(M_PI,3.0/2.0)*alpha[i]->getValue()*alpha[j]->getValue()/pow(alpha[i]->getValue()+alpha[j]->getValue(),5.0/2.0);
      A(i,j) = -4*M_PI/(alpha[i]->getValue()+alpha[j]->getValue());
      if (prevObjective) {
	unsigned int M=prevObjective->getNumComponents();
	MatrixXd QQ(M,M);
	VectorXd CC(M);
	for (unsigned int k=0;k<M;k++) {
	  CC(k) = prevObjective->getC(k)->getValue();
	  for (unsigned int l=0;l<M;l++) {
	    QQ(k,l) = 2.0*pow(M_PI,5.0/2.0)
	      /(alpha[i]->getValue() + alpha[j]->getValue())
	      /(prevObjective->getAlpha(k)->getValue() + prevObjective->getAlpha(l)->getValue())
	      /sqrt(alpha[i]->getValue() + alpha[j]->getValue() + prevObjective->getAlpha(k)->getValue() + prevObjective->getAlpha(l)->getValue());
	  }
	}
	F(i,j) = (CC.transpose()*QQ*CC)(0,0);
      }
    }
  }
  h=T+A+F;
  return (C.transpose()*h*C)(0,0)/(C.transpose()*S*C)(0,0);
}

HeliumObjective::Wavefunction HeliumObjective::wavefunction() const {
  Wavefunction wf;
  Variable R;
  double NORM=norm();
  for (unsigned int i=0;i<_N;i++) {
    wf.accumulate((c[i]->getValue()/NORM) * Exp()(-alpha[i]->getValue()*R*R));
  }
  return wf;
}
double HeliumObjective::energy () const {
  MatrixXd S(_N,_N), T(_N,_N), A(_N,_N), F=MatrixXd::Zero(_N,_N),h=MatrixXd::Zero(_N,_N);
  VectorXd C(_N);
  for (unsigned int i=0;i<_N;i++) {
    C(i) = c[i]->getValue();
    for (unsigned int j=0;j<_N;j++) {
      S(i,j) = pow(M_PI/(alpha[i]->getValue()+alpha[j]->getValue()), 3.0/2.0);
      T(i,j) = 3.0*pow(M_PI,3.0/2.0)*alpha[i]->getValue()*alpha[j]->getValue()/pow(alpha[i]->getValue()+alpha[j]->getValue(),5.0/2.0);
      A(i,j) = -4*M_PI/(alpha[i]->getValue()+alpha[j]->getValue());
      if (prevObjective) {
	unsigned int M=prevObjective->getNumComponents();
	MatrixXd QQ(M,M);
	VectorXd CC(M);
	for (unsigned int k=0;k<M;k++) {
	  CC(k) = prevObjective->getC(k)->getValue();
	  for (unsigned int l=0;l<M;l++) {
	    QQ(k,l) = 2.0*pow(M_PI,5.0/2.0)
	      /(alpha[i]->getValue() + alpha[j]->getValue())
	      /(prevObjective->getAlpha(k)->getValue() + prevObjective->getAlpha(l)->getValue())
	      /sqrt(alpha[i]->getValue() + alpha[j]->getValue() + prevObjective->getAlpha(k)->getValue() + prevObjective->getAlpha(l)->getValue());
	  }
	}
	F(i,j) = (CC.transpose()*QQ*CC)(0,0);
      }
    }
  }
  h=2*(T+A)+F;
  return (C.transpose()*h*C)(0,0)/(C.transpose()*S*C)(0,0);
}
