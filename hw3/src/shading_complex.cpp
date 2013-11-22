#include <cmath>
#include "shading_complex.h"

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
        Face<ShadingData> * face = new Face<ShadingData>();
        for ( int j = 0; j < meshWSC[i]->size(); j++ ) {
            face->push_back( ShadingData( meshWSC[i]->at(j), meshNDC[i]->at(j), meshWSN[i]->at(j) ) );
        }
        this->mesh.push_back(face);
    }

    BackFaceCuller bfc(meshNDC);
    bfc(this->mesh);
}

const Mesh<ShadingData>& ShadingModule::get_mesh() const { return mesh; }
const Material& ShadingModule::get_material() const { return material; }

// ShadingComplex

ShadingComplex::ShadingComplex(const Inventor& inv) \
: pCamera(inv.get_camera()), plList(inv.get_light_list()) {

    const PtrList<Separator>& sepList =  inv.get_separator_list();

    for ( PtrList<Separator>::const_iterator it = sepList.begin(); it != sepList.end(); it++ )
        moduleList.push_back( new ShadingModule(pCamera, **it) );
}

const PtrList<ShadingModule>& ShadingComplex::get_module_list() const { return moduleList; };

const PtrList<PointLight>& ShadingComplex::get_lights() const { return plList; }

const Vec3& ShadingComplex::get_camera_position() const { return pCamera.get_position(); }
