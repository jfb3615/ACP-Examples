#ifndef _CLUSTER_H_
#define _CLUSTER_H_
#include <iostream>
#include <set>
struct Index   {
  Index(unsigned int i, unsigned int j) :i(i),j(j){}
  bool operator==(const Index &right) const { return right.i==i && right.j==j;} 
  bool operator!=(const Index &right) const { return right.i!=i || right.j!=j;} 
  bool operator< (const Index &right) const { return i<right.i || (i==right.i && j<right.j);}
  bool neighbors (const Index &right) const { 
    return 
      (i==right.i && abs(int(j)-int(right.j))==1) ||
      (j==right.j && abs(int(i)-int(right.i))==1)  ;
  }
  unsigned int i;
  unsigned int j;
};

class Cluster {
  
public:
  
  bool percolates(unsigned int gridSize) {
    if (!indexSet.empty()) {
      if ((*indexSet.begin()).i==0 && (*indexSet.rbegin()).i==gridSize-1) return true;
    }
    return false;
  }

  bool contains(const Index & index) const {
    return indexSet.find(index)!=indexSet.end();
  }
  
  void add(const Index & index) {
    indexSet.insert(index);
  }
  
  void add(const Cluster & cluster) {
    //copy(cluster.indexSet.begin(), cluster.indexSet.end(),inserter(indexSet, indexSet.begin() ) );
    auto c=cluster.indexSet.begin();
    while (c!=cluster.indexSet.end()) {
      indexSet.insert(*c);
      c++;
    }
  }
  
  unsigned int size() const {
    return indexSet.size();
  }
  
 private:
  std::set<Index> indexSet;

  friend std::ostream & operator << (std::ostream &, const Cluster &);
};

class ClusterSet {
public:
  
  void add(const Cluster & cluster) {
    _cluster.push_back(cluster);
  }
  
  const Cluster & operator()(unsigned int i) const {
    return _cluster[i];
  }
  
  Cluster & operator()(unsigned int i) {
    return _cluster[i];
  }
  
  unsigned int clusterIndex (Index index) const {
    for (unsigned int i=0;i<_cluster.size();i++) {
      if (_cluster[i].contains(index)) return i;
    }
    return _cluster.size();
  }
  
  unsigned int size() const {
    return _cluster.size();
  }
  
  void erase(unsigned int i) {
    _cluster.erase(_cluster.begin()+i);
  }
  
private:
  
  std::vector<Cluster> _cluster;
  
};

inline std::ostream & operator << (std::ostream & o, const Index & I) {
  return o << I.i << "," << I.j;
}

inline std::ostream & operator << (std::ostream & o, const Cluster & c) {
  auto i=c.indexSet.begin();
  while (i!=c.indexSet.end()) {
    o << *i << " ";
    i++;
  }
  o << std::endl;
  return o;
}

inline std::ostream & operator << (std::ostream & o, const ClusterSet & cs) {
  for (unsigned int i=0;i<cs.size();i++) {
    o << cs(i) << std::endl;
  }
  return o;
}


#endif
