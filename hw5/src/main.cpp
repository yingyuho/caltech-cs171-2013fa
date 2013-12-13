#include <iostream>
using namespace std;

#include "Viewer.h"

int main( int argc, char** argv )
{

    Viewer viewer;
    viewer.read();
    viewer.init(argc, argv);

    return 1;
}

