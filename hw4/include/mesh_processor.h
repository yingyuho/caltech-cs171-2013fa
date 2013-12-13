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
#include "mesh.h"

// A subclass of this accept the reference of a list of polygons with coordinate type T
// and put more polygons into the list according to the data with type U.
template< typename T, typename U > class MeshProcessor;

// A specialization of MeshProcessor that only need the reference of a polygon list
template< typename T > class MeshProcessor<T,void>;

template< typename T, typename U >
class MeshProcessor {
public:
    virtual ~MeshProcessor() {}
    virtual void process_mesh(Mesh<T>& mesh, U data) const = 0;
};

template< typename T >
class MeshProcessor<T,void> {
public:
    virtual ~MeshProcessor() {}
    virtual void process_mesh(Mesh<T>& mesh) const = 0;
};

#endif //   _mesh_processor_h