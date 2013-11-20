//
//  raster.h
//  
//  
//
//  Created by Ying-Yu Ho on 11/20/13.
//
//

#ifndef _raster_h
#define _raster_h

#include <vector>
#include "mesh.h"

class Raster {
private:
    const double xMin;
    const double xMax;
    const double yMin;
    const double yMax;
    const int xRes;
    const int yRes;

    static double f(double,double,double,double,double,double);
    std::vector<double*> raster_face(int, double**) const;

public:
	Raster(int, int, double = -1., double = 1. , double = -1., double = 1.);

    double ndc_to_px_x(double) const;
    double ndc_to_px_y(double) const;

    std::vector<IntVec2> operator() (Face<Vec4>&) const;
};


#endif //   _raster_h