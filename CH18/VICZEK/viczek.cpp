
/*

the Viczek boids model in two dimensions

*/

#include <iostream>
#include <fstream>
#include <cmath>
#include <random>
#include <vector>

using namespace std;
typedef mt19937_64 MersenneTwister;
int N;     // number of boids
double L;  // box size
double R;   // interaction radius
double eta; // range in random component

class TwoVect {

public:

  double x;
  double y;

  TwoVect () {
    x=0.0; y=0.0;
  };
 
  TwoVect (double th) { 
    x = cos(th); y = sin(th);
  }
  
  TwoVect (double x0, double y0) {
    x=x0; y=y0;
  };

  void setV(double x0, double y0) {
    x=x0; y=y0;
  }

  double magSq() {
    return  x*x + y*y;
  }


 TwoVect operator*(const TwoVect & other);

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

TwoVect TwoVect::operator*(const TwoVect & other){
  // multiply two unit vectors
  return TwoVect(x*other.x - y*other.y,x*other.y + y*other.x);
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
TwoVect *rold = NULL;
TwoVect *v = NULL;

void getBC(int i) {
  if (r[i].x > L) {
    r[i].x -=  L;
  }
  if (r[i].x < 0) {
    r[i].x +=  L;
  }
  if (r[i].y > L) {
    r[i].y -= L;
  }
  if (r[i].y < 0) {
    r[i].y += L;
  }
}

TwoVect getV(int B) {
  // we retain the self velocity since this returns a value if
  // there are no neighbors
  TwoVect va;
  for (int b=0;b<N;b++) {
    if ((r[b]-r[B]).magSq() < R*R) va = va + v[b];
  }
  double vmag = sqrt(va.magSq());
  return va/vmag;
}

int main(void) {
  int T,seed;
  double v0;
  uniform_real_distribution<double> Dist(-1.0,1.0);
  ofstream ofs;
  ofs.open("Vboids.dat");
  cout << " input N, box size, T, R, v0, eta, seed " << endl;
  cin >> N >> L >> T >> R >> v0 >> eta >> seed;
  ofs << "# Vboids.cpp " << endl;
  ofs << "# to plot: plot .. u 1:2:($3-$1):($4-$2) w vectors " << endl;
  MersenneTwister mt(seed);
  r = new TwoVect[N];
  rold = new TwoVect[N];
  v = new TwoVect[N];
  
  for (int b=0;b<N;b++) {
    r[b].setV(L*Dist(mt),L*Dist(mt));
    double th = 2.0*M_PI*Dist(mt);
    v[b].setV(v0*cos(th),v0*sin(th));
  }

  int dt=5;

  for (int t=0;t<T;t++) {
    for (int b=0;b<N;b++) {
       double xi = eta*(Dist(mt)-0.5);
       TwoVect vr(xi);
       v[b] = v0*getV(b)*vr;
       r[b] = r[b] + v[b];
       getBC(b);
       if (t == T-1-dt) rold[b] = r[b];
       if (t == T-1)  {
         if ((rold[b]-r[b]).magSq() < L*L/4.0) ofs << rold[b] << " " << r[b] << endl;
       }
    }
  } // end of the time loop 

  delete [] r;
  delete [] v;
  ofs.close();
  cout << "t x(t) x(t+1) in Vboids.dat" << endl;
  return 0;
}
