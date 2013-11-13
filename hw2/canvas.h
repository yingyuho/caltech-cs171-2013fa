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

struct PixelCoord
{ 
    int x; int y; 
    PixelCoord(int x1, int y1) { x = x1; y = y1; }
};

// convert a line segment into a list of pixels with Bresenham algorithm
// template< template<typename> class T >
// T<IntVec2> bresenham(const IntVec2& xy1, const IntVec2& xy2);

// template<>
// std::vector<IntVec2> bresenham< std::vector >(const PixelCoord& xy1, const PixelCoord& xy2);

// template< typename T >
// T bresenham(const IntVec2& xy1, const IntVec2& xy2);

// template<>
// void bresenham<void>(const PixelCoord& xy1, const PixelCoord& xy2);

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
    void draw_pixel(const T<IntVec2>& pList, int r = 255, int g = 255, int b = 255);

	// output PPM image
	std::ostream &print_ppm(std::ostream &os) const;

    // convert a line segment into a list of pixels with Bresenham algorithm
    T<IntVec2> bresenham(const IntVec2& xy1, const IntVec2& xy2) const;
};

template<template<typename,typename> class T>
std::ostream& Canvas<T>::print_ppm(std::ostream &os) const {
    os << "P3" << std::endl \
       << xRes << " " << yRes << std::endl \
       << maxIntensity << std::endl << std::endl;

    for (int i = 0; i < yRes; i++)
        for (int j = 0; j < xRes; j++)
            os << at(j,i,0) << " "\
               << at(j,i,1) << " "\
               << at(j,i,2) << std::endl;

    return os;
}

template< template<typename,typename=std::allocator<IntVec2> > class T >
void Canvas<T>::draw_pixel(const T<IntVec2>& pList, int r, int g, int b) {
    typedef typename T<IntVec2>::const_iterator T_IT;
    // for each pixel in the pixelList
    for (T_IT it = pList.begin(); it != pList.end(); it++)
    {
        int x = (*it)[0];
        int y = (*it)[1];
        if (x >= 0 && x < xRes && y >= 0 && y < yRes)
        {
            at(x,y,0) = r;
            at(x,y,1) = g;
            at(x,y,2) = b;
        }
    }
}

template< template<typename,typename=std::allocator<IntVec2> > class T >
T<IntVec2> Canvas<T>::bresenham(const IntVec2& xy1, const IntVec2& xy2) const {
    typedef typename T<IntVec2>::iterator T_IT;

    int x1 = xy1[0]; int y1 = xy1[1]; int x2 = xy2[0]; int y2 = xy2[1];
    
    T<IntVec2> pixelList;

    int temp_int;
    #define SWAP_INT(i1,i2)\
    temp_int = i1; i1 = i2; i2 = temp_int;

    // if abs(slope) <= 1, x dominant
    if ((x1>x2?x1-x2:x2-x1)>=(y1>y2?y1-y2:y2-y1))
    {
        // reorder points so that x2>=x1
        if (x1>x2) { SWAP_INT(x1,x2) SWAP_INT(y1,y2) }

        int dx = x2 - x1;

        pixelList = T<IntVec2>(1+dx);

        // 1 if slope >= 0, -1 if slope < 0 (and <= 1)
        const int sign_dy = (y2>=y1) ? 1 : -1;

        T_IT it = pixelList.begin();

        int y = y1; 
        int dy = sign_dy * (y2-y1); // depends on sign(slope)
        int dxdy = dy+x1-x2;
        int F = dxdy; int x;
        for (x = x1; x <= x2; x++)
        {
            (*it)[0] = x; (*it)[1] = y; it++;

            if (F < 0) { F += dy; } else
            { 
                y += sign_dy; // depends on sign(slope)
                F += dxdy;
            }
        }
    }
    // if abs(slope) > 1, y dominant
    else
    {
        // reorder points so that y2>=y1
        if (y1>y2) { SWAP_INT(y1,y2) SWAP_INT(x1,x2) }

        int dy = y2 - y1;

        pixelList = T<IntVec2>(1+dy);

        // 1 if 1/slope >= 0, -1 if 1/slope < 0 (and < 1)
        const int sign_dx = (x2>=x1) ? 1 : -1;

        T_IT it = pixelList.begin();

        int x = x1; 
        int dx = (x2-x1) * sign_dx; // depends on sign(slope)
        int dydx = dx+y1-y2;
        int F = dydx; int y;
        for (y = y1; y <= y2; y++)
        {
            (*it)[0] = x; (*it)[1] = y; it++;

            if (F < 0) { F += dx; } else
            { 
                x += sign_dx; // depends on sign(slope)
                F += dydx;
            }
        }

    }

    return pixelList;
}

#endif //   _canvas_h