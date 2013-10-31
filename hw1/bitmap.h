//
//  bitmap.h
//  
//
//  Created by Ying-Yu Ho on 10/29/13.
//
//

#ifndef _bitmap_h
#define _bitmap_h

#include <stdexcept>
#include <algorithm>
#include <list>

struct PixelCoord
{ 
    int x; int y; 
    PixelCoord(int x1, int y1) { x = x1; y = y1; }
};

typedef list<PixelCoord*> PixelList;

// convert a line segment into a list of pixels with Bresenham algorithm
PixelList* bresenham(int x1, int x2, int y1, int y2);

// struct PixelColor
// { 
//     int r; int g; int b; 
//     PixelColor(int r1, int g1, int b1) : r(r1), g(g1), b(b1) {}
// };

class Bitmap {
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
    Bitmap(double xMin, double xMax, double yMin, double yMax \
    , int xRes, int yRes, int maxIntensity = 255) throw ( std::invalid_argument )  \
    : xMin(xMin), xMax(xMax), yMin(yMin), yMax(yMax) \
    , xRes(xRes), yRes(yRes), maxIntensity(maxIntensity), length(3*xRes*yRes) {

        // check whether arguments are valid
	    #define CHECK_ARG_POSITIVITY(arg) \
	    if (double(arg)<=0) \
	        throw invalid_argument(#arg " must be positive.");

	    CHECK_ARG_POSITIVITY((xMax - xMin))
	    CHECK_ARG_POSITIVITY((yMax - yMin))
	    CHECK_ARG_POSITIVITY(xRes)
	    CHECK_ARG_POSITIVITY(yRes)
	    CHECK_ARG_POSITIVITY(maxIntensity)


        pixelData = new int[length];
        std::fill_n(pixelData, length, 0);
    }

    virtual ~Bitmap() {
    	delete[] pixelData;
    }

    // accessors
    int &at(int x, int y, int c) {
        return pixelData[ c + 3 * (x + xRes * y) ];
    }
    const int &at(int x, int y, int c) const {
        return pixelData[ c + 3 * (x + xRes * y) ];
    }
    int &at(const PixelCoord &xy, int c) {
        return at(xy.x, xy.y, c);
    }
    const int &at(const PixelCoord &xy, int c) const {
        return at(xy.x, xy.y, c);
    }

    // PixelColor getColor(int x, int y) const {
    //     return PixelColor(at(x,y,0),at(x,y,1),at(x,y,2));
    // }
    // PixelColor getColor(const PixelCoord &xy) const {
    //     return getColor(xy.x, xy.y);
    // }


    // utility functions

    PixelCoord ndc_to_pixel(double x, double y) {
	    int x1 = int(floor( xRes * (x-xMin) / (xMax-xMin) ));
        int y1 = int(floor( yRes * (yMax-y) / (yMax-yMin) ));
        return PixelCoord(x1,y1);
    }

    // fill color according to a list of pixels
    void draw_pixel(PixelList* pList, int r = 255, int g = 255, int b = 255)
    {
        // for each pixel in the pixelList
        for (PixelList::iterator it = pList->begin();
                                 it != pList->end();
                                 it++)
        {
            PixelCoord *px = *it;
            if (px->x >= 0 && px->x < xRes && px->y >= 0 && px->y < yRes)
            {
                at(px->x,px->y,0) = r;
                at(px->x,px->y,1) = g;
                at(px->x,px->y,2) = b;
            }
        }
    }

	// output PPM image
	ostream &print_ppm(ostream &os)
	{
	    os << "P3" << endl \
	       << xRes << " " << yRes << endl \
	       << maxIntensity << endl << endl;

	    for (int i = 0; i < yRes; i++)
	        for (int j = 0; j < xRes; j++)
	            os << at(j,i,0) << " "\
	               << at(j,i,1) << " "\
	               << at(j,i,2) << endl;

	    return os;
	}
};

// convert a line segment into a list of pixels with Bresenham algorithm
PixelList* bresenham(const PixelCoord &xy1, const PixelCoord &xy2)
{
    int x1 = xy1.x; int y1 = xy1.y; int x2 = xy2.x; int y2 = xy2.y;
    
    PixelList* pixel_list = new PixelList;

    int temp_int;
    #define SWAP_INT(i1,i2)\
    temp_int = i1; i1 = i2; i2 = temp_int;

    // if abs(slope) <= 1, x dominant
    if ((x1>x2?x1-x2:x2-x1)>=(y1>y2?y1-y2:y2-y1))
    {
        // reorder points so that x2>=x1
        if (x1>x2) { SWAP_INT(x1,x2) SWAP_INT(y1,y2) }

        // if slope >= 0 (and <= 1)
        if (y2>=y1)
        {
            int y = y1; 
            int dy = y2-y1; // depends on sign(slope)
            int dxdy = dy+x1-x2;
            int F = dxdy; int x;
            for (x = x1; x <= x2; x++)
            {
                pixel_list->push_back(new PixelCoord(x,y));
                if (F < 0) { F += dy; } else
                { 
                    y++; // depends on sign(slope)
                    F += dxdy;
                }
            }
        }
        // if slope < 0 (and >= -1)
        else
        {
            int y = y1; 
            int dy = y1-y2; // depends on sign(slope)
            int dxdy = dy+x1-x2;
            int F = dxdy; int x;
            for (x = x1; x <= x2; x++)
            {
                pixel_list->push_back(new PixelCoord(x,y));
                if (F < 0) { F += dy; } else 
                { 
                    y--; // depends on sign(slope)
                    F += dxdy;
                }
            }
        }
    }
    // if abs(slope) > 1, y dominant
    else
    {
        // reorder points so that y2>=y1
        if (y1>y2) { SWAP_INT(y1,y2) SWAP_INT(x1,x2) }

        // if 1/slope >= 0 (and < 1)
        if (x2>=x1)
        {
            int x = x1; 
            int dx = x2-x1; // depends on sign(slope)
            int dydx = dx+y1-y2;
            int F = dydx; int y;
            for (y = y1; y <= y2; y++)
            {
                pixel_list->push_back(new PixelCoord(x,y));
                if (F < 0) { F += dx; } else
                { 
                    x++; // depends on sign(slope)
                    F += dydx;
                }
            }
        }
        // if 1/slope < 0 (and > -1)
        else
        {
            int x = x1; 
            int dx = x1-x2; // depends on sign(slope)
            int dydx = dx+y1-y2;
            int F = dydx; int y;
            for (y = y1; y <= y2; y++)
            {
                pixel_list->push_back(new PixelCoord(x,y));
                if (F < 0) { F += dx; } else 
                { 
                    x--; // depends on sign(slope)
                    F += dydx;
                }
            }
        }
    }

    return pixel_list;
}

#endif //   _bitmap_h