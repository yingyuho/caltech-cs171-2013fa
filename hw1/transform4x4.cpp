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
/*    cout << "argc = " << argc << endl; 
    for (int i = 0; i < argc; i++) 
        cout << "argv[" << i << "] = " << argv[i] << endl;*/
    
    listMat44 *transform_list = parse_transform(cin);

    Mat44 transform = Mat44(1.);

    for (listMat44::iterator it = transform_list->begin();
                             it != transform_list->end();
			     it++)
    {
	transform = transform*(**it);
    }

    cout << transform;

    return 0; 
}
