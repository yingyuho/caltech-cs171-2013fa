#include <iostream> 
#include <list>
#include <cmath>
#include "matrix.h"

using namespace std;
typedef Matrix<double,4,4> Mat44;
typedef Matrix<double,3,1> Vec3;
typedef list<Mat44*> listMat44;

listMat44* parse_transform(istream &datafile);

int main(int argc, char* argv[])
{   
    listMat44 *transform_list = parse_transform(cin);

    // start with the identity matrix
    Mat44 transform = Mat44(1.);

    for (listMat44::iterator it = transform_list->begin();
                             it != transform_list->end();
                             it++)
    {
        // left multiplication by matrices in the list
        transform = transform * (**it);
    }

    cout << transform;

    return 0; 
}
