#include "EulerForm.h"
#include <iostream>
#include <stdexcept>


class EulerForm::Clockwork {
  
public:
  Clockwork();
};


EulerForm::Clockwork::Clockwork()
{

  
}

EulerForm::EulerForm(QWidget *parent):QWidget(parent),c(new Clockwork)
{
  ui.setupUi(this);

}





 EulerForm::~EulerForm() {
   delete c;
 }




