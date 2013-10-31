#include <iostream>
#include "matrix.h"

int main()
{
    typedef Matrix<double,4,4> Mat44;
    typedef Matrix<double,4,1> Vec4;
    double a1[16] = {2,0,0,0,0,-1,0,0,0,0,0,1,0,0,-2,0};
    double a2[16] = {4,0,0,0,0,-3,0,0,0,0,0,-1,0,0,-1,0};
    double a3[16] = {1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16};
    double a4[16] = {2,3,5,7,11,13,17,19,23,29,31,37,41,43,47,53};
    double v1[4] = {3,5,7,9};
    double v2[4] = {1,-1,2,-2};
    double v3[4] = {1,1,-1,-1};
    //{2,3,5,7,11,13,17,19,23};

    int i = 1;

    cout << "Example " << i++ << " (normalization):" << endl \
         << "The unit vector of" << endl << Vec4(v3) << "is" << endl \
         << Vec4(v3).normalize() << endl;

    cout << "Example " << i++ << " (matrix+matrix):" << endl \
         << Mat44(a3) << "plus" << endl << Mat44(a4) << "is" << endl \
         << Mat44(a3) + Mat44(a4) << endl;

    cout << "Example " << i++ << " (scalar*matrix):" << endl \
         << "3 times" << endl << Mat44(a3) << "is" << endl \
         << Mat44(a3) * 3 << endl;

    cout << "Example " << i++ << " (matrix*matrix):" << endl \
         << Mat44(a1) << "times" << endl << Mat44(a2) << "is" << endl \
         << Mat44(a1) * Mat44(a2) << endl;

    cout << "Example " << i++ << " (matrix*vector):" << endl \
         << Mat44(a1) << "times" << endl << Vec4(v1) << "is" << endl \
         << Mat44(a1) * Vec4(v1) << endl;

    cout << "Example " << i++ << " (vector.vector):" << endl \
         << Vec4(v1) << "dots" << endl << Vec4(v2) << "is " \
         << Vec4(v1).dot(Vec4(v2)) << endl << endl;

    cout << "Example " << i++ << " (transpose):" << endl \
         << "the transpose of" << endl << Mat44(a3) << "is" << endl \
         << Mat44(a3).transpose() << endl;

    cout << "Example " << i++ << " (inverse):" << endl \
         << "the inverse of" << endl << Mat44(a4) << "is" << endl \
         << Mat44(a4).inverse() << endl \
         << Mat44(a4) << "times" << endl << Mat44(a4).inverse() << "is" << endl \
         << Mat44(a4) * Mat44(a4).inverse() << endl \
         << Mat44(a4).inverse() << "times" << endl << Mat44(a4) << "is" << endl \
         << Mat44(a4).inverse() * Mat44(a4) << endl;

    /*Mat44 m1(array);
    Mat44 m2 = 2. * m1.inverse();
    Mat44 m3 = 2. * m2.inverse().normalize();
    Mat44 m4 = m2.dot(m1.transpose());

    cout << m1 << endl;
    cout << m2 << endl;
    cout << m3 << endl;
    cout << m4 << endl;*/

    return 0;
}
