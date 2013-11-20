#include <cmath>
#include "raster.h"

Raster::Raster(int xRes, int yRes, double xMin, double xMax, double yMin, double yMax) \
: xMin(xMin), xMax(xMax), yMin(yMin), yMax(yMax) , xRes(xRes), yRes(yRes) {}

double Raster::ndc_to_px_x(double x) const {
    return (x-xMin) / (xMax-xMin) * xRes;
}

double Raster::ndc_to_px_y(double y) const {
    return (y-yMin) / (yMax-yMin) * yRes;
}

double Raster::f(double xa, double ya, double xb, double yb, double x, double y) {
    return (xa-x) * (yb-y) - (ya-y) * (xb-x);
}

std::vector<double*> Raster::raster_face(int dataSize, double** dataArray) const {
    const int A_SIZE = 3;

    std::vector<double*> result;
    if ( dataSize < 2 ) return result;

    const double x0 = ndc_to_px_x(dataArray[0][0]);
    const double y0 = ndc_to_px_y(dataArray[0][1]);
    const double x1 = ndc_to_px_x(dataArray[1][0]);
    const double y1 = ndc_to_px_y(dataArray[1][1]);
    const double x2 = ndc_to_px_x(dataArray[2][0]);
    const double y2 = ndc_to_px_y(dataArray[2][1]);
    const double xRef = 1.4142;
    const double yRef = 2.7183;
    const int xMin = (int) floor( (x0<x1)?( (x0<x2)?x0:x2 ):( (x1<x2)?x1:x2 ) );
    const int xMax = (int) ceil ( (x0>x1)?( (x0>x2)?x0:x2 ):( (x1>x2)?x1:x2 ) );
    const int yMin = (int) floor( (y0<y1)?( (y0<y2)?y0:y2 ):( (y1<y2)?y1:y2 ) );
    const int yMax = (int) ceil ( (y0>y1)?( (y0>y2)?y0:y2 ):( (y1>y2)?y1:y2 ) );
    const double f012 = f(x0,y0,x1,y1,x2,y2);
    // std::cout \
    //     << xRes << " " << yRes << " " \
    //     << xMin << " " << yMin << " " \
    //     << xMax << " " << yMax << " " << std::endl;
    // std::cout \
    //     << x0 << " " << y0 << " " \
    //     << x1 << " " << y1 << " " \
    //     << x2 << " " << y2 << " " << std::endl;

    if ( fabs(f012) < .0001 ) return result;
    bool aRef = (f(x0,y0,x1,y1,xRef,yRef) * f012) >= 0;
    bool bRef = (f(x1,y1,x2,y2,xRef,yRef) * f012) >= 0;
    bool cRef = (f(x2,y2,x0,y0,xRef,yRef) * f012) >= 0;

    for ( int y = yMin; y <= yMax; y++ ) for ( int x = xMin; x <= xMax; x++ ) {
        double a = f(x0,y0,x1,y1,x,y) / f012;
        double b = f(x1,y1,x2,y2,x,y) / f012;
        double c = f(x2,y2,x0,y0,x,y) / f012;
        // std::cout << a << " " << b << " " << c << std::endl;

        if ( a>=0 && b>=0 && c>=0 && ( a>0 || aRef ) && ( b>0 || bRef ) && ( c>0 || cRef ) ) {
            double * entry = new double[dataSize];
            entry[0] = x; entry[1] = y;
            for ( int i = 2; i < dataSize; i++ )
                entry[i] = a * dataArray[0][i] + b * dataArray[1][i] + c * dataArray[2][i];
            result.push_back(entry);
        }
    }

    // double f01(double x, double y) { return raster_func_f(x0,y0,x1,y1,x,y); }
    // double f12(double x, double y) { return raster_func_f(x1,y1,x2,y2,x,y); }
    // double f20(double x, double y) { return raster_func_f(x2,y2,x0,y0,x,y); }

    return result;
}

std::vector<IntVec2> Raster::operator()(Face<Vec4>& f) const {
    double** dataArray = new double*[3];

    for ( int i = 0; i < 3; i++ ) {
        dataArray[i] = new double[2];
        dataArray[i][0] = f[i][0] / f[i][3];
        dataArray[i][1] = f[i][1] / f[i][3];
    }

    std::vector<double*> floatPixel = raster_face(2, dataArray);
    std::vector<IntVec2> result(floatPixel.size());

    for ( int i = 0; i < floatPixel.size(); i++ ) {
        result[i][0] = (int) floor(0.5+floatPixel[i][0]);
        result[i][1] = (int) floor(0.5+floatPixel[i][1]);
        delete floatPixel[i];
    }

    for ( int i = 0; i < 3; i++ )
        delete[] dataArray[i];
    delete dataArray;

    return result;
}


