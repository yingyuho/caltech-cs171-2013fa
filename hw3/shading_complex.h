//
//  shading_complex.h
//  
//  
//
//  Created by Ying-Yu Ho on 11/17/13.
//
//

#ifndef _shading_complex_h
#define _shading_complex_h

#include "inventor.h"
#include "shading_data.h"
#include "mesh_utility.h"

class ShadingModule;
class ShadingComplex;

class ShadingModule {
private:
    Mesh<ShadingData> mesh;
    Material material;
public:
    ShadingModule(const PerspectiveCamera&, const Separator&);
    const Mesh<ShadingData>& get_mesh() const;
    const Material& get_material() const;
    Color lighting(const ShadingData&, const ShadingComplex&) const;
};

class ShadingComplex {
private:
    PerspectiveCamera pCamera;
    PtrList<PointLight> plList;
    PtrList<ShadingModule> moduleList;
public:
    ShadingComplex(const Inventor&);
    const PtrList<ShadingModule>& get_module_list() const;
    const PtrList<PointLight>& get_light_list() const;
    const Vec3& get_camera_position() const;
};


#endif //   _shading_complex_h