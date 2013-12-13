#include <iostream>
using namespace std;

#include "Viewer.h"

Inventor* parse_inventor(istream &datafile);

int main( int argc, char** argv )
{
    /*if( argc != 2 )
    {
        cerr << "usage: " << argv[0] << " in.obj" << endl;
        return 1;
    }*/

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

    Viewer viewer;
    viewer.read(inv);
    viewer.init(argc, argv);

    return 1;
}

