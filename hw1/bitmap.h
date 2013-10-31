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

// struct PixelColor
// { 
//     int r; int g; int b; 
//     PixelColor(int r1, int g1, int b1) : r(r1), g(g1), b(b1) {}
// };

typedef list<PixelCoord*> PixelList;

class Bitmap {
private:
    int *pixelData;
    const int xRes;
    const int yRes;
    const int maxIntensity;
    const int length;

public:
    Bitmap(int xRes, int yRes, int maxIntensity = 255) throw ( std::invalid_argument )  \
    : xRes(xRes), yRes(yRes), maxIntensity(maxIntensity), length(3*xRes*yRes) {
        if (xRes <= 0 || yRes <= 0 || maxIntensity <= 0)
            throw invalid_argument("arguments must be positive integers");
        pixelData = new int[length];
        std::fill_n(pixelData, length, 0);
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
#endif //   _bitmap_h