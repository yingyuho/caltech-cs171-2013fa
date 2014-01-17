#include "Viewer.h"

#include <iostream>
#include <algorithm>
#include <string>
#include <getopt.h>
#include <readpng.h>

using namespace std;
using namespace DDG;

png_bytepp readpng(const char *filename, int* width, int* height);

std::string Viewer::textureDir = "";

const double Viewer::ANGLE_CONV = 180. / 3.141592653589793238462;

int     Viewer::eyelight_flag   = 0;
double  Viewer::eyelight_str    = 0.2;

Inventor * Viewer::inventor = 0;
std::vector<GLMesh> Viewer::glMesh;

GLMesh Viewer::waterGLMesh;

int     Viewer::window_w = 1;
int     Viewer::window_h = 1;

bool    Viewer::renderWireframe = false;
bool    Viewer::renderAnimatedWater = true;

int     Viewer::mouseX = 0;
int     Viewer::mouseY = 0;

int     Viewer::mouseState = mouseNone;

double  Viewer::userTrans[3] = { 0., 0., 0. };
Quaternion Viewer :: userRot(1.);

int     Viewer::t0 = 0;

Viewer::Viewer() {}

void Viewer::read(Inventor* inv) {
    Viewer::inventor = inv;

    const int L = inventor->get_separators().size();

    Viewer::glMesh.resize(L);

    for ( int i = 0; i < L; i++ ) {
        const auto &sep = inventor->get_separators()[i];

        Mesh<Vec4> meshC;
        sep->process_mesh(meshC, sep->get_obj_space_coord());
        meshC.triangulate();

        Mesh<Vec2> meshT;
        sep->process_mesh(meshT, sep->get_texture_coord());
        meshT.triangulate();

        // Mesh<NVec3> meshN;
        // sep->process_mesh(meshN, sep->get_obj_space_normal());
        // meshN.triangulate();

        std::vector<GLfloat>& p = Viewer::glMesh[i].facePositions;
        std::vector<GLfloat>& t = Viewer::glMesh[i].faceTexturePositions;
        // std::vector<GLfloat>& n = Viewer::glMesh[i].faceNormals;
        std::vector<GLfloat>& e = Viewer::glMesh[i].edgePositions;

        const int F = meshC.size();

        Viewer::glMesh[i].faceSize = F;
        Viewer::glMesh[i].edgeSize = 3*F;

        p.resize(3*3*F);
        t.resize(2*3*F);
        e.resize(3*6*F);

        for ( int i = 0; i < F; i++ ) {
            for ( int j = 0; j < 3; j++ ) {
                const int offset = 3 * i + j;

                Vec3 position3(&meshC[i]->at(j)[0]);
                position3 /= meshC[i]->at(j)[3];
                std::copy_n( &position3[0],       3, 3 * offset + (GLfloat*) &p[0] );

                std::copy_n( &meshT[i]->at(j)[0], 2, 2 * offset + (GLfloat*) &t[0]   );
            }

            std::copy_n( (GLfloat*) &p[0+9*i], 3, (GLfloat*) &e[15+18*i] );
            std::copy_n( (GLfloat*) &p[0+9*i], 3, (GLfloat*) &e[ 0+18*i] );
            std::copy_n( (GLfloat*) &p[3+9*i], 3, (GLfloat*) &e[ 3+18*i] );
            std::copy_n( (GLfloat*) &p[3+9*i], 3, (GLfloat*) &e[ 6+18*i] );
            std::copy_n( (GLfloat*) &p[6+9*i], 3, (GLfloat*) &e[ 9+18*i] );
            std::copy_n( (GLfloat*) &p[6+9*i], 3, (GLfloat*) &e[12+18*i] );
        }



    }
}

void Viewer::init(int& argc, char* argv[]) {

    if ( initGLUT(argc, argv) ) return;

    initGL();
    initCamera();
    initLighting();
    if ( initTexture() ) cerr << "error in binding textures" << endl;
    if ( initWater("./data/sky.png") ) cerr << "error in making water" << endl;

    // From here on, GLUT has control,
    glutMainLoop();
}

