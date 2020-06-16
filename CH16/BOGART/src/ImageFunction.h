//-----------------------------------------------------------//
//                                                           //
// This function presents an image as a two-dimensional      //
// function.                                                 //
// ----------------------------------------------------------// 
#ifndef _IMAGEFUNCTION_H_
#define _IMAGEFUNCTION_H_
#include "QatGenericFunctions/AbsFunction.h"
#include <QPixmap>
#include <QImage>
#include <stdexcept>
namespace Genfun {
  class ImageFunction:public AbsFunction {

    // Required macro:
    FUNCTION_OBJECT_DEF(ImageFunction)

  public:

    // The contructor
    ImageFunction(const std::string & imageFileName);

    // The copy constructor
    ImageFunction(const ImageFunction & source)=default;

    // The destructor
    ~ImageFunction()=default;

    // This form of the function call operator cannot be used:
    double operator() (double ) const {
      throw std::runtime_error("ImageFunction:  dimensionality mismatch");
      return 0;
    }

    // Function call operator, requires two dimensions
    double operator() (const Argument & a) const;

    // Accessors:
    
    // The pixmap:
    const QPixmap *pixmap() const;
    QPixmap *pixmap(); 

    // The image:
    const QImage *image() const;
    QImage *image(); 
    
    // The dimensions:
    unsigned int xmax() const;
    unsigned int ymax() const;

    // Deleted operations:    
    ImageFunction & operator=(const ImageFunction &) = delete;

  private:

    QPixmap _pixmap;
    QImage  _image;
  };
}
#endif
