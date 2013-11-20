#include "canvas.h"

Canvas::Canvas(int xRes, int yRes, int maxIntensity, double xMin, double xMax \
, double yMin, double yMax) throw ( std::invalid_argument )  \
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

Canvas::~Canvas() {
    delete[] pixelData;
}

// accessors
int& Canvas::at(int x, int y, int c) {
    return pixelData[ c + 3 * (x + xRes * y) ];
}
const int& Canvas::at(int x, int y, int c) const {
    return pixelData[ c + 3 * (x + xRes * y) ];
}
int& Canvas::at(const IntVec2& xy, int c) {
    return at(xy[0], xy[1], c);
}
const int& Canvas::at(const IntVec2& xy, int c) const {
    return at(xy[0], xy[1], c);
}

std::ostream& Canvas::print_ppm(std::ostream &os) const {
    os << "P3" << std::endl \
       << xRes << " " << yRes << std::endl \
       << maxIntensity << std::endl << std::endl;

    for (int i = yRes - 1; i >= 0; i--)
        for (int j = 0; j < xRes; j++)
            os << at(j,i,0) << " "\
               << at(j,i,1) << " "\
               << at(j,i,2) << std::endl;

    return os;
}

void Canvas::draw_pixel(const std::vector<IntVec2>& pList, int r, int g, int b) {
    typedef std::vector<IntVec2>::const_iterator T_IT;
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

void Canvas::draw_pixel(const std::vector<IntVec2>& pList) { draw_pixel(pList, 255, 255, 255); }

std::vector<IntVec2> Canvas::bresenham(const IntVec2& xy1, const IntVec2& xy2) const {
    typedef std::vector<IntVec2>::iterator T_IT;

    int x1 = xy1[0]; int y1 = xy1[1]; int x2 = xy2[0]; int y2 = xy2[1];
    int xInd = 0; int yInd = 1;
    
    std::vector<IntVec2> pixelList;

    int temp_int;
    #define SWAP_INT(i1,i2)\
    temp_int = i1; i1 = i2; i2 = temp_int;

    // swap the roles of x and y if slope > 1
    if ((x1>x2?x1-x2:x2-x1)<(y1>y2?y1-y2:y2-y1)) {
        SWAP_INT(x1,y1) SWAP_INT(x2,y2) SWAP_INT(xInd,yInd)
    }

    // the following codes were written as if slope <= 1
    // reorder points so that x2>=x1
    if (x1>x2) { SWAP_INT(x1,x2) SWAP_INT(y1,y2) }

    int dx = x2 - x1;

    pixelList = std::vector<IntVec2>(1+dx);

    // 1 if slope >= 0, -1 if slope < 0
    const int sign_dy = (y2>=y1) ? 1 : -1;

    T_IT it = pixelList.begin();

    int y = y1; 
    int dy = sign_dy * (y2-y1); // depends on sign(slope)
    int dxdy = dy+x1-x2;
    int F = dxdy; int x;
    for (x = x1; x <= x2; x++)
    {
        (*it)[xInd] = x; (*it)[yInd] = y; it++;

        if (F < 0) { F += dy; } else
        { 
            y += sign_dy; // depends on sign(slope)
            F += dxdy;
        }
    }

    return pixelList;
}

void Canvas::draw_line(const IntVec2& xy1, const IntVec2& xy2, int r, int g, int b) {
    draw_pixel(bresenham(xy1,xy2), r, g, b);
}
void Canvas::draw_line(const IntVec2& xy1, const IntVec2& xy2) { 
    draw_line(xy1, xy2, 255, 255, 255);
}