void Viewer::menu( int value )
{
  switch( value )
  {
     case( menuWireframe ):
        mWireframe();
        break;
     case( menuAnimatedWater ):
        mAnimatedWater();
        break;
     case( menuResetView ):
        mResetView();
        break;
     default:
        break;
  }
}

void Viewer::keyfunc(GLubyte key, GLint x, GLint y)
{
    switch (key)
    {
    case 27:
    case 'Q': case 'q':
        exit(0);
        break;
    case 'W': case 'w':
        mWireframe();
        break;
    case 'A': case 'a':
        mAnimatedWater();
        break;
    case 'R': case 'r':
        mResetView();
        break;
    default:
        break;
    }
}

void Viewer::mWireframe() 
{
    renderWireframe = !renderWireframe;
    glutPostRedisplay();
}

void Viewer::mAnimatedWater() 
{
    renderAnimatedWater = !renderAnimatedWater;
    glutPostRedisplay();
}

void Viewer::mResetView() 
{
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
    glPushMatrix();
    glutPostRedisplay();
}

int Viewer::parseOptions(int& argc, char* argv[]) {
    int c;
    const char* help = \
    " xRes yRes [-eyelight[=strength]] < input.iv";

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

    glutCreateWindow("CS171 HW6");
    
    // set up GLUT callbacks.
    glutDisplayFunc     (Viewer::redraw);
    glutReshapeFunc     (Viewer::resize);
    glutKeyboardFunc    (Viewer::keyfunc);
    glutMouseFunc       (Viewer::mouse);
    glutMotionFunc      (Viewer::motion);
    glutIdleFunc        (Viewer::idle);

    int mainMenu = glutCreateMenu( Viewer::menu );
    glutSetMenu( mainMenu );
    glutAddMenuEntry( "[w] Wireframe",          menuWireframe       );
    glutAddMenuEntry( "[a] Animated Water",     menuAnimatedWater   );
    glutAddMenuEntry( "[r] Reset View",         menuResetView       );
    glutAttachMenu( GLUT_RIGHT_BUTTON );

    return 0;
}

void Viewer::initGL() {
    // Tell openGL to use gauraud shading:
    glShadeModel(GL_SMOOTH);
    
    // Enable back-face culling:
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    // Enable depth-buffer test.
    glEnable(GL_DEPTH_TEST);
}

void Viewer::initCamera() {
    const PerspectiveCamera& pc = inventor->get_camera();

    // GLfloat mat[16];

    // Set up projection and modelview matrices ("camera" settings) 
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glFrustum(pc.l, pc.r, pc.b, pc.t, pc.n, pc.f);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    // camera view angle
    glRotatef(-pc.rot_angle * ANGLE_CONV, pc.axis[0], pc.axis[1], pc.axis[2]);
    glTranslatef(-pc.pos[0], -pc.pos[1], -pc.pos[2]);

    glPushMatrix();
}

