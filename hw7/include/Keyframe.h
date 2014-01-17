#ifndef _Keyframe_h
#define _Keyframe_h

#include "FrameStruct.h"

class Keyframe
{
public:
    int index;
    FrameStruct data;
};

std::ostream& operator<<( std::ostream&, const Keyframe& );

#endif
