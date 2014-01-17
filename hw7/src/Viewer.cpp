#include "Viewer.h"

#include <iostream>
#include <fstream>
#include <algorithm>
#include <string>
#include <cmath>

using namespace std;
using namespace DDG;

#include <getopt.h>

const double Viewer::RADTODEG = 180. / 3.141592653589793238462;

KeyframeSeries  * Viewer::script = 0;
GLUquadricObj   * Viewer::quad = 0;

std::vector<FrameStruct>    Viewer::keyframeSlopes;
double          Viewer::replaySpeed = 15.0;
double          Viewer::replayTime = 0.0;
bool            Viewer::replayLoop = false;
bool            Viewer::replayPlay = true;
unsigned int    Viewer::replayJump = 0;

int     Viewer::window_w = 500;
int     Viewer::window_h = 500;

int     Viewer::mouseX = 0;
int     Viewer::mouseY = 0;

int     Viewer::mouseState = mouseNone;

double  Viewer::cameraRadius = 10.;
double  Viewer::cameraRadiusMomentum = 0.;
double  Viewer::cameraAngle = 0.;
double  Viewer::cameraAngleMomentum = 0.;

double  Viewer::userTrans[3] = { 0., 0., 0. };
Quaternion Viewer::userRot(1.);

int     Viewer::t0 = -1;

KeyframeSeries * parse_keyframe(istream &datafile);

Viewer::Viewer() {}

