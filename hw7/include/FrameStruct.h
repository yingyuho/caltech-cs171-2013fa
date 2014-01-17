#ifndef _FrameStruct_h
#define _FrameStruct_h

#include "Quaternion.h"
using namespace DDG;

class FrameStruct
{
public:
    Vector translation;
    Quaternion rotation;
    Vector scale;

public:
    FrameStruct( void );

    FrameStruct operator+( const FrameStruct& ) const;
    FrameStruct operator-( const FrameStruct& ) const;

    FrameStruct operator-( void ) const;

    FrameStruct operator*( double ) const;
    FrameStruct operator/( double ) const;

    FrameStruct & operator+=( const FrameStruct& );
    FrameStruct & operator-=( const FrameStruct& );

    FrameStruct & operator*=( double );
    FrameStruct & operator/=( double );

    FrameStruct unit( void );
};

FrameStruct operator*( double, const FrameStruct& );

std::ostream& operator<<( std::ostream&, const FrameStruct& );

#endif