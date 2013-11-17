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

#endif //   _face_h