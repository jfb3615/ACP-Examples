/*

channel.cpp

solve for flow rates in a rectangular channel


Nx/Ny = number of nodes along a side

g++ -I/usr/local/include/eigen3 ...

*/

#include <iostream>
#include <fstream>
#include <cmath>
#include <random>
#include <vector>
#include <Eigen/Dense>

using namespace std;
using namespace Eigen;

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
typedef Matrix<int,Dynamic,3> eMat;  // custom Eigen matrix type
eMat nL;                             // track nodes associated with element 0,1,2 = CCW nodes

double area(element e)  {
  double a3 = rNode[nL(e,1)].x - rNode[nL(e,0)].x;
  double a2 = rNode[nL(e,0)].x - rNode[nL(e,2)].x;
  double b3 = rNode[nL(e,0)].y - rNode[nL(e,1)].y;
  double b2 = rNode[nL(e,2)].y - rNode[nL(e,0)].y;
  return 0.5*(a3*b2 - a2*b3);
}

TwoVect cm(element e) {
  // center of mass of element e
  return (rNode[nL(e,0)] + rNode[nL(e,1)] + rNode[nL(e,2)])/3.0;
}

int main(void) {
  int verbose=0;
  int Nx,Ny;
  double hw,Pz,Vz;
  ofstream ofs,ofs2,ofs3,ofs4;
  ofs.open("channel.dat");
  ofs2.open("c2.dat");
  ofs3.open("c3.dat");
  ofs4.open("c4.dat");
  cout << " input Nx, Ny, H/W [0.5], Pz/mu [-6], Vz [0] " << endl;
  cin >> Nx >> Ny >> hw >> Pz >> Vz;
  cout << " enter 1 for verbose mode " << endl;
  cin >> verbose;
  int Nnodes = Nx*Ny;
  int Nelements = (Nx-1)*(Ny-1)*2;
  cout << " number of nodes: " << Nnodes << endl;
  cout << " number of elements: " << Nelements << endl;

  VectorXd f(Nnodes),w(Nnodes);
  MatrixXd K(Nnodes,Nnodes);
  nL = eMat(Nelements,3);

  // define rectangle nodes
  rNode = new TwoVect[Nnodes];
  node nodeNumber = 0;
  for (int iy=0;iy<Ny;iy++) {
  for (int ix=0;ix<Nx;ix++) {
     double x = (double) ix/(Nx-1);
     double y = (double) iy/(Ny-1);
     y *= hw;                        // scale to desired proportions
     rNode[nodeNumber].setV(x,y);
     nodeNumber++;
  }}

  //label elements and specify their nodes
  if (verbose) cout << endl << " element and associated nodes: " << endl << endl;
  for (int iy=0;iy<Ny-1;iy++) {
  for (int ix=0;ix<Nx-1;ix++) {
     node i = iy*(Nx-1)+ix;
     element eNumber = 2*i;
     node j = ix + Nx*iy;
     nL(eNumber,0) = j;
     nL(eNumber,1) = j+Nx+1;
     nL(eNumber,2) = j+Nx;
     if (verbose) cout << eNumber << " " << nL(eNumber,0) << " " << nL(eNumber,1) << " " << 
             nL(eNumber,2) << endl;
     eNumber++;
     nL(eNumber,0) = j;
     nL(eNumber,1) = j+1;
     nL(eNumber,2) = j+1+Nx;
     if (verbose) cout << eNumber << " " << nL(eNumber,0) << " " << nL(eNumber,1) << " " << 
             nL(eNumber,2) << endl;
  }}

  if (verbose) {
    cout << " ----------- " << endl;
    // print mesh
     for (element e=0;e<Nelements;e++) {
        ofs << rNode[nL(e,0)] << " " << rNode[nL(e,1)] << endl;
        ofs << rNode[nL(e,1)] << " " << rNode[nL(e,2)] << endl;
        ofs << rNode[nL(e,2)] << " " << rNode[nL(e,0)] << endl;
        ofs2 << e << " " << cm(e) << " " << area(e) << endl;
     }
  }

  // assemble FEM stiffness matrix, K
  for (element e=0;e<Nelements;e++) {
     double beta[3],gamma[3];
     for (int i=0;i<3;i++) {
       int j = (i+1)%3;
       int k = (i+2)%3;
       beta[i]  = rNode[nL(e,j)].y - rNode[nL(e,k)].y;
       gamma[i] = rNode[nL(e,k)].x - rNode[nL(e,j)].x;
       if (verbose) cout << e << " " << i << " " << beta[i] << " " << gamma[i] << endl;
     }
     for (int i=0;i<3;i++) {
     for (int j=0;j<3;j++) {
        node I = nL(e,i);
        node J = nL(e,j);
        K(I,J) += (beta[i]*beta[j] + gamma[i]*gamma[j])/(4.0*area(e));
     }}
  }

  if (verbose) {
    cout << " ----------- " << endl;
    cout << "K " << endl;
    cout << "det(K) = " << K.determinant() << endl;
    for (node i=0;i<Nnodes;i++) {
    for (node j=0;j<Nnodes;j++) {
      cout << i << " " << j << " " << K(i,j) << endl;
    }}
    cout << " ----------- " << endl;
  }
  

  // assemble force vector  [pressure gradient/viscosity  = dp/dz / mu  == Pz]
  for (element e=0;e<Nelements;e++) { 
     double ff = -Pz*area(e)/3.0;
     f(nL(e,0)) += ff;  // sum force contribution to nodes
     f(nL(e,1)) += ff;
     f(nL(e,2)) += ff; 
  }
  if (verbose) {
    cout << " f " << endl;
    for (node i=0;i<Nnodes;i++) cout << i << " " << f(i) << endl;
    cout << " ----------- " << endl;
  }

  // boundary conditions    [trick to force w(...) = f(...) = 0]
  // we need to identify the nodes on the boundary !

  for (node n=0;n<Nx;n++) {
     if (verbose) cout << " boundary nodes: " << n << " " << n + Nx*(Ny-1) << endl;
     K(n,n) *= 1e12; 
     K(n+Nx*(Ny-1),n+Nx*(Ny-1)) *= 1e12;
     f(n) = 0.0*K(n,n);                                 // bottom
     f(n+Nx*(Ny-1)) = Vz*K(n+Nx*(Ny-1),n+Nx*(Ny-1));    // top  
  }
  for (node n=1;n<Ny-1;n++) {
     if (verbose) cout << " boundary nodes: " << n*Nx << " " << n*Nx + Nx - 1 << endl;
     K(n*Nx,n*Nx) *= 1e12;
     K(n*Nx+Nx-1,n*Nx+Nx-1) *= 1e12;  
     f(n*Nx) = 0.0*K(n*Nx,n*Nx);             // left
     f(n*Nx+Nx-1) = 0.0;                     // right
  }

  int num=0;
  for (node n=0;n<Nnodes;n++) {
  for (node m=0;m<Nnodes;m++) {
     if (K(n,m) != 0.0) num++;
  }}
  cout << --num << " nonzero elements in K " << endl;

  // obtain solution
  w = K.inverse()*f;

  // store solution 
  int nx = 1;
  for (node n=0;n<Nnodes;n++) {
    ofs4 << rNode[n] << " " << w(n) << endl;
    nx++;
    if (nx > Nx) {
       ofs4 << "  " << endl;
       nx=1;
    }
  }

  // integrate to obtain flow rate  [= int dx dy w(x,y)]
  double in = 0.0;
  for (element e=0;e<Nelements;e++) {
     in +=area(e)*(w(nL(e,0)) + w(nL(e,1)) + w(nL(e,2)));
  }
  in /= 3.0;
  cout << " flow rate = " << in << endl;

  delete [] rNode;
  ofs.close();
  ofs2.close();
  ofs3.close();
  ofs4.close();
  cout << " mesh coords in channel.dat " << endl;
  cout << " e cm(e) area(e) in c2.dat " << endl;
  cout << " r_node w(r_node)  in c4.dat" << endl;
  return 0;
}
