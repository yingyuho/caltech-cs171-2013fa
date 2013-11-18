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

// class SimplePixel : public Pixel {
// private:
//     int x;
//     int y;
//     double z;
//     int r; int g; int b;
// public:
//     SimplePixel(int x, int y, double z, int r, int g, int b) : x(x), y(y), z(z), r(r), g(g), b(b) {}
//     virtual ~SimplePixel() {}
//     virtual int x() const { return x; }
//     virtual int y() const { return y; }
//     virtual double z() const { return z; }
//     virtual int r() const { return r; }
//     virtual int g() const { return g; }
//     virtual int b() const { return b; }
// };

#endif //   _pixel_h