#include <iostream> 
#include <list>
#include <vector>
#include <cmath>
#include "inventor.h"
#include "canvas.h"

using namespace std;

Inventor* parse_inventor(istream &datafile);

int main(int argc, char* argv[])
{   
    // check if all required arguments are present
    if (argc < 3) {
        cerr << "Usage: " << argv[0] << " xRes yRes" << endl;
        return 1;
    }

    int res[2] = { atoi(argv[1]), atoi(argv[2]) };

    Canvas<vector> * canvas;

    // make sure xRes, yRes > 0 so that canvas is successfully built
    try {
        canvas = new Canvas<vector> (res[0],res[1]);
    } catch (const std::invalid_argument& e) {
        cerr << e.what() << endl;
        return 1;
    }

    // parse inventer commands from stdin
    Inventor * inv = parse_inventor(cin);

    // make sure the parser doesn't return a null pointer
    if ( !inv ) { cerr << "Inventor object is not made." << endl; return 1; }

    // retrieve list of polygons (in pixel coordinates) from inventor object
    typedef PolygonBuilder<IntVec2> PB;
    PB::VertexType resVec(res);
    PB::PolygonList plList;
    inv->build_polygon_list( plList, resVec );


    for ( PB::PolygonCIter it1 = plList.begin(); it1 != plList.end(); it1++ ) {
        // ignore "faces" with only zero or one vertex
        if ( (*it1)->size() < 2 ) { continue; }

        // connect p(i) and p(i+1)
        PB::VertexCIter it2_1 = (*it1)->begin();
        for ( PB::VertexCIter it2_2 = ++((*it1)->begin()); it2_2 != (*it1)->end(); it2_2++ ) {
            canvas->draw_line(*it2_1,*it2_2);
            it2_1 = it2_2;
        }

        // close the polygon by connecting p(n) and p(1)
        canvas->draw_line(*it2_1,(*it1)->front());
    }

    // send ppm image to stdout
    canvas->print_ppm(cout);

    delete inv;
    delete canvas;

    return 0; 
}
