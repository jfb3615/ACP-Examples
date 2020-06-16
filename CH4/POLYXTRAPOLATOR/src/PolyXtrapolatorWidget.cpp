#include "PolyXtrapolatorWidget.h"
#include "QatGenericFunctions/InterpolatingPolynomial.h"
#include "QatPlotting/PlotPoint.h"
#include "QatPlotting/PlotFunction1D.h"
#include "QatPlotting/VisFunction.h"
#include <QInputDialog>
#include <QColor>
#include <iostream>
#include <set>
#include <map>
#include <vector>
#include <libgen.h>
#include <dlfcn.h>
#include <sstream>
#include <stdexcept>
const QColor color[]= {"black","black", "black"};
const unsigned int cSize=sizeof(color)/sizeof(QColor);
 
class Window:public Cut<double> {
public:

  Window(double min, double max):Cut<double>(), min(min), max(max) {};

  virtual bool operator () (const double & x) const {
    return x>min && x<max;
  }

  virtual Window *clone() const {
    return new Window(*this);
  }
private:
  double min;
  double max;
};

class PolyXtrapolatorWidget::Clockwork {

 public:

  struct Point{
    Point(double x, double y):x(x),y(y) {}
    double x;
    double y;
    bool operator < (const Point & p) const { return x<p.x;}
  };
    
  QListWidgetItem *currentItem;

  std::multimap<QListWidgetItem *, Point>       pointMap;
  std::multimap<QListWidgetItem *, PlotPoint *> plPtMap;
  std::vector<PlotFunction1D *>                 fcnVec;
  unsigned int counter;
  VisFunction                                  *ff;
};

PolyXtrapolatorWidget::PolyXtrapolatorWidget(QWidget *parent, const std::string  filename):QMainWindow(parent),c(new Clockwork)
{
  c->currentItem=NULL;
  c->counter=0;
  ui.setupUi(this);
  
  ui.plotView->setBox(false);
  ui.plotView->setGrid(false);
  ui.plotView->setXZero(false);
  ui.plotView->setYZero(false);
  
  connect(ui.actionQuit,SIGNAL(triggered()), this, SLOT(quit()));
  connect(ui.actionNewInterpolation,SIGNAL(triggered()), this, SLOT(add()));
  connect(ui.actionDeleteInterpolation,SIGNAL(triggered()), this, SLOT(del()));
  connect(ui.listWidget,SIGNAL(itemSelectionChanged()), this, SLOT(functionSelect()));
  connect(ui.listWidget,SIGNAL(itemSelectionChanged()), this, SLOT(functionSelect()));
  connect(ui.plotView, SIGNAL(pointChosen(double, double)), this, SLOT(point(double, double)));
  connect(ui.actionInterpolate,SIGNAL(triggered()), this, SLOT(refresh()));
  connect(ui.actionExtrapolate,SIGNAL(triggered()), this, SLOT(refresh()));

  if (filename!="") {

    void *handle = dlopen(filename.c_str(),RTLD_NOW);
    if (!handle) throw std::runtime_error(std::string(dlerror()));
 
    std::string base = basename((char *) filename.c_str());
    int pos = base.find(".so");
    base=base.substr(0,pos);
   
    typedef void * (*CreationMethod) ();
    std::ostringstream createFunctionName;
    createFunctionName << "create_"<< base;
    CreationMethod createMethod;
    union { CreationMethod func; void* data; } sym;
    sym.data=  dlsym(handle, createFunctionName.str().c_str());
    createMethod = sym.func;
    if (!createMethod)  {
       std::string msg=  std::string("Warning, cannot load function ");
       msg += base;
       msg += std::string(" from file ") + filename + ": " ;
       msg += std::string(dlerror());
       throw std::runtime_error (msg);
    }
    c->ff =  (VisFunction *) (*createMethod)();
    if (c->ff) ui.plotView->setRect(c->ff->rectHint());
  }
  else {
    c->ff=NULL;
  }
  refresh();
}
void PolyXtrapolatorWidget::add() {
  QString function=QInputDialog::getText(this, "Interpolating Function", "Input Function Name");
  if (function.size()!=0) {
    ui.listWidget->addItem(function);
    
    // Set the color:
    unsigned int currentCount=ui.listWidget->count();
    QListWidgetItem *item = ui.listWidget->item(currentCount-1);
    if (item) {
      QBrush brush=item->foreground();
      brush.setColor(color[(c->counter++)%cSize]);
      item->setForeground(brush);
    }
  }
 
}

void PolyXtrapolatorWidget::del() {
  if (c->currentItem) {
    c->pointMap.erase(c->pointMap.lower_bound(c->currentItem),c->pointMap.upper_bound(c->currentItem));
    c->plPtMap.erase(c->plPtMap.lower_bound(c->currentItem),c->plPtMap.upper_bound(c->currentItem));
    delete c->currentItem;
    c->currentItem=NULL;
  }
  functionSelect();
  refresh();
}

void PolyXtrapolatorWidget::quit() {
  exit(0);
}

