//
//  canvas.h
//
//  A canvas that supports Bresenham's algorithm and PPM output
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
#include <vector>
#include "pixel.h"
#include "matrix.h"

// different containers T<IntVec2> can be used to process lists of pixels
class Canvas {
private:
    int * pixelData;
    double * zBuffer;
    const double xMin;
    const double xMax;
    const double yMin;
    const double yMax;
    const int xRes;
    const int yRes;
    const int maxIntensity;

    Canvas(const Canvas &);
    Canvas& operator= (const Canvas&);

public:
    Canvas(int xRes, int yRes, int maxIntensity = 255, double xMin = -1., double xMax = 1. \
    , double yMin = -1., double yMax = 1. ) throw ( std::invalid_argument );

    virtual ~Canvas();

    // accessors
    int &at(int x, int y, int c);
    const int &at(int x, int y, int c) const;
    int &at(const IntVec2& xy, int c);
    const int &at(const IntVec2& xy, int c) const;

    // utility functions

    // fill color according to a vector of pixels
    void draw_pixel(const std::vector<IntVec2>& pList, int r, int g, int b);
    void draw_pixel(const std::vector<IntVec2>& pList);
    void draw_pixel(const std::vector<Pixel>&);

	// output PPM image
	std::ostream &print_ppm(std::ostream &os) const;

    // convert a line segment into a list of pixels with Bresenham algorithm
    std::vector<IntVec2> bresenham(const IntVec2& xy1, const IntVec2& xy2) const;

    void draw_line(const IntVec2& xy1, const IntVec2& xy2, int r, int g, int b);
    void draw_line(const IntVec2& xy1, const IntVec2& xy2);
};

#endif //   _canvas_h