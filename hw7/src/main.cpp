#include <iostream>
#include <fstream>
#include <string>

using namespace std;

#include "Viewer.h"

int main( int argc, char** argv )
{
    #if 0
    if ( argc < 2 )
    {
        cerr << "Usage: " << argv[0] << " script" << endl;
        return 1;
    }

    string inName = argv[1];

    ifstream inFile;

    try
    {
        inFile.open(inName);
    }
    catch (const exception& e)
    {
        cerr << e.what() << endl;
        return 1;
    }

    if ( !inFile.is_open() )
    {
        cerr << "cannot open file: " << inName << endl;
        return 1;
    }

    KeyframeSeries *script = parse_keyframe(inFile);

    if ( !script )
    {
        cerr << "KeyframeSeries object is not made" << endl;
        return 1;
    }

    if ( !script->validate() )
    {
        cerr << "invalid script" << endl;
        return 1;
    }

    cout << *script << endl;

    cout << script->eval(15.1) << endl;
    #endif

    Viewer viewer;
    viewer.init(argc, argv);

    return 0;

    #if 0
    if ( argc < 4 )
    {
        cerr << "Usage: " << argv[0] << " xRes yRes file" << endl;
        return 1;
    }

    string invName = argv[3];

    for (int i = 3; i < argc-1; i++)
        argv[i] = argv[i+1];

    argc--;

    ifstream invFile;

    try
    {
        invFile.open(invName);
    }
    catch (const exception& e)
    {
        cerr << e.what() << endl;
        return 1;
    }

    if (!invFile.is_open())
    {
        cerr << "cannot open file: " << invName << endl;
        return 1;
    }

    // parse inventer commands from stdin
    Inventor * inv = parse_inventor(invFile);
    // make sure the parser doesn't return a null pointer
    if ( !inv ) { cerr << "Inventor object is not made." << endl; return 1; }

    #if 0
    // validate that coordIndex and normalIndex are consistent for each separator
    if ( inv->validate_index_msg() != "" ) {
        cerr << "Inconsistent coordIndex and normalIndex:" << endl \
             << inv->validate_index_msg() << endl;
        return 1;
    }
    #endif

    Viewer viewer;
    viewer.textureDir = findDir(invName);
    viewer.read(inv);
    viewer.init(argc, argv);

    return 1;
    #endif
}