void PolyXtrapolatorWidget::functionSelect() {
  QList<QListWidgetItem*> sItems = ui.listWidget->selectedItems();
  if (sItems.empty()) {
    ui.actionDeleteInterpolation->setEnabled(false);
    ui.actionInterpolate->setEnabled(false);
    ui.actionExtrapolate->setEnabled(false);
  }
  else {
    ui.actionDeleteInterpolation->setEnabled(true);
    ui.actionInterpolate->setEnabled(true);
    ui.actionExtrapolate->setEnabled(true);
    c->currentItem=sItems.at(0);
  }
 }

 PolyXtrapolatorWidget::~PolyXtrapolatorWidget() {
   for (unsigned int i=0;i<c->fcnVec.size();i++) delete c->fcnVec[i];
   for (std::multimap<QListWidgetItem *, PlotPoint *>::iterator p=c->plPtMap.begin();p!=c->plPtMap.end();p++) {
     delete (*p).second;
   }
   delete c;
 }


void PolyXtrapolatorWidget::point(double x, double y) {
  if (ui.actionInterpolate->isEnabled()||ui.actionExtrapolate->isEnabled()) {

    if (c->ff) {
      if (c->ff->getNumFunctions()==1) {
	y = (*c->ff->getFunction(0))(x);
      }
    }


    if (c->currentItem) { // Redundant check?

      // Insert new point:
      c->pointMap.insert(
		    std::pair<QListWidgetItem *, Clockwork::Point> 
		    (c->currentItem, Clockwork::Point(x,y))
		    );

      // Insert its graphical doppelgaenger:
      PlotPoint * plPt = new PlotPoint(x,y);
      {
	PlotPoint::Properties prop;
	prop.symbolSize=8;   
	prop.brush.setStyle(Qt::SolidPattern);
	prop.brush.setColor(c->currentItem->foreground().color());
	plPt->setProperties(prop);
      }
      c->plPtMap.insert(
		    std::pair<QListWidgetItem *, PlotPoint *> 
		    (c->currentItem, plPt)
			 );
      refresh();
    }
  }
}

void PolyXtrapolatorWidget::refresh() {

  

  ui.plotView->clear();
  ui.tableWidget->setRowCount(0);
  ui.tableWidget->clear();
  for (unsigned int i=0;i<c->fcnVec.size();i++) delete c->fcnVec[i];
  c->fcnVec.erase(c->fcnVec.begin(), c->fcnVec.end());


  // Points:
  for ( std::multimap<QListWidgetItem *, PlotPoint *>::const_iterator m=c->plPtMap.begin();
	m!= c->plPtMap.end();m++) {
    ui.plotView->add((*m).second);
  }

  // And also add it to the list
  {
    std::multimap<QListWidgetItem *, Clockwork::Point>::const_iterator begin=c->pointMap.begin(),
      end=c->pointMap.end(), mx = begin==end? end : c->pointMap.upper_bound((*begin).first), cx=begin;
    while (cx!=end)  {
      std::set<Clockwork::Point> sortedSet;
      QColor color = (*cx).first->foreground().color();
      while (cx!=mx) {
	sortedSet.insert((*cx).second);
	cx++;
      }
      for (std::set<Clockwork::Point>::const_iterator s=sortedSet.begin();s!=sortedSet.end();s++) {
	std::ostringstream streamx,streamy;
	int nrows = ui.tableWidget->rowCount();
	streamx << (*s).x;
	streamy << (*s).y;
	ui.tableWidget->setRowCount(nrows+1);
	QTableWidgetItem *xItem = new QTableWidgetItem (streamx.str().c_str());
	QTableWidgetItem *yItem = new QTableWidgetItem (streamy.str().c_str());
	QBrush xBrush=xItem->foreground(), yBrush=yItem->foreground();
	xBrush.setColor(color);
	yBrush.setColor(color);
	xItem->setForeground(xBrush);
	yItem->setForeground(yBrush);
	ui.tableWidget->setItem(nrows,0,xItem);
	ui.tableWidget->setItem(nrows,1,yItem);
      }
      mx=c->pointMap.upper_bound((*cx).first);
    }
  }
  
  // Plots:
  if (ui.actionInterpolate->isChecked()||ui.actionExtrapolate->isChecked()) {
    std::multimap<QListWidgetItem *, Clockwork::Point>::const_iterator begin=c->pointMap.begin(),
      end=c->pointMap.end(), mx = begin==end? end : c->pointMap.upper_bound((*begin).first),cx=begin;
    while (cx!=end) {

      Genfun::InterpolatingPolynomial IP;
      PlotFunction1D::Properties prop;
      prop.pen.setColor((*cx).first->foreground().color());
      prop.pen.setWidth(4);
      while (cx!=mx) {
	IP.addPoint((*cx).second.x,(*cx).second.y);
	cx++;
      }
      double minX,maxX;
      IP.getRange(minX,maxX);
      const Cut<double> & w    =Window(minX,maxX);
      const Cut<double> & wBar = !w;
      if (ui.actionInterpolate->isChecked()) {
	PlotFunction1D *pf=new PlotFunction1D(IP,w);
	pf->setProperties(prop);
	ui.plotView->add(pf);
	c->fcnVec.push_back(pf);
      }
      if (ui.actionExtrapolate->isChecked()) {
	PlotFunction1D *pf=new PlotFunction1D(IP,wBar);
	pf->setProperties(prop);
	ui.plotView->add(pf);
	c->fcnVec.push_back(pf);
      }
      mx=c->pointMap.upper_bound((*cx).first);
    }
  }
  if (c->ff) {
    for (unsigned int i=0;i<c->ff->getNumFunctions();i++) {
      const Genfun::AbsFunction *gf = c->ff->getFunction(i);
      PlotFunction1D *pf=new PlotFunction1D(*gf);
      ui.plotView->add(pf);
      c->fcnVec.push_back(pf);
    }
  }
}
