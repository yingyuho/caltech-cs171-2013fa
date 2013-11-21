//
//  pixel.h
//  
//  
//
//  Created by Ying-Yu Ho on 11/17/13.
//
//

#ifndef _pixel_h
#define _pixel_h

#include "matrix.h"

class Pixel {
private:
    int _x;
    int _y;
    double _z;
    int _r; int _g; int _b;
public:
    Pixel(int x, int y, double z, int r, int g, int b) : _x(x), _y(y), _z(z), _r(r), _g(g), _b(b) {}
    virtual ~Pixel() {};
    int x() const { return _x; }
    int y() const { return _y; }
    double z() const { return _z; }
    int r() const { return _r; }
    int g() const { return _g; }
    int b() const { return _b; }
};

#endif //   _pixel_h