int Viewer::parseArgs(int& argc, char* argv[]) {
    glutInit(&argc, argv);

    int c;
    const char* help = " script";

    // parse the options
    while (1) {
        static struct option long_options[] = { {0,0,0,0} };

        /* getopt_long stores the option index here. */
        int option_index = 0;
     
        c = getopt_long_only(argc, argv, "", long_options, &option_index);
 
        /* Detect the end of the options. */
        if (c == -1) break;
     
        switch (c) {
        // case 0:
        //     if ( (long_options[option_index].flag == &eyelight_flag) && optarg)
        //         eyelight_str = std::atof(optarg);
        //     break;
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

    // check if all required arguments (script) are present
    if (argc - optind < 1) {
        cerr << "Usage: " << argv[0] << help << endl;
        return 1;
    }

    string inName = argv[optind+0];

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

    Viewer::script = parse_keyframe(inFile);

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

    keyframeSlopes = script->buildSlopes();

    window_w = 500;
    window_h = 500;

    return 0;
}

void Viewer::init(int& argc, char* argv[])
{
    if ( parseArgs(argc, argv) ) return;
    initGLUT();
    initGL();
    initCamera();
    initLights();

    // From here on, GLUT has control,
    glutMainLoop();
}


void Viewer::keyfunc(GLubyte key, GLint x, GLint y)
{
    if ( key >= 33 && key <= 126 )
        cerr << (char) key;
    else switch (key)
    {
        case 27: cerr << "[Esc]"; break;
        case 32: cerr << "[Space]"; break;
        default: break;
    }

    if ( key >= 48 && key <= 57 )
    {
        replayJump = 10 * replayJump + ( key - 48 );
        return;
    }

    cerr << endl;

    switch (key)
    {
    case 27:
    case 'Q': case 'q':
        cerr << "Exit" << endl;
        exit(0);
        break;

    case ' ':
        replayPlay = !replayPlay;
        if ( replayTime >= script->length )
            replayTime = 0.;
        cerr << ( replayPlay ? "Play" : "Stop" ) << endl;
        break;

    case 'L': case 'l':
        replayLoop = !replayLoop;
        cerr << ( replayLoop ? "Loop" : "Unloop" ) << endl;
        break;

    case 'J': case 'j':
        if ( replayJump > script->length )
            replayJump = script->length;
        replayTime = double(replayJump);
        replayPlay = false;
        cerr << "Jump to Frame " << replayJump << endl;
        break;

    case 'F': case 'f':
        replayJump = (int) ceil(replayTime + 0.01);
        if ( replayJump > script->length )
            replayJump = script->length;
        replayTime = double(replayJump);
        replayPlay = false;
        cerr << "Forward to Frame " << replayJump << endl;
        break;

    case 'R': case 'r':
        if ( replayTime < 1. )
            replayJump = 0;
        else
            replayJump = (int) floor(replayTime - 0.01);

        replayTime = double(replayJump);
        replayPlay = false;
        cerr << "Reverse to Frame " << replayJump << endl;
        break;

    case 'V': case 'v':
        cameraRadius = 10.;
        cameraRadiusMomentum = 0.;
        cameraAngle = 0.;
        cameraAngleMomentum = 0.;
        break;

    default:
        break;
    }

    replayJump = 0;
    glutPostRedisplay();
}

void Viewer::special(int key, int x, int y)
{
    switch (key)
    {
    case GLUT_KEY_UP:
        cameraRadiusMomentum -= 1.0;
        if ( cameraRadius + cameraRadiusMomentum < 7.0 )
            cameraRadiusMomentum = 7.0 - cameraRadius;
        cerr << "[Up]" << endl << "Zoom in" << endl;
        break;
    case GLUT_KEY_DOWN:
        cameraRadiusMomentum += 1.0;
        cerr << "[Down]" << endl << "Zoom out" << endl;
        break;
    case GLUT_KEY_LEFT:
        cameraAngleMomentum += 30.;
        cerr << "[Left]" << endl << "Left rotate" << endl;
        break;
    case GLUT_KEY_RIGHT:
        cameraAngleMomentum -= 30.;
        cerr << "[Right]" << endl << "Right rotate" << endl;
        break;
    default:
        break;
    }
}

void Viewer::initGLUT() 
{
    // Get a double-buffered, depth-buffer-enabled window, with an
    // alpha channel.
    // These options aren't really necessary but are here for examples.
    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);

    glutInitWindowSize(window_w, window_h);
    glutInitWindowPosition(300, 100);

    glutCreateWindow("CS171 HW7");
    
    // set up GLUT callbacks.
    glutDisplayFunc     (Viewer::redraw);
    glutReshapeFunc     (Viewer::resize);
    glutKeyboardFunc    (Viewer::keyfunc);
    glutSpecialFunc     (Viewer::special);
    // glutMouseFunc       (Viewer::mouse);
    // glutMotionFunc      (Viewer::motion);
    glutIdleFunc        (Viewer::idle);
}

void Viewer::initGL() {
    // Tell openGL to use gauraud shading:
    glShadeModel(GL_SMOOTH);
    
    // Enable back-face culling:
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    // Enable depth-buffer test.
    glEnable(GL_DEPTH_TEST);

    quad = gluNewQuadric();
    gluQuadricOrientation(quad, GLU_OUTSIDE);
}

void Viewer::initCamera() {

    // Set up projection and modelview matrices ("camera" settings) 
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glFrustum(-25., 25., -25., 25., 7., 100.);
    // glFrustum(-2., 2., -2., 2., 3., 50.);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    // camera view angle
    // glRotatef( -90., 1., 0., 0. );
    // glTranslatef( 0.0, 0.0, -6. );

    glPushMatrix();
}

void Viewer::initLights() {
    GLfloat amb[] = { 1.0, 1.0, 1.0, 1.0 };
    GLfloat diff[]= { 1.0f, 1.0f, 1.0f, 1.0f };
    GLfloat spec[]= { 1.0f, 1.0f, 1.0f, 1.0f };
    GLfloat lightpos[]= { 2.0f, 2.0f, 5.0f, 1.0f };
    GLfloat shiny = 4.0f; 

    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, amb);
    glLightfv(GL_LIGHT0, GL_AMBIENT, amb);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, diff);
    glLightfv(GL_LIGHT0, GL_SPECULAR, spec);
    glLightfv(GL_LIGHT0, GL_POSITION, lightpos);
    glLightf(GL_LIGHT0, GL_SHININESS, shiny);
    glEnable(GL_LIGHT0);

    // Turn on lighting.  You can turn it off with a similar call to
    // glDisable().
    // glEnable(GL_LIGHTING);
}

