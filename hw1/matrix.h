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
//#include <functional>
#include <stdexcept>
using namespace std;

typedef unsigned int matdim_t;

class MatrixException { public: virtual ~MatrixException() {} };
class WrongDimException : public MatrixException, public logic_error
{ 
public:
    WrongDimException() : logic_error("incorrect dimensions") {}
    ~WrongDimException() throw () {}
};
class ZeroDetException : public MatrixException, public runtime_error
{ 
public:
    ZeroDetException() : runtime_error("determinant is zero") {}
    ~ZeroDetException() throw () {}
};
class ZeroNormException : public MatrixException, public runtime_error
{
public:
    ZeroNormException() : runtime_error("norm is zero") {}
    ~ZeroNormException() throw () {}
};

template<typename T, matdim_t R, matdim_t C>
class Matrix
{
private:
    T array[R*C];

    void copy_array(T *target, const T *source)
    {
    	for (matdim_t i = 0; i < R*C; i++) { target[i] = source[i]; }
    }

    void fill_scalar(T *target, T x)
    { for (matdim_t i = 0; i < R*C; i++) { target[i] = x*T(i%C==i/C); } }

    T dot_row(const T *row1, const T *row2) const
    {
    	T x = T();
        for (matdim_t j = 0; j < C; j++) { x += row1[j]*row2[j]; }
	return x;
    }

public:
    // constructors
    Matrix(const T *array2) { copy_array(array, array2); }
    Matrix(const Matrix &m) { copy_array(array, m.array); }
    Matrix(T x = T()) { fill_scalar(array, x); }

    // assignment
    Matrix& operator= (const Matrix& m)
    {
        if (this!=&m) { copy_array(array, m.array); }
	return *this;
    }

    // accessors
    const T &operator[](matdim_t i) const { return array[i]; }
    T &operator[](matdim_t i) { return array[i]; }
    const T &at(matdim_t i, matdim_t j) const { return (*this)[j+C*i]; }
    T &at(matdim_t i, matdim_t j) { return (*this)[j+C*i]; }

    // operators
    // elementwise + - by another matrix
    #define MAKE_VAL_MATRIX_OP_MATRIX(op_func, op) \
    Matrix op_func (const Matrix &m) const \
    { \
        Matrix result(*this); \
    	for (matdim_t i = 0; i < R*C; i++) { result[i] op m[i]; } \
	return result; \
    }
    MAKE_VAL_MATRIX_OP_MATRIX(operator+, +=)
    MAKE_VAL_MATRIX_OP_MATRIX(operator-, -=)

    // elementwise += -= by another matrix
    #define MAKE_REF_MATRIX_OP_MATRIX(op_func, op) \
    Matrix& op_func (const Matrix &m) \
    { \
    	for (matdim_t i = 0; i < R*C; i++) { (*this)[i] op m[i]; } \
	return *this; \
    }
    MAKE_REF_MATRIX_OP_MATRIX(operator+=, +=)
    MAKE_REF_MATRIX_OP_MATRIX(operator-=, -=)

    // elementwise * / by scalar
    #define MAKE_VAL_MATRIX_OP_SCALAR(op_func, op) \
    Matrix op_func (const T x) const \
    { \
        Matrix result(*this); \
    	for (matdim_t i = 0; i < R*C; i++) { result[i] op x; } \
	return result; \
    }
    MAKE_VAL_MATRIX_OP_SCALAR(operator*, *=)
    MAKE_VAL_MATRIX_OP_SCALAR(operator/, /=)

    // elementwise *= /= by scalar
    #define MAKE_REF_MATRIX_OP_SCALAR(op_func, op) \
    Matrix& op_func (const T x) \
    { \
    	for (matdim_t i = 0; i < R*C; i++) { (*this)[i] op x; } \
	return *this; \
    }
    MAKE_REF_MATRIX_OP_SCALAR(operator*=, *=)
    MAKE_REF_MATRIX_OP_SCALAR(operator/=, /=)

    // matrix multiplication
    template<matdim_t C2>
    Matrix operator* (const Matrix<T,C,C2> m) const
    {
        Matrix<T,R,C2> result;
        Matrix<T,C2,C> m2 = m.transpose();
	for (matdim_t i = 0; i < R; i++)
	    for (matdim_t j = 0; j < C2; j++)
	        result.at(i,j) = dot_row(&(this->at(i,0)), &(m2.at(j,0)));
	return result;
    }

