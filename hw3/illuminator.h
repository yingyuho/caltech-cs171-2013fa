//
//  illuminator.h
//  
//  
//
//  Created by Ying-Yu Ho on 11/20/13.
//
//

#ifndef _illuminator_h
#define _illuminator_h

#include <vector>
#include "point_light.h"
#include "material.h"
#include "ptr_container.h"

class Illuminator {
private:
    const Material material;
    const Vec3 cameraPos;
    const int lightNum;
    std::vector<Vec3> lightPos;
    std::vector< Color<double> > lightColor;

public:
	Illuminator(const Vec3& cameraPos, const PtrList<PointLight>& lights, const Material& material);
    Color<double> operator() (const Vec3& coord, const NVec3& normal);
};


#endif //   _illuminator_h