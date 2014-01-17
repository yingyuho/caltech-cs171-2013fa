#include <iostream>
using namespace std;

#include "KeyframeSeries.h"

FrameStruct KeyframeSeries::eval( double t ) const
{
    return eval( t, buildSlopes() );
}

FrameStruct KeyframeSeries::eval( double t, const std::vector<FrameStruct>& slopes ) const
{
    if ( !validate() )
    {
        cerr << "KeyframeSeries::operator() returns FrameStruct() by default" << endl;
        return FrameStruct();
    }

    if ( t < double( keyframes.front().index ) )
        return keyframes.front().data;

    if ( t >= double( keyframes.back().index ) )
        return keyframes.back().data;

    int i = 0;
    for ( i = 0; i < keyframes.size(); i++ )
        if ( t < keyframes[i].index ) break;

    double u = \
        ( t - double( keyframes[i-1].index ) )              \
        /                                                   \
        double( keyframes[i].index - keyframes[i-1].index );

    double v = 1. - u;

    return ( \
        keyframes[i-1].data * ( 1. + 2. * u ) * v * v   + \
        keyframes[ i ].data * ( 1. + 2. * v ) * u * u   + \
        slopes[i-1]         * v * v * u                 + \
        slopes[ i ]         * v * u * u                 ).unit();
}

#if 0
std::vector<FrameStruct> KeyframeSeries::operator()( std::vector<double> t ) const
{
    std::vector<FrameStruct> result( t.size() );

    if ( !validate() )
    {
        cerr << "KeyframeSeries::operator() returns std::vector<FrameStruct>() by default" << endl;
        return result;
    }


}
#endif

std::vector<FrameStruct> KeyframeSeries::buildSlopes( void ) const
{
    std::vector<FrameStruct> result( keyframes.size() );

    if ( keyframes.size() <= 1 || !validate() )
        return result;

    for ( int i = 1; i < keyframes.size(); i++ )
    {
        result[i] = \
            ( keyframes[i].data - keyframes[i-1].data )         \
            /                                                   \
            double( keyframes[i].index - keyframes[i-1].index );
    }

    result[0] = result[1];

    for ( int i = 1; i < keyframes.size() - 1; i++ )
    {
        result[i] = ( result[i] + result[i+1] ) / 2.;
    }

    return result;
}

bool KeyframeSeries::validate( void ) const
{
    if ( keyframes.empty() )
    {
        cerr << "no keyframes" << endl;
        return false;
    }

    for ( int i = 1; i < keyframes.size(); i++ )
    {
        if ( keyframes[i].index <= keyframes[i-1].index )
        {
            cerr << "non-increasing indices between keyframes " << i-1 << " and " << i << endl;
            return false;
        }
    }

    return true;
}

std::ostream& operator<<( std::ostream& os, const KeyframeSeries& k )
{
    os << "Length: " << k.length << " frames" << endl << endl;
    for ( const auto &f : k.keyframes )
        os << f;
    return os;
}
