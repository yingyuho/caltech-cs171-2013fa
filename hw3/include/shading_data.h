//
//  shading_data.h
//  
//  
//
//  Created by Ying-Yu Ho on 11/17/13.
//
//

#ifndef _shading_data_h
#define _shading_data_h

#include "matrix.h"

class ShadingData;

class ShadingData {
public:
    Vec3 ws_coord;
    Vec3 nd_coord;
    NVec3 ws_normal;
    ShadingData();
    ShadingData(const Vec4&, const Vec4&, const NVec3&);
};

std::ostream& operator<<(std::ostream&, const ShadingData&);


#endif //   _shading_data_h