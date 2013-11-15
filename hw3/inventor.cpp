#include "inventor.h"

// Inventor

Inventor::~Inventor() {
    if (pCamera) delete pCamera;
    sepList.remove_if(InventorHelper::deleteAllPtr<Separator>);
}

void Inventor::process_mesh(MBCoord::Mesh& mesh) const {
    InventorHelper::watch("Inventor::process_mesh()\n");
    // for each separator
    for ( std::list<Separator*>::const_iterator it = sepList.begin();\
        it != sepList.end(); it++ ) {
        // get world space coordinates from the separator
        std::vector<Vec4> wsc = \
            (*it)->get_world_space_coord();
        // and use them to retrieve polygons from the separator
        (*it)->process_mesh(mesh, wsc);
    }
}

void Inventor::process_mesh(MBNorml::Mesh& mesh) const {
    InventorHelper::watch("Inventor::process_mesh()\n");
    // for each separator
    for ( std::list<Separator*>::const_iterator it = sepList.begin();\
        it != sepList.end(); it++ ) {
        // get world space coordinates from the separator
        std::vector<NVec3> wsn = \
            (*it)->get_world_space_normal();
        // and use them to retrieve polygons from the separator
        (*it)->process_mesh(mesh, wsn);
    }
}


bool Inventor::validate_index() const {
    int v = 0;
    // for each separator
    for ( std::list<Separator*>::const_iterator it = sepList.begin()\
        ; it != sepList.end(); it++ ) { if ( ! (*it)->validate_index() ) return false; }
    return true;
}

const std::string Inventor::validate_index_msg() const {
    std::string s = "";
    int i = 0;
    char buffer [50];

    // for each separator
    for ( std::list<Separator*>::const_iterator it = sepList.begin()\
    ; it != sepList.end(); it++ ) {
        std::string ss = (*it)->validate_index_msg();
        if ( ss != "" ) {
            std::sprintf(buffer, "Separator[%d]:\n", i);
            s += buffer + ss;
            return s;
        }
    }

    return "";
}

// PerspectiveCamera

Mat44 PerspectiveCamera::to_left_matrix() const {
    // perspective projection P
    double proj[16] =   { 2*n/(r-l), 0, (r+l)/(r-l), 0 \
                        , 0, 2*n/(t-b), (t+b)/(t-b), 0 \
                        , 0, 0, -(f+n)/(f-n), -2*f*n/(f-n) \
                        , 0, 0, -1, 0 };

    // compute P * C^(-1), with camera transform C = T * R
    return Mat44(proj) * Rotation(axis, -rot_angle).to_left_matrix() \
                                * Translation(-pos).to_left_matrix();
}

// Separator

Separator::~Separator() {
    if (tPtr) delete tPtr;
    if (coord3Ptr) delete coord3Ptr;
    if (ifsPtr) delete ifsPtr;
}

#define MAKE_METHOD_SEP_BUILD_POLYGON_LIST(MB)                                  \
void Separator::process_mesh(MB::Mesh& mesh, const MB::PointMap& pMap) const {  \
    InventorHelper::watch("Separator::process_mesh()\n");                       \
    /* retrieve polygons from each indexedFaceSet */                            \
    ifsPtr->process_mesh(mesh, pMap);                                           \
}

MAKE_METHOD_SEP_BUILD_POLYGON_LIST(MBCoord);
MAKE_METHOD_SEP_BUILD_POLYGON_LIST(MBNorml);

const std::vector<Vec4>& Separator::get_obj_space_coord() const {
    return coord3Ptr->get_obj_space_coord();
}

const std::vector<Vec4> Separator::get_world_space_coord() const {
    const std::vector<Vec4>& osc = get_obj_space_coord();
    std::vector<Vec4> wsc(osc.size());

    // v_wsc = O * v_osc
    // O = object transforms = tPtr->to_left_matrix()
    Mat44 mat = tPtr->to_left_matrix();

    // std::vector<Vec4> osc => std::vector<Vec4> wsc
    for ( int i = 0; i < osc.size(); i++ )
        wsc[i] = mat * osc[i];

    return wsc;
}

const std::vector<Vec4> Separator::get_norm_device_coord(const Mat44& pcMatrix) const {
    const std::vector<Vec4>& osc = get_obj_space_coord();
    std::vector<Vec4> ndc(osc.size());

    // v_ndc = P * C^(-1) * O * v_osc
    // P * C^(-1) = pcMatrix
    // O = object transforms = tPtr->to_left_matrix()
    Mat44 mat = pcMatrix * tPtr->to_left_matrix();

    // std::vector<Vec4> osc => std::vector<Vec4> ndc
    for ( int i = 0; i < osc.size(); i++ )
        ndc[i] = mat * osc[i];

    return ndc;
}

const std::vector<IntVec2> Separator::get_pixel_coord(const Mat44& pcMatrix\
    , int xRes, int yRes) const {
    // v_ndc = P * C^(-1) * O * v_osc
    // P * C^(-1) = pcMatrix
    // O = object transforms = tPtr->to_left_matrix()
    Mat44 mat = pcMatrix * tPtr->to_left_matrix();

    // std::vector<Vec4> osc => std::vector<IntVec2> pc
    const std::vector<Vec4>& osc = get_obj_space_coord();
    std::vector<IntVec2> pc(osc.size());

    for ( int i = 0; i < osc.size(); i++ ) {
        Vec4 ndc = mat * osc[i];
        // range of canvas corresponds to -1 <= ndc < 1
        pc[i][0] = (int)floor(xRes*(1 + ndc[0]/ndc[3])/2);
        pc[i][1] = (int)floor(yRes*(1 + ndc[1]/ndc[3])/2);
    }

    return pc;
}

