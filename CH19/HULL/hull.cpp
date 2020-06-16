/*

hull.cpp

Jarvis March algorithm for the convex hull

*/

#include <iostream>
#include <fstream>
#include <cmath>
#include <random>
#include <vector>

using namespace std;

class TwoVect {

public:

  double x;
  double y;

  TwoVect () {
    x=0.0; y=0.0;
  };
  
  TwoVect (double x0, double y0) {
    x=x0; y=y0;
  };

  void setV(double x0, double y0) {
    x=x0; y=y0;
  }

  double mag() {
    return  sqrt(x*x + y*y);
  }

 double operator*(const TwoVect & other);

 TwoVect operator+(const TwoVect & other);

 TwoVect operator-(const TwoVect & other);

 friend std::ostream &operator<<(std::ostream &out, TwoVect v0) {
  out << v0.x << " " << v0.y;
  return out;
 }

}; // end of class TwoVect

TwoVect TwoVect::operator-(const TwoVect & other) {
  return TwoVect(x-other.x,y-other.y);
}

TwoVect TwoVect::operator+(const TwoVect & other) {
  return TwoVect(x+other.x,y+other.y);
}

double TwoVect::operator*(const TwoVect & other) {
  // dot product
  return x*other.x + y*other.y;
}

TwoVect operator*(const double & lhs, const TwoVect & rhs) {
  TwoVect v0 = TwoVect(lhs*rhs.x,lhs*rhs.y);
  return v0;
}

TwoVect operator*(const TwoVect & lhs, const double & rhs) {
  TwoVect v0 = TwoVect(rhs*lhs.x,rhs*lhs.y);
  return v0;
}

TwoVect operator/(const TwoVect & lhs, const double & rhs) {
  TwoVect v0 = TwoVect(lhs.x/rhs,lhs.y/rhs);
  return v0;
}

// --------------------------------------------------------------------

typedef int node;
typedef int element;
TwoVect *rNode = NULL;               // vector of node coordinates


bool left(node n1, node n2, node n) {
  // return true if p is left of the line from p1 to p2
  if ( (rNode[n].x - rNode[n1].x)*(rNode[n2].y - rNode[n1].y) < (rNode[n].y - rNode[n1].y)*(rNode[n2].x - rNode[n1].x) ) return true;
  return false;
}


int main(void) {
  int seed;
  node N;

  ofstream ofs,ofs2;
  ofs.open("hull.dat");
  ofs2.open("hull2.dat");
  cout << " input number of nodes, seed " << endl;
  cin >> N >> seed;
  mt19937_64 mt(seed);
  uniform_real_distribution<double> dist(-1.0,1.0);
  vector<node> nHull;

  // set up nodes and determine the leftmost point.
  // probably best track the hull node numbers rather than the coords
  rNode = new TwoVect[N];
  node hullStart;
  vector<node> hullPoints;
  double hPt = 1000.0;
  for (node n=0;n<N;n++) {
    double x = dist(mt);
    double y = dist(mt);
    rNode[n].setV(x,y);
    ofs << rNode[n] << endl;
    if (x < hPt) {
      hPt = x;
      hullStart = n;
    }
  }
  cout << " leftmost point: " << rNode[hullStart] << " " << hullStart << endl;

  // Jarvis March
  node endPoint;
  node hullPt = hullStart;
  ofs2 << rNode[hullStart] << endl;
  node n=0;
  do {
   hullPoints.push_back (hullPt);
   endPoint = 0;
   for (node m=1;m<N;m++) {
      if ( (endPoint == hullPt) || left(hullPoints[n],endPoint,m) ) {
        endPoint = m;
      }
   }
   n++;
   hullPt = endPoint;
   ofs2 << rNode[hullPt] << endl;
  } while (endPoint != hullStart);
  cout << n << " hull points found " << endl;
  delete [] rNode;
  ofs.close();
  ofs2.close();
  cout << " node coords in hull.dat " << endl;
  cout << " convex hull coords in hull2.dat " << endl;
  return 0;
}
