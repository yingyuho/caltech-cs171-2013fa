#include <iostream> 
#include <list>
#include <vector>
#include <cmath>
#include <string>
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
    const char* help = \
    " mode xRes yRes [-eyelight[=strength]] < input.iv\n       mode: 0 = Flat, 1 = Gouraud, 2 = Phong";

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

    #define ERROR_MSG() \
    cerr << "Arguments given: "; \
    while ( optind < argc ) cerr << argv[optind++] << " "; \
    cerr << endl; \
    cerr << "Usage: " << argv[0] << help << endl;

    // check if all required arguments (xRes and yRes) are present
    if (argc - optind < 3) {
        cerr << "Usage: " << argv[0] << help << endl;
        return 1;
    }

    int mode;
    int res[2];


    try {
        // set shading mode
        mode = stoi(argv[optind]);
        // set res = { xRes, yRes }
        res[0] = stoi(argv[optind+1]);
        res[1] = stoi(argv[optind+2]);
    } catch (const exception& e) {
        cerr << e.what() << endl;
        ERROR_MSG()
        return 1;
    }

    if ( mode < 0 || mode > 2 ) {
        cerr << "Shading mode \"mode\" is out of range." << endl;
        ERROR_MSG()
        return 1;
    }

    Canvas * canvas;

    // make sure xRes, yRes > 0 so that canvas is successfully built
    try {
        canvas = new Canvas(res[0],res[1]);
    } catch (const exception& e) {
        cerr << e.what() << endl;
        ERROR_MSG()
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

    // convert Inventor to ShadingComplex
    ShadingComplex sComp(*inv);
    const PtrList<ShadingModule>& smList = sComp.get_module_list();

    // rasterization machine
    Raster ras(res[0], res[1]);

    for ( PtrList<ShadingModule>::const_iterator it = smList.cbegin(); \
        it != smList.cend(); it++ ) {

        // lighting machine for each separator
        Illuminator lighter(sComp.get_camera_position(), \
            sComp.get_lights(), (*it)->get_material(), eyelight_flag, eyelight_str);

        const Mesh<ShadingData>& mesh = (*it)->get_mesh();

        // combine rasterization and lighting machines with differet modes
        for ( Mesh<ShadingData>::CIter it2 = mesh.begin(); it2 != mesh.end(); it2++ ) {
            switch (mode) {
                case 0:
                canvas->draw_pixel( ras.raster_flat(**it2, lighter) );
                break;
                case 1:
                canvas->draw_pixel( ras.raster_gouraud(**it2, lighter) );
                break;
                case 2:
                canvas->draw_pixel( ras.raster_phong(**it2, lighter) );
                break;
                default:
                cerr << "Unknown shading mode = " << mode << endl;
                return 1;
            }
        }
    
    }

    // send ppm image to stdout
    canvas->print_ppm(cout);

    delete inv;
    delete canvas;

    return 0; 
}
