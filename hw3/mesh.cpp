#include "mesh.h"

// CoordMesh

void CoordMesh::transform(const CoordTransformer& t) {
    for ( Iter it = this->begin(); it != this->end(); it++ )
        for ( Face<Vec4>::Iter it2 = (*it)->begin(); it2 != (*it)->end(); it2++ )
            (*it2) = t.to_left_matrix() * (*it2);
}

// NormalMesh

void NormalMesh::normalize() {
    for ( Iter it = this->begin(); it != this->end(); it++ )
        for ( Face<NVec3>::Iter it2 = (*it)->begin(); it2 != (*it)->end(); it2++ )
            (*it2) = it2->normalize();
}

// PixelMesh

PixelMesh::PixelMesh(const CoordMesh& x, int xRes, int yRes) \
: xRes(xRes), yRes(yRes) {
    for ( CoordMesh::CIter it = x.begin(); it != x.end(); it++ ) {
    	Face<IntVec2>* face = new Face<IntVec2>();
        for ( Face<Vec4>::CIter it2 = (*it)->begin(); it2 != (*it)->end(); it2++ ) {
            int xy[2] = 	{ (int)floor(xRes*(1 + (*it2)[0]/(*it2)[3])/2) \
            				, (int)floor(yRes*(1 + (*it2)[1]/(*it2)[3])/2) };
            face->push_back(IntVec2(xy));
        }
        this->push_back(face);
    }
}