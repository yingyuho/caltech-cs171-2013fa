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
public:
    virtual ~Pixel() {};
    virtual int x() const = 0;
    virtual int y() const = 0;
    virtual double z() const = 0;
    virtual int r() const = 0;
    virtual int g() const = 0;
    virtual int b() const = 0;
};

class SimplePixel : public Pixel {
private:
    int x;
    int y;
    double z;
    int r; int g; int b;
public:
    SimplePixel(int x, int y, double z, int r, int g, int b) : x(x), y(y), z(z), r(r), g(g), b(b) {}
    ~virtual SimplePixel() {}
    int x() { return x; }
    int y() { return y; }
    double z() { return z; }
    int r() { return r; }
    int g() { return g; }
    int b() { return b; }
};

#endif //   _pixel_h