//
//  mesh_processor.h
//  
//  
//
//  Created by Ying-Yu Ho on 11/15/13.
//
//

#ifndef _mesh_processor_h
#define _mesh_processor_h

#include <list>
#include <vector>
#include "matrix.h"

// A subclass of this accept the reference of a list of polygons with coordinate type T
// and put more polygons into the list according to the data with type U.
template< typename T, typename U > class MeshProcessor;

// A specialization of MeshProcessor that only need the reference of a polygon list
template< typename T > class MeshProcessor<T,void>;

template< typename T, typename U >
class MeshProcessor {
public:
    typedef T VertexType;
    typedef typename std::list<T> Face;
    typedef typename Face::iterator FaceIter;
    typedef typename Face::const_iterator FaceCIter;
    typedef typename std::list<Face*> Mesh;
    typedef typename Mesh::iterator MeshIter;
    typedef typename Mesh::const_iterator MeshCIter;
    typedef U PointMap;

    virtual ~MeshProcessor() {}
    virtual void process_mesh(Mesh& mesh, U data) const = 0;
};

template< typename T >
class MeshProcessor<T,void> {
public:
    typedef T VertexType;
    typedef typename std::list<T> Face;
    typedef typename Face::iterator FaceIter;
    typedef typename Face::const_iterator FaceCIter;
    typedef typename std::list<Face*> Mesh;
    typedef typename Mesh::iterator MeshIter;
    typedef typename Mesh::const_iterator MeshCIter;

    virtual ~MeshProcessor() {}
    virtual void process_mesh(Mesh& mesh) const = 0;
};

#endif //   _mesh_processor_h