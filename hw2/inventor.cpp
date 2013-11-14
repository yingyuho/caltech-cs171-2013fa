#include "inventor.h"

// Inventor

Inventor::~Inventor() {
    if (pCamera) delete pCamera;
    sepList.remove_if(InventorHelper::deleteAllPtr<Separator>);
}

void Inventor::build_polygon_list(PolygonList& pList, const IntVec2& res) const {
    InventorHelper::watch("Inventor::build_polygon_list()\n");
    // for each separator
    for ( std::list<Separator*>::const_iterator it = sepList.begin();\
        it != sepList.end(); it++ ) {
        // get integer coordinates from the separator
        std::vector<IntVec2> pixel_coord = \
            (*it)->get_pixel_coord(pCamera->to_matrix(), res[0], res[1]);
        // and use them to retrieve polygons from the separator
        (*it)->build_polygon_list(pList, pixel_coord);
    }
}

// PerspectiveCamera

Mat44 PerspectiveCamera::to_matrix() const {
    // perspective projection P
    double proj[16] =   { 2*n/(r-l), 0, (r+l)/(r-l), 0 \
                        , 0, 2*n/(t-b), (t+b)/(t-b), 0 \
                        , 0, 0, -(f+n)/(f-n), -2*f*n/(f-n) \
                        , 0, 0, -1, 0 };

    // compute P * C^(-1), with camera transform C = T * R
    return Mat44(proj) * Rotation(axis, -rot_angle).to_matrix() \
                                * Translation(-pos).to_matrix();
}

// Separator

Separator::~Separator() {
    if (tPtr) delete tPtr;
    if (coord3Ptr) delete coord3Ptr;
    if (ifsPtr) delete ifsPtr;
}

void Separator::build_polygon_list(PolygonList& pList, const PointMap& pMap) const {
    InventorHelper::watch("Separator::build_polygon_list()\n");
    // retrieve polygons from each indexedFaceSet
    ifsPtr->build_polygon_list(pList, pMap);
}

const std::vector<Vec4>& Separator::get_obj_space_coord() const {
    return coord3Ptr->get_obj_space_coord();
}

const std::vector<Vec4> Separator::get_norm_device_coord(const Mat44& pcMatrix) const {
    const std::vector<Vec4>& osc = get_obj_space_coord();
    std::vector<Vec4> ndc(osc.size());

    // v_ndc = P * C^(-1) * O * v_osc
    // P * C^(-1) = pcMatrix
    // O = object transforms = tPtr->to_matrix()
    Mat44 mat = pcMatrix * tPtr->to_matrix();

    // std::vector<Vec4> osc => std::vector<Vec4> ndc
    for ( int i = 0; i < osc.size(); i++ )
        ndc[i] = mat * osc[i];

    return ndc;
}

const std::vector<IntVec2> Separator::get_pixel_coord(const Mat44& pcMatrix\
    , int xRes, int yRes) const {
    // v_ndc = P * C^(-1) * O * v_osc
    // P * C^(-1) = pcMatrix
    // O = object transforms = tPtr->to_matrix()
    Mat44 mat = pcMatrix * tPtr->to_matrix();

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

// IndexedFaceSet

IndexedFaceSet::~IndexedFaceSet() {
    ifsLineList.remove_if(InventorHelper::deleteAllPtr<IndexedFaceLine>);
}

void IndexedFaceSet::build_polygon_list(PolygonList& pList, const PointMap& pMap) const {
    InventorHelper::watch("IndexedFaceSet::build_polygon_list()\n");
    for ( CIter it = ifsLineList.begin(); it != ifsLineList.end(); it++ ) {
        // retrieve polygons from each indexedFaceLine
        (*it)->build_polygon_list(pList, pMap);
    }
}

// IndexedFaceLine

void IndexedFaceLine::build_polygon_list(PolygonList& pList, const PointMap& pMap) const {
    InventorHelper::watch("IndexedFaceLine::build_polygon_list()\n");

    std::list<VertexType>* polyLPtr = new std::list<VertexType>();

    for ( std::vector<int>::const_iterator it = indexVec.begin(); it != indexVec.end(); it++ ) {
        InventorHelper::watch(*it);
        if ( *it == -1 ) {
            // push polygon
            pList.push_back(polyLPtr);
            // create an empty polygon
            polyLPtr = new std::list<VertexType>();
        } else {
            // add a vertex to polygon
            polyLPtr->push_back(pMap[*it]);
        }
    }
    InventorHelper::watch("\n");

    if ( polyLPtr->empty() )
        // delete empty polygon
        delete polyLPtr;
    else
        // push non-empty polygon
        pList.push_back(polyLPtr);
}