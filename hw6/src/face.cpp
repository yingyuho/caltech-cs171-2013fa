#include "face.h"

template<>
std::ostream& operator<<(std::ostream &os, const Face<Vec4> &m) {
    for ( Face<Vec4>::CIter it = m.begin(); it != m.end(); it++ )
        os << it->transpose();
    return os;
}

template<>
std::ostream& operator<<(std::ostream &os, const Face<IntVec2> &m) {
    for ( Face<IntVec2>::CIter it = m.begin(); it != m.end(); it++ )
        os << it->transpose();
    return os;
}