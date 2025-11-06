/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*-----------------------------HEPVis---------------------------------------*/
/*                                                                          */
/* Node:             SoTubs                                                 */
/* Description:      Represents the G4Tubs Geant Geometry entity            */
/* Author:           Joe Boudreau Nov 11 1996                               */
/*                                                                          */
/*--------------------------------------------------------------------------*/

#include "SoTubs.h"

#include "RevolutionSurfaceUtil.h"

#include <cassert>
#include <cmath>

#include <Inventor/SbBox.h>
#include <Inventor/actions/SoGLRenderAction.h>
// #include <Inventor/fields/SoSFFloat.h>
#include <Inventor/misc/SoChildList.h>
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoIndexedFaceSet.h>
#include <Inventor/nodes/SoNormal.h>
#include <Inventor/nodes/SoCoordinate3.h>
#include <Inventor/nodes/SoNormalBinding.h>
#include <Inventor/SoPrimitiveVertex.h>
#include <Inventor/elements/SoTextureCoordinateElement.h>
#include <Inventor/elements/SoGLCacheContextElement.h>
#include <Inventor/C/glue/gl.h>

#include "SbMath.h"
#include <iostream>

// This statement is required
SO_NODE_SOURCE(SoTubs)

// Constructor
SoTubs::SoTubs() {


  // This statement is required
  SO_NODE_CONSTRUCTOR(SoTubs);

  // Data fields are initialized like this:
  SO_NODE_ADD_FIELD(pRMin,               (0));
  SO_NODE_ADD_FIELD(pRMax,               (1));
  SO_NODE_ADD_FIELD(pDz,                 (10));
  SO_NODE_ADD_FIELD(pSPhi,               (0));
  SO_NODE_ADD_FIELD(pDPhi,               ((float)(2*M_PI)));
  SO_NODE_ADD_FIELD(pOverrideNPhi,       (0));
  SO_NODE_ADD_FIELD(alternateRep,        (NULL));
  SO_NODE_ADD_FIELD(drawEdgeLines,       (false));

  m_children = new SoChildList(this);

  setNodeType(EXTENSION);
}

// Destructor
SoTubs::~SoTubs() {
  delete m_children;
}


//____________________________________________________________________
bool SoTubs::s_didInit = false;
void SoTubs::initClass()
{
  if ( !s_didInit ) {
    SO_NODE_INIT_CLASS(SoTubs,SoShape,"Shape");
    s_didInit = true;
  }
}