void Viewer::initLighting() {
    const GLfloat amb[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
    GLfloat array4[4] = { 0.0f, 0.0f, 0.0f, 1.0f };

    int offset = 0;

    if ( eyelight_flag ) {
        glLightModelfv(GL_LIGHT_MODEL_AMBIENT, amb);
        glLightfv(GL_LIGHT0 + offset, GL_AMBIENT,   amb);

        array4[0] = array4[1] = array4[2] = eyelight_str;
        glLightfv(GL_LIGHT0 + offset, GL_DIFFUSE,   array4);
        glLightfv(GL_LIGHT0 + offset, GL_SPECULAR,  array4);

        std::copy_n(&inventor->get_camera().pos[0], 3, array4);
        glLightfv(GL_LIGHT0 + offset, GL_POSITION,  array4);

        glEnable(GL_LIGHT0 + offset);

        offset++;
    }

    for ( auto &light : inventor->get_lights() ) {
        if ( offset >= GL_MAX_LIGHTS ) {
            std::cerr << "Number of lights exceeds " << GL_MAX_LIGHTS \
                << ". Ignore the remaining." << std::endl;
            break;
        }
        glLightModelfv(GL_LIGHT_MODEL_AMBIENT, amb);
        glLightfv(GL_LIGHT0 + offset, GL_AMBIENT,   amb);

        std::copy_n(&light->color[0],       3, array4);
        glLightfv(GL_LIGHT0 + offset, GL_DIFFUSE,   array4);
        glLightfv(GL_LIGHT0 + offset, GL_SPECULAR,  array4);

        std::copy_n(&light->position[0],    3, array4);
        glLightfv(GL_LIGHT0 + offset, GL_POSITION,  array4);

        glEnable(GL_LIGHT0 + offset);

        offset++;
    }

    // Turn on lighting
    glEnable(GL_LIGHTING);
}

int Viewer::initTexture() {

    for ( int i = 0; i < inventor->get_separators().size(); i++ ) {
        const auto &sep = *inventor->get_separators()[i];

        glGenTextures(1, &glMesh[i].textureName);

        glBindTexture(GL_TEXTURE_2D, glMesh[i].textureName);

        if (sep.texturePath == "")
            continue;

        string texturePath = textureDir + sep.texturePath;

        int w, h;

        png_bytepp p = readpng(texturePath.c_str(), &w, &h);

        if (p == NULL)
            return 1;

        glMesh[i].texturePtr = new png_byte[3*w*h];

        size_t offset = 0;

        for (int y = h - 1; y >= 0; y--) {
            std::copy_n(p[y], 3 * w, offset + glMesh[i].texturePtr);
            offset += 3 * w;
        }

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, \
            w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, \
            glMesh[i].texturePtr);

        glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    }

    glEnable(GL_TEXTURE_2D);

    return 0;
}

int Viewer::initWater(string texturePath) {
    // first, mesh
    Mesh<Vec2> meshW;

    int nX = 100;
    int nY = 100;

    Face<Vec2> *f;
    Vec2 v(0.);
    for (int i = 0; i < nX; i++)
        for (int j = 0; j < nY; j++)
        {
            f = new Face<Vec2>();
            v[0] =  1. / nX * (i+1);
            v[1] =  1. / nY * j;
            f->push_back(v);
            v[0] =  1. / nX * (i+1);
            v[1] =  1. / nY * (j+1);
            f->push_back(v);
            v[0] =  1. / nX * i;
            v[1] =  1. / nY * (j+1);
            f->push_back(v);
            v[0] =  1. / nX * i;
            v[1] =  1. / nY * j;
            f->push_back(v);
            meshW.push_back(f);

            f = new Face<Vec2>();
            v[0] = -1. / nX * i;
            v[1] =  1. / nY * (j+1);
            f->push_back(v);
            v[0] = -1. / nX * (i+1);
            v[1] =  1. / nY * (j+1);
            f->push_back(v);
            v[0] = -1. / nX * (i+1);
            v[1] =  1. / nY * j;
            f->push_back(v);
            v[0] = -1. / nX * i;
            v[1] =  1. / nY * j;
            f->push_back(v);
            meshW.push_back(f);

            f = new Face<Vec2>();
            v[0] = -1. / nX * (i+1);
            v[1] = -1. / nY * j;
            f->push_back(v);
            v[0] = -1. / nX * (i+1);
            v[1] = -1. / nY * (j+1);
            f->push_back(v);
            v[0] = -1. / nX * i;
            v[1] = -1. / nY * (j+1);
            f->push_back(v);
            v[0] = -1. / nX * i;
            v[1] = -1. / nY * j;
            f->push_back(v);
            meshW.push_back(f);

            f = new Face<Vec2>();
            v[0] =  1. / nX * i;
            v[1] = -1. / nY * (j+1);
            f->push_back(v);
            v[0] =  1. / nX * (i+1);
            v[1] = -1. / nY * (j+1);
            f->push_back(v);
            v[0] =  1. / nX * (i+1);
            v[1] = -1. / nY * j;
            f->push_back(v);
            v[0] =  1. / nX * i;
            v[1] = -1. / nY * j;
            f->push_back(v);
            meshW.push_back(f);
        }

    meshW.triangulate();

    std::vector<GLfloat>& p = waterGLMesh.facePositions;
    std::vector<GLfloat>& n = waterGLMesh.faceNormals;
    std::vector<GLfloat>& e = waterGLMesh.edgePositions;

    const int F = meshW.size();

    waterGLMesh.faceSize = F;
    waterGLMesh.edgeSize = 3*F;

    p.resize(3*3*F);
    n.resize(3*3*F);
    e.resize(3*6*F);

    Vec3 pos;
    GLfloat zeros[] = {0.f, 0.f, 0.f};

    for ( int i = 0; i < F; i++ ) {
        for ( int j = 0; j < 3; j++ )
        {
            const int offset = 3 * i + j;

            pos[0] = meshW[i]->at(j)[0];
            pos[1] = meshW[i]->at(j)[1];
            pos[2] = 0.;

            std::copy_n( &pos[0],   3, 3 * offset + (GLfloat*) &p[0] );
            std::copy_n( &zeros[0], 3, 3 * offset + (GLfloat*) &n[0] );
        }
    }
    

    // second, texture
    glGenTextures(1, &waterGLMesh.textureName);

    glBindTexture(GL_TEXTURE_2D, waterGLMesh.textureName);

    int w, h;

    png_bytepp pb = readpng(texturePath.c_str(), &w, &h);

    if (pb == NULL)
        return 1;

    waterGLMesh.texturePtr = new png_byte[3*w*h];

    size_t offset = 0;

    for (int y = h - 1; y >= 0; y--) {
        std::copy_n(pb[y], 3 * w, offset + waterGLMesh.texturePtr);
        offset += 3 * w;
    }

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, \
        w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, \
        waterGLMesh.texturePtr);

    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    return 0;
}

