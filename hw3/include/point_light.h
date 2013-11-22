//
//  point_light.h
//  
//  
//
//  Created by Ying-Yu Ho on 11/17/13.
//
//

#ifndef _point_light_h
#define _point_light_h

#include "color.h"

struct PointLight {
public:
    Vec3 position;
    Color<double> color;
    PointLight(const Vec3& position, const Vec3& color);
};

#endif //   _point_light_h