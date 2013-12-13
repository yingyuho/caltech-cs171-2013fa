#include "Viewer.h"

#include <iostream>
#include <algorithm>
#include <string>
#include <cmath>
#include <getopt.h>

using namespace std;
using namespace DDG;

const double Viewer::ANGLE_CONV = 180. / 3.141592653589793238462;

Viewer::SP              Viewer::spline;
std::vector<double>     Viewer::splineKnots;
std::vector<GLfloat>    Viewer::splinePositions;
bool Viewer::reParameterize = true;

int     Viewer::window_w = 1;
int     Viewer::window_h = 1;

double Viewer::camera_l0 = -1.5;
double Viewer::camera_r0 =  1.5;
double Viewer::camera_b0 = -1.5;
double Viewer::camera_t0 =  1.5;
double Viewer::camera_l = Viewer::camera_l0;
double Viewer::camera_r = Viewer::camera_r0;
double Viewer::camera_b = Viewer::camera_b0;
double Viewer::camera_t = Viewer::camera_t0;
double Viewer::camera_n = -1.0;
double Viewer::camera_f = 10.0;

bool    Viewer::renderCtrlPoints = true;

int     Viewer::mouseX = 0;
int     Viewer::mouseY = 0;

int     Viewer::mouseState = mouseNone;

int     Viewer::mousePickedVertex = -1;

double  Viewer::userTrans[2] = { 0., 0. };
double  Viewer::userZoom = 1.;

Viewer::Viewer() {}

void Viewer::read()
{
    double knots[] = { 0., 0., 0., 0., 1., 1., 1., 1. };
    double ctrlPts[] = { -1.,  1., -1., -1.,  1., -1.,  1.,  1., 0., 0. };

    Viewer::spline = SP();

    spline.knots.resize(8);
    copy_n( knots, 8, spline.knots.begin() );

    spline.points.emplace_back( 0 + ctrlPts );
    spline.points.emplace_back( 2 + ctrlPts );
    spline.points.emplace_back( 4 + ctrlPts );
    spline.points.emplace_back( 6 + ctrlPts );
}

void Viewer::init(int& argc, char* argv[]) {

    if ( initGLUT(argc, argv) ) return;

    initGL();
    initCamera();
    initLighting();

    // parameterize();
    /*cerr << splineKnots.size() << endl;
    for ( auto& u : splineKnots )
        cerr << u << " ";
    cerr << endl;*/

    // From here on, GLUT has control,
    glutMainLoop();
}

int Viewer::initGLUT(int& argc, char* argv[])
{
    // OpenGL will take out any arguments intended for its use here.
    // Useful ones are -display and -gldebug.
    glutInit(&argc, argv);

    if ( parseOptions(argc, argv) ) return 1;

    // Get a double-buffered, depth-buffer-enabled window, with an
    // alpha channel.
    // These options aren't really necessary but are here for examples.
    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);

    glutInitWindowSize(window_w, window_h);
    glutInitWindowPosition(300, 100);

    glutCreateWindow("CS171 HW5");

    // set up GLUT callbacks.
    glutDisplayFunc     (Viewer::redraw);
    glutReshapeFunc     (Viewer::resize);
    glutKeyboardFunc    (Viewer::keyfunc);
    glutMouseFunc       (Viewer::mouse);
    glutMotionFunc      (Viewer::motion);

    return 0;
}

void Viewer::keyfunc(GLubyte key, GLint x, GLint y)
{
    switch (key)
    {
    case 27:
    case 'Q': case 'q':
        exit(0);
        break;
    case 'C': case 'c':
        mCtrlPoints();
        break;
    case 'R': case 'r':
        mResetView();
        break;
    case 'I': case 'i':
        mZoomIn();
        break;
    case 'O': case 'o':
        mZoomOut();
        break;
    default:
        break;
    }
}

void Viewer::mCtrlPoints()
{
    renderCtrlPoints = ! renderCtrlPoints;
    glutPostRedisplay();
}

void Viewer::mResetView()
{
    userTrans[0] = userTrans[1] = 0.;
    userZoom = 1.;

    camera_l = camera_l0;
    camera_r = camera_r0;
    camera_b = camera_b0;
    camera_t = camera_t0;

    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glPushMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
    glPushMatrix();

    glutPostRedisplay();
}

void Viewer::mZoomIn() {
    userZoom *= sqrt(2.);

    camera_l = camera_l0 / userZoom;
    camera_r = camera_r0 / userZoom;
    camera_b = camera_b0 / userZoom;
    camera_t = camera_t0 / userZoom;

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(camera_l, camera_r, camera_b, camera_t, camera_n, camera_f);

    glutPostRedisplay();
}

