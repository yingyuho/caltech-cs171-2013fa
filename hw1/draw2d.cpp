#include <iostream> 
#include <cstdlib> 
#include <list>
#include <cmath>
#include "matrix.h"
#include "image2d.h"
#include "bitmap.h"

using namespace std;

// parse drawing commands from input stream
Image2* parse_image(istream &datafile);

int main(int argc, char* argv[])
{
    // check if all required arguments are present
    if (argc < 7)
    {
        cerr << "Usage: " << argv[0] << " xMin xMax yMin yMax xRes yRes" << endl;
        return 1;
    }

    // 1:xMin 2:xMax 3:yMin 4:yMax 5:xRes 6:yRes
    Bitmap canvas( atof(argv[1]), atof(argv[2]), atof(argv[3]), atof(argv[4]) \
                 , atoi(argv[5]), atoi(argv[6]) );

    // parse drawing commands from standard input
    Image2 *image = parse_image(cin);

    // for each polyline
    for (Image2::iterator it1 = image->begin();
                          it1 != image->end();
                          it1++)
    {
        Vec2 *v = (*it1)->front();
        // for each vertex (the first vertex will be repeated)
        for (Polygon2::iterator it2 = (*it1)->begin();
                                it2 != (*it1)->end();
                                it2++)
        {
            // draw a line between the previous and the current vertex
            PixelCoord xy1 = canvas.ndc_to_pixel( (*v)[0], (*v)[1] );
            v = (*it2);
            PixelCoord xy2 = canvas.ndc_to_pixel( (*v)[0], (*v)[1] );

            canvas.draw_pixel(bresenham(xy1,xy2));
        }
    }

    canvas.print_ppm(cout);

    return 0;
}


