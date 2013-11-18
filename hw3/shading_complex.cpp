#include "shading_complex.h"

// ShadingData

ShadingData::ShadingData()\
: ws_coord(0.), nd_coord(0.), ws_normal(0.) {}

ShadingData::ShadingData(const Vec4& ws_coord, const Vec4& nd_coord, const NVec3& ws_normal)\
: ws_coord(ws_coord), nd_coord(nd_coord), ws_normal(ws_normal) {}

ShadingData::ShadingData(const ShadingData& x)\
: ws_coord(x.ws_coord), nd_coord(x.nd_coord), ws_normal(x.ws_normal) {}

// ShadingModule

ShadingModule::ShadingModule(const PerspectiveCamera& pc, const Separator& sep) \
: mesh(Mesh<ShadingData>()), material(sep.get_material()) {
    const std::vector<Vec4> wsc = sep.get_world_space_coord();
    const std::vector<Vec4> ndc = sep.get_norm_device_coord(pc);
    const std::vector<NVec3> wsn = sep.get_world_space_normal();

    CoordMesh meshWSC; sep.process_mesh(meshWSC, wsc);
    CoordMesh meshNDC; sep.process_mesh(meshNDC, ndc);
    NormalMesh meshWSN; sep.process_mesh(meshWSN, wsn);

    for ( int i = 0; i < meshWSC.size(); i++ ) {
    	Face<ShadingData> * face = new Face<ShadingData>(meshWSC[i]->size());
    	for ( int j = 0; j < meshWSC[i]->size(); j++ ) {
    		face->at(j).ws_coord = meshWSC[i]->at(j);
    		face->at(j).nd_coord = meshNDC[i]->at(j);
    		face->at(j).ws_normal = meshWSN[i]->at(j);
    	}
    	this->mesh.push_back(face);
    }

    BackFaceCuller bfc(meshNDC);
    bfc(this->mesh);
}

// ShadingComplex

ShadingComplex::ShadingComplex(const Inventor& inv) \
: pCamera(inv.get_camera()), plList(inv.get_light_list()) {

	const PtrList<Separator>& sepList =  inv.get_separator_list();

	for ( PtrList<Separator>::const_iterator it = sepList.begin(); it != sepList.end(); it++ )
		moduleList.push_back( new ShadingModule(pCamera, **it) );
}
