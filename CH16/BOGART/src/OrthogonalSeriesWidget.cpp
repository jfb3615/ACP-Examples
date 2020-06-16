#include "OrthogonalSeriesWidget.h"
#include "QatPlotting/PlotStream.h"

class OrthogonalSeriesWidget::Clockwork{
};
OrthogonalSeriesWidget::OrthogonalSeriesWidget(QWidget *parent):
  QMainWindow(parent),c(new Clockwork)
{
  ui.setupUi(this);

  ui.actionQuit->setShortcut(QKeySequence("q"));
  connect(ui.actionQuit,SIGNAL(triggered()), qApp, SLOT(quit()));

  ui.xView->setGrid(false);
  ui.xView->setXZero(false);
  ui.xView->setYZero(false);

  ui.yView->setGrid(false);
  ui.yView->setXZero(false);
  ui.yView->setYZero(false);
  {
    PlotStream titleStream(ui.xView->titleTextEdit());
    titleStream << PlotStream::Clear()
		<< PlotStream::Center() 
		<< PlotStream::Family("Sans Serif") 
		<< PlotStream::Size(16)
		<< "Projection into X"
		<< PlotStream::EndP();
    
    
    PlotStream xLabelStream(ui.xView->xLabelTextEdit());
    xLabelStream << PlotStream::Clear()
		 << PlotStream::Center()
		 << PlotStream::Family("Sans Serif")
		 << PlotStream::Size(16)
		 << "x"
		 << PlotStream::EndP();
    
    PlotStream yLabelStream(ui.xView->yLabelTextEdit());
    yLabelStream << PlotStream::Clear()
		 << PlotStream::Center()
		 << PlotStream::Family("Symbol")
		 << PlotStream::Size(16)
		 << "r"
		 << PlotStream::Family("Sans Serif")
		 << "(x)"
		 << PlotStream::EndP();
  }
  {
    PlotStream titleStream(ui.yView->titleTextEdit());
    titleStream << PlotStream::Clear()
		<< PlotStream::Center() 
		<< PlotStream::Family("Sans Serif") 
		<< PlotStream::Size(16)
		<< "Projection into Y"
		<< PlotStream::EndP();
    
    
    PlotStream xLabelStream(ui.yView->xLabelTextEdit());
    xLabelStream << PlotStream::Clear()
		 << PlotStream::Center()
		 << PlotStream::Family("Sans Serif")
		 << PlotStream::Size(16)
		 << "y"
		 << PlotStream::EndP();
    
    PlotStream yLabelStream(ui.yView->yLabelTextEdit());
    yLabelStream << PlotStream::Clear()
		 << PlotStream::Center()
		 << PlotStream::Family("Symbol")
		 << PlotStream::Size(16)
		 << "r"
		 << PlotStream::Family("Sans Serif")
		 << "(y)"
		 << PlotStream::EndP();
    
  }
}

OrthogonalSeriesWidget::~OrthogonalSeriesWidget() {
  delete c;
}

F2DViewerWidget *OrthogonalSeriesWidget::getOriginal()    { return ui.fvOriginal;}
F2DViewerWidget *OrthogonalSeriesWidget::getRealization() { return ui.fvRealization;}
F2DViewerWidget *OrthogonalSeriesWidget::getFit()         { return ui.fvFit;}
PlotView        *OrthogonalSeriesWidget::getXView()       { return ui.xView;     }
PlotView        *OrthogonalSeriesWidget::getYView()       { return ui.yView;     }
