//
//  canvas.h
//  
//
//  Created by Ying-Yu Ho on 10/29/13.
//
//

#ifndef _canvas_h
#define _canvas_h

#include <stdexcept>
#include <iostream>
#include <algorithm>
#include <memory>
#include "matrix.h"

// different containers T<IntVec2> can be used to process lists of pixels
template< template< typename,typename=std::allocator<IntVec2> > class T >
class Canvas {
private:
    int *pixelData;
    const double xMin;
    const double xMax;
    const double yMin;
    const double yMax;
    const int xRes;
    const int yRes;
    const int maxIntensity;
    const int length;

public:
    Canvas(int xRes, int yRes, int maxIntensity = 255, double xMin = -1., double xMax = 1. \
    , double yMin = -1., double yMax = 1. ) throw ( std::invalid_argument )  \
    : xMin(xMin), xMax(xMax), yMin(yMin), yMax(yMax) \
    , xRes(xRes), yRes(yRes), maxIntensity(maxIntensity), length(3*xRes*yRes) {

        // check whether arguments are valid
	    #define CHECK_ARG_POSITIVITY(arg) \
	    if (double(arg)<=0) \
	        throw std::invalid_argument(#arg " must be positive.");

	    CHECK_ARG_POSITIVITY((xMax - xMin))
	    CHECK_ARG_POSITIVITY((yMax - yMin))
	    CHECK_ARG_POSITIVITY(xRes)
	    CHECK_ARG_POSITIVITY(yRes)
	    CHECK_ARG_POSITIVITY(maxIntensity)

        pixelData = new int[length];
        std::fill_n(pixelData, length, 0);
    }

    virtual ~Canvas() {
    	delete[] pixelData;
    }

    // accessors
    int &at(int x, int y, int c) {
        return pixelData[ c + 3 * (x + xRes * y) ];
    }
    const int &at(int x, int y, int c) const {
        return pixelData[ c + 3 * (x + xRes * y) ];
    }
    int &at(const IntVec2& xy, int c) {
        return at(xy[0], xy[1], c);
    }
    const int &at(const IntVec2& xy, int c) const {
        return at(xy[0], xy[1], c);
    }

    // utility functions

    // fill color according to a list of pixels
    //template< template<typename> class T >
    void draw_pixel(const T<IntVec2>& pList, int r, int g, int b);
    void draw_pixel(const T<IntVec2>& pList) { draw_pixel(pList, 255, 255, 255); }

	// output PPM image
	std::ostream &print_ppm(std::ostream &os) const;

    // convert a line segment into a list of pixels with Bresenham algorithm
    T<IntVec2> bresenham(const IntVec2& xy1, const IntVec2& xy2) const;

    void draw_line(const IntVec2& xy1, const IntVec2& xy2, int r, int g, int b) {
        draw_pixel(bresenham(xy1,xy2), r, g, b);
    }
    void draw_line(const IntVec2& xy1, const IntVec2& xy2) { 
        draw_line(xy1, xy2, 255, 255, 255);
    }
};

#include "canvas.cpp"

#endif //   _canvas_h