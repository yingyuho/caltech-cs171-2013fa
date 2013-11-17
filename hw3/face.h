//
//  face.h
//  
//  
//
//  Created by Ying-Yu Ho on 11/16/13.
//
//

#ifndef _face_h
#define _face_h

#include <list>
#include "matrix.h"

template< typename T >
class Face : public std::list<T> {
public:
    typedef T Type;
    typedef typename std::list<T>::iterator Iter;
    typedef typename std::list<T>::const_iterator CIter;
    Face() {}
    Face(const Face& face) : std::list<T>(face) {}
    Face& operator= (const Face& face) {
    	std::list<T>::operator=(face);
    }
    virtual ~Face() {}
};

template< typename T >
std::ostream& operator<<(std::ostream &os, const Face<T> &m) {
    for ( typename Face<T>::CIter it = m.begin(); it != m.end(); it++ )
        os << *it;
    return os;
}

template<>
std::ostream& operator<<(std::ostream &os, const Face<Vec4> &m);

template<>
std::ostream& operator<<(std::ostream &os, const Face<IntVec2> &m);

#endif //   _face_h