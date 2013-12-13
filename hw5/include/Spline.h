#ifndef _Spline_h
#define _Spline_h

#include <vector>
#include <iostream>
#include <algorithm>
#include "matrix.h"

/*template < int D >
class ControlPoint
{
public:
    typedef Matrix<double,1,D> PtType;
    double knot;
    PtType position;
    ControlPoint() {}
    ControlPoint(double k, const PtType& p) : knot(k), position(p) {}
};*/

template < int K, int D >
class Spline
{
public:
    typedef Matrix<double,1,D> PtType;

    // Spline(double, const PtType&, double, const PtType&);
    // Spline(const PtType&, const PtType&);
    Spline();

    PtType operator() (double knot);
    void insertKnot(double knot);

    // std::vector<PtType> getPoints();

    // ControlPoint<D> c0, c1;
    // std::vector< ControlPoint<D> > ctrlPts;
    std::vector<double> knots;
    std::vector<PtType> points;

    double& t(int);
    PtType& p(int);

    int k() { return K; }
};

/*
template < int K, int D >
Spline<K,D>::Spline(double t0, const PtType& p0, double t1, const PtType& p1)\
: c0(t0,p0), c1(t1,p1) {}

template < int K, int D >
Spline<K,D>::Spline(const PtType& p0, const PtType& p1)\
: c0(0.,p0), c1(1.,p1) {}
*/

template < int K, int D >
Spline<K,D>::Spline() {}

/*
template < int K, int D >
std::vector< Matrix<double,1,D> > Spline<K,D>::getPoints()
{
    typename std::vector<PtType> result(2+ctrlPts.size());
    int i = 0;

    result[i++] = c0.position;

    for ( auto& c : ctrlPts )
        result[i++] = c.position;

    result[i++] = c1.position;

    return result;
}
*/

template < int K, int D >
double& Spline<K,D>::t(int i)
{
    return knots[i];
}

template < int K, int D >
typename Spline<K,D>::PtType& Spline<K,D>::p(int i)
{
    return points[i];
}

template < int K, int D >
typename Spline<K,D>::PtType Spline<K,D>::operator() (double u) {
    int i0;
    for ( i0 = 0; i0 < knots.size(); i0++ )
        if ( u < t(i0) || t(i0) >= knots.back() ) { i0--; break; }
    if ( i0 < 0 ) { i0 = 0; }

    double n[K+1];
    std::fill_n( n, K+1, 0. );
    n[K-1] = 1.;

    int off = i0 - K + 1;

    for ( int j = 2; j <= K; j++ )
    {
        for ( int i = K-j; i < K; i++ )
        {
            double a = t(i+j-1+off) - t(i+off);
            double b = t(i+j+off) - t(i+off+1);
            a = ( a < 1E-6 ) ? 0. : ( u - t(i+off)   ) / a;
            b = ( b < 1E-6 ) ? 0. : ( t(i+j+off) - u ) / b;
            n[i] = a * n[i] + b * n[i+1];
        }
    }

    // for ( int i = 0; i < K; i++ )
    //     std::cerr << t(i+K-2+off) << " ";
    // std::cerr << std::endl;

    // for ( int i = 0; i < K; i++ )
    //     std::cerr << n[i] << " ";
    // std::cerr << std::endl;

    PtType result(0.);
    for ( int i = 0; i < K; i++ )
        result += n[i] * p(i+off);

    return result;
}

template < int K, int D >
void Spline<K,D>::insertKnot (double u)
{
    if ( u <= knots.front() || u >= knots.back() ) return;

    int j = -1;
    for ( j = 0; j < knots.size(); j++ )
        if ( u <= t(j) ) { j--; break; }

    // std::cerr << j << std::endl;

    points.insert( j + points.begin(), points[j] );

    // for ( auto& p : points )
    //     std::cerr << p << " ";
    // std::cerr << std::endl;

    for ( int i = j; i > j - K + 1; i-- )
    {
        // std::cerr << i << " ";
        double a = ( u - t(i) ) / ( t(i+K-1) - t(i) );
        // std::cerr << a << " ";
        if ( i > 0 )
            p(i) = ( 1 - a ) * p(i-1) + a * p(i);
    }
    // std::cerr << std::endl;

    // for ( auto& p : points )
    //     std::cerr << p << " ";
    // std::cerr << std::endl;

    knots.insert( j+1 + knots.begin(), u );

    // for ( auto& knot : knots )
    //     std::cerr << knot << " ";
    // std::cerr << std::endl;
}

#endif