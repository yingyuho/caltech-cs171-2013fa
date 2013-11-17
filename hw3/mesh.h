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

#include <iostream>
#include "ptr_container.h"
#include "transform.h"
#include "face.h"

template< typename T >
class Mesh;

class CoordMesh;
class PixelMesh;

template< typename T >
class Mesh : public PtrList< Face<T> > {
public:
    typedef T Type;
    typedef typename PtrList< Face<T> >::iterator Iter;
    typedef typename PtrList< Face<T> >::const_iterator CIter;

    Mesh() {}
    Mesh(const Mesh& x) : PtrList< Face<T> >(x) {};
    Mesh& operator= (const Mesh& x) { PtrList< Face<T> >::operator=(x); }
    virtual ~Mesh() {};

    bool triangulated() const;
    void triangulate();
};

class CoordMesh : public Mesh<Vec4> {
public:
    CoordMesh() {}
    CoordMesh(const CoordMesh& mesh) : Mesh(mesh) {}
    virtual ~CoordMesh() {}
    void transform(const CoordTransformer&);
};

class PixelMesh : public Mesh<IntVec2> {
public:
    const int xRes;
    const int yRes;
    PixelMesh(int xRes, int yRes) : xRes(xRes), yRes(yRes) {}
    PixelMesh(const PixelMesh& m) : Mesh(m), xRes(m.xRes), yRes(m.yRes) {}
    PixelMesh(const CoordMesh&, int, int);
    virtual ~PixelMesh() {}
};

// Mesh

template< typename T >
bool Mesh<T>::triangulated() const {
    for ( CIter it = this->begin(); it != this->end(); it++ )
        if ( (*it)->size() != 3 ) return false;
    return true;
}

// template< typename T >
// Mesh<T>::Mesh(const Mesh& mesh) {
//     for ( CIter it = mesh.begin(); it != mesh.end(); it++ )
//         this->push_back(new Face<T>(**it));
// }

// template< typename T >
// Mesh<T>::~Mesh() {
//     for ( Iter it = this->begin(); it != this->end(); it++ )
//         delete *it;
// }

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

        typename Face<T>::CIter it0 = polygon->begin();
        typename Face<T>::CIter it1 = polygon->begin(); it1++;

        for ( int j = 0; j < V-2; j++ ) {
            Face<T> * triangle = new Face<T>();

            triangle->push_back(*it0);
            triangle->push_back(*it1); it1++;
            triangle->push_back(*it1);

            this->push_back(triangle);
        }

        delete polygon;
        this->pop_front();
    }
}

// operator<< (std::ostream, const Mesh<T>)

template< typename T >
std::ostream& operator<<(std::ostream &os, const Mesh<T> &m) {
    os << "Mesh = {" << std::endl;
    int i = 0;
    for ( typename Mesh<T>::CIter it = m.begin(); it != m.end(); it++ ) {
        if ( it != m.begin() ) { os << "," << std::endl; }
        os << "Face " << i++ << " = " << std::endl << (**it); 
    }
    os << "}" << std::endl;
    return os;
}

#endif //   _mesh_h