#ifndef _PERCOLATOR_H_
#define _PERCOLATOR_H_
#include <vector>
#include <set>
class Percolator {
 public:

  // Constructor
  Percolator(unsigned int NX=100, unsigned int NY=100, unsigned int NZ=100, double p=0.5);

  // Destructor
  ~Percolator();

  // Get dimensions of this model:
  unsigned int NX() const;
  unsigned int NY() const;
  unsigned int NZ() const;

  // Access & modification of the occupation probability
  double & p();
  const double &p() const;

  // Goto next state
  void next();
  
  // Get the state of each dipole:
  bool isOccupied(unsigned int i, unsigned int j, unsigned int k) const;

  // Cluster the data:
  void cluster();

  // Get the cluster number for a particular node, -1 if unclustered
  const int & getClusterId(unsigned int i, unsigned int j, unsigned int k) const;
  int & getClusterId(unsigned int i, unsigned int j, unsigned int k);

  // Get the number of clusters.
  unsigned int getNumClusters() const;

  // Get the size of each cluster.
  unsigned int getClusterSize(int clusterId) const; 
  
  // Get the contents of each cluster. 
  const std::set<unsigned int> & getClusterContents(int i)  const;

  // Get the color of a cluster serial number
  unsigned int getColor(int serialNumber) const;

 private:

  Percolator(const Percolator &) = delete;
  Percolator & operator =(const Percolator &) = delete;

  class Clockwork;
  Clockwork *c;
};
#include "Percolator.icc"
#endif

