//
//  matrix.h
//  
//
//  Created by Ying-Yu Ho on 10/2/13.
//
//

#ifndef _matrix_h
#define _matrix_h

#include <cmath>
#include <iostream>
#include <algorithm>
#include <stdexcept>

typedef unsigned int matdim_t;

template< typename T, matdim_t R, matdim_t C >
class Matrix;

template< typename T, matdim_t R >
Matrix<T,R-1,1> homogenize(const Matrix<T,R,1>& v);

typedef Matrix<double,4,4> Mat44;
typedef Matrix<double,3,3> Mat33;
typedef Matrix<double,3,1> Vec3;
typedef Matrix<double,4,1> Vec4;
typedef Matrix<double,1,3> NVec3;
typedef Matrix<int,2,1> IntVec2;

class MatrixException { public: virtual ~MatrixException() {} };

// call when, for example, trying to invert a non-square matrix
class WrongDimException : public MatrixException, public std::logic_error { 
public:
    WrongDimException() : std::logic_error("incorrect dimensions") {}
    virtual ~WrongDimException() throw () {}
};

class ZeroDetException : public MatrixException, public std::runtime_error { 
public:
    ZeroDetException() : std::runtime_error("determinant is zero") {}
    ~ZeroDetException() throw () {}
};

class ZeroNormException : public MatrixException, public std::runtime_error {
public:
    ZeroNormException() : std::runtime_error("norm is zero") {}
    ~ZeroNormException() throw () {}
};

template<typename T, matdim_t R, matdim_t C>
class Matrix {
private:
    typedef T ElementType;

    T array[R*C];

    void copy_array(T *target, const T *source) {
        for ( int i = 0; i < R*C; i++ )
            target[i] = source[i];
    }

    // fill the main diagonal with given scalar
    void fill_scalar(T *target, T x) { 
        for (matdim_t i = 0; i < R*C; i++) { target[i] = x*T(i%C==i/C); } }

    // take the dot product of two rows
    T dot_row(const T *row1, const T *row2) const {
        T x = T();
        for (matdim_t j = 0; j < C; j++) { x += row1[j]*row2[j]; }
        return x;
    }

public:
    // constructors
    Matrix(const T *array2) { copy_array(array, array2); }
    Matrix(const Matrix &m) { copy_array(array, m.array); }
    // this makes scalar matrices when R = C
    Matrix(T x = T()) { fill_scalar(array, x); }

    // assignment
    Matrix& operator= (const Matrix& m) {
        if (this!=&m) { copy_array(array, m.array); }
        return *this;
    }

    // accessors
    const T &operator[](matdim_t i) const { return array[i]; }
    T &operator[](matdim_t i) { return array[i]; }
    const T &at(matdim_t i, matdim_t j) const { return (*this)[j+C*i]; }
    T &at(matdim_t i, matdim_t j) { return (*this)[j+C*i]; }

    // operators
    // negation
    Matrix operator- () const {
        Matrix result(*this);
        for (matdim_t i = 0; i < R*C; i++) { result[i] = -result[i]; }
        return result;
    }

    // elementwise + - by another matrix
    #define MAKE_VAL_MATRIX_OP_MATRIX(op_func, op) \
    Matrix op_func (const Matrix &m) const { \
        Matrix result(*this); \
        for (matdim_t i = 0; i < R*C; i++) { result[i] op m[i]; } \
        return result; \
    }
    MAKE_VAL_MATRIX_OP_MATRIX(operator+, +=)
    MAKE_VAL_MATRIX_OP_MATRIX(operator-, -=)

    // elementwise += -= by another matrix
    #define MAKE_REF_MATRIX_OP_MATRIX(op_func, op) \
    Matrix& op_func (const Matrix &m) { \
            for (matdim_t i = 0; i < R*C; i++) { (*this)[i] op m[i]; } \
        return *this; \
    }
    MAKE_REF_MATRIX_OP_MATRIX(operator+=, +=)
    MAKE_REF_MATRIX_OP_MATRIX(operator-=, -=)

    // elementwise * / by scalar
    #define MAKE_VAL_MATRIX_OP_SCALAR(op_func, op) \
    Matrix op_func (const T x) const { \
        Matrix result(*this); \
            for (matdim_t i = 0; i < R*C; i++) { result[i] op x; } \
        return result; \
    }
    MAKE_VAL_MATRIX_OP_SCALAR(operator*, *=)
    MAKE_VAL_MATRIX_OP_SCALAR(operator/, /=)

    // elementwise *= /= by scalar
    #define MAKE_REF_MATRIX_OP_SCALAR(op_func, op) Matrix& op_func (const T x) \
    { \
            for (matdim_t i = 0; i < R*C; i++) { (*this)[i] op x; } \
        return *this; \
    }
    MAKE_REF_MATRIX_OP_SCALAR(operator*=, *=)
    MAKE_REF_MATRIX_OP_SCALAR(operator/=, /=)

