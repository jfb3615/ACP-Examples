/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef REVOLUTIONSURFACEUTIL_H
#define REVOLUTIONSURFACEUTIL_H

#include <algorithm>//for std::min,std::max
#include <cmath>

class  RevolutionSurfaceUtil {

  //Added by Thomas Kittelmann, September 2007.
  //
  //Used by SoTubs, SoPcons, SoCons, SoLAr, ... to get correct and tight bounding boxes.
  //

public:

  //_________________________________________________________________________________________________
  static int nphiDivisions( const float& dphi, const float& complexity, int OverrideNPhi = 0 ) {
    int NPHI =  ( OverrideNPhi > 0 ? OverrideNPhi : std::max(3,static_cast<int>(fabs(20.0*complexity*dphi/M_PI)) ) );
    int lim1 = static_cast<int>(dphi*8.0/(2*M_PI)+0.5);
    if (NPHI<lim1) {
      if (OverrideNPhi<=0) {
	if (NPHI<lim1)
	  NPHI = lim1;
      } else {
	int lim2 = static_cast<int>(dphi*5.0/(2*M_PI)+0.5);
	if (NPHI<lim2)
	  NPHI = lim2;
      }
    }
    if (NPHI<2)
      NPHI = 2;
    return NPHI;
  }


  //_________________________________________________________________________________________________
  static void setBBoxPars(const float& sphi, const float& dphi, const float& rmin, const float& rmax,
			  const float& zmin, const float& zmax, SbBox3f &box, SbVec3f &center ) {


    if (dphi>(2.0*M_PI*0.99)) {
      SbVec3f vmin(-rmax,-rmax,zmin),
	vmax( rmax, rmax, zmax);
      center.setValue(0,0,0);
      box.setBounds(vmin,vmax);
    } else {
      //Get start/end angle in correct interval:
      float anglestart = sphi;
      while (anglestart < 0.0) anglestart += 2*M_PI;
      while (anglestart > 2*M_PI) anglestart -= 2*M_PI;
      if (anglestart<0.0) anglestart = 0.0;
      float angleend = anglestart + dphi;

      //Get bounds:
      double cosanglestart(cos(anglestart));
      double cosangleend(cos(angleend));
      double sinanglestart(sin(anglestart));
      double sinangleend(sin(angleend));

      float ymax = rmax * ( (0.5*M_PI>=anglestart&&0.5*M_PI<=angleend) ? 1.0 : std::max(sinanglestart,sinangleend) );
      float ymin = rmax * ( (1.5*M_PI>=anglestart&&1.5*M_PI<=angleend) ? -1.0 : std::min(sinanglestart,sinangleend) );
      float xmax = rmax * ( (anglestart <= 0.0 || angleend >= 2*M_PI) ? 1.0 : std::max(cosanglestart,cosangleend) );
      float xmin = rmax * ( (M_PI>=anglestart&&M_PI<=angleend) ? -1.0 : std::min(cosanglestart,cosangleend) );

      //Not only the bounds of the arc - also of the lines to the center:
      if (dphi >= 0.5*M_PI || rmin==0.0f ) {
	//(0,0) is included
	if (ymax<0.0) ymax = 0.0;
	if (ymin>0.0) ymin = 0.0;
	if (xmax<0.0) xmax = 0.0;
	if (xmin>0.0) xmin = 0.0;
      } else {
	//The two inner "corners" must be included
	double x1 = rmin*cosanglestart;
	double y1 = rmin*sinanglestart;
	double x2 = rmin*cosangleend;
	double y2 = rmin*sinangleend;
	if (xmax<x1) xmax = x1;
	if (xmax<x2) xmax = x2;
	if (xmin>x1) xmin = x1;
	if (xmin>x2) xmin = x2;
	if (ymax<y1) ymax = y1;
	if (ymax<y2) ymax = y2;
	if (ymin>y1) ymin = y1;
	if (ymin>y2) ymin = y2;
      }
      SbVec3f vmin(xmin,ymin,zmin), vmax( xmax, ymax, zmax);
      center.setValue(0.5*(xmin+xmax),0.5*(ymin+ymax),0.5*(zmin+zmax));
      box.setBounds(vmin,vmax);
    }
  }


  //Fixme: Add common code to deal with complexity here.
};

#endif