void Viewer::mZoomOut() {
    userZoom /= sqrt(2.);

    camera_l = camera_l0 / userZoom;
    camera_r = camera_r0 / userZoom;
    camera_b = camera_b0 / userZoom;
    camera_t = camera_t0 / userZoom;

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(camera_l, camera_r, camera_b, camera_t, camera_n, camera_f);

    glutPostRedisplay();
}

int Viewer::parseOptions(int& argc, char* argv[]) {
    int c;
    const char* help = \
    " xRes yRes";

    // parse the options
    while (1) {
        static struct option long_options[] = { \
            {0,0,0,0} };

        /* getopt_long stores the option index here. */
        int option_index = 0;

        c = getopt_long_only(argc, argv, "", long_options, &option_index);

        /* Detect the end of the options. */
        if (c == -1) break;

        switch (c) {
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
    if (argc - optind < 2) {
        cerr << "Usage: " << argv[0] << help << endl;
        return 1;
    }

    int res[2];


    try {
        // set res = { xRes, yRes }
        res[0] = stoi(argv[optind+0]);
        res[1] = stoi(argv[optind+1]);
    } catch (const exception& e) {
        cerr << e.what() << endl;
        ERROR_MSG()
        return 1;
    }

    if ( res[0] <= 0 || res[1] <= 0 ) {
        cerr << "xRes and yRes must be positive" << endl;
        ERROR_MSG()
        return 1;
    }

    window_w = res[0];
    window_h = res[1];

    return 0;
}

void Viewer::initGL() {
    // Tell openGL to use gauraud shading:
    glShadeModel(GL_SMOOTH);
}

void Viewer::initCamera() {
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(camera_l, camera_r, camera_b, camera_t, camera_n, camera_f);
    glPushMatrix();

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glPushMatrix();
}

void Viewer::initLighting() {}

void Viewer::setMaterial() {}

void Viewer::parameterize()
{
    const double t0 = 0.;
    const double t1 = 1.;
    const double delta2 = 4.;

    splineKnots.clear();
    splinePositions.clear();

    double t  = t0;
    double dt = ( t1 - t0 ) / 128.;

    PtType point;
    double x0, y0, x1, y1;

    while ( t < t1 )
    {
        splineKnots.push_back(t);
        point = spline(t);
        splinePositions.push_back( (GLfloat) point[0] );
        splinePositions.push_back( (GLfloat) point[1] );

        worldToScreen( point[0], point[1], x0, y0 );

        dt *= 2.;
        do
        {
            dt /= 2.;
            point = spline(t+dt);
            worldToScreen( point[0], point[1], x1, y1 );
        } while ( (x0-x1)*(x0-x1) + (y0-y1)*(y0-y1) >= delta2 );

        t += dt;
        dt *= 2.;
    }

    t = t1;
    splineKnots.push_back(t);
    point = spline(t);
    splinePositions.push_back( (GLfloat) point[0] );
    splinePositions.push_back( (GLfloat) point[1] );
}

void Viewer::redraw() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glPushAttrib( GL_ALL_ATTRIB_BITS );

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();

    glTranslatef( userTrans[0], userTrans[1], 0.f );

    glDisable( GL_LIGHTING );

    // glEnable(GL_COLOR_MATERIAL);
    glEnable( GL_POINT_SMOOTH );
    glEnable( GL_LINE_SMOOTH );
    glHint( GL_POINT_SMOOTH_HINT, GL_NICEST );
    glHint( GL_LINE_SMOOTH_HINT, GL_NICEST );
    glEnable( GL_BLEND );
    glBlendEquation( GL_FUNC_ADD );
    glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

    if ( renderCtrlPoints )
    {
        const auto& points = spline.points;

        GLfloat * positions = new GLfloat[2*points.size()];

        int offset = 0;
        for ( auto& p : points )
        {
            std::copy_n( &p[0], 2, offset + positions );
            offset += 2;
        }

        glColor4f( 0.5, 0.5, 0.5, 1.0 );
        glPointSize( 21 );
        glLineWidth( 2. );

        glEnableClientState(GL_VERTEX_ARRAY);
        glVertexPointer( 2, GL_FLOAT, 0, positions );
        glDrawArrays( GL_POINTS,     0, points.size() );
        glDrawArrays( GL_LINE_STRIP, 0, points.size() );
        glDisableClientState(GL_VERTEX_ARRAY);

        delete[] positions;
    }

    glTranslatef( 0.f, 0.f, 0.02f );

    if ( reParameterize )
    {
        reParameterize = false;
        parameterize();
    }

    glColor4f( 1.0, 1.0, 1.0, 1.0 );
    glLineWidth( 3. );

    glEnableClientState(GL_VERTEX_ARRAY);
    glVertexPointer( 2, GL_FLOAT, 0, &splinePositions[0] );
    glDrawArrays( GL_LINE_STRIP, 0, splinePositions.size()/2 );
    glDisableClientState(GL_VERTEX_ARRAY);

    // cerr << splinePositions.size()/2 << endl;

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
    reParameterize = true;
    glutPostRedisplay();
}

void Viewer::screenToWorld(int sx, int sy, double& wx, double& wy)
{
    GLint viewport[4];
    glGetIntegerv( GL_VIEWPORT, viewport );

    wx = double(sx - viewport[0]) / viewport[2] * (camera_r - camera_l) + camera_l;
    wy = double(sy - viewport[1]) / viewport[3] * (camera_b - camera_t) + camera_t;
}

void Viewer::worldToScreen(double wx, double wy, int& sx, int& sy)
{
    GLint viewport[4];
    glGetIntegerv( GL_VIEWPORT, viewport );

    sx = int((wx - camera_l) / (camera_r - camera_l) * viewport[2] + viewport[0]);
    sy = int((wy - camera_t) / (camera_b - camera_t) * viewport[3] + viewport[1]);
}

void Viewer::worldToScreen(double wx, double wy, double& sx, double& sy)
{
    GLint viewport[4];
    glGetIntegerv( GL_VIEWPORT, viewport );

    sx = (wx - camera_l) / (camera_r - camera_l) * viewport[2] + viewport[0];
    sy = (wy - camera_t) / (camera_b - camera_t) * viewport[3] + viewport[1];
}

void Viewer::worldToScreenFactor(double& x, double& y)
{
    GLint viewport[4];
    glGetIntegerv( GL_VIEWPORT, viewport );

    x = viewport[2] / (camera_r - camera_l);
    y = viewport[3] / (camera_b - camera_t);
}

void Viewer::mouse(int button, int state, int x, int y)
{
    int m = glutGetModifiers() & \
            (GLUT_ACTIVE_SHIFT + GLUT_ACTIVE_CTRL + GLUT_ACTIVE_ALT);

    if ( state == GLUT_DOWN )
    {
        mouseX = x;
        mouseY = y;


        if ( button == GLUT_LEFT_BUTTON && m == 0 && renderCtrlPoints )
        {
            const auto& vertices = spline.points;
            if ( vertices.size() == 0 ) { return; }

            int vI = -1;
            double dist2 = 200.;

            for ( int i = 0; i < vertices.size(); i++ )
            {
                double wx = vertices[i][0] + userTrans[0];
                double wy = vertices[i][1] + userTrans[1];
                double sx, sy;
                worldToScreen( wx, wy, sx, sy );
                double r2 = (x-sx)*(x-sx) + (y-sy)*(y-sy);
                if ( r2 < dist2 )
                {
                    vI = i;
                    dist2 = r2;
                }
            }

            Viewer::mouseState = ( vI == -1 ) ? mouseNone : mouseMove;
            mousePickedVertex = vI;
        }
        else if ( button == GLUT_LEFT_BUTTON && m == GLUT_ACTIVE_SHIFT )
        {
            Viewer::mouseState = mouseTranslate;
        }
    }
    else
    {
        if ( button == GLUT_RIGHT_BUTTON && m == 0 && renderCtrlPoints )
        {
            if ( splinePositions.size() == 0 ) { return; }

            int vI = -1;
            double dist2 = 16.;

            for ( int i = 0; i < splinePositions.size()/2; i++ )
            {
                double wx = splinePositions[2*i+0] + userTrans[0];
                double wy = splinePositions[2*i+1] + userTrans[1];
                double sx, sy;
                worldToScreen( wx, wy, sx, sy );
                double r2 = (x-sx)*(x-sx) + (y-sy)*(y-sy);
                if ( r2 < dist2 )
                {
                    vI = i;
                    dist2 = r2;
                }
            }

            if ( vI != -1 )
            {
                spline.insertKnot(splineKnots[vI]);
                reParameterize = true;
                glutPostRedisplay();
            }

            return;
        }

        Viewer::mouseState = mouseNone;

        glutPostRedisplay();
    }
}

void Viewer::motion(int x, int y)
{
    if ( Viewer::mouseState == Viewer::mouseNone ) { return; }

    double xf, yf;
    worldToScreenFactor(xf, yf);

    if ( Viewer::mouseState == mouseTranslate )
    {
        userTrans[0] += ( x - mouseX ) / xf;
        userTrans[1] += ( y - mouseY ) / yf;
    }
    else if ( Viewer::mouseState == mouseMove )
    {
        spline.p( mousePickedVertex )[0] += ( x - mouseX ) / xf;
        spline.p( mousePickedVertex )[1] += ( y - mouseY ) / yf;
        reParameterize = true;
    }

    mouseX = x;
    mouseY = y;

    glutPostRedisplay();
}

