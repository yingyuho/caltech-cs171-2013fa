#include <cmath>
#include "raster.h"

Raster::Raster(\
int xRes, int yRes, double xMin, double xMax, double yMin, double yMax, int colorScale ) \
: xMin(xMin), xMax(xMax), yMin(yMin), yMax(yMax) , xRes(xRes), yRes(yRes), colorScale(colorScale) {}

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

    int xMin = (int) floor( (x0<x1)?( (x0<x2)?x0:x2 ):( (x1<x2)?x1:x2 ) );
    int xMax = (int) ceil ( (x0>x1)?( (x0>x2)?x0:x2 ):( (x1>x2)?x1:x2 ) );
    int yMin = (int) floor( (y0<y1)?( (y0<y2)?y0:y2 ):( (y1<y2)?y1:y2 ) );
    int yMax = (int) ceil ( (y0>y1)?( (y0>y2)?y0:y2 ):( (y1>y2)?y1:y2 ) );
    xMin = (xMin >= 0) ? xMin : 0; xMax = (xMax < xRes) ? xMax : xRes - 1;
    yMin = (yMin >= 0) ? yMin : 0; yMax = (yMax < yRes) ? yMax : yRes - 1;

    const double f012 = f(x0,y0,x1,y1,x2,y2);

    if ( fabs(f012) < .0001 ) return result;
    bool aRef = (f(x1,y1,x2,y2,xRef,yRef) * f012) >= 0;
    bool bRef = (f(x2,y2,x0,y0,xRef,yRef) * f012) >= 0;
    bool cRef = (f(x0,y0,x1,y1,xRef,yRef) * f012) >= 0;

    for ( int y = yMin; y <= yMax; y++ ) for ( int x = xMin; x <= xMax; x++ ) {
        double a = f(x1,y1,x2,y2,x,y) / f012;
        double b = f(x2,y2,x0,y0,x,y) / f012;
        double c = f(x0,y0,x1,y1,x,y) / f012;

        if ( a>=0 && b>=0 && c>=0 && ( a>0 || aRef ) && ( b>0 || bRef ) && ( c>0 || cRef ) ) {
            double * entry = new double[dataSize];

            entry[0] = x; entry[1] = y;

            for ( int i = 2; i < dataSize; i++ ) {
                entry[i] = (a * dataArray[0][i]) + (b * dataArray[1][i]) + (c * dataArray[2][i]);
            }
            result.push_back(entry);
        }
    }

    return result;
}

std::vector<Pixel> Raster::raster_plain(const Face<Vec4>& f, const Color<double>& color) const {

    Color<int> intColor;
    intColor[0] = (int) ( colorScale * color[0] );
    intColor[1] = (int) ( colorScale * color[1] );
    intColor[2] = (int) ( colorScale * color[2] );

    double** dataArray = new double*[3];

    for ( int i = 0; i < 3; i++ ) {
        dataArray[i] = new double[3];
        dataArray[i][0] = f[i][0] / f[i][3];
        dataArray[i][1] = f[i][1] / f[i][3];
        dataArray[i][2] = f[i][2] / f[i][3];
    }

    std::vector<double*> floatPixel = raster_face(3, dataArray);
    std::vector<Pixel> result(floatPixel.size());

    for ( int i = 0; i < floatPixel.size(); i++ ) {
        result[i].x = (int) floor(0.5+floatPixel[i][0]);
        result[i].y = (int) floor(0.5+floatPixel[i][1]);
        result[i].z = floatPixel[i][2];
        result[i].color = intColor;
        delete floatPixel[i];
    }

    for ( int i = 0; i < 3; i++ )
        delete[] dataArray[i];
    delete dataArray;

    return result;
}

