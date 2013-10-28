#include <iostream>
#include "matrix.h"

int main()
{
    const int N = 3;
    typedef Matrix<double,N,N> SquareMatrixN;
    double array[N*N] = {2,3,5,7,11,13,17,19,23};

    SquareMatrixN m1(array);
    SquareMatrixN m2 = 2. * m1.inverse();
    SquareMatrixN m3 = 2. * m2.inverse().normalize();
    SquareMatrixN m4 = m2.dot(m1.transpose());

    cout << m1 << endl;
    cout << m2 << endl;
    cout << m3 << endl;
    cout << m4 << endl;

    return 0;
}
