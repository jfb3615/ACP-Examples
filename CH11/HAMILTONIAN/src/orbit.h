#ifndef _orbit_h_
#define _orbit_h_
#include "QatGenericFunctions/AbsFunction.h"
#include <QApplication>
void orbit ( QApplication &app, Genfun::GENFUNCTION f1, Genfun::GENFUNCTION f2,
	     std::string title="Orbit", std::string xLabel="x", std::string yLabel="y");
#endif
