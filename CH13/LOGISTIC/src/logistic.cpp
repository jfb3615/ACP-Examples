#include "QatPlotWidgets/PlotView.h"
#include "QatPlotWidgets/MultipleViewWindow.h"
#include "QatPlotting/PlotStream.h"
#include "QatDataAnalysis/Hist1D.h"
#include "QatPlotting/PlotHist1D.h"
#include <QApplication>
#include <QMainWindow>
#include <QToolBar>
#include <QLabel>
#include <QAction>
#include <QDoubleSpinBox>
#include <cstdlib>
#include <iostream>
#include <string>


class LogisticCatcher:public QObject {
  Q_OBJECT

public:
  
  LogisticCatcher (PlotView * view):view(view){}

private:
  PlotView *view{nullptr};

public slots:

  void update(double value);

private:

  Hist1D h1{1000,0.0,1.0};
  PlotHist1D p1{h1};
};

void LogisticCatcher::update(double mu) {
  h1.clear();
  double x{0.5};
  for (int i=0;i<10000000;i++) {
    x = mu*x*(1.0-x);
    h1.accumulate(x);
  }
  view->clear();
  view->add(&p1);
  view->recreate();
}


class ExponentialCatcher:public QObject {
  Q_OBJECT

public:
  
  ExponentialCatcher (PlotView * view):view(view){}

private:
  PlotView *view{nullptr};

public slots:

  void update(double value);

private:

  Hist1D h1{1000,0.0,16};
  PlotHist1D p1{h1};
};

void ExponentialCatcher::update(double lambda) {
  h1.clear();
  double x{0.5};
  for (int i=0;i<10000000;i++) {
    x = lambda*x*std::exp(-x);
    h1.accumulate(x);
  }
  view->clear();
  view->add(&p1);
  view->recreate();
}


class CosineCatcher:public QObject {
  Q_OBJECT

public:
  
  CosineCatcher (PlotView * view):view(view){}

private:
  PlotView *view{nullptr};

public slots:

  void update(double value);

private:

  Hist1D h1{1000,0.0,3};
  PlotHist1D p1{h1};
};

void CosineCatcher::update(double a) {
  h1.clear();
  double x{0.5};
  for (int i=0;i<10000000;i++) {
    x = a*x*std::cos(x);
    h1.accumulate(x);
  }
  view->clear();
  view->add(&p1);
  view->recreate();
}


int main (int argc, char * * argv) {

  // Automatically generated:-------------------------:

  std::string usage= std::string("usage: ") + argv[0]; 
  if (argc!=1) {
    std::cout << usage << std::endl;
  }


  QApplication     app(argc,argv);
  
  MultipleViewWindow window;
  QToolBar *toolBar=window.addToolBar("Tools");
  QAction  *quitAction=toolBar->addAction("Quit");
  
  quitAction->setShortcut(QKeySequence("q"));
  
  QObject::connect(quitAction, &QAction::triggered, &app, &QApplication::quit);

  QLabel   *label = new QLabel("mu");
  toolBar->addWidget(label);
  QDoubleSpinBox *muSpinBox=new QDoubleSpinBox;
  muSpinBox->setSingleStep(0.01);
  muSpinBox->setMaximum(4.0);
  toolBar->addWidget(muSpinBox);

  QLabel   *label2 = new QLabel("lambda");
  toolBar->addWidget(label2);
  QDoubleSpinBox *lambdaSpinBox=new QDoubleSpinBox;
  lambdaSpinBox->setSingleStep(0.4);
  lambdaSpinBox->setMaximum(40.0);
  toolBar->addWidget(lambdaSpinBox);

  QLabel   *label3 = new QLabel("a");
  toolBar->addWidget(label3);
  QDoubleSpinBox *aSpinBox=new QDoubleSpinBox;
  aSpinBox->setSingleStep(0.01);
  aSpinBox->setMaximum(4.0);
  toolBar->addWidget(aSpinBox);


  
  PlotView viewLogistic({0,1,1,1200000});
  viewLogistic.setLogY(true);
  window.add(&viewLogistic,"Logistic");
  
  PlotView viewExponential({0,16,1,1200000});
  viewExponential.setLogY(true);
  window.add(&viewExponential,"Exponential");

  PlotView viewCosine({0,3,1,1200000});
  viewCosine.setLogY(true);
  window.add(&viewCosine,"Cosine");


  LogisticCatcher logisticCatcher(&viewLogistic);
  QObject::connect(muSpinBox, &QDoubleSpinBox::valueChanged, &logisticCatcher, &LogisticCatcher::update);

  ExponentialCatcher exponentialCatcher(&viewExponential);
  QObject::connect(lambdaSpinBox, &QDoubleSpinBox::valueChanged, &exponentialCatcher, &ExponentialCatcher::update);


  CosineCatcher cosineCatcher(&viewCosine);
  QObject::connect(aSpinBox, &QDoubleSpinBox::valueChanged, &cosineCatcher, &CosineCatcher::update);

std::map<PlotView *,std::string> title=
    {{&viewLogistic,"f(x)=μ᛫x᛫(1-x)"},
     {&viewExponential, "f(x)=λ᛫x᛫exp(-x)"},
     {&viewCosine, "f(x)=a᛫x᛫cos(x)"}
    };

  for (PlotView *view : {&viewLogistic,&viewExponential,&viewCosine}) {
    PlotStream titleStream(view->titleTextEdit());
    titleStream << PlotStream::Clear()
		<< PlotStream::Center() 
		<< PlotStream::Family("Arial") 
		<< PlotStream::Size(16)
		<< title[view]
		<< PlotStream::EndP();
    
    
    PlotStream xLabelStream(view->xLabelTextEdit());
    xLabelStream << PlotStream::Clear()
		 << PlotStream::Center()
		 << PlotStream::Family("Arial")
		 << PlotStream::Size(16)
		 << "points/bin"
		 << PlotStream::EndP();
    
    PlotStream yLabelStream(view->yLabelTextEdit());
    yLabelStream << PlotStream::Clear()
		 << PlotStream::Center()
		 << PlotStream::Family("Arial")
		 << PlotStream::Size(16)
		 << "x"
	       << PlotStream::EndP();
    
  }
  
  window.show();
  app.exec();
  return 1;
}

#include "logistic.moc"

