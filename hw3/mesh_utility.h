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

    int i = 0;
    Mesh<Vec4>::Iter mit = mesh.begin();

    while ( mit != mesh.end() ) {
        Face<Vec4>::Iter fit = (*mit)->begin();
        const Vec4& v0 = *fit; fit++;
        const Vec4& v1 = *fit; fit++;
        const Vec4& v2 = *fit;

        double n_z = (v2[0]-v1[0])*(v0[1]-v1[1]) - (v2[1]-v1[1])*(v0[0]-v1[0]);

        if ( n_z <= 0 ) {
            backFaceIndex.push_back(i);
            delete *mit;
            mit = mesh.erase(mit);
        } else { mit++; }

        i++;
    }
}

template< typename T >
void BackFaceCuller::operator() (Mesh<T>& mesh) const {

    mesh.triangulate();

    int i = 0;
    typename Mesh<T>::Iter mit = mesh.begin();
    std::vector<int>::const_iterator iit = backFaceIndex.begin();

    while ( mit != mesh.end() ) {
        if ( *iit == i ) {
            delete *mit;
            mit = mesh.erase(mit);
            iit++;
        } else { mit++; }

        i++;
    }
}


#endif //   _mesh_utility_h