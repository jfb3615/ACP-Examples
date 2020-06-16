#include "Percolator.h"
#include <algorithm>
#include <iostream>
Percolator::Percolator(unsigned int NX, unsigned int NY, double p):c(new Clockwork()) 
{

  c->NX=NX;
  c->NY=NY;
  c->p=p;
  c->array=new int[NX*NY];
  std::fill(c->array, c->array+NX*NY, 0);
  c->clusterArray=new int[NX*NY];
  std::fill(c->clusterArray, c->clusterArray+NX*NY, -1);
  c->percolates=false;
  next();
}

Percolator::~Percolator() {
  delete [] c->array;
  delete [] c->clusterArray;
  delete c;
} 

// Goto next state
void Percolator::next() {

  std::fill(c->clusterArray, c->clusterArray+c->NX*c->NY, -1);

  
  if (c->p==0) {
    std::fill(c->array, c->array+c->NX*c->NY, 0);
    return;
  }
  std::bernoulli_distribution flip(c->p);
  for (unsigned int i=0;i<c->NX;i++) {
    for (unsigned int j=0;j<c->NY;j++) {
      unsigned int index = c->index(i,j);
      c->array[index]=flip(c->engine);
    }
  }
  return;
}

void Percolator::cluster() {
  
  std::vector<unsigned int> serialVector;
  struct Neighbor{
    int di;
    int dj;
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
  
  c->percolates=false;
  std::vector<std::set <unsigned int> > snSet; 
  static const std::vector<Neighbor> nb={{0,1}, {1, 0}};
  for (unsigned int i=0;i<c->NX;i++) {
    for (unsigned int j=0;j<c->NY;j++) {
      if (isOccupied(i,j)) {
	for (unsigned int d=0;d<nb.size();d++) {
	  unsigned int ni=i + nb[d].di; if (ni> c->NX-1) continue;
	  unsigned int nj=j + nb[d].dj; if (nj> c->NY-1) continue;
	  if (isOccupied(ni,nj)) {
	    int & c0=getClusterId(i,j);
	    int & c1=getClusterId(ni,nj);
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
  //
  // Check for percolation:
  //
  std::set<int> left, right;
  for (unsigned int i=0;i<c->NX;i++) {
    left.insert(getClusterId(i,0));
    right.insert(getClusterId(i,c->NY-1));
  }
  for (auto l=left.begin();l!=left.end();l++) {
    for (unsigned int i=0;i<snSet.size();i++) {
      if (snSet[i].find(*l)!=snSet[i].end()) {
	for (auto r=right.begin();r!=right.end();r++) {
	  if (snSet[i].find(*r)!=snSet[i].end()) {
	    c->percolates=true;
	    return;
	  }
	}
      }
    }
  }
}