void Viewer::idle()
{
    const double freq = 3;

    int t1 = glutGet(GLUT_ELAPSED_TIME);
    
    if ( t0 < 0 )
    {
        t0 = t1;
        return;
    }

    double dt = (t1-t0) / 1000.;
    t0 = t1;

    static double phi = 0.;

    phi = fmod(phi + freq * dt, 6.283185307179586);

    if (!renderAnimatedWater) return;

    Matrix<GLfloat,1,3> norm;
    double dist;
    double k = 20.;
    double z0 = 0.05;

    GLfloat *p, *n, *e;

    for ( int i = 0; i < waterGLMesh.faceSize; i++ ) {
        for ( int j = 0; j < 3; j++ )
        {
            p = &waterGLMesh.facePositions[9*i+3*j];
            n = &waterGLMesh.faceNormals[9*i+3*j];

            dist = sqrt(p[0] * p[0] + p[1] * p[1]);
            p[2] = z0 * cos(k * dist - phi);

            norm[0] = -k * z0 * p[0] / dist * sin(k * dist - phi);
            norm[1] = -k * z0 * p[1] / dist * sin(k * dist - phi);
            norm[2] = 1.;
            norm = norm.normalize();

            std::copy_n( &norm[0],  3, n );
        }

        p = &waterGLMesh.facePositions[9*i];
        e = &waterGLMesh.edgePositions[18*i];

        std::copy_n( p + 0, 3, e + 15 );
        std::copy_n( p + 0, 3, e +  0 );
        std::copy_n( p + 3, 3, e +  3 );
        std::copy_n( p + 3, 3, e +  6 );
        std::copy_n( p + 6, 3, e +  9 );
        std::copy_n( p + 6, 3, e + 12 );
    }

    glutPostRedisplay();
}

