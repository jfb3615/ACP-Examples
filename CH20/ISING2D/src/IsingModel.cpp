#include "IsingModel.h"
IsingModel::IsingModel(unsigned int NX, unsigned int NY, double tau):c(new Clockwork()) 
{

  static const int eps=-1; // Ferromagnet. Not Antiferromagnet.

  c->NX=NX;
  c->NY=NY;
  c->tau=tau;
  c->array=new int[NX*NY];
  c->m=0;
  c->u=0.0;
  std::fill(c->array, c->array+NX*NY, 0);

  //
  // Step 1:  make a random assignment of spins
  //          compute the magnetization as you go.
  std::bernoulli_distribution flip;
  for (unsigned int i=0;i<NX;i++) {
    for (unsigned int j=0;j<NY;j++) {
      unsigned int index = c->index(i,j);
      if (flip(c->engine)) {
	c->m++;
	c->array[index]=1;
      }
      else {
	c->array[index]=-1;
	c->m--;
      }
      for (int ip=int(i-1);ip<=int(i+1);ip++) {
	int IP=ip;
	while (IP<0)    IP+=NX;
	while (IP>int(NX)-1) IP-=NX;
	for (int jp=int(j)-1;jp<=int(j)+1;jp++) {
	  int JP=jp;
	  while (JP<0)    JP+=NY;
	  while (JP>int(NY)-1) JP-=NY;
	  // This works because the energy is only counted when both
	  // spins in the pair are initialized.  Before that, one of
	  // the indices is zero and will give no contribution. 
	  c->u += eps*c->array[c->index(i,j)]*c->array[c->index(IP,JP)];
	}
      }
    }
  }
}

IsingModel::~IsingModel() {
  delete [] c->array;
  delete c;
}
// Goto next state
void IsingModel::next() {

  if (c->tau==0.0) return;

  // 
  // The following builds a cache if needed. The cache is emptied 
  // whenever the non-const accessor to the temperature is accessed,
  // which could signal a change in temperature and should invalidate
  // the cache.
  //
  if (c->cacheDE.empty()) {
    c->cacheDE.resize(5);
    for (int i=0;i<5;i++) {
      c->cacheDE[i]=exp(-2*i/c->tau);
    }
  }


  static const int eps=-1; // Ferromagnet. Not Antiferromagnet.

  std::uniform_int_distribution<unsigned int> chooseI(0,c->NX-1);
  std::uniform_int_distribution<unsigned int> chooseJ(0,c->NY-1);
  std::uniform_real_distribution<double>      flat;
  unsigned int i=chooseI(c->engine);
  unsigned int j=chooseJ(c->engine);
    
  int dE=0;
  for (int di=-1;di<=1;di++) {
    int IP=i+di;
    while (IP<0)    IP+=c->NX;
    while (IP>int(c->NX)-1) IP-=c->NX;
    for (int dj=-1;dj<=1;dj++) {
      if ((di==0 || dj==0) && (di!=0 || dj!=0)) {
	int JP=j+dj;
	while (JP<0)    JP+=c->NY;
	while (JP>int(c->NY)-1) JP-=c->NY;
	
	dE -= eps*c->array[c->index(i,j)]*c->array[c->index(IP,JP)];
      }
    }
  }
  
  //
  // Hastings-Metropolis in one line:
  //
  double uniform=flat(c->engine);
  bool accept =dE<=0 ? true: c->cacheDE[dE] > uniform ? true: false; 
  if (accept) {
    c->array[c->index(i,j)] *= -1;
    c->iPrev=i;
    c->jPrev=j;
    c->delta=-2*dE;
    c->u -= c->delta;
    c->m += 2*c->array[c->index(i,j)];
  }

}

