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
#include "face.h"
#include "pixel.h"
#include "shading_data.h"
#include "illuminator.h"

class Raster {
private:
    const double xMin;
    const double xMax;
    const double yMin;
    const double yMax;
    const int xRes;
    const int yRes;
    const int colorScale;

    static double f(double,double,double,double,double,double);
    std::vector<double*> raster_face(int, double**) const;

public:
	Raster(int xRes, int yRes, \
        double xMin = -1., double xMax = 1., 
        double yMin = -1., double yMax = 1., \
        int colorScale = 255);

    double ndc_to_px_x(double) const;
    double ndc_to_px_y(double) const;

    std::vector<Pixel> raster_plain(const Face<Vec4>&, const Color<double>& = Color<double>(1.)) const;
    std::vector<Pixel> raster_flat(const Face<ShadingData>&, const Illuminator&) const;
    std::vector<Pixel> raster_gouraud(const Face<ShadingData>&, const Illuminator&) const;
    std::vector<Pixel> raster_phong(const Face<ShadingData>&, const Illuminator&) const;
};


#endif //   _raster_h