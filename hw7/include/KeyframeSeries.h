#ifndef _KeyframeSeries_h
#define _KeyframeSeries_h

#include <vector>
#include "Keyframe.h"

class KeyframeSeries
{
public:
    int length;
    std::vector<Keyframe> keyframes;

public:
    FrameStruct eval( double ) const;
    FrameStruct eval( double, const std::vector<FrameStruct>& ) const;
    std::vector<FrameStruct> buildSlopes( void ) const;

    // std::vector<FrameStruct> operator()( std::vector<double> ) const;
    bool validate( void ) const;
};

std::ostream& operator<<( std::ostream&, const KeyframeSeries& );

#endif