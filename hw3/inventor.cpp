#include "inventor.h"

// Inventor

Inventor::~Inventor() {
    if (pCamera) delete pCamera;
    sepList.remove_if(InventorHelper::deleteAllPtr<Separator>);
}

void Inventor::build_polygon_list(PBCoord::PolygonList& pList) const {
    InventorHelper::watch("Inventor::build_polygon_list()\n");
    // for each separator
    for ( std::list<Separator*>::const_iterator it = sepList.begin();\
        it != sepList.end(); it++ ) {
        // get world space coordinates from the separator
        std::vector<Vec4> wsc = \
            (*it)->get_world_space_coord();
        // and use them to retrieve polygons from the separator
        (*it)->build_polygon_list(pList, wsc);
    }
}

void Inventor::build_polygon_list(PBNorml::PolygonList& pList) const {
    InventorHelper::watch("Inventor::build_polygon_list()\n");
    // for each separator
    for ( std::list<Separator*>::const_iterator it = sepList.begin();\
        it != sepList.end(); it++ ) {
        // get world space coordinates from the separator
        std::vector<NVec3> wsn = \
            (*it)->get_world_space_normal();
        // and use them to retrieve polygons from the separator
        (*it)->build_polygon_list(pList, wsn);
    }
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

#define MAKE_METHOD_SEP_BUILD_POLYGON_LIST(PB)                                                  \
void Separator::build_polygon_list(PB::PolygonList& pList, const PB::PointMap& pMap) const {    \
    InventorHelper::watch("Separator::build_polygon_list()\n");                                 \
    /* retrieve polygons from each indexedFaceSet */                                            \
    ifsPtr->build_polygon_list(pList, pMap);                                                    \
}

MAKE_METHOD_SEP_BUILD_POLYGON_LIST(PBCoord);
MAKE_METHOD_SEP_BUILD_POLYGON_LIST(PBNorml);

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

// IndexedFaceSet

IndexedFaceSet::~IndexedFaceSet() {
    InventorHelper::watch("IndexedFaceSet::~IndexedFaceSet()\n");
    if (coordIndex) delete coordIndex;
    if (normalIndex) delete normalIndex;
}

#define MAKE_METHOD_IFS_BUILD_POLYGON_LIST(PB, indVal)                                              \
void IndexedFaceSet::build_polygon_list(PB::PolygonList& pList, const PB::PointMap& pMap) const {   \
    InventorHelper::watch("IndexedFaceSet::build_polygon_list()\n");                                \
                                                                                                    \
    PB::VertexList * polyLPtr = new PB::VertexList();                                               \
                                                                                                    \
    for ( std::vector<int>::const_iterator it = indVal->begin(); it != indVal->end(); it++ ) {      \
        InventorHelper::watch(*it);                                                                 \
        if ( *it == -1 ) {                                                                          \
            /* push polygon */                                                                      \
            pList.push_back(polyLPtr);                                                              \
            /* create an empty polygon */                                                           \
            polyLPtr = new PB::VertexList();                                                        \
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
        pList.push_back(polyLPtr);                                                                  \
}

MAKE_METHOD_IFS_BUILD_POLYGON_LIST(PBCoord, coordIndex);
MAKE_METHOD_IFS_BUILD_POLYGON_LIST(PBNorml, normalIndex);
