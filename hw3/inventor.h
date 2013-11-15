//
//  inventor.h
//  
//  Data structure representing input file in Open Inventer format
//
//  Created by Ying-Yu Ho on 11/01/13.
//
//

#ifndef _inventor_h
#define _inventor_h

#include <list>
#include <vector>
#include "transform.h"

namespace InventorHelper {
    // helper function for deleting all pointers in a STL container
    template<typename T> static bool deleteAllPtr(T* elemPtr) { delete elemPtr; return true; }
    // debug message
    //template<typename T> static void watch(T) {}
    static void watch(const char *s) { std::cout << s; }
    static void watch(const int i) { std::cout << i << " "; }
}

// A subclass of this takes any number of parameters of type T
// Rule: if input data is copied, use plain value type or const& for T
//       if input data is NOT copied, use non-const pointer for T
template< typename T > class ParamEater;

// A subclass of this accept the reference of a list of polygons with coordinate type T
// and put more polygons into the list according to the data with type U.
// Usually the data is a std::vector<T> containing all vertex coordinates
template< typename T, typename U = std::vector<T> > class PolygonBuilder;

// A specialization of PolygonBuilder that only need the reference of a polygon list
template< typename T > class PolygonBuilder<T,void>;

class Inventor; // PerspectiveCamera (PointLight)+ (Separator)+
class PerspectiveCamera;
class PointLight;
class Separator; // Material Coordinate3 Normal3 (IndexedFaceSet)+
class Material;
class Coordinate3;
class Normal3;
class IndexedFaceSet;
//class IndexedFaceLine;

template< typename T >
class ParamEater {
public:
    virtual ~ParamEater() {}
    virtual void feed_param(T) = 0;
};

template< typename T, typename U >
class PolygonBuilder {
public:
    typedef T VertexType;
    typedef typename std::list<T> VertexList;
    typedef typename VertexList::iterator VertexIter;
    typedef typename VertexList::const_iterator VertexCIter;
    typedef typename std::list<VertexList*> PolygonList;
    typedef typename PolygonList::iterator PolygonIter;
    typedef typename PolygonList::const_iterator PolygonCIter;
    typedef U PointMap;

    virtual ~PolygonBuilder() {}
    virtual void build_polygon_list(PolygonList& pList, const U& pMap) const = 0;
};

template< typename T >
class PolygonBuilder<T,void> {
public:
    typedef T VertexType;
    typedef typename std::list<T> VertexList;
    typedef typename VertexList::iterator VertexIter;
    typedef typename VertexList::const_iterator VertexCIter;
    typedef typename std::list<VertexList*> PolygonList;
    typedef typename PolygonList::iterator PolygonIter;
    typedef typename PolygonList::const_iterator PolygonCIter;

    virtual ~PolygonBuilder() {}
    virtual void build_polygon_list(PolygonList& pList) const = 0;
};

class Inventor : public PolygonBuilder<Vec4,void>\
, public ParamEater<PointLight*>, public ParamEater<Separator*> {
private:
    PerspectiveCamera* pCamera;
    std::list<PointLight*> plList;
    std::list<Separator*> sepList;
public:
    Inventor(PerspectiveCamera* pCamera) : pCamera(pCamera) {}
    virtual ~Inventor();
    virtual void feed_param(PointLight* pl) { plList.push_back(pl); }
    virtual void feed_param(Separator* sep) { sepList.push_back(sep); }
    virtual void build_polygon_list(PolygonList& pList) const;
};

class PerspectiveCamera : public CoordTransformer {
private:
    Vec3 pos; Vec3 axis; double rot_angle;
    //      nearDistance    farDistance     left    right   top     bottom
    double  n,              f,              l,      r,      t,      b;
public:
    PerspectiveCamera(Vec3 pos, Vec4 orient \
        , double n, double f, double l, double r, double t, double b) \
        : pos(pos), axis(Vec3(&(orient[0])).normalize()), rot_angle(orient[3]) \
        , n(n), f(f), l(l), r(r), t(t), b(b) {}

    virtual Mat44 to_left_matrix() const;
};

class PointLight {
public:
    const Vec3 location;
    const Vec3 color;
    PointLight(const Vec3& location, const Vec3& color) : location(location), color(color) {}
};

class Separator : public PolygonBuilder<Vec4>, public CoordTransformer {
private:
    ComboTransform * tPtr;
    Material * mPtr;
    Coordinate3 * coord3Ptr;
    Normal3 * normalPtr;
    IndexedFaceSet * ifsPtr;
public:
    Separator(ComboTransform* tPtr, Material * mPtr\
    , Coordinate3* cPtr, Normal3* nPtr, IndexedFaceSet* iPtr) \
    : tPtr(tPtr), mPtr(mPtr), coord3Ptr(cPtr), normalPtr(nPtr), ifsPtr(iPtr) {}
    ~Separator();

    virtual Mat44 to_left_matrix() const { return tPtr->to_left_matrix(); }
    virtual void build_polygon_list(PolygonList& pList, const PointMap& pMap) const;

    const std::vector<Vec4>& get_obj_space_coord() const;
    const std::vector<Vec4> get_norm_device_coord(const Mat44& pcMatrix) const;
    const std::vector<IntVec2> get_pixel_coord(const Mat44& pcMatrix, int xRes, int yRes) const;
};

class Material {
public:
    const Vec3 aColor;
    const Vec3 dColor;
    const Vec3 sColor;
    const double shininess;
    Material(const Vec3& aColor, const Vec3& dColor, const Vec3& sColor, double shininess)\
     : aColor(aColor), dColor(dColor), sColor(sColor), shininess(shininess) {}
};

class Coordinate3 : public ParamEater<const Vec3&> {
private:
    std::vector<Vec4> ptVec;
public:
    Coordinate3() {}
    virtual ~Coordinate3() {}

    virtual void feed_param(const Vec3& v) {
        double v4[4] = {v[0], v[1], v[2], 1};
        ptVec.push_back(Vec4(v4));
    }

    const std::vector<Vec4>& get_obj_space_coord() const { return ptVec; }
};

class Normal3 : public ParamEater<const Vec3&> {
private:
    std::vector<NVec3> normalVec;
public:
    Normal3() {}
    virtual ~Normal3() {}

    virtual void feed_param(const Vec3& v) {
        normalVec.push_back(v.transpose());
    }

    const std::vector<NVec3>& get_obj_space_coord() const { return normalVec; }
};

class IndexedFaceSet : public PolygonBuilder<Vec4> {
private:
    std::vector<int> * coordIndex;
    std::vector<int> * normalIndex;
public:
    IndexedFaceSet(std::vector<int> * coordIndex, std::vector<int> * normalIndex)\
    : coordIndex(coordIndex), normalIndex(normalIndex) {}

    virtual ~IndexedFaceSet();
    virtual void build_polygon_list(PolygonList& pList, const PointMap& pMap) const;
};

#endif //   _inventor_h