#include <iostream> 
#include <cstdlib> 
#include <list>
#include <cmath>
#include "matrix.h"

using namespace std;
typedef Matrix<double,2,1> Vec2;
typedef list<Vec2*> Polygon2;
typedef list<Polygon2*> Image2;

struct PixelCoord
{ 
    int x; int y; 
    PixelCoord(int x1, int y1) { x = x1; y = y1; }
};

typedef list<PixelCoord*> PixelList;

// parse drawing commands from input stream
Image2* parse_image(istream &datafile);
// convert a line segment into a list of pixels with Bresenham algorithm
PixelList* bresenham(int x1, int x2, int y1, int y2);
// fill color according to a list of pixels
void draw_pixel(int ***pixel_data, int xRes, int yRes, \
                PixelList* pixel_list, \
                int r = 255, int g = 255, int b = 255);
// output PPM image
ostream &print_ppm(ostream &os, int ***pixel_data, int xRes, int yRes, \
                   int color_scale = 255);

int main(int argc, char* argv[])
{
    // check if all required arguments are present
    if (argc < 7)
    {
        cerr << "Usage: " << argv[0] << " xMin xMax yMin yMax xRes yRes" << endl;
        return 1;
    }

    double xMin = atof(argv[1]);
    double xMax = atof(argv[2]);
    double yMin = atof(argv[3]);
    double yMax = atof(argv[4]);
    int xRes = atoi(argv[5]);
    int yRes = atoi(argv[6]);

    // check if Res>0 and Max>Min 
    #define CHECK_ARG_POSITIVITY(arg) \
    if (double(arg)<=0) \
    { \
        cerr << #arg " must be positive." << endl; \
	return 1; \
    }
    CHECK_ARG_POSITIVITY((xMax - xMin))
    CHECK_ARG_POSITIVITY((yMax - yMin))
    CHECK_ARG_POSITIVITY(yRes)
    CHECK_ARG_POSITIVITY(xRes)

    double xDiff = xMax - xMin;
    double yDiff = yMax - yMin;

    // parse drawing commands from standard input
    Image2 *image = parse_image(cin);
    int ***pixel_data = new int**[yRes];

    // fill pixel_data with black
    for (int i = 0; i < yRes; i++)
    {
        pixel_data[i] = new int*[xRes];
	for (int j = 0; j < xRes; j++)
	{
	    pixel_data[i][j] = new int[3];
	    pixel_data[i][j][0] = pixel_data[i][j][1] \
	                        = pixel_data[i][j][2] = 0;
	}
    }

    // for each polyline
    for (Image2::iterator it1 = image->begin();
                             it1 != image->end();
			     it1++)
    {
    	//cout << "polygon" << endl;
	Vec2 *v = (*it1)->front();
	// for each vertex
        for (Polygon2::iterator it2 = (*it1)->begin();
                                 it2 != (*it1)->end();
	    		     it2++)
        {
	    //cout << **it2;
	    int x1 = int( floor(xRes*(v->at(0,0)-xMin)/xDiff) );
	    int y1 = int( floor(yRes*(v->at(1,0)-yMin)/yDiff) );
	    v = (*it2);
	    int x2 = int( floor(xRes*(v->at(0,0)-xMin)/xDiff) );
	    int y2 = int( floor(yRes*(v->at(1,0)-yMin)/yDiff) );

	    draw_pixel(pixel_data, xRes, yRes, bresenham(x1,x2,y1,y2));
	}
	//cout << endl;
    }

    print_ppm(cout, pixel_data, xRes, yRes);

    return 0;
}

PixelList* bresenham(int x1, int x2, int y1, int y2)
{
    PixelList* pixel_list = new PixelList;
    int temp_int;

    #define SWAP_INT(i1,i2)\
    temp_int = i1; i1 = i2; i2 = temp_int;

    // if abs(slope) <= 1, x dominant
    if ((x1>x2?x1-x2:x2-x1)>=(y1>y2?y1-y2:y2-y1))
    {
        // reorder points so that x2>=x1
        if (x1>x2) { SWAP_INT(x1,x2) SWAP_INT(y1,y2) }

	// if slope >= 0 (and <=1)
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
    // if abs(slope) < 1, y dominant
    else
    {
        // reorder points so that y2>=y1
        if (y1>y2) { SWAP_INT(y1,y2) SWAP_INT(x1,x2) }

	// if 1/slope >= 0 (and <=1)
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

void draw_pixel(int ***pixel_data, int xRes, int yRes, \
                PixelList* pixel_list, \
                int r, int g, int b)
{
    // for each pixel
    for (PixelList::iterator it = pixel_list->begin();
                             it != pixel_list->end();
			     it++)
    {
        PixelCoord *px = *it;
	if (px->x >= 0 && px->x < xRes && px->y >= 0 && px->y < yRes)
	{
	    pixel_data[px->y][px->x][0] = r;
	    pixel_data[px->y][px->x][1] = g;
	    pixel_data[px->y][px->x][2] = b;
	}
    }
}

ostream &print_ppm(ostream &os, int ***pixel_data, int xRes, int yRes, \
                   int color_scale)
{
    os << "P3" << endl \
       << xRes << " " << yRes << endl \
       << color_scale << endl << endl;

    for (int i = yRes-1; i >= 0; i--)
	for (int j = 0; j < xRes; j++)
	    os << pixel_data[i][j][0] << " "\
	       << pixel_data[i][j][1] << " "\
	       << pixel_data[i][j][2] << endl;

    return os;
}
