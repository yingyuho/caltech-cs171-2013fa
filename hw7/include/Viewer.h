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

#include "KeyframeSeries.h"
#include "Quaternion.h"

class Viewer {
private:
    Viewer(const Viewer&);
    Viewer& operator= (const Viewer&);

public:
    static KeyframeSeries *script;

    Viewer();

    static void init(int& argc, char* argv[]);

protected:
    static const double RADTODEG;

    static std::vector<FrameStruct> keyframeSlopes;
    static double   replaySpeed;
    static double   replayTime;
    static bool     replayLoop;
    static bool     replayPlay;
    static unsigned int replayJump;

    static int window_w;
    static int window_h;

    static int mouseX;
    static int mouseY;

    static int mouseState;

    static double userTrans[3];
    static DDG::Quaternion userRot;

    static double cameraRadius;
    static double cameraRadiusMomentum;
    static double cameraAngle;
    static double cameraAngleMomentum;

    static GLUquadricObj* quad;

    static int t0;

    enum {
        menuWireframe,
        menuAnimatedWater,
        menuFlat,
        menuGouraud,
        menuResetView,

        mouseNone
    };

    static void mResetView();

    static int parseArgs(int& argc, char* argv[]);

    static void initGLUT();
    static void initGL();
    static void initCamera();
    static void initLights();


    static void redraw();
    static void resize(GLint w, GLint h);
    static void keyfunc(GLubyte key, GLint x, GLint y);
    static void special(int key, int x, int y);
    static void mouse(int button, int state, int x, int y);
    static void motion(int x, int y);
    static void idle();
};

#endif