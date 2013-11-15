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

#include "mesh_processor.h"

template< typename T >
class Triangulator;

template< typename T >
class Triangulator : public MeshProcessor<T,void> {
private:
    typedef MeshProcessor<T,void> MP;
public:
    Triangulator() {}
    virtual ~Triangulator() {}
    virtual void process_mesh(typename MP::Mesh& mesh) const;
};

template< typename T >
void triangulate(typename Triangulator<T>::Mesh& mesh) {
    static const Triangulator<T> tri;
    tri.process_mesh(mesh);
}

// Triangulator

template< typename T >
void Triangulator<T>::process_mesh(typename MP::Mesh& mesh) const {
    const int L = mesh.size();

    for ( int i = 0; i < L; i++ ) {
        typename MP::Face * polygon = mesh.front();
        const int V = polygon->size();

        if ( V < 3 ) {
            delete polygon;
            mesh.pop_front();
            continue;
        } else if ( V == 3 ) {
            mesh.push_back(polygon);
            mesh.pop_front();
            continue;
        }

        typename MP::FaceCIter it1 = polygon->begin()++;
        typename MP::FaceCIter it2 = polygon->begin()++++;

        for ( int j = 0; j < V-2; j++ ) {
            typename MP::Face * triangle = new typename MP::Face();

            triangle->push_back(polygon->front());
            triangle->push_back(*it1);
            triangle->push_back(*it2);

            mesh.push_back(triangle);

            it1++; it2++;
        }

        delete polygon;
        mesh.pop_front();
    }
}

#endif //   _mesh_utility_h