// generatePrimitives
void SoTubs::generatePrimitives(SoAction *action) {
  // This variable is used to store each vertex
  SoPrimitiveVertex pv;

  // Access the stat from the action
  SoState *state = action->getState();
  if (!state)
    return;

  // See if we have to use a texture coordinate function,
  // rather than generating explicit texture coordinates.
  SbBool useTexFunction=
    (SoTextureCoordinateElement::getType(state) ==
     SoTextureCoordinateElement::FUNCTION);

  // If we need to generate texture coordinates with a function,
  // we'll need an SoGLTextureCoordinateElement.  Otherwise, we'll
  // set up the coordinates directly.
  const SoTextureCoordinateElement* tce = NULL;
  SbVec4f texCoord;
  if (useTexFunction) {
    tce = SoTextureCoordinateElement::getInstance(state);
  }
  else {
    texCoord[2] = 0.0;
    texCoord[3] = 1.0;
  }
  SbVec3f point, normal;


  ///////////////////////////////////////////////////////
  //-----------------------------------------------------
#define GEN_VERTEX(pv,x,y,z,s,t,nx,ny,nz)               \
  point.setValue((float)(x),(float)(y),(float)(z));     \
  normal.setValue((float)(nx),(float)(ny),(float)(nz)); \
  if (useTexFunction) {                                 \
    texCoord=tce->get(point,normal);                    \
  } else {                                              \
    texCoord[0]=(float)(s);                             \
    texCoord[1]=(float)(t);                             \
  }                                                     \
  pv.setPoint(point);                                   \
  pv.setNormal(normal);                                 \
  pv.setTextureCoords(texCoord);                        \
  shapeVertex(&pv);
  //-----------------------------------------------------
  ///////////////////////////////////////////////////////

  int NPHI = RevolutionSurfaceUtil::nphiDivisions( pDPhi.getValue(), this->getComplexityValue(action), pOverrideNPhi.getValue() );

  double deltaPhi = pDPhi.getValue()/NPHI, phi0 = pSPhi.getValue(),phi1=phi0+pDPhi.getValue();
  double rMax=pRMax.getValue(),rMin=pRMin.getValue();
  double zMax=pDz.getValue(),zMin=-zMax;
  double cosPhi0=cos(phi0), sinPhi0=sin(phi0);
  double cosPhi1=cos(phi1), sinPhi1=sin(phi1);
  double cosDeltaPhi=cos(deltaPhi),sinDeltaPhi=sin(deltaPhi);
  const bool noPhiCutout=fabs(pDPhi.getValue())==0.F || fabs(fabs(pDPhi.getValue())-2.0*M_PI)<0.01; // FIXME - better way to do this?
  const bool disableLighting(glIsEnabled(GL_LIGHTING));
  const bool transparencyOn(glIsEnabled(GL_BLEND));
  
  //
  // The outer surface!
  //
  int i;
  double sinPhi,cosPhi;
  beginShape(action,TRIANGLE_STRIP);
  sinPhi=sinPhi0;
  cosPhi=cosPhi0;
  for (i = 0; i<=NPHI; i++) {
    GEN_VERTEX(pv,rMax*cosPhi,rMax*sinPhi,zMax,0.0,0.0,cosPhi,sinPhi,0);
    GEN_VERTEX(pv,rMax*cosPhi,rMax*sinPhi,zMin,1.0,1.0,cosPhi,sinPhi,0);
    inc(sinPhi, cosPhi, sinDeltaPhi, cosDeltaPhi);
  }
  endShape();
  //
  // The inner surface!
  //
  if(rMin!=0.F) {
    beginShape(action,TRIANGLE_STRIP);
    sinPhi=sinPhi0;
    cosPhi=cosPhi0;
    for (i = 0; i<=NPHI; i++) {
      GEN_VERTEX(pv,rMin*cosPhi,rMin*sinPhi,zMax,0.0,0.0,-cosPhi,-sinPhi,0);
      GEN_VERTEX(pv,rMin*cosPhi,rMin*sinPhi,zMin,1.0,1.0,-cosPhi,-sinPhi,0);
      inc(sinPhi, cosPhi, sinDeltaPhi, cosDeltaPhi);
    }
    endShape();
  }
//  if (fabs(deltaPhi)<2.0*M_PI) { // Old value - probably responsible for the funny internal stuff.
  if (!noPhiCutout) {

    //
    // The end - inner part of wedge
    //
    
    // FIXME - Use GL_POLYGON? Tried it - no change when selecting and seemed to mess up phong-type shading.
    // Try again, to see if it's faster?
    
    beginShape(action,TRIANGLE_STRIP);
    sinPhi=sinPhi0;
    cosPhi=cosPhi0;
    GEN_VERTEX(pv,rMax*cosPhi,rMax*sinPhi,zMax,0.0,0.0,sinPhi,-cosPhi,0);
    GEN_VERTEX(pv,rMax*cosPhi,rMax*sinPhi,zMin,1.0,1.0,sinPhi,-cosPhi,0);
    GEN_VERTEX(pv,rMin*cosPhi,rMin*sinPhi,zMax,1.0,0.0,sinPhi,-cosPhi,0);
    GEN_VERTEX(pv,rMin*cosPhi,rMin*sinPhi,zMin,0.0,1.0,sinPhi,-cosPhi,0);
    endShape();
    //
    // The other end
    //
    beginShape(action,TRIANGLE_STRIP);
    sinPhi=sinPhi1;
    cosPhi=cosPhi1;
    GEN_VERTEX(pv,rMax*cosPhi,rMax*sinPhi, zMax,0.0,0.0,-sinPhi,+cosPhi,0);
    GEN_VERTEX(pv,rMax*cosPhi,rMax*sinPhi, zMin,1.0,1.0,-sinPhi,+cosPhi,0);
    GEN_VERTEX(pv,rMin*cosPhi,rMin*sinPhi, zMax,1.0,0.0,-sinPhi,+cosPhi,0);
    GEN_VERTEX(pv,rMin*cosPhi,rMin*sinPhi, zMin,0.0,1.0,-sinPhi,+cosPhi,0);
    endShape();
    
    if (drawEdgeLines.getValue()) {
      if (disableLighting) glDisable(GL_LIGHTING);
      if (transparencyOn) glDisable(GL_BLEND);
      
      glBegin(GL_LINES);
      glVertex3f(rMax*cosPhi0,rMax*sinPhi0, zMax);
      glVertex3f(rMax*cosPhi0,rMax*sinPhi0, zMin);
      glVertex3f(rMin*cosPhi0,rMin*sinPhi0, zMax);
      glVertex3f(rMin*cosPhi0,rMin*sinPhi0, zMin);
      glVertex3f(rMax*cosPhi1,rMax*sinPhi1, zMax);
      glVertex3f(rMax*cosPhi1,rMax*sinPhi1, zMin);
      glVertex3f(rMin*cosPhi1,rMin*sinPhi1, zMax);
      glVertex3f(rMin*cosPhi1,rMin*sinPhi1, zMin);
      glEnd();
      if (disableLighting) glEnable(GL_LIGHTING);
      if (transparencyOn) glEnable(GL_BLEND);
    }
    
  }
  //
  // The outer surface at z=+PDZ
  //
  if(rMin==0.F) {
    beginShape(action,TRIANGLE_FAN);
    sinPhi=sinPhi0;
    cosPhi=cosPhi0;
    GEN_VERTEX(pv,0,0,zMax,0.0,0.0,0,0,1);
    for (i = 0; i<=NPHI; i++) {
      GEN_VERTEX(pv,rMax*cosPhi,rMax*sinPhi,zMax,1.0,1.0,0,0,1);
      inc(sinPhi, cosPhi, sinDeltaPhi, cosDeltaPhi);
    }
    endShape();
    //
    // The outer surface at z=-PDZ
    //
    beginShape(action,TRIANGLE_FAN);
    sinPhi=sinPhi0;
    cosPhi=cosPhi0;
    GEN_VERTEX(pv,0,0,zMin,0.0,0.0,0,0,-1);
    for (i = 0; i<=NPHI; i++) {
      GEN_VERTEX(pv,rMax*cosPhi,rMax*sinPhi,zMin,1.0,1.0,0,0,-1);
      inc(sinPhi, cosPhi, sinDeltaPhi, cosDeltaPhi);
    }
    endShape();
    
    
    if (drawEdgeLines.getValue()) {
      if (disableLighting) glDisable(GL_LIGHTING);
      if (transparencyOn) glDisable(GL_BLEND);
      glBegin(GL_LINE_STRIP);      
      sinPhi=sinPhi0;
      cosPhi=cosPhi0;
      if (!noPhiCutout) glVertex3f(0,0,zMax); // only draw if phi range
      for (i = 0; i<=NPHI; i++) {
        glVertex3f(rMax*cosPhi,rMax*sinPhi,zMax);
        inc(sinPhi, cosPhi, sinDeltaPhi, cosDeltaPhi);
      }
      if (!noPhiCutout) glVertex3f(0,0,zMax);

      glEnd();
      
      glBegin(GL_LINE_STRIP);      
      sinPhi=sinPhi0;
      cosPhi=cosPhi0;
      if (!noPhiCutout) glVertex3f(0,0,zMin); // only draw if phi range
      for (i = 0; i<=NPHI; i++) {
        glVertex3f(rMax*cosPhi,rMax*sinPhi,zMin);
        inc(sinPhi, cosPhi, sinDeltaPhi, cosDeltaPhi);
      }
      if (!noPhiCutout) glVertex3f(0,0,zMin);
      
      glEnd();
      
      if (disableLighting) glEnable(GL_LIGHTING);
      if (transparencyOn) glEnable(GL_BLEND);
    }
    
  } else {
    beginShape(action,TRIANGLE_STRIP);
    sinPhi=sinPhi0;
    cosPhi=cosPhi0;
    for (i = 0; i<=NPHI; i++) {
      GEN_VERTEX(pv,rMin*cosPhi,rMin*sinPhi,zMax,0.0,0.0,0,0,1);
      GEN_VERTEX(pv,rMax*cosPhi,rMax*sinPhi,zMax,1.0,1.0,0,0,1);
      inc(sinPhi, cosPhi, sinDeltaPhi, cosDeltaPhi);
    }
    endShape();
    //
    // The outer surface at z=-PDZ
    //
    beginShape(action,TRIANGLE_STRIP);
    sinPhi=sinPhi0;
    cosPhi=cosPhi0;
    for (i = 0; i<=NPHI; i++) {
      GEN_VERTEX(pv,rMin*cosPhi,rMin*sinPhi,zMin,0.0,0.0,0,0,-1);
      GEN_VERTEX(pv,rMax*cosPhi,rMax*sinPhi,zMin,1.0,1.0,0,0,-1);
      inc(sinPhi, cosPhi, sinDeltaPhi, cosDeltaPhi);
    }
    endShape();
    
    if (drawEdgeLines.getValue()) {      
      if (disableLighting) glDisable(GL_LIGHTING);
      if (transparencyOn) glDisable(GL_BLEND);
      glBegin(GL_LINE_STRIP);      
      sinPhi=sinPhi0;
      cosPhi=cosPhi0;
      if (!noPhiCutout) glVertex3f(rMin*cosPhi,rMin*sinPhi,zMax); // only draw if phi range
      for (i = 0; i<=NPHI; i++) {
        glVertex3f(rMax*cosPhi,rMax*sinPhi,zMax);
        inc(sinPhi, cosPhi, sinDeltaPhi, cosDeltaPhi);
      }
      if (!noPhiCutout) glVertex3f(rMin*cosPhi1,rMin*sinPhi1,zMax); // only draw if phi range
      glEnd();
      
      glBegin(GL_LINE_STRIP);      
      sinPhi=sinPhi0;
      cosPhi=cosPhi0;
      for (i = 0; i<=NPHI; i++) {
        glVertex3f(rMin*cosPhi,rMin*sinPhi,zMax);
        inc(sinPhi, cosPhi, sinDeltaPhi, cosDeltaPhi);
      }
      glEnd();
      
      glBegin(GL_LINE_STRIP);      
      sinPhi=sinPhi0;
      cosPhi=cosPhi0;
      if (!noPhiCutout) glVertex3f(rMin*cosPhi,rMin*sinPhi,zMin); // only draw if phi range
      for (i = 0; i<=NPHI; i++) {
        glVertex3f(rMax*cosPhi,rMax*sinPhi,zMin);
        inc(sinPhi, cosPhi, sinDeltaPhi, cosDeltaPhi);
      }
      if (!noPhiCutout) glVertex3f(rMin*cosPhi1,rMin*sinPhi1,zMin); // only draw if phi range
      glEnd();
      
      glBegin(GL_LINE_STRIP);      
      sinPhi=sinPhi0;
      cosPhi=cosPhi0;
      for (i = 0; i<=NPHI; i++) {
        glVertex3f(rMin*cosPhi,rMin*sinPhi,zMin);
        inc(sinPhi, cosPhi, sinDeltaPhi, cosDeltaPhi);
      }
      glEnd();
      
      if (disableLighting) glEnable(GL_LIGHTING);
      if (transparencyOn) glEnable(GL_BLEND);
    }
    
  }

  if (state&&state->isElementEnabled(SoGLCacheContextElement::getClassStackIndex())) {
    //Encourage auto caching
    SoGLCacheContextElement::shouldAutoCache(state, SoGLCacheContextElement::DO_AUTO_CACHE);
#if ((COIN_MAJOR_VERSION>=3)||((COIN_MAJOR_VERSION==2)&&(COIN_MINOR_VERSION>=5)))
    SoGLCacheContextElement::incNumShapes(state);
#endif
  }
}

