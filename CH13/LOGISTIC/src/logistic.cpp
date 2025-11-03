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
class SignalCatcher:public QObject {
  Q_OBJECT

public:
  
  SignalCatcher (PlotView * view):view(view){}

private:
  PlotView *view{nullptr};

public slots:

  void update(double value);

private:

  Hist1D h1{1000,0.0,1.0};
  PlotHist1D p1{h1};
};

void SignalCatcher::update(double mu) {
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


  
  PlotView view({0,1,1,12000000});
  view.setLogY(true);
  window.add(&view,"Density");
  

  SignalCatcher signalCatcher(&view);
  
  QObject::connect(muSpinBox, &QDoubleSpinBox::valueChanged, &signalCatcher, &SignalCatcher::update);

  
  PlotStream titleStream(view.titleTextEdit());
  titleStream << PlotStream::Clear()
	      << PlotStream::Center() 
	      << PlotStream::Family("Arial") 
	      << PlotStream::Size(16)
	      << PlotStream::EndP();
  
  
  PlotStream xLabelStream(view.xLabelTextEdit());
  xLabelStream << PlotStream::Clear()
	       << PlotStream::Center()
	       << PlotStream::Family("Arial")
	       << PlotStream::Size(16)
	       << PlotStream::EndP();
  
  PlotStream yLabelStream(view.yLabelTextEdit());
  yLabelStream << PlotStream::Clear()
	       << PlotStream::Center()
	       << PlotStream::Family("Arial")
	       << PlotStream::Size(16)
	       << PlotStream::EndP();
  
  
  
  window.show();
  app.exec();
  return 1;
}

#include "logistic.moc"