void Viewer::setMaterial(const Material& material) {
    GLfloat emit[] = {0.0, 0.0, 0.0, 1.0};
    glMaterialfv(GL_FRONT, GL_EMISSION, emit);

    GLfloat array4[4]   = { 0.0f, 0.0f, 0.0f, 1.0f };

    std::copy_n(&material.aColor[0], 3, array4);
    glMaterialfv(GL_FRONT, GL_AMBIENT,  array4);

    std::copy_n(&material.dColor[0], 3, array4);
    glMaterialfv(GL_FRONT, GL_DIFFUSE,  array4);

    std::copy_n(&material.sColor[0], 3, array4);
    glMaterialfv(GL_FRONT, GL_SPECULAR, array4);

    glMaterialf(GL_FRONT, GL_SHININESS, material.shininess);
}

void Viewer::redraw() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glPushAttrib( GL_ALL_ATTRIB_BITS );

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();

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

    if (renderWireframe) {
        glDisable( GL_LIGHTING );
        glDisable( GL_TEXTURE_2D );
        glColor4f( 1., 1., 1., 1. );
    }

    // draw water

    if (renderAnimatedWater)
    {
        glPushMatrix();
        glTranslatef( 0.0, -1.5, 0.0 );
        glRotatef( -90., 1., 0., 0. );
        glScalef( 2.0, 2.0, 1.0 );

        if (renderWireframe)
        {
            glEnableClientState(GL_VERTEX_ARRAY);
            glVertexPointer(  3, GL_FLOAT, 0, &waterGLMesh.edgePositions[0]  );
            glDrawArrays(GL_LINES, 0, 2 * waterGLMesh.edgeSize);
            glDisableClientState(GL_VERTEX_ARRAY);
        }
        else
        {
            // Texture use for automatic coordinate generation
            glBindTexture(GL_TEXTURE_2D, waterGLMesh.textureName);

            // Sphere mapping and enable s & t texture generation
            glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
            glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
            glEnable(GL_TEXTURE_GEN_S);
            glEnable(GL_TEXTURE_GEN_T);

            glEnableClientState(GL_VERTEX_ARRAY);
            glEnableClientState(GL_NORMAL_ARRAY);
            glVertexPointer(    3,  GL_FLOAT, 0, &waterGLMesh.facePositions[0]    );
            glNormalPointer(        GL_FLOAT, 0, &waterGLMesh.faceNormals[0]      );
            glDrawArrays(GL_TRIANGLES, 0, 3 * waterGLMesh.faceSize);
            glDisableClientState(GL_NORMAL_ARRAY);
            glDisableClientState(GL_VERTEX_ARRAY);

            glDisable(GL_TEXTURE_GEN_S);
            glDisable(GL_TEXTURE_GEN_T);
        }

        glPopMatrix();
    }

    // draw model
    const int L = inventor->get_separators().size();

    for ( int i = 0; i < L; i++ ) {
        const auto &sep = inventor->get_separators()[i];
        const auto &glm = Viewer::glMesh[i];

        if ( !renderWireframe ) {
            glBindTexture(GL_TEXTURE_2D, glMesh[i].textureName);
        }

        glPushMatrix();

        for ( auto &tr : sep->transforms ) {
            glTranslatef( tr->t.x, tr->t.y, tr->t.z );
            glRotatef( tr->r.th * ANGLE_CONV, tr->r.axis[0], tr->r.axis[1], tr->r.axis[2] );
            glScalef( tr->s.x, tr->s.y, tr->s.z );
        }

        glEnableClientState(GL_VERTEX_ARRAY);

        if ( renderWireframe ) {
            glVertexPointer(  3, GL_FLOAT, 0, &glm.edgePositions[0]  );
            glDrawArrays(GL_LINES, 0, 2 * glm.edgeSize);
        } else {
            glEnableClientState(GL_TEXTURE_COORD_ARRAY);
            glVertexPointer(    3, GL_FLOAT, 0, &glm.facePositions[0]           );
            glTexCoordPointer(  2, GL_FLOAT, 0, &glm.faceTexturePositions[0]    );
            glDrawArrays(GL_TRIANGLES, 0, 3 * glm.faceSize);
            glDisableClientState(GL_TEXTURE_COORD_ARRAY);
        }

        glDisableClientState(GL_VERTEX_ARRAY);

        glPopMatrix();
    }

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