    // vector dot
    Matrix dot (const Matrix m) const
    {
        T x = T();
	for (matdim_t i = 0; i < R; i++)
	    for (matdim_t j = 0; j < R; j++)
	        x += dot_row(&(this->at(i,0)), &(m.at(j,0)));
	return x;
    }

    // utility functions
    Matrix<T,C,R> transpose() const
    {
        Matrix<T,C,R> m2;
	for (matdim_t i = 0; i < R; i++)
	    for (matdim_t j = 0; j < C; j++)
	        m2.at(j,i) = this->at(i,j);
	return m2;
    }

    T norm() const
    {
        T x = T(0);
    	for (matdim_t i = 0; i < R*C; i++) { x += array[i]*array[i]; }
	return sqrt(x);
    }

    Matrix normalize(const T new_norm = T(1.)) const throw ( ZeroNormException )
    {
        T old_norm = norm();
	if (0 == old_norm) { throw ZeroNormException(); }
	return Matrix(*this) *= (new_norm/old_norm);
    }

    Matrix inverse() const throw ( WrongDimException, ZeroDetException )
    {
        if (R!=C) throw WrongDimException();
	static const matdim_t N = R;

        T augArray[N*N*2];
	static const matdim_t N2 = 2*N;
	
	// juxtapose this matrix and the identity
	for (matdim_t i = 0; i < N; i++)
	{
	    for (matdim_t j = 0; j < N; j++)
	    {
	        augArray[j+N2*i] = (*this)[j+N*i];
		augArray[j+N+N2*i] = T(i == j);
	    }
	}
	// row reduction
	for (matdim_t j = 0; j < N; j++)
	{
	    // find i_pivot as i s.t. array[i,j] has max abs value, j<=i<=N
	    matdim_t i_pivot = j;
	    for (matdim_t i = j+1; i < N; i++)
	    {
	        if ( fabs(augArray[j+N2*i]) > fabs(augArray[j+N2*i_pivot]) )
		{
		    i_pivot = i;
		}
	    }
	    // throw ZeroDetException if there is no usable pivot
	    if ( 0 == fabs(augArray[j+N2*i_pivot]) ) throw ZeroDetException();

	    // exchange j-th row with i_pivot-th row
	    if ( i_pivot != j )
	    {
		for (matdim_t j2 = 0; j2 < N2; j2++)
		{
		    T temp = augArray[j2+N2*j];
		    augArray[j2+N2*j] = augArray[j2+N2*i_pivot];
		    augArray[j2+N2*i_pivot] = temp;
		}
	    }

	    // (j-th row) /= array[j,j]
	    T row_divisor = augArray[j+N2*j];
	    for (matdim_t j2 = 0; j2 < N2; j2++)
	    {
	        augArray[j2+N2*j] /= row_divisor;
            }

	    // use j-th row to eliminate i-th row (i!=j)
	    for (matdim_t i = 0; i < N; i++)
	    {
	        if (i==j) { continue; }
	        T row_deduct = augArray[j+N2*i];
		for (matdim_t j2 = 0; j2 < N2; j2++)
		{
		    augArray[j2+N2*i] -= row_deduct * augArray[j2+N2*j];
		}
	    }
	}

	// generate output
	Matrix matrix_inv;
	for (matdim_t i = 0; i < N; i++)
	{
	    for (matdim_t j = 0; j < N; j++)
	    {
	        matrix_inv[j+N*i] = augArray[j+N+N2*i];
	    }
	}

	return matrix_inv;
    }
};

template<typename T, matdim_t R, matdim_t C>
Matrix<T,R,C> operator* (const T x, const Matrix<T,R,C> &m) { return m*x; }

template<typename T, matdim_t R, matdim_t C>
ostream& operator<<(ostream &os, const Matrix<T,R,C> &m)
{
    os << "[";
    for (matdim_t i = 0; i < R; i++)
    {
        if (i > 0) { os << " ["; } else { os << "["; }
	for (matdim_t j = 0; j < C; j++)
	{
	    if (j > 0) { os << " "; }
	    os.width(10);
	    os << m.at(i,j);
	}
	os << "]";
	if (i < R-1) { os << endl; }
    }
    os << "]" << endl;
    return os;
}

#endif //   _matrix_h
