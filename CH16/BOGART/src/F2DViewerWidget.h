#ifndef _F2DVIEWERWIDGET_H_
#define _F2DVIEWERWIDGET_H_
#include "QatGenericFunctions/AbsFunction.h"
#include <QWidget>
#include <stdexcept>
class F2DViewerWidget:public QWidget {

  Q_OBJECT

public:

  F2DViewerWidget(QWidget *parent=NULL,
		  unsigned int NX=100, double XMIN=0, double XMAX=1, 
		  unsigned int NY=100, double YMIN=0, double YMAX=1);
  ~F2DViewerWidget();

  // Add a point to the display.  
  void addPoint(double x, double y, double z);

  // Should be a function of two variables:
  void setFunction (const Genfun::AbsFunction *F); 

  void setSize(unsigned int NX=100, double XMIN=0, double XMAX=1, 
	       unsigned int NY=100, double YMIN=0, double YMAX=1);

public slots:

  void capture(); 

private:

  class Clockwork;
  Clockwork *c;

  // Do not copy or assign:
  F2DViewerWidget(const F2DViewerWidget &)=delete;
  F2DViewerWidget & operator = (const F2DViewerWidget &)=delete;

};
#endif
