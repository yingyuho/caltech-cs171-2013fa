#include "mesh_utility.h"

// BackFaceCuller

BackFaceCuller::BackFaceCuller(Mesh<Vec4>& mesh) {

    mesh.triangulate();

    Mesh<Vec4> newMesh;

    for ( size_t i = 0; i < mesh.size(); i++ ) {
        const Vec4& v0 = mesh[i]->at(0);
        const Vec4& v1 = mesh[i]->at(1);
        const Vec4& v2 = mesh[i]->at(2);

        double n_z = (v2[0]-v1[0])*(v0[1]-v1[1]) - (v2[1]-v1[1])*(v0[0]-v1[0]);

        if ( n_z <= 0 )
            delete mesh[i];
        else {
            newMesh.push_back(mesh[i]);
            backFaceIndex.push_back(i);
        }
    }

    mesh.clear();
    mesh.swap(newMesh);
}