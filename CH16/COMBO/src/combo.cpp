#include "QatPlotting/PlotStream.h"
#include "QatPlotWidgets/MultipleViewWindow.h"
#include "QatPlotWidgets/PlotView.h"
#include "QatDataAnalysis/Hist1D.h"
#include "QatPlotting/PlotHist1D.h"
#include "QatPlotting/PlotProfile.h"
#include "QatPlotting/PlotFunction1D.h"
#include "QatPlotting/PlotErrorEllipse.h"
#include "QatPlotting/RealArg.h"
#include "QatDataModeling/MinuitMinimizer.h"
#include "QatGenericFunctions/AbsFunctional.h"
#include "QatGenericFunctions/Variable.h"
#include "QatGenericFunctions/Parameter.h"
#include <QApplication>
#include <QAction>
#include <QToolBar>
#include "Eigen/Dense"
#include <random>
#include <map>

class SignalCatcher:public QObject {
Q_OBJECT
public slots:
  void quit() const { exit(0);}
};

#include "combo.moc"

class ChiSq:public Genfun::AbsFunctional {
public:

  // Constructor
  ChiSq(){}

  // Destructor:
  ~ChiSq(){}
  
  // Function Call Operator
  virtual double operator () (const Genfun::AbsFunction & f) const {
    double c2=0.0;
    for (unsigned int i=0;i<_points.size();i++) {
      point p = _points[i];
      c2 += (p.y-f(p.x))*(p.y-f(p.x))/p.dy/p.dy;
    }
    return c2;
  }

  // Add a data point:
  void addPoint (double x, double y, double dy) {
    point p;
    p.x=x;
    p.y=y;
    p.dy=dy;
    _points.push_back(p);
  }
  
private:
  struct point {
    double x;
    double y;
    double dy;
  };
  
  std::vector<point> _points;

};

