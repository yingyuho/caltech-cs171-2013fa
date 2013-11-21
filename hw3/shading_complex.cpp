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

Color<double> ShadingModule::lighting(const ShadingData& sData, const ShadingComplex& sComp) const {
    const Vec3 n = sData.ws_normal.transpose();
    const Vec3 v = sData.ws_coord;
    const Material material = get_material();
    const PtrList<PointLight>& lights = sComp.get_light_list();
    const Vec3 camerapos = sComp.get_camera_position();

    const Color<double>& acolor = material.aColor;
    const Color<double>& dcolor = material.dColor;
    const Color<double>& scolor = material.sColor;
    const double shiny = material.shininess;

    Color<double> diffuse(0.);
    Color<double> specular(0.);
    Color<double> ambient(acolor);

    for ( PtrList<PointLight>::const_iterator it = lights.begin(); it != lights.end(); it++ ) {
        const Vec3 lx = (*it)->position;
        const Color<double> lc = (*it)->color;

        diffuse += (lc * n.dot( (lx - v).normalize() )).zeroclip();

        double k = n.dot( ( (camerapos - v).normalize() + (lx - v).normalize() ).normalize() );
        if ( k < 0. ) k = 0.;

        specular += ( Color<double>(lc*std::pow(k, shiny)) ).zeroclip();
    }

    return Color<double>( ambient + diffuse.oneclip()*dcolor + specular*scolor ).oneclip();
}

// ShadingComplex

ShadingComplex::ShadingComplex(const Inventor& inv) \
: pCamera(inv.get_camera()), plList(inv.get_light_list()) {

    const PtrList<Separator>& sepList =  inv.get_separator_list();

    for ( PtrList<Separator>::const_iterator it = sepList.begin(); it != sepList.end(); it++ )
        moduleList.push_back( new ShadingModule(pCamera, **it) );
}

const PtrList<ShadingModule>& ShadingComplex::get_module_list() const { return moduleList; };

const PtrList<PointLight>& ShadingComplex::get_light_list() const { return plList; }

const Vec3& ShadingComplex::get_camera_position() const { return pCamera.get_position(); }
