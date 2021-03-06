//
//  image2d.h
//  
//
//  Created by Ying-Yu Ho on 10/29/13.
//
//

#ifndef _image2d_h
#define _image2d_h

#include <list>
#include "matrix.h"

typedef Matrix<double,2,1> Vec2;
typedef std::list<Vec2*> Polygon2;
typedef std::list<Polygon2*> Image2;

#endif //   _image2d_h