int main(int argc, char ** argv ) {

  QApplication app(argc, argv);

  MultipleViewWindow window;
  
  QToolBar *toolBar=window.addToolBar("Tools");

  QAction  *nextAction=toolBar->addAction("Next");
  nextAction->setShortcut(QKeySequence("n"));
  QObject::connect(nextAction, SIGNAL(triggered()), &app, SLOT(quit()));

  SignalCatcher catcher;
  QAction  *quitAction=toolBar->addAction("Quit");
  quitAction->setShortcut(QKeySequence("q"));
  QObject::connect(quitAction, SIGNAL(triggered()), &catcher, SLOT(quit()));
  
  
  PRectF   rect1;
  rect1.setXmin(-12);
  rect1.setXmax(12);
  rect1.setYmin   (-110);
  rect1.setYmax( 110);
  PlotView view1(rect1);
  
  PRectF rect2;
  rect2.setXmin(9);
  rect2.setXmax(12);
  rect2.setYmin(-10);
  rect2.setYmax(10);
  PlotView view2(rect2);

  std::map<PlotView *, std::string> tmap=
    {{&view1, "Two segments; two line fits"},
     {&view2, "Fit results and combination"}};
  
  std::map<PlotView *, std::string> xmap=
    {{&view1, "x"},
     {&view2, "slope m"}};

  std::map<PlotView *, std::string> ymap=
    {{&view1, "y"},
     {&view2, "intercept b"}};

    
  for (PlotView * view : {&view1, &view2} ) {
    view->setBox(false);
    view->setGrid(false);
    view->setXZero(false);
    view->setYZero(false);
    PlotStream titleStream(view->titleTextEdit());
    titleStream << PlotStream::Clear()
		<< PlotStream::Center()
		<< PlotStream::Family("Sans Serif")
		<< PlotStream::Size(16)
		<< tmap[view]
		<< PlotStream::EndP();
    PlotStream xLabelStream(view->xLabelTextEdit());
    xLabelStream << PlotStream::Clear()
		 << PlotStream::Center()
		 << PlotStream::Family("Sans Serif")
		 << PlotStream::Size(16)
		 << xmap[view]
		 << PlotStream::EndP();
    PlotStream yLabelStream(view->yLabelTextEdit());
    yLabelStream << PlotStream::Clear()
		 << PlotStream::Center()
		 << PlotStream::Family("Sans Serif")
		 << PlotStream::Size(16)
		 << ymap[view]
		 << PlotStream::EndP();
  }

  window.add(&view1, "Raw data");
  window.add(&view2, "Fit result");
  

  // MAKE RANDOM NUMBER GENERATORS
  std::mt19937 engine;
  std::normal_distribution<double> gauss;

  while (1) {
    // MAKE TWO PROFILE PLOTS
    PlotProfile profilePlot1;
    PlotProfile profilePlot2;
    
    {
      PlotProfile::Properties prop;
      prop.symbolSize=4;
      prop.brush.setStyle(Qt::SolidPattern);
      profilePlot1.setProperties(prop);
    }
    
    
    {
      PlotProfile::Properties prop;
      prop.symbolSize=4;
      prop.brush.setStyle(Qt::SolidPattern);
      profilePlot2.setProperties(prop);
    }
    
    
    ChiSq chisq1,chisq2;
    {
      // CARRY OUT THE FIRST PSEUDO EXPERIMENT
      double g =  +10.0;
      double v0 = +0.0;
      for (double t=-10; t<-5; t += 0.5) {
	double v = v0 + g*t;
	double dv = 2.0*gauss(engine);
	QPointF point(t,v+dv);
	profilePlot1.addPoint(point,2.0);
	chisq1.addPoint(t, v+dv, 2.0);
      }
      // CARRY OUT THE SECOND PSEUDO EXPERIMENT
      for (double t=5; t<10; t += 0.5) {
	double v = v0 + g*t;
	double dv = 2.0*gauss(engine);
	QPointF point(t,v+dv);
	profilePlot2.addPoint(point,2.0);
	chisq2.addPoint(t, v+dv, 2.0);
      }
    }
    
    Eigen::Vector2d A1=Eigen::Vector2d::Zero(),A2=Eigen::Vector2d::Zero();
    Eigen::Matrix2d C1=Eigen::Matrix2d::Zero(),C2=Eigen::Matrix2d::Zero();
    
    
    // SET THE PARAMETERS OF A FIT:
    Genfun::Parameter g_1("SLOPE 1", 9,   7, 11);
    Genfun::Parameter v0_1  ("INT1   ", 0, -10, 10);
    
    
    // MAKE A MODEL FOR THE DATA:
    Genfun::Variable T;
    Genfun::GENFUNCTION F_1= g_1*T+v0_1;
    
    
    // MAKE A MINIMIZER:
    MinuitMinimizer minimizer1(false);
    minimizer1.addParameter(&g_1);
    minimizer1.addParameter(&v0_1);
    minimizer1.addStatistic(&chisq1, &F_1);
    minimizer1.minimize();
    A1[0]=minimizer1.getValue(&g_1);
    A1[1]=minimizer1.getValue(&v0_1);
    C1(0,0)=minimizer1.getError(&g_1,&g_1);
    C1(1,1)=minimizer1.getError(&v0_1,&v0_1);
    C1(0,1)=minimizer1.getError(&g_1,&v0_1);
    C1(1,0)=minimizer1.getError(&v0_1,&g_1);
    
    PlotFunction1D v1(F_1, RealArg::Gt(-10.0) && RealArg::Lt(-5.0));
    {
      PlotFunction1D::Properties prop;
      prop.pen.setWidth(3);
      v1.setProperties(prop);
    }
    
    // SET THE PARAMETERS OF A FIT:
    Genfun::Parameter g_2("SLOPE 2", 9,   7, 11);
    Genfun::Parameter v0_2  ("INT2   ", 0, -10, 10);
    
    
    // MAKE A MODEL FOR THE DATA:
    Genfun::GENFUNCTION F_2= g_2*T+v0_2;
    
    // MAKE A MINIMIZER:
    MinuitMinimizer minimizer2(false);
    minimizer2.addParameter(&g_2);
    minimizer2.addParameter(&v0_2);
    minimizer2.addStatistic(&chisq2, &F_2);
    minimizer2.minimize();
    A2[0]=minimizer2.getValue(&g_2);
    A2[1]=minimizer2.getValue(&v0_2);
    C2(0,0)=minimizer2.getError(&g_2,&g_2);
    C2(1,1)=minimizer2.getError(&v0_2,&v0_2);
    C2(0,1)=minimizer2.getError(&g_2,&v0_2);
    C2(1,0)=minimizer2.getError(&v0_2,&g_2);
    
    PlotFunction1D v2(F_2, RealArg::Gt(5.0) && RealArg::Lt(10.0));
    {
      PlotFunction1D::Properties prop;
      prop.pen.setWidth(3);
      v2.setProperties(prop);
    }
    
    
    view1.add(&profilePlot1);
    view1.add(&profilePlot2);
    view1.add(&v1);
    view1.add(&v2);
    
    
    
    PlotErrorEllipse e1(A1[0], A1[1], C1(0,0), C1(1,1), C1(0,1), PlotErrorEllipse::ONESIGMA);
    {
      PlotErrorEllipse::Properties prop;
      QColor darkX("darkGray");
      darkX.setAlpha(128);
      prop.brush.setStyle(Qt::SolidPattern);
      prop.brush.setColor(darkX);
      e1.setProperties(prop);
    }
    
    PlotErrorEllipse e2(A2[0], A2[1], C2(0,0), C2(1,1), C2(0,1), PlotErrorEllipse::ONESIGMA);
    {
      PlotErrorEllipse::Properties prop;
      QColor darkX("darkGray");
      darkX.setAlpha(128);
      prop.brush.setStyle(Qt::SolidPattern);
      prop.brush.setColor(darkX);
      e2.setProperties(prop);
    }
    
    Eigen::Matrix2d C=(C1.inverse() + C2.inverse()).inverse();
    
    Eigen::Vector2d A = C*(C1.inverse()*A1 + C2.inverse()*A2);
    
    PlotErrorEllipse e(A[0], A[1], C(0,0), C(1,1), C(0,1), PlotErrorEllipse::ONESIGMA);
    {
      PlotErrorEllipse::Properties prop;
      prop.brush.setStyle(Qt::SolidPattern);
      e.setProperties(prop);
    }
    
    
    view2.add(&e1);
    view2.add(&e2);
    view2.add(&e);
    window.show();

    app.exec();
    view1.clear();
    view2.clear();
  }
  
  return 0;

}