const std::vector<NVec3>& Separator::get_obj_space_normal() const {
    return normalPtr->get_obj_space_normal();
}

const std::vector<NVec3> Separator::get_world_space_normal() const {
    const std::vector<NVec3>& osn = get_obj_space_normal();
    std::vector<NVec3> wsn(osn.size());

    // v_wsc = v_osc * O^(-1)
    // O^(-1) = inverse object transforms = tPtr->to_right_matrix()
    Mat33 mat = tPtr->to_right_matrix();

    // std::vector<Vec4> osc => std::vector<Vec4> wsc
    for ( int i = 0; i < osn.size(); i++ )
        wsn[i] = osn[i] * mat;

    return wsn;
}

const std::vector<int>& Separator::get_coord_index() const {
    return ifsPtr->get_coord_index();
}

const std::vector<int>& Separator::get_normal_index() const {
    return ifsPtr->get_normal_index();
}

bool Separator::validate_index() const {
    return ( "" == validate_index_msg() );
}

const std::string Separator::validate_index_msg() const {
    const std::vector<int>& ci = get_coord_index();
    const std::vector<int>& ni = get_normal_index();

    char buffer [100];

    if ( ci.size() != ni.size() ) {
        std::sprintf(buffer\
            , "coordIndex.size() = %lu and normalIndex.size() = %lu are not equal."\
            , ci.size(), ni.size());
        return buffer;
    }

    for ( int i = 0; i < ci.size(); i++ ) {
        if ( ci[i] < -1 ) { 
            std::sprintf(buffer\
                , "coordIndex[%d] = %d < -1 is not allowed."\
                , i, ci[i]);
            return buffer;
        }
        if ( ni[i] < -1 ) { 
            std::sprintf(buffer\
                , "normalIndex[%d] = %d < -1 is not allowed."\
                , i, ni[i]);
            return buffer;
        }
        if ( ci[i] >= (int)get_obj_space_coord().size() ) { 
            std::sprintf(buffer\
                , "coordIndex[%d] = %d > Coordinate3.size() = %lu is not allowed."\
                , i, ci[i], get_obj_space_coord().size());
            return buffer;
        }
        if ( ni[i] >= (int)get_obj_space_normal().size() ) { 
            std::sprintf(buffer\
                , "normalIndex[%d] = %d > Normal.size() = %lu is not allowed."\
                , i, ni[i], get_obj_space_normal().size());
            return buffer;
        }
        if ( ci[i] == -1 && ni[i] > -1 ) {
            std::sprintf(buffer\
                , "coordIndex[%d] = %d while normalIndex[%d] = %d != -1 is not allowed."\
                , i, ci[i], i, ni[i]);
            return buffer;
        }
        if ( ni[i] == -1 && ci[i] > -1 ) {
            std::sprintf(buffer\
                , "normalIndex[%d] = %d while coordIndex[%d] = %d != -1 is not allowed."\
                , i, ni[i], i, ci[i]);
            return buffer;
        }
    }

    return "";
}

// IndexedFaceSet

IndexedFaceSet::~IndexedFaceSet() {
    InventorHelper::watch("IndexedFaceSet::~IndexedFaceSet()\n");
    if (coordIndex) delete coordIndex;
    if (normalIndex) delete normalIndex;
}

#define MAKE_METHOD_IFS_BUILD_POLYGON_LIST(MB, indVal)                                              \
void IndexedFaceSet::process_mesh(MB::Mesh& mesh, const MB::PointMap& pMap) const {                 \
    InventorHelper::watch("IndexedFaceSet::process_mesh()\n");                                      \
                                                                                                    \
    MB::Face * polyLPtr = new MB::Face();                                                           \
                                                                                                    \
    for ( std::vector<int>::const_iterator it = indVal->begin(); it != indVal->end(); it++ ) {      \
        InventorHelper::watch(*it);                                                                 \
        if ( *it == -1 ) {                                                                          \
            /* push polygon */                                                                      \
            mesh.push_back(polyLPtr);                                                               \
            /* create an empty polygon */                                                           \
            polyLPtr = new MB::Face();                                                              \
        } else {                                                                                    \
            /* add a vertex to polygon */                                                           \
            polyLPtr->push_back(pMap[*it]);                                                         \
        }                                                                                           \
    }                                                                                               \
                                                                                                    \
    if ( polyLPtr->empty() )                                                                        \
        /* delete empty polygon */                                                                  \
        delete polyLPtr;                                                                            \
    else                                                                                            \
        /* push non-empty polygon */                                                                \
        mesh.push_back(polyLPtr);                                                                   \
}

MAKE_METHOD_IFS_BUILD_POLYGON_LIST(MBCoord, coordIndex);
MAKE_METHOD_IFS_BUILD_POLYGON_LIST(MBNorml, normalIndex);
