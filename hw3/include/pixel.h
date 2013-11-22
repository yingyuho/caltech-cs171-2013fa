//
//  pixel.h
//  
//  
//
//  Created by Ying-Yu Ho on 11/17/13.
//
//

#ifndef _pixel_h
#define _pixel_h

#include "color.h"

struct Pixel {
public:
    int x;
    int y;
    double z;
    Color<int> color;
};

#endif //   _pixel_h