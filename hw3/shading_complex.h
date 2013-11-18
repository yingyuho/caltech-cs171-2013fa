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
#include "mesh_utility.h"

struct ShadingData;
class ShadingModule;
class ShadingComplex;


struct ShadingData {
public:
    Vec4 ws_coord;
    Vec4 nd_coord;
    NVec3 ws_normal;
    ShadingData();
    ShadingData(const Vec4&, const Vec4&, const NVec3&);
    ShadingData(const ShadingData&);
};

class ShadingModule {
private:
	Mesh<ShadingData> mesh;
	Material material;
public:
	ShadingModule(const PerspectiveCamera&, const Separator&);
	const Mesh<ShadingData>& get_mesh() const { return mesh; }
	const Material& get_material() const { return material; }
};

class ShadingComplex {
private:
	PerspectiveCamera pCamera;
    PtrList<PointLight> plList;
	PtrList<ShadingModule> moduleList;
public:
	ShadingComplex(const Inventor&);
};


#endif //   _shading_complex_h