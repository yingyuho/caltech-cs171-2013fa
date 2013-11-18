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

struct ShadingData {
public:
    Vec4 coordWS;
    Vec4 coordND;
    NVec3 normalWS;
};


#endif //   _shading_data_h