void GaussLegendreEigen (Eigen::VectorXd & x, Eigen::VectorXd & w, int n, double low, double hi) {

/*
  Eigen requires passing by reference

     **********************************************************
     *                                                        *
     *   Calculate weights and abscissae for Gauss-Legendre   *
     *   quadrature.                                          *
     *   These are to be used in the formula                  *
     *                                                        *
     *     Intgl_{low}^{hi}  f(x) dx                          *
     *          =   Sum_{j=0 to N-1} w(j)*f(x_j)                *
     *                                                        *
     **********************************************************
*/

  int m;
  double eps=3e-14;  // w(n) and x(n)
  double p1,p2,p3,pp,xl,xm,z,z1,pi;
  pi = 4.0*atan(1.0);
  m = (n+1)/2;
  xm = 0.50*(hi + low);
  xl = 0.50*(hi - low);
  for (int i=1; i<=m; i++) {
    z= std::cos(pi*(i-0.25)/(0.5+n));
    z1 = 1e10;
    while (std::abs(z-z1) > eps) {
      p1= 1.0;
      p2= 0.0;
      for (int j=1;j<=n;j++) { 
        p3 = p2;
        p2 = p1;
        p1 = ( (2.0*j -1.0)*z*p2 +(1.0 -j)*p3 )/j;
      }
      pp = n*(z*p1 -p2)/(z*z -1.0);
      z1 = z;
      z = z1 - p1/pp;
    }
    x(i-1) = xm - xl*z;
    x(n-i)= xm + xl*z;
    w(i-1)= 2.0*xl/( (1.0-z*z)*pp*pp );
    w(n-i)= w(i-1);
  }
}