void Viewer::mouse(int button, int state, int x, int y)
{
    if ( state == GLUT_DOWN )
    {
        glutGetModifiers();

        int m = (glutGetModifiers() & GLUT_ACTIVE_SHIFT) + \
                (glutGetModifiers() & GLUT_ACTIVE_CTRL) + \
                (glutGetModifiers() & GLUT_ACTIVE_ALT);

        switch (m)
        {
        case 0:
            Viewer::mouseState = mouseTranslate;
            userTrans[0] = userTrans[1] = 0.;
            break;
        case 1:
            Viewer::mouseState = mouseScale;
            userTrans[2] = 0.;
            break;
        case 2:
            Viewer::mouseState = mouseRotate;
            userRot = Quaternion(1.);
            break;
        default:
            Viewer::mouseState = mouseNone;
        }

        // cout << "Down " << x << " " << y << " " << m << endl;

        mouseX = x;
        mouseY = y;

    }
    else
    {
        glMatrixMode(GL_MODELVIEW);

        // BEGIN : multiply matrix from the left
        GLfloat mat[16];
        glGetFloatv(GL_MODELVIEW_MATRIX, mat);

        if ( Viewer::mouseState == mouseTranslate )
        {
            glLoadIdentity();
            glTranslatef( userTrans[0], userTrans[1], 0.f );
            glMultMatrixf(mat);
            userTrans[0] = userTrans[1] = 0.;
        }
        else if ( Viewer::mouseState == mouseScale )
        {
            glLoadIdentity();
            glTranslatef( 0.f, 0.f, userTrans[2] );
            glMultMatrixf(mat);
            userTrans[2] = 0.;
        }
        else if ( Viewer::mouseState == mouseRotate )
        {
            Vector a = userRot.im();
            if ( a.norm2() >= 1E-8f )
            {
                // convert quaternion to axis and angle
                GLfloat th = 2.f * atan2(a.norm(), userRot.re());
                a.normalize();

                // rotate w.r.t. (0,0,3)
                glLoadIdentity();
                glTranslatef( 0.f, 0.f, -3.f );
                glRotatef(th * ANGLE_CONV, a[0], a[1], a[2]);
                glTranslatef( 0.f, 0.f,  3.f );
                glMultMatrixf(mat);
            }
            userRot = Quaternion(1.);
        }
        // END : multiply matrix from the left

        Viewer::mouseState = mouseNone;
        glutPostRedisplay();
    }
}

void Viewer::motion(int x, int y)
{
    if ( Viewer::mouseState == Viewer::mouseNone ) { return; }

    GLint viewport[4];
    glGetIntegerv( GL_VIEWPORT, viewport );
    int w = viewport[2];
    int h = viewport[3];

    if ( Viewer::mouseState == mouseTranslate )
    {
        userTrans[0] += 3. * (x - mouseX) / w;
        userTrans[1] -= 3. * (y - mouseY) / h;
        // userTrans[0] += 2./3. * (3. - userTrans[2]) * (x - mouseX) / w;
        // userTrans[1] -= 2./3. * (3. - userTrans[2]) * (y - mouseY) / h;
    }
    else if ( Viewer::mouseState == mouseScale )
    {
        userTrans[2] -= 2. * (y - mouseY) / h;
    }
    else if ( Viewer::mouseState == mouseRotate )
    {
        Quaternion p(   0., \
                        2. * (y - mouseY) / h , \
                        2. * (x - mouseX) / w , \
                        0. );

        if( p.norm2() > 1. )
            p.normalize();
        else
            p.re() = sqrt( 1. - p.norm2() );

        userRot = p * userRot;
    }

    mouseX = x;
    mouseY = y;

    glutPostRedisplay();
}

