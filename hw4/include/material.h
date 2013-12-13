//
//  material.h
//  
//  
//
//  Created by Ying-Yu Ho on 11/17/13.
//
//

#ifndef _material_h
#define _material_h

#include "color.h"

class Material {
public:
    Color<double> aColor;
    Color<double> dColor;
    Color<double> sColor;
    double shininess;

    Material(const Vec3& aColor, const Vec3& dColor, const Vec3& sColor, double shininess);
};

#endif //   _material_h