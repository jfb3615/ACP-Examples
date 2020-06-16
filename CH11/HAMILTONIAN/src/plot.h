#ifndef _plot_h_
#define _plot_h_
#include "QatGenericFunctions/AbsFunction.h"
#include <QApplication>
void plot ( QApplication &app, 
	    Genfun::GENFUNCTION x0,
	    Genfun::GENFUNCTION x1,
	    std::string title="X & P",
	    std::string xLabel="t",
	    std::string yLabel="coord & momenta");
void plot ( QApplication &app, 
	    Genfun::GENFUNCTION x,
	    std::string title="",
	    std::string xLabel="t",
	    std::string yLabel=""
	    );
#endif
