#include <iostream>
#include <fstream>

using namespace std;

#include "Viewer.h"

Inventor* parse_inventor(istream &datafile);

string findDir(const string& path){
    size_t slashPos = path.find_last_of("/\\");
    if (slashPos == string::npos)
        return "";
    else
        return path.substr(0, slashPos+1);
}

int main( int argc, char** argv )
{
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
}

