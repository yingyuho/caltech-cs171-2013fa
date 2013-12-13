//
//  Viewer.h
//  
//  
//
//  Created by Ying-Yu Ho on 11/17/13.
//
//

#ifndef _Viewer_h
#define _Viewer_h

#ifdef MACOSX
#include "OpenGL/gl.h"
#include "OpenGL/glu.h"
#include "GLUT/glut.h"
#else
#include "GL/gl.h"
#include "GL/glu.h"
#include "GL/glut.h"
#endif

#include <vector>
#include "Quaternion.h"
#include "Spline.h"


class Viewer {
private:
    Viewer(const Viewer&);
    Viewer& operator= (const Viewer&);

public:
    Viewer();

    typedef Spline<4,2> SP;
    typedef SP::PtType PtType;

    static SP spline;

    static void read();
    static void init(int& argc, char* argv[]);

protected:
    static const double ANGLE_CONV;

    static int window_w;
    static int window_h;

    static double camera_l0;
    static double camera_r0;
    static double camera_b0;
    static double camera_t0;
    static double camera_l;
    static double camera_r;
    static double camera_b;
    static double camera_t;
    static double camera_n;
    static double camera_f;

    static int mouseX;
    static int mouseY;

    static int mouseState;

    static int mousePickedVertex;

    static double userTrans[2];
    static double userZoom;
    // static DDG::Quaternion userRot;

    enum {
        mouseNone,
        mouseMove,
        mouseTranslate,
        mouseRotate,
        mouseScale
    };

    static bool renderCtrlPoints;

    static std::vector<double> splineKnots;
    static std::vector<GLfloat> splinePositions;
    static bool reParameterize;

    static void mCtrlPoints();
    static void mResetView();
    static void mZoomIn();
    static void mZoomOut();

    static int parseOptions(int& argc, char* argv[]);

    static int initGLUT(int& argc, char* argv[]);
    static void initGL();
    static void initCamera();
    static void initLighting();

    static void setMaterial();

    static void parameterize();

    static void redraw();
    static void resize(GLint w, GLint h);
    static void menu( int value );
    static void keyfunc(GLubyte key, GLint x, GLint y);
    static void mouse(int button, int state, int x, int y);
    static void motion(int x, int y);

    static void screenToWorld(int sx, int sy, double& wx, double& wy);
    static void worldToScreen(double wx, double wy, int& sx, int& sy);
    static void worldToScreen(double wx, double wy, double& sx, double& sy);
    static void worldToScreenFactor(double& x, double& y);
};

#endif