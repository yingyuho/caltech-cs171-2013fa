//
//  mesh_utility.h
//  
//  
//
//  Created by Ying-Yu Ho on 11/15/13.
//
//

#ifndef _mesh_utility_h
#define _mesh_utility_h

#include "mesh.h"

class BackFaceCuller;

class BackFaceCuller {
private:
    std::vector<int> backFaceIndex;
public:
    BackFaceCuller(Mesh<Vec4>&);
    virtual ~BackFaceCuller() {}
    template< typename T >
    void operator() (Mesh<T>&) const;
};

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
        else
            newMesh.push_back(mesh[i]);
    }

    mesh.clear();
    mesh.swap(newMesh);
}

template< typename T >
void BackFaceCuller::operator() (Mesh<T>& mesh) const {

    mesh.triangulate();

    int i = 0;
    Mesh<T> newMesh(backFaceIndex.size());

    for ( size_t j = 0; j < mesh.size(); j++ ) {
        if ( j == backFaceIndex[i] ) 
            newMesh[i++] = mesh[j];
        else
            delete mesh[j];
    }

    mesh.clear();
    mesh.swap(newMesh);

    // typename Mesh<T>::Iter mit = mesh.begin();
    // std::vector<int>::const_iterator iit = backFaceIndex.begin();

    // while ( mit != mesh.end() ) {
    //     if ( *iit == i ) {
    //         delete *mit;
    //         mit = mesh.erase(mit);
    //         iit++;
    //     } else { mit++; }

    //     i++;
    // }
}


#endif //   _mesh_utility_h