std::vector<Pixel> Raster::raster_flat(const Face<ShadingData>& f, const Illuminator& illu) const {
    Vec3 avg_ws_coord = ( f[0].ws_coord + f[1].ws_coord + f[2].ws_coord ) / 3.;
    NVec3 avg_ws_normal = ( f[0].ws_normal + f[1].ws_normal + f[2].ws_normal ).normalize();

    Color<double> fColor = illu(avg_ws_coord, avg_ws_normal);

    Color<int> iColor;
    iColor[0] = (int) ( colorScale * fColor[0] );
    iColor[1] = (int) ( colorScale * fColor[1] );
    iColor[2] = (int) ( colorScale * fColor[2] );

    double** dataArray = new double*[3];

    for ( int i = 0; i < 3; i++ ) {
        dataArray[i] = new double[3];
        dataArray[i][0] = f[i].nd_coord[0];
        dataArray[i][1] = f[i].nd_coord[1];
        dataArray[i][2] = f[i].nd_coord[2];
    }

    std::vector<double*> fPixel = raster_face(3, dataArray);

    std::vector<Pixel> result(fPixel.size());

    for ( int i = 0; i < fPixel.size(); i++ ) {
        result[i].x = (int) floor(0.5+fPixel[i][0]);
        result[i].y = (int) floor(0.5+fPixel[i][1]);
        result[i].z = fPixel[i][2];
        result[i].color = iColor;
        delete fPixel[i];
    }

    for ( int i = 0; i < 3; i++ )
        delete[] dataArray[i];
    delete dataArray;

    return result;
}

std::vector<Pixel> Raster::raster_gourand(const Face<ShadingData>& f, const Illuminator& illu) const {

    double** dataArray = new double*[3];

    Color<double> vColor[3];

    for ( int i = 0; i < 3; i++ ) {
        vColor[i] = illu(f[i].ws_coord, f[i].ws_normal);
        dataArray[i] = new double[6];
        dataArray[i][0] = f[i].nd_coord[0];
        dataArray[i][1] = f[i].nd_coord[1];
        dataArray[i][2] = f[i].nd_coord[2];
        dataArray[i][3] = vColor[i][0];
        dataArray[i][4] = vColor[i][1];
        dataArray[i][5] = vColor[i][2];
    }

    std::vector<double*> fPixel = raster_face(6, dataArray);

    std::vector<Pixel> result(fPixel.size());

    for ( int i = 0; i < fPixel.size(); i++ ) {
        result[i].x = (int) floor(0.5+fPixel[i][0]);
        result[i].y = (int) floor(0.5+fPixel[i][1]);
        result[i].z = fPixel[i][2];

        Color<int> iColor;
        iColor[0] = (int) ( colorScale * fPixel[i][3] );
        iColor[1] = (int) ( colorScale * fPixel[i][4] );
        iColor[2] = (int) ( colorScale * fPixel[i][5] );

        result[i].color = iColor;

        delete fPixel[i];
    }

    for ( int i = 0; i < 3; i++ )
        delete[] dataArray[i];
    delete dataArray;

    return result;
}

std::vector<Pixel> Raster::raster_phong(const Face<ShadingData>& f, const Illuminator& illu) const {
    double** dataArray = new double*[3];

    for ( int i = 0; i < 3; i++ ) {
        dataArray[i] = new double[9];
        dataArray[i][0] = f[i].nd_coord[0];
        dataArray[i][1] = f[i].nd_coord[1];
        dataArray[i][2] = f[i].nd_coord[2];
        dataArray[i][3] = f[i].ws_coord[0];
        dataArray[i][4] = f[i].ws_coord[1];
        dataArray[i][5] = f[i].ws_coord[2];
        dataArray[i][6] = f[i].ws_normal[0];
        dataArray[i][7] = f[i].ws_normal[1];
        dataArray[i][8] = f[i].ws_normal[2];
    }

    std::vector<double*> fPixel = raster_face(9, dataArray);

    std::vector<Pixel> result(fPixel.size());

    for ( int i = 0; i < fPixel.size(); i++ ) {
        result[i].x = (int) floor(0.5+fPixel[i][0]);
        result[i].y = (int) floor(0.5+fPixel[i][1]);
        result[i].z = fPixel[i][2];

        Color<double> fColor = illu(Vec3(fPixel[i]+3), NVec3(fPixel[i]+6).normalize());

        Color<int> iColor;
        iColor[0] = (int) ( colorScale * fColor[0] );
        iColor[1] = (int) ( colorScale * fColor[1] );
        iColor[2] = (int) ( colorScale * fColor[2] );

        result[i].color = iColor;

        delete fPixel[i];
    }

    for ( int i = 0; i < 3; i++ )
        delete[] dataArray[i];
    delete dataArray;

    return result;
}

