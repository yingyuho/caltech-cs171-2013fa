#include <iostream> 
#include <list>
#include <vector>
#include <cmath>
#include <getopt.h>
#include "inventor.h"
#include "canvas.h"
#include "raster.h"
#include "shading_complex.h"

using namespace std;

Inventor* parse_inventor(istream &datafile);

static int eyelight_flag = 0;
static double eyelight_str = 0.2;

int main(int argc, char* argv[])
{   
    int c;
    const char* help = " xRes yRes [-eyelight[=strength]] < input.iv";

    // parse the options
    while (1) {
        static struct option long_options[] = { \
            {"eyelight", optional_argument, &eyelight_flag, 1}, \
            {0,0,0,0} };

        /* getopt_long stores the option index here. */
        int option_index = 0;
     
        c = getopt_long_only(argc, argv, "", long_options, &option_index);
 
        /* Detect the end of the options. */
        if (c == -1) break;
     
        switch (c) {
        case 0:
            if ( (long_options[option_index].flag == &eyelight_flag) && optarg)
                eyelight_str = std::atof(optarg);
            break;
        case '?':
            /* getopt_long already printed an error message. */
            break;
        default:
            abort ();
        }
    }

    // check if all required arguments (xRes and yRes) are present
    if (argc - optind < 2) {
        cerr << "Usage: " << argv[0] << help << endl;
        return 1;
    }

    // set res = { xRes, yRes }
    int res[2] = { atoi(argv[optind]), atoi(argv[optind+1]) };

    Canvas * canvas;

    // make sure xRes, yRes > 0 so that canvas is successfully built
    try {
        canvas = new Canvas(res[0],res[1]);
    } catch (const std::invalid_argument& e) {
        cerr << e.what() << endl;
        cerr << "Arguments given: ";
        while ( optind < argc ) cerr << argv[optind++] << " ";
        cerr << endl;
        cerr << "Usage: " << argv[0] << help << endl;
        return 1;
    }

    // parse inventer commands from stdin
    Inventor * inv = parse_inventor(cin);

    // make sure the parser doesn't return a null pointer
    if ( !inv ) { cerr << "Inventor object is not made." << endl; return 1; }

    // validate that coordIndex and normalIndex are consistent for each separator
    if ( inv->validate_index_msg() != "" ) {
        cerr << "Inconsistent coordIndex and normalIndex:" << endl \
             << inv->validate_index_msg() << endl;
        return 1;
    }

    ShadingComplex sComp(*inv);
    const PtrList<ShadingModule>& smList = sComp.get_module_list();

    for ( PtrList<ShadingModule>::const_iterator it = smList.cbegin(); \
        it != smList.cend(); it++ ) {
        //cout << (*it)->get_mesh();
    }



    // retrieve list of polygons (in pixel coordinates) from inventor object

    //PB::VertexType resVec(res);
    CoordMesh coord_ws;
    inv->process_mesh(coord_ws);
    coord_ws.triangulate();

    NormalMesh normal_ws;
    inv->process_mesh(normal_ws);
    normal_ws.normalize();
    normal_ws.triangulate();

    CoordMesh coord_nd(coord_ws);
    coord_nd.transform(inv->get_camera());

    BackFaceCuller bfc(coord_nd);
    bfc(coord_ws);
    bfc(normal_ws);

    PixelMesh pxMesh(coord_nd, res[0], res[1]);

    Raster ras(res[0], res[1]);
    int ca[6] = {40,80,120,160,200,240};
    int ci = 0;
    for ( CoordMesh::CIter it1 = coord_nd.begin(); it1 != coord_nd.end(); it1++ ) {
        canvas->draw_pixel(ras.raster_plain(**it1),ca[ci%6],ca[ci%6],ca[ci%6]);
        ci++;
    }

    /*for ( PixelMesh::CIter it1 = pxMesh.begin(); it1 != pxMesh.end(); it1++ ) {
        // ignore "faces" with only zero or one vertex
        if ( (*it1)->size() < 2 ) { continue; }

        // connect p(i) and p(i+1)
        Face<IntVec2>::CIter it2_1 = (*it1)->begin();
        for ( Face<IntVec2>::CIter it2_2 = ++((*it1)->begin()); it2_2 != (*it1)->end(); it2_2++ ) {
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