// getChildren
SoChildList *SoTubs::getChildren() const {
  return m_children;
}


// computeBBox
void SoTubs::computeBBox(SoAction *, SbBox3f &box, SbVec3f &center ){
  RevolutionSurfaceUtil::setBBoxPars(pSPhi.getValue(), pDPhi.getValue(),
				     pRMin.getValue(), pRMax.getValue(),
				     -pDz.getValue(),pDz.getValue(),
				     box, center );
}


// updateChildren
void SoTubs::updateChildren() {

  // Redraw the G4Tubs....

  assert(m_children->getLength()==1);
  SoSeparator       *sep                = (SoSeparator *)  ( *m_children)[0];
  SoCoordinate3     *theCoordinates     = (SoCoordinate3 *)      ( sep->getChild(0));
  SoNormal          *theNormals         = (SoNormal *)           ( sep->getChild(1));
  SoNormalBinding   *theNormalBinding   = (SoNormalBinding *)    ( sep->getChild(2));
  SoIndexedFaceSet  *theFaceSet         = (SoIndexedFaceSet *)   ( sep->getChild(3));


  const int NPHI=96, NPOINTS=2*(2*NPHI+2), NFACES=4*NPHI+2, NINDICES = NFACES*5;
  float points[NPOINTS][3],normals[NFACES][3];
#ifdef INVENTOR2_0
  static long     indices[NINDICES];
#else
  static int32_t  indices[NINDICES];
#endif

  static int init=0;
  double phi, pp, DeltaPhi;

  // Indices need to be generated once! This is here to keep it close to the point
  // generation, since otherwise it will be confusing.

  int i;
  if (!init) {
    init = 1;
    // Outer face
    for (i = 0; i< NPHI; i++) {
      // 0 1 3 2;
      indices[5*i+0] =  2*i+0;
      indices[5*i+1] =  2*i+1;
      indices[5*i+2] =  2*i+3;
      indices[5*i+3] =  2*i+2;
      indices[5*i+4] = SO_END_FACE_INDEX;
    }
    // the inner face
    for (i=0;i<NPHI;i++) {
      indices[5*1*NPHI + 5*i+0] = 2*NPHI+2 + 2*i+0;
      indices[5*1*NPHI + 5*i+1] = 2*NPHI+2 + 2*i+1;
      indices[5*1*NPHI + 5*i+2] = 2*NPHI+2 + 2*i+3;
      indices[5*1*NPHI + 5*i+3] = 2*NPHI+2 + 2*i+2;
      indices[5*1*NPHI + 5*i+4] = SO_END_FACE_INDEX;
    }
    // the top side
    for (i=0;i<NPHI;i++) {
      indices[5*2*NPHI + 5*i+0] = 2*i+0;
      indices[5*2*NPHI + 5*i+1] = 2*i+2;
      indices[5*2*NPHI + 5*i+2] = NPOINTS - (2*i+4);
      indices[5*2*NPHI + 5*i+3] = NPOINTS - (2*i+2);
      indices[5*2*NPHI + 5*i+4] = SO_END_FACE_INDEX;
    }
    // the bottom side
    for (i=0;i<NPHI;i++) {
      indices[5*3*NPHI + 5*i+0] = 2*i+1;
      indices[5*3*NPHI + 5*i+1] = NPOINTS - (2*i+1);
      indices[5*3*NPHI + 5*i+2] = NPOINTS - (2*i+3);
      indices[5*3*NPHI + 5*i+3] = 2*i+3;
      indices[5*3*NPHI + 5*i+4] = SO_END_FACE_INDEX;
    }
    // the odd side
    indices[5*4*NPHI +0] = 2*NPHI;
    indices[5*4*NPHI +1] = 2*NPHI+1;
    indices[5*4*NPHI +2] = 2*NPHI+3;
    indices[5*4*NPHI +3] = 2*NPHI+2;
    indices[5*4*NPHI +4] = SO_END_FACE_INDEX;
    // aother odd side
    indices[5*4*NPHI +5 +0] = 0;
    indices[5*4*NPHI +5 +1] = NPOINTS-2;
    indices[5*4*NPHI +5 +2] = NPOINTS-1;
    indices[5*4*NPHI +5 +3] = 1;
    indices[5*4*NPHI +5 +4] = SO_END_FACE_INDEX;
  }
  // Points need to be generated each time:
  if (pDPhi.getValue()<2*M_PI) {
    // the odd side
    indices[5*4*NPHI +0] = 2*NPHI;
    indices[5*4*NPHI +1] = 2*NPHI+1;
    indices[5*4*NPHI +2] = 2*NPHI+3;
    indices[5*4*NPHI +3] = 2*NPHI+2;
    indices[5*4*NPHI +4] = SO_END_FACE_INDEX;
    // aother odd side
    indices[5*4*NPHI +5 +0] = 0;
    indices[5*4*NPHI +5 +1] = NPOINTS-2;
    indices[5*4*NPHI +5 +2] = NPOINTS-1;
    indices[5*4*NPHI +5 +3] = 1;
    indices[5*4*NPHI +5 +4] = SO_END_FACE_INDEX;
  }
  else {
    // the odd side
    indices[5*4*NPHI +0] = SO_END_FACE_INDEX;
    indices[5*4*NPHI +1] = SO_END_FACE_INDEX;
    indices[5*4*NPHI +2] = SO_END_FACE_INDEX;
    indices[5*4*NPHI +3] = SO_END_FACE_INDEX;
    indices[5*4*NPHI +4] = SO_END_FACE_INDEX;
    // aother odd side
    indices[5*4*NPHI +5 +0] = SO_END_FACE_INDEX;
    indices[5*4*NPHI +5 +1] = SO_END_FACE_INDEX;
    indices[5*4*NPHI +5 +2] = SO_END_FACE_INDEX;
    indices[5*4*NPHI +5 +3] = SO_END_FACE_INDEX;
    indices[5*4*NPHI +5 +4] = SO_END_FACE_INDEX;
  }
  // The outer surface
  DeltaPhi = pDPhi.getValue()/NPHI, phi = pSPhi.getValue();
  for (i = 0; i<=NPHI; i++) {
    points[2*i+0][0] = pRMax.getValue()*FCOS(phi);
    points[2*i+0][1]= pRMax.getValue()*FSIN(phi);
    points[2*i+0][2] = +pDz.getValue();

    points[2*i+1][0] = pRMax.getValue()*FCOS(phi);
    points[2*i+1][1]= pRMax.getValue()*FSIN(phi);
    points[2*i+1][2] = -pDz.getValue();

    pp = phi+DeltaPhi/2.0;
    if (i!=NPHI) {
      normals[i][0] = FCOS(pp);
      normals[i][1] = FSIN(pp);
      normals[i][2] = 0;
    }
    phi+=DeltaPhi;
  }
  // The inner surface
  phi = pSPhi.getValue() + pDPhi.getValue();
  for (i = 0; i<=NPHI; i++) {
    points[2*NPHI+2+2*i+0][0] = pRMin.getValue()*FCOS(phi);
    points[2*NPHI+2+2*i+0][1] = pRMin.getValue()*FSIN(phi);
    points[2*NPHI+2+2*i+0][2] = +pDz.getValue();
    points[2*NPHI+2+2*i+1][0] = pRMin.getValue()*FCOS(phi);
    points[2*NPHI+2+2*i+1][1] = pRMin.getValue()*FSIN(phi);
    points[2*NPHI+2+2*i+1][2] = -pDz.getValue();
    pp = phi-DeltaPhi/2.0;
    if (i!=NPHI) {
      normals[NPHI+i][0] = -FCOS(pp);
      normals[NPHI+i][1] = -FSIN(pp);
      normals[NPHI+i][2] = 0;
    }
    phi-=DeltaPhi;
  }
  // The top side
  for (i=0;i<NPHI;i++) {
    normals[2*NPHI+i][0]=normals[2*NPHI+i][1]=0;
    normals[2*NPHI+i][2]=  1.0;
  }
  // The bottom side
  for (i=0;i<NPHI;i++) {
    normals[3*NPHI+i][0]=normals[3*NPHI+i][1]=0;
    normals[3*NPHI+i][2]= -1.0;
  }
  // The odd side
  phi = pSPhi.getValue();
  normals[4*NPHI+0][0]=  FSIN(phi);
  normals[4*NPHI+0][1]= -FCOS(phi);
  normals[4*NPHI+0][2]=0;

    // Another odd side
  phi = pSPhi.getValue()+pDPhi.getValue();
  normals[4*NPHI+1][0]= -FSIN(phi);
  normals[4*NPHI+1][1]= +FCOS(phi);
  normals[4*NPHI+1][2]=0;

  for (int np=0;np<NPOINTS; np++) theCoordinates->point.set1Value(np,points[np][0],points[np][1],points[np][2]);
  for (int ni=0;ni<NINDICES;ni++) theFaceSet->coordIndex.set1Value(ni,indices[ni]);
  for (int nf=0;nf<NFACES;nf++) theNormals->vector.set1Value(nf,normals[nf][0],normals[nf][1],normals[nf][2]);
  theNormalBinding->value=SoNormalBinding::PER_FACE;
}

