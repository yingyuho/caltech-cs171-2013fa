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

#include <png.h>

#include "inventor.h"
#include "Quaternion.h"

class GLMesh {
public:
    int faceSize;
    std::vector<GLfloat> facePositions;
    std::vector<GLfloat> faceNormals;
    std::vector<GLfloat> faceTexturePositions;

    int edgeSize;
    std::vector<GLfloat> edgePositions;

    GLuint textureName;
    int textureW;
    int textureH;
    png_bytep texturePtr;
};

class Viewer {
private:
    Viewer(const Viewer&);
    Viewer& operator= (const Viewer&);

public:
    Viewer();
    static Inventor * inventor;
    static std::vector<GLMesh> glMesh;
    static std::string textureDir;

    static GLMesh waterGLMesh;

    static void read(Inventor*);
    static void init(int& argc, char* argv[]);

protected:
    static const double ANGLE_CONV;

    static int eyelight_flag;
    static double eyelight_str;

    static int window_w;
    static int window_h;

    static int mouseX;
    static int mouseY;

    static int mouseState;

    static double userTrans[3];
    static DDG::Quaternion userRot;

    static int t0;

    enum {
        menuWireframe,
        menuAnimatedWater,
        menuFlat,
        menuGouraud,
        menuResetView,

        mouseNone,
        mouseTranslate,
        mouseRotate,
        mouseScale
    };

    static bool renderWireframe;
    static bool renderAnimatedWater;

    static void mWireframe();
    static void mFlat();
    static void mGouraud();
    static void mAnimatedWater();
    static void mResetView();

    static int parseOptions(int& argc, char* argv[]);

    static int initGLUT(int& argc, char* argv[]);
    static void initGL();
    static void initCamera();
    static void initLighting();
    static int initTexture();
    static int initWater(std::string path);

    static void setMaterial(const Material&);

    static void drawWireframe();
    static void drawTriangles();

    static void redraw();
    static void resize(GLint w, GLint h);
    static void menu( int value );
    static void keyfunc(GLubyte key, GLint x, GLint y);
    static void mouse(int button, int state, int x, int y);
    static void motion(int x, int y);
    static void idle();
};

#endif