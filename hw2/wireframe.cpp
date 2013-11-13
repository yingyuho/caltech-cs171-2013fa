#include <iostream> 
#include <list>
#include <vector>
#include <cmath>
#include "transform.h"
#include "inventor.h"
#include "canvas.h"

using namespace std;
typedef Matrix<double,4,4> Mat44;
typedef Matrix<double,3,1> Vec3;
typedef list<Mat44*> listMat44;

Inventor* parse_inventor(istream &datafile);

int main(int argc, char* argv[])
{   
    // check if all required arguments are present
    if (argc < 3)
    {
        cerr << "Usage: " << argv[0] << " xRes yRes" << endl;
        return 1;
    }

    int res[2] = { atoi(argv[1]), atoi(argv[2]) };

    Canvas<vector> * canvas;

    try {
        canvas = new Canvas<vector> (res[0],res[1]);
    } catch (const std::invalid_argument& e) {
        cerr << e.what() << endl;
        return 1;
    }

    Inventor * inv = parse_inventor(cin);

    IntVec2 resVec(res);
    list<list<IntVec2>*> plList;
    inv->build_polygon_list( plList, resVec );


    for ( list<list<IntVec2>*>::const_iterator it1 = plList.begin(); it1 != plList.end(); it1++ ) {
        if ( (*it1)->size() < 2 ) { continue; }

        list<IntVec2>::const_iterator it2_1 = (*it1)->begin();

        for ( list<IntVec2>::const_iterator it2_2 = ++((*it1)->begin()); it2_2 != (*it1)->end(); it2_2++ ) {
            canvas->draw_pixel(canvas->bresenham(*it2_1,*it2_2));
            it2_1 = it2_2;
        }

        canvas->draw_pixel(canvas->bresenham(*it2_1,(*it1)->front()));
    }

    canvas->print_ppm(cout);

    // // start with the identity matrix
    // Mat44 transform = Mat44(1.);

    // for (listMat44::iterator it = transform_list->begin();
    //                          it != transform_list->end();
    //                          it++)
    // {
    //     // right multiplication by matrices in the list
    //     transform = transform * (**it);
    // }

    // cout << t->to_matrix() << endl;

    delete inv;
    delete canvas;

    return 0; 
}