// generateChildren
void SoTubs::generateChildren() {

  // This routines creates one SoSeparator, one SoCoordinate3, and
  // one SoLineSet, and puts it in the child list.  This is done only
  // once, whereas redrawing the position of the coordinates occurs each
  // time an update is necessary, in the updateChildren routine.

  assert(m_children->getLength() ==0);
  SoSeparator      *sep              = new SoSeparator();
  SoCoordinate3    *theCoordinates   = new SoCoordinate3();
  SoNormal         *theNormals       = new SoNormal();
  SoNormalBinding  *theNormalBinding = new SoNormalBinding();
  SoIndexedFaceSet *theFaceSet       = new SoIndexedFaceSet();
  //
  // This line costs some in render quality! but gives speed.
  //
  sep->addChild(theCoordinates);
  sep->addChild(theNormals);
  sep->addChild(theNormalBinding);
  sep->addChild(theFaceSet);
  m_children->append(sep);
}

// generateAlternateRep
void SoTubs::generateAlternateRep() {

  // This routine sets the alternate representation to the child
  // list of this mode.

  if (m_children->getLength() == 0) generateChildren();
  updateChildren();
  alternateRep.setValue((SoSeparator *)  ( *m_children)[0]);
}

// clearAlternateRep
void SoTubs::clearAlternateRep() {
  alternateRep.setValue(NULL);
}
