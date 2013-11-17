#include "mesh.h"

// CoordMesh

void CoordMesh::transform(const CoordTransformer& t) {
    for ( Iter it = this->begin(); it != this->end(); it++ )
        for ( Face<Vec4>::Iter it2 = (*it)->begin(); it2 != (*it)->end(); it2++ )
            (*it2) = t.to_left_matrix() * (*it2);
}