#include "illuminator.h"

Illuminator::Illuminator(const Vec3& cp, const PtrList<PointLight>& pl, const Material& mat,
    bool eyeLightFlag, double eyeLightStr) :\
cameraPos(cp), lightNum(pl.size()+eyeLightFlag), material(mat) {

	lightPos = std::vector<Vec3> (lightNum);
	lightColor = std::vector< Color<double> > (lightNum);

    Color<double> avgColor(0.);

	PtrList<PointLight>::const_iterator it = pl.begin();
	for ( int i = 0; i < (lightNum-eyeLightFlag); i++ ) {
		lightPos[i] = (*it)->position;
		lightColor[i] = (*it)->color;
        avgColor += lightColor[i];
		it++;
	}

    if ( eyeLightFlag ) {
        if ( lightNum > 1 ) {
            Color<double> eyeLightColor(avgColor[0]+avgColor[1]+avgColor[2]);
            eyeLightColor = eyeLightColor / 3. / ( lightNum - 1 ) * eyeLightStr;

            lightPos[lightNum-1] = cameraPos;
            lightColor[lightNum-1] = eyeLightColor;
        } else {
            Color<double> eyeLightColor(eyeLightStr);

            lightPos[lightNum-1] = cameraPos;
            lightColor[lightNum-1] = eyeLightColor;
        }
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