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

}


#endif //   _mesh_utility_h