void Viewer::idle()
{
    int t1 = glutGet(GLUT_ELAPSED_TIME);
    double dt = (t1-t0) / 1000.;

    if ( t0 < 0 )
    {
        t0 = t1;
        return;
    }

    t0 = t1;

    double dr = cameraRadiusMomentum * dt / 1.0;
    cameraRadiusMomentum -= dr;
    cameraRadius += dr;

    double dp = cameraAngleMomentum * dt / 1.0;
    cameraAngleMomentum -= dp;
    cameraAngle += dp;

    if ( replayPlay )
    {
        replayTime += dt * replaySpeed;

        if ( replayTime > double(script->length) )
        {
            if ( replayLoop )
                replayTime = fmod(replayTime, double(script->length));
            else
            {
                replayTime = double(script->length);
                replayPlay = false;
                cerr << "Stop" << endl;
            }
        }
    }

    glutPostRedisplay();
}

void Viewer::redraw() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glPushAttrib( GL_ALL_ATTRIB_BITS );

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();

    // camera
    glTranslatef( 0.0, 0.0, -cameraRadius );
    glRotatef( cameraAngle, 0., 1., 0. );

    FrameStruct f = script->eval(replayTime);
    Vector a = f.rotation.im();
    GLfloat th = 2.f * atan2( a.norm(), f.rotation.re() );

    // glTranslatef( 25., 25., 0. );
    glTranslatef( f.translation[0], f.translation[1], f.translation[2] );
    glRotatef( th * RADTODEG, a[0], a[1], a[2] );
    glScalef( f.scale[0], f.scale[1], f.scale[2] );

    #if 0
    // BEGIN : multiply matrix from the left
    GLfloat mat[16];
    glGetFloatv(GL_MODELVIEW_MATRIX, mat);

    glLoadIdentity();
            
    // user translation or scaling (z-translation) while dragging
    glTranslatef( userTrans[0], userTrans[1], userTrans[2] );

    // user rotation while dragging
    Vector a = userRot.im();
    if ( a.norm2() >= 1E-8f )
    {
        // convert quaternion to axis and angle
        GLfloat th = 2.f * atan2(a.norm(), userRot.re());
        a.normalize();

        // rotate w.r.t. (0,0,3)
        glTranslatef( 0.f, 0.f, -3.f );
        glRotatef(th * ANGLE_CONV, a[0], a[1], a[2]);
        glTranslatef( 0.f, 0.f,  3.f );
    }

    glMultMatrixf(mat);
    // END : multiply matrix from the left
    #endif

    // draw the I-bar
    glRotatef( 90., 1., 0., 0. );
    glTranslatef( 0.f, 0.f, -1.f );

    glColor4f( 0., 0., 1., 1. );
    gluCylinder( quad, 0.25, 0.25, 2.0, 8, 2 );

    glRotatef( 90., 0., 1., 0. );

    glColor4f( 0., 1., 1., 1. );
    gluCylinder( quad, 0.25, 0.25, 1.0, 8, 2 );

    glRotatef( 180., 0., 1., 0. );

    glColor4f( 0., 1., 0., 1. );
    gluCylinder( quad, 0.25, 0.25, 1.0, 8, 2 );

    glTranslatef( 2.f, 0.f, 0.f );

    glColor4f( 1., 0., 1., 1. );
    gluCylinder( quad, 0.25, 0.25, 1.0, 8, 2 );

    glRotatef( 180., 0., 1., 0. );

    glColor4f( 1., 1., 0., 1. );
    gluCylinder( quad, 0.25, 0.25, 1.0, 8, 2 );

    glPopMatrix();

    glPopAttrib();

    glutSwapBuffers();
}

void Viewer::resize(GLint w, GLint h) {
    if (h == 0)
        h = 1;

    int x0, y0;

    // ensure that we are always square (even if whole window not used)
    if (w > h) {
        y0 = 0;
        x0 = (w-h)/2;
        w = h;
    } else {
        x0 = 0;
        y0 = (h-w)/2;
        h = w;
    }

    // Reset the current viewport and perspective transformation
    glViewport(x0, y0, w, h);

    // Tell GLUT to call redraw()
    glutPostRedisplay();
}

