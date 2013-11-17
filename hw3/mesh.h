//
//  mesh.h
//  
//  
//
//  Created by Ying-Yu Ho on 11/16/13.
//
//

#ifndef _mesh_h
#define _mesh_h

#include <list>
#include "transform.h"
#include "face.h"

template< typename T >
class Mesh;

class CoordMesh;

template< typename T >
class Mesh : public std::list<Face<T>*> {
public:
    typedef T Type;
    typedef typename std::list<Face<T>*>::iterator Iter;
    typedef typename std::list<Face<T>*>::const_iterator CIter;

    Mesh() {}
    Mesh(const Mesh&);

    virtual ~Mesh();

    bool triangulated() const;
    void triangulate();
};

class CoordMesh : public Mesh<Vec4> {
    CoordMesh() {}
    CoordMesh(const CoordMesh& mesh) : Mesh(mesh) {}
    virtual ~CoordMesh() {}
    void transform(const CoordTransformer&);
};

// Mesh

template< typename T >
bool Mesh<T>::triangulated() const {
    for ( CIter it = this->begin(); it != this->end(); it++ )
        if ( (*it)->size() != 3 ) return false;
    return true;
}

template< typename T >
Mesh<T>::Mesh(const Mesh& mesh) {
    for ( CIter it = mesh.begin(); it != mesh.end(); it++ )
        this->push_back(new Face<T>(**it));
}

template< typename T >
Mesh<T>::~Mesh() {
    for ( Iter it = this->begin(); it != this->end(); it++ )
        delete *it;
}

template< typename T >
void Mesh<T>::triangulate() {
    if ( triangulated() ) return;

    const int L = this->size();

    for ( int i = 0; i < L; i++ ) {
        Face<T> * polygon = this->front();
        const int V = polygon->size();

        if ( V < 3 ) {
            delete polygon;
            this->pop_front();
            continue;
        } else if ( V == 3 ) {
            this->push_back(polygon);
            this->pop_front();
            continue;
        }

        typename Face<T>::CIter it1 = polygon->begin()++;
        typename Face<T>::CIter it2 = polygon->begin()++++;

        for ( int j = 0; j < V-2; j++ ) {
            Face<T> * triangle = new Face<T>();

            triangle->push_back(polygon->front());
            triangle->push_back(*it1);
            triangle->push_back(*it2);

            this->push_back(triangle);

            it1++; it2++;
        }

        delete polygon;
        this->pop_front();
    }
}

#endif //   _mesh_h