    // matrix multiplication
    template<matdim_t C2>
    Matrix<T,R,C2> operator* (const Matrix<T,C,C2> &m) const {
        Matrix<T,R,C2> result;
        // the second matrix is pre-transposed for speed
        Matrix<T,C2,C> m2 = m.transpose();
        for (matdim_t i = 0; i < R; i++)
            for (matdim_t j = 0; j < C2; j++)
                result.at(i,j) = dot_row(&(this->at(i,0)), &(m2.at(j,0)));
        return result;
    }

    // vector dot(A,B) := trace(A.B^T)
    T dot (const Matrix &m) const {
        T x = T();
        for (matdim_t i = 0; i < R*C; i++)
            x += (*this)[i] * m[i];
        return x;
    }

    // utility functions
    Matrix<T,C,R> transpose() const {
        Matrix<T,C,R> m2;
        for (matdim_t i = 0; i < R; i++)
            for (matdim_t j = 0; j < C; j++)
                m2.at(j,i) = this->at(i,j);
        return m2;
    }

    T norm2() const {
        T x = T(0);
            for (matdim_t i = 0; i < R*C; i++) { x += array[i]*array[i]; }
        return x;
    }

    T norm() const { return sqrt(norm2()); }

    Matrix normalize(const T new_norm = T(1.)) const throw ( ZeroNormException ) {
        T old_norm = norm();
        if (0 == old_norm) { throw ZeroNormException(); }
        return Matrix(*this) *= (new_norm/old_norm);
    }

    Matrix inverse() const throw ( WrongDimException, ZeroDetException ) {
        if (R!=C) throw WrongDimException();
        static const matdim_t N = R;

        T augArray[N*N*2];
        static const matdim_t N2 = 2*N;
        
        // juxtapose this matrix and the identity
        for (matdim_t i = 0; i < N; i++) {
            for (matdim_t j = 0; j < N; j++) {
                augArray[j+N2*i] = (*this)[j+N*i];
                augArray[j+N+N2*i] = T(i == j);
            }
        }
        // row reduction
        for (matdim_t j = 0; j < N; j++) {
            // find i_pivot as i s.t. array[i,j] has max abs value, j<=i<=N
            matdim_t i_pivot = j;
            for (matdim_t i = j+1; i < N; i++) {
                if ( fabs(augArray[j+N2*i]) > fabs(augArray[j+N2*i_pivot]) ) {
                    i_pivot = i;
                }
            }
            // throw ZeroDetException if there is no usable pivot
            if ( 0 == fabs(augArray[j+N2*i_pivot]) ) throw ZeroDetException();

            // exchange j-th row with i_pivot-th row
            if ( i_pivot != j ) {
                for (matdim_t j2 = 0; j2 < N2; j2++) {
                    T temp = augArray[j2+N2*j];
                    augArray[j2+N2*j] = augArray[j2+N2*i_pivot];
                    augArray[j2+N2*i_pivot] = temp;
                }
            }

            // (j-th row) /= array[j,j]
            T row_divisor = augArray[j+N2*j];
            for (matdim_t j2 = 0; j2 < N2; j2++) {
                augArray[j2+N2*j] /= row_divisor;
            }

            // use j-th row to eliminate i-th row (i!=j)
            for (matdim_t i = 0; i < N; i++) {
                if (i==j) { continue; }
                T row_deduct = augArray[j+N2*i];
                for (matdim_t j2 = 0; j2 < N2; j2++) {
                    augArray[j2+N2*i] -= row_deduct * augArray[j2+N2*j];
                }
            }
        }

        // generate output
        Matrix matrix_inv;
        for (matdim_t i = 0; i < N; i++) {
            for (matdim_t j = 0; j < N; j++) {
                matrix_inv[j+N*i] = augArray[j+N+N2*i];
            }
        }

        return matrix_inv;
    }
};

// homogenize a vector

template< typename T, matdim_t R >
Matrix<T,R-1,1> homogenize(const Matrix<T,R,1>& v) {
    Matrix<T,R-1,1> v2(&(v[0]));
    return v2 / v[R-1];
}

// left multiplication by a scalar
template<typename T, matdim_t R, matdim_t C>
Matrix<T,R,C> operator* (const T x, const Matrix<T,R,C> &m) { return m*x; }

// printing out nicely
template<typename T, matdim_t R, matdim_t C>
std::ostream& operator<<(std::ostream &os, const Matrix<T,R,C> &m) {
    os << "[";
    for (matdim_t i = 0; i < R; i++) {
        if (i > 0) { os << " ["; } else { os << "["; }
        for (matdim_t j = 0; j < C; j++) {
            if (j > 0) { os << " "; }
            os.width(12);
            os << m.at(i,j);
        }
        os << "]";
        if (i < R-1) { os << std::endl; }
    }
    os << "]" << std::endl;
    return os;
}

#endif //   _matrix_h
