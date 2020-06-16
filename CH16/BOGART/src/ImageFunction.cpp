#include "ImageFunction.h"
namespace Genfun {

  // Required macro:
  FUNCTION_OBJECT_IMP(ImageFunction)


  ImageFunction::ImageFunction(const std::string & fileName):
  _pixmap(fileName.c_str()),_image(_pixmap.toImage())
  {
  }
  
  
  // Function call operator.  Here we evaluate the function given
  // the solution which was calculated in the constructor. 
  double ImageFunction::operator() (const Argument & a) const {
    double x=a[0],y=a[1];
    y=_image.height()-y;
    if (x<0 || x>double(_image.width()-1) || 
	y<0 || y>double(_image.height()-1) ) return 0;
    
    return qGray(_image.pixel(int(x),int(y)))/255.0;
  }
  
  const QPixmap * ImageFunction::pixmap() const{
    return &_pixmap;
  }
  
  QPixmap * ImageFunction::pixmap(){
    return &_pixmap;
  } 
  
  const QImage * ImageFunction::image() const{
    return &_image;
  }
  
  QImage * ImageFunction::image(){
    return &_image;
  }
  
  unsigned int  ImageFunction::xmax() const{
    return _image.width();
  }
  
  unsigned int  ImageFunction::ymax() const{
    return _image.height();
  }
}
