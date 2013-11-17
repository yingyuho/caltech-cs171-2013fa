#include <iostream> 
#include <list>
#include <vector>
#include <cmath>
#include "inventor.h"
#include "canvas.h"
#include "mesh.h"
#include "mesh_utility.h"
#include "ptr_container.h"

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

    // cout << "inv = " << inv << endl;

    // make sure the parser doesn't return a null pointer
    if ( !inv ) { cerr << "Inventor object is not made." << endl; return 1; }

    // validate that coordIndex and normalIndex are consistent for each separator
    if ( inv->validate_index_msg() != "" ) {
        cerr << "Inconsistent coordIndex and normalIndex:" << endl \
             << inv->validate_index_msg() << endl;
        return 1;
    }

    // retrieve list of polygons (in pixel coordinates) from inventor object
    //PB::VertexType resVec(res);
    CoordMesh coord_ws;
    Mesh<NVec3> normal_ws;
    inv->process_mesh(coord_ws);
    inv->process_mesh(normal_ws);

    coord_ws.triangulate();
    normal_ws.triangulate();

    CoordMesh coord_nd(coord_ws);
    coord_nd.transform(inv->get_camera());


    /*for ( PB::MeshCIter it1 = plList.begin(); it1 != plList.end(); it1++ ) {
        // ignore "faces" with only zero or one vertex
        if ( (*it1)->size() < 2 ) { continue; }

        // connect p(i) and p(i+1)
        PB::FaceCIter it2_1 = (*it1)->begin();
        for ( PB::FaceCIter it2_2 = ++((*it1)->begin()); it2_2 != (*it1)->end(); it2_2++ ) {
            canvas->draw_line(*it2_1,*it2_2);
            it2_1 = it2_2;
        }

        // close the polygon by connecting p(n) and p(1)
        canvas->draw_line(*it2_1,(*it1)->front());
    }*/

    // send ppm image to stdout
    canvas->print_ppm(cout);

    delete inv;
    delete canvas;

    return 0; 
}
