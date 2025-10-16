/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef HEPVis_SbMath_h
#define HEPVis_SbMath_h

#include <cmath>

#define SbMinimum(a,b) ((a)<(b)?a:b)
#define SbMaximum(a,b) ((a)>(b)?a:b)

#define FCOS(x)   ((float)cos((double)(x)))
#define FSIN(x)   ((float)sin((double)(x)))
//#define FACOS(x)  ((float)acos((double)(x)))
// #define FASIN(x)  ((float)asin((double)(x)))
#define FTAN(x)   ((float)tan((double)(x)))
#define FATAN(x)  ((float)atan((double)(x)))
// #define FSQRT(x)  ((float)sqrt((double)(x)))
// #define FPOW(x,y) ((float)pow((double)(x),(double)(y)))
// #define FLOG(x)   ((float)log((double)(x)))
// #define FLOG10(x) ((float)log10((double)(x)))
// #define FFLOOR(x) ((float)floor((double)(x)))
#define FFABS(x)  ((float)fabs((double)(x)))
// #define FCEIL(x)  ((float)ceil((double)(x)))

#endif
