/*

BH.cpp

test the Barnes-But algorithm in two dimensions

*/

#include <iostream>
#include <cmath>
#include <vector>

using namespace std;
int N;      // number of point masses
double theta;

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

  double magSq() {
    return  x*x + y*y;
  }
 
 double operator*(const TwoVect & other);

 TwoVect operator+(const TwoVect & other);

 TwoVect operator-(const TwoVect & other);

 TwoVect operator^(const TwoVect & other);

 friend std::ostream &operator<<(std::ostream &out, TwoVect v0) {
  out << "(" << v0.x << "," << v0.y << ")";
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


TwoVect *r = NULL;
vector <TwoVect> coords;
TwoVect x0(-1.0,-1.0);
TwoVect x1(1.0,-1.0);
TwoVect x2(1.0,1.0);
TwoVect x3(-1.0,1.0);

struct BHnode {
    
  double mass;
  double dx;
  TwoVect r;
  BHnode *node0;  //  3 2
  BHnode *node1;  //  0 1 
  BHnode *node2;
  BHnode *node3;

  BHnode() {
    mass = 0.0;
    dx = 0.0;
    r.setV(0.0,0.0);
    node0 = NULL;
    node1 = NULL;
    node2 = NULL;
    node3 = NULL;
  }
  
  BHnode(double m, double x, TwoVect R) {
    mass = m;
    dx = x;
    r = R;
    node0 = NULL;
    node1 = NULL;
    node2 = NULL;
    node3 = NULL;
  }
 
  BHnode(double m, double x, TwoVect R, BHnode * n0, BHnode * n1, BHnode * n2, BHnode * n3) {
    mass = m;
    dx = x;
    r = R;
    node0 = n0;
    node1 = n1;
    node2 = n2;
    node3 = n3;
  }

};


TwoVect  BHforce(BHnode * rt, TwoVect R) {
  TwoVect f;
  if ((rt != NULL) && (rt->mass != 0.0)) {
    TwoVect r0;
    r0 = R - rt->r;
    double rsq = r0.magSq();
    if (rsq == 0.0) return r0;
    if ((rt->mass == 1.0) || 
        (rt->dx/sqrt(rsq) < theta)) {
       f = (-1.0)* rt->mass*r0/(sqrt(rsq)*rsq);
       cout << " adding " << rt->mass << " " << rt->dx << " " << " " << rt->r << " " << sqrt(rsq) << endl;
    } else { // go deeper
       f = f + BHforce(rt->node0,R);
       f = f + BHforce(rt->node1,R);
       f = f + BHforce(rt->node2,R);
       f = f + BHforce(rt->node3,R);
    } 
  }
  return f;
}


int BHcount(BHnode * rt) {
   if (rt == NULL )
      return 0;  
   else {
      int count = 1;  
      count += BHcount(rt->node0);
      count += BHcount(rt->node1);
      count += BHcount(rt->node2);
      count += BHcount(rt->node3);
      return count; 
  }
}


void BHprint (BHnode * rt) {
  if (rt != NULL) {
    cout << rt->mass << " " << rt->dx << " " << rt->r << endl;
    BHprint(rt->node0);
    BHprint(rt->node1);
    BHprint(rt->node2);
    BHprint(rt->node3);
  }
}

bool in(TwoVect r, TwoVect r0, double dx) {
  if ((r0.x-dx < r.x) && (r.x < r0.x+dx) && 
      (r0.y-dx < r.y) && (r.y < r0.y+dx)) return true;
  return false;
}


void BHinsert (BHnode * rt, TwoVect r, TwoVect Rnode, double dx) {
   /* 
      rt = pointer to starting BHnode
      r = particle posn
      Rnode defines the cell via its cm
      dx = cell radius ie   R-dx < x < R+dx
   */

  cout <<  " working on " << r << " at " << Rnode << " " << dx << endl;
  if (rt->mass == 0.0) { // insert at current cell
       cout << " inserting " << r << " at " << Rnode << " " << dx << endl;
     rt->mass = 1.0;
     rt->r = r;
     rt->dx = dx;
   } else if (rt->mass > 1.0) {    // more than one particle, so an internal node
     dx /= 2.0;
     if (in(r,Rnode+dx*x0,dx)) {
         cout << " continuing 0 " << Rnode << " " << dx << endl;
        BHinsert(rt->node0,r,Rnode+dx*x0,dx);
     } else if (in(r,Rnode+dx*x1,dx)) {
         cout << " continuing 1 " << Rnode << " " << dx << endl;
        BHinsert(rt->node1,r,Rnode+dx*x1,dx);
     } else if (in(r,Rnode+dx*x2,dx)) {
         cout << " continuing 2 " << Rnode << " " << dx << endl;
        BHinsert(rt->node2,r,Rnode+dx*x2,dx);
     } else {
         cout << " continuing 3 " << Rnode << " " << dx << endl;
        BHinsert(rt->node3,r,Rnode+dx*x3,dx);
     }
     rt->r = (rt->mass*rt->r + r)/(rt->mass + 1.0);    // update CM and mass
     rt->mass = rt->mass + 1.0;
   } else { // so m=1 therefore a leaf
        cout << " at leaf " << Rnode << " " << dx << endl;
     dx /= 2.0;   
     rt->node0 = new BHnode;            // split
     rt->node1 = new BHnode;
     rt->node2 = new BHnode;
     rt->node3 = new BHnode;
     if (in(rt->r,Rnode+dx*x0,dx)) {     // move original mass
       rt->node0->mass = 1.0;            // could have already been moved!
       rt->node0->dx = dx;
       rt->node0->r = rt->r;
        cout << " moved 0 " << endl;
     } else if (in(rt->r,Rnode+dx*x1,dx)) {
       rt->node1->mass = 1.0;
       rt->node1->dx = dx;
       rt->node1->r = rt->r;
        cout << " moved 1 " << endl;
     } else if (in(rt->r,Rnode+dx*x2,dx)) {
       rt->node2->mass = 1.0;
       rt->node2->dx = dx;
       rt->node2->r = rt->r;
        cout << " moved 2 " << endl;
     } else {
       rt->node3->mass = 1.0;
       rt->node3->dx = dx; 
       rt->node3->r = rt->r;
        cout << " moved 3 " << endl;
     }
     if (in(r,Rnode+dx*x0,dx)) {
           cout << "0 " << r <<  "  " << Rnode+dx*x0 << " " << dx << endl;
        BHinsert(rt->node0,r,Rnode+dx*x0,dx);
     } else if (in(r,Rnode+dx*x1,dx)) {
           cout << "1 " << r <<  "  " << Rnode+dx*x1 << " " << dx << endl;
        BHinsert(rt->node1,r,Rnode+dx*x1,dx);
     } else if (in(r,Rnode+dx*x2,dx)) {
           cout << "2 " << r <<  "  " << Rnode+dx*x2 << " " << dx << endl;
        BHinsert(rt->node2,r,Rnode+dx*x2,dx);
     } else {
           cout << "3 " << r <<  "  " << Rnode+dx*x3 << " " << dx << endl;
        BHinsert(rt->node3,r,Rnode+dx*x3,dx);
     }
     rt->r = (rt->mass*rt->r + r)/(rt->mass + 1.0);    // update CM and mass
     rt->mass = rt->mass + 1.0;
   }
} // end of BHinsert
   

int main(void) {

  cout << " enter theta " << endl;
  cin >> theta;
  N=5;
  // initial configuration and BH root. Place (0,0) in the middle
  double dx;
  r = new TwoVect[N];
  // another test
  BHnode root;
  dx = 1.0;
  TwoVect Rcm;

  r[0].setV(-0.294026,-0.107904);
  BHinsert(&root,r[0],Rcm,dx);
  r[1].setV(-0.0805377,-0.934822);
  BHinsert(&root,r[1],Rcm,dx);
  r[2].setV(0.0650164,0.18109);
  BHinsert(&root,r[2],Rcm,dx);
  r[3].setV(-0.239071,0.920418);
  BHinsert(&root,r[3],Rcm,dx);
  r[4].setV(-0.816836,0.060623);
  BHinsert(&root,r[4],Rcm,dx);

  TwoVect rCM;
  for (int i=0;i<5;i++) {
    rCM = rCM + r[i];
  }
  rCM = rCM/5.0;

  cout << " CM:  " << rCM << endl;
  cout << " number of nodes = " << BHcount(&root) << endl;
  BHprint(&root);
  cout << "  " << endl;


  for (int i=0;i<5;i++) {
    cout << " force on " << i << "  = " << BHforce(&root,r[i]) << endl;
  }
  // comparisons
  cout<< " largest scale (includes self-force)" << endl;
  for (int i=0;i<N;i++) {
    double rsq = (r[i]-rCM).magSq();
    TwoVect f = -5.0*(r[i]-rCM)/(sqrt(rsq)*rsq);
    cout << i << " " << f << endl;
  }
  cout<< " smallest scale " << endl;
  for (int i=0;i<N;i++) {
    TwoVect f;
    for (int j=0;j<N;j++) {
      if (j != i) {
        double rsq = (r[i]-r[j]).magSq();
        f = f - (r[i]-r[j])/(sqrt(rsq)*rsq);
      }
    }
    cout << i << " " << f << endl;
  }

  delete [] r;
  return 0;
}
