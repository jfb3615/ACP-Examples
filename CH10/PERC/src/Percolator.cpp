#include "Percolator.h"
#include <algorithm>
#include <iostream>
Percolator::Percolator(unsigned int NX, unsigned int NY, unsigned int NZ, double p):c(new Clockwork()) 
{

  c->NX=NX;
  c->NY=NY;
  c->NZ=NZ;
  c->p=p;
  c->array=new int[NX*NY*NZ];
  std::fill(c->array, c->array+NX*NY*NZ, 0);
  c->clusterArray=new int[NX*NY*NZ];
  std::fill(c->clusterArray, c->clusterArray+NX*NY*NZ, -1);
  next();
}

Percolator::~Percolator() {
  delete [] c->array;
  delete [] c->clusterArray;
  delete c;
} 

// Goto next state
void Percolator::next() {

  std::fill(c->clusterArray, c->clusterArray+c->NX*c->NY*c->NZ, -1);

  
  if (c->p==0) {
    std::fill(c->array, c->array+c->NX*c->NY*c->NZ, 0);
    return;
  }
  std::bernoulli_distribution flip(c->p);
  for (unsigned int i=0;i<c->NX;i++) {
    for (unsigned int j=0;j<c->NY;j++) {
      for (unsigned int k=0;k<c->NZ;k++) {
	unsigned int index = c->index(i,j,k);
	c->array[index]=flip(c->engine);
      }
    }
  }
  return;
}

void Percolator::cluster() {
  
  std::vector<unsigned int> serialVector;
  struct Neighbor{
    int di;
    int dj;
    int dk;
  };
  
  class SetContains {
    
  public:
    
    bool operator () (const std::set<unsigned int> &snSet) const {
      return snSet.find(serial)!=snSet.end();
    }
    
    SetContains(int serial):serial(serial){}
    
  private:
    unsigned int serial;
  };
  
  std::vector<std::set <unsigned int> > snSet; 
  static const std::vector<Neighbor> nb={{1,0,0}, {0, 1, 0}, {0,0,1}};
  for (unsigned int i=0;i<c->NX;i++) {
    for (unsigned int j=0;j<c->NY;j++) {
      for (unsigned int k=0;k<c->NZ;k++) {
	if (isOccupied(i,j,k)) {
	  for (unsigned int d=0;d<nb.size();d++) {
	    unsigned int ni=i + nb[d].di; if (ni> c->NX-1) continue;
	    unsigned int nj=j + nb[d].dj; if (nj> c->NY-1) continue;
	    unsigned int nk=k + nb[d].dk; if (nk> c->NZ-1) continue;
	    if (isOccupied(ni,nj,nk)) {
	      int & c0=getClusterId(i,j,k);
	      int & c1=getClusterId(ni,nj,nk);
	      if (c0==-1 && c1==-1) {
		// A cluster is born .com/with two nodes.
		c0=serialVector.size();
		c1=serialVector.size();
		std::set<unsigned int> sn;
		sn.insert(serialVector.size());
		snSet.push_back(sn);       // Keep this
		serialVector.push_back(2); // order please
	      }
	      else if (c0==-1 && c1!=-1) {
		c0=c1;
		serialVector[c1]++;
	      }
	      else if (c0!=-1 && c1==-1) {
		c1=c0;
		serialVector[c0]++;
	      }
	      else if (c0<c1) {
		auto sPtr = std::find_if(snSet.begin(),snSet.end(), SetContains(c0));
		auto pPtr = std::find_if(snSet.begin(),snSet.end(), SetContains(c1));
		if (sPtr!=pPtr) {
		  std::copy((*pPtr).begin(),(*pPtr).end(), std::inserter(*sPtr,(*sPtr).begin()));
		  (*pPtr).clear();
		}
		c1=c0;
		serialVector[c0]++;
		serialVector[c1]--;
	      }
	      else if (c0>c1) {
		auto sPtr = std::find_if(snSet.begin(),snSet.end(), SetContains(c1));
		auto pPtr = std::find_if(snSet.begin(),snSet.end(), SetContains(c0));
		if (sPtr!=pPtr) {
		  std::copy((*pPtr).begin(),(*pPtr).end(), std::inserter(*sPtr,(*sPtr).begin()));
		  (*pPtr).clear();
		}
		c0=c1;
		serialVector[c1]++;
		serialVector[c0]--;
	      }
	    }
	  }
	}
      }
    }
  }
  c->finalClusterSize.clear();
  c->finalClusterContents.clear();
  c->finalClusterColor=serialVector;
  
  // Loop over clusters:
  for (auto ptr0=snSet.begin();ptr0!=snSet.end();ptr0++) {
    unsigned int color = (c->engine()&0x00FFFFFF)|0XFF000000;
    if (!(*ptr0).empty()) {
      unsigned int count=0;
      // loop over cluster segments.
      for (auto ptr1=(*ptr0).begin();ptr1!=(*ptr0).end();ptr1++) {
	count += serialVector[*ptr1];
	c->finalClusterColor[*ptr1]=color;
      }
      c->finalClusterSize.push_back(count);
    }
  }
}
