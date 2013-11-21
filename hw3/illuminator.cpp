#include "illuminator.h"

Illuminator::Illuminator(const Vec3& cp, const PtrList<PointLight>& pl, const Material& mat) :\
cameraPos(cp), lightNum(pl.size()), material(mat) {

	lightPos = std::vector<Vec3> (lightNum);
	lightColor = std::vector< Color<double> > (lightNum);

	PtrList<PointLight>::const_iterator it = pl.begin();
	for ( int i = 0; i < lightNum; i++ ) {
		lightPos[i] = (*it)->position;
		lightColor[i] = (*it)->color;
		it++;
	}
}

Color<double> Illuminator::operator() (const Vec3& coord, const NVec3& normal) const {
    const Vec3  n = normal.transpose().normalize();
    const Vec3& v = coord;

    Color<double> diffuse(0.);
    Color<double> specular(0.);

    for ( int i = 0; i < lightNum; i++ ) {
        const Vec3& lx = lightPos[i];
        const Color<double>& lc = lightColor[i];

        diffuse += ( lc * n.dot((lx - v).normalize()) ).zeroclip();

        double k = n.dot( ( (cameraPos - v).normalize() + (lx - v).normalize() ).normalize() );
        if ( k < 0. ) k = 0.;

        specular += ( lc * std::pow(k, material.shininess) ).zeroclip();
    }

    return ( \
    	material.aColor + \
		diffuse.oneclip() * material.dColor + \
		specular * material.sColor ).oneclip();
}