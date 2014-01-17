//
//  color.h
//  
//  
//
//  Created by Ying-Yu Ho on 11/20/13.
//
//

#ifndef _color_h
#define _color_h

#include "matrix.h"


template< typename T >
class Color : public Matrix<T,3,1> {
public:
    Color(T x = T(0));
    Color(const Matrix<T,3,1> &);
    Color operator+ (const Color&) const;
    Color operator- (const Color&) const;
    Color operator* (const Color&) const;
    Color operator* (const T& x) const;
    Color operator/ (const T& x) const;
    Color zeroclip() const;
    Color oneclip() const;
    Color zerooneclip() const;
};

// Color

template< typename T >
Color<T>::Color(T x) { (*this)[0] = (*this)[1] = (*this)[2] = x; }

template< typename T >
Color<T>::Color(const Matrix<T,3,1>& v) : Matrix<T,3,1>(v) {}

template< typename T >
Color<T> Color<T>::operator* (const Color& c) const {
    Color result(*this);
    result[0] *= c[0]; result[1] *= c[1]; result[2] *= c[2];
    return result;
}

template< typename T >
Color<T> Color<T>::operator+ (const Color& c) const {
    Color result(*this);
    result[0] += c[0]; result[1] += c[1]; result[2] += c[2];
    return result;
}

template< typename T >
Color<T> Color<T>::operator- (const Color& c) const {
    Color result(*this);
    result[0] -= c[0]; result[1] -= c[1]; result[2] -= c[2];
    return result;
}

template< typename T >
Color<T> Color<T>::operator* (const T& x) const {
    Matrix<T,3,1> result(*this);
    return Color(result*x);
}

template< typename T >
Color<T> Color<T>::operator/ (const T& x) const {
    Matrix<T,3,1> result(*this);
    return Color(result/x);
}

template< typename T >
Color<T> operator* (const T& x, const Color<T> c) {
    return c*x;
}

template< typename T >
Color<T> Color<T>::zeroclip() const {
    Color result(*this);
    for ( int i = 0; i < 3; i++ )
        if ( result[i] < T(0) ) result[i] = T(0);
    return result;
}

template< typename T >
Color<T> Color<T>::oneclip() const {
    Color result(*this);
    for ( int i = 0; i < 3; i++ )
        if ( result[i] > T(1) ) result[i] = T(1);
    return result;
}

template< typename T >
Color<T> Color<T>::zerooneclip() const {
    return zeroclip().oneclip();
}

#endif //   _color_h