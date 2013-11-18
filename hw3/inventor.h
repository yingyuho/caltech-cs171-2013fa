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
#include "mesh_processor.h"
#include "ptr_container.h"
#include "transform.h"

namespace InventorHelper {
    // debug message
    template<typename T> static void watch(T) {}
    // template<typename T> static void watch(const T t) { std::cout << t << " "; }
    // static void watch(const char *s) { std::cout << s; }
}

// A subclass of this takes any number of parameters of type T
// Rule: if input data is copied, use plain value type or const& for T
//       if input data is NOT copied, use non-const pointer for T
template< typename T > class ParamEater;

template< typename T > class MeshBuilder;
typedef MeshBuilder<Vec4> MBCoord;
typedef MeshBuilder<NVec3> MBNorml;

class Inventor; // PerspectiveCamera (PointLight)+ (Separator)+
class PerspectiveCamera;
class PointLight;
class Separator; // Material Coordinate3 Normal3 (IndexedFaceSet)+
class Material;
class Coordinate3;
class Normal3;
class IndexedFaceSet;

template< typename T >
class ParamEater {
public:
    virtual ~ParamEater() {}
    virtual void feed_param(T) = 0;
};

template< typename T >
class MeshBuilder : public MeshProcessor< T, const std::vector<T>& > {
public:
    typedef std::vector<T> PointMap;
};

class Inventor \
: public MeshProcessor<Vec4,void>, public MeshProcessor<NVec3,void>\
, public ParamEater<PointLight*>, public ParamEater<Separator*> {

private:
    PerspectiveCamera* pCamera;
    PtrList<PointLight> plList;
    PtrList<Separator> sepList;
public:
    Inventor(PerspectiveCamera* pCamera) : pCamera(pCamera) {}
    virtual ~Inventor();
    virtual void feed_param(PointLight*);
    virtual void feed_param(Separator*);
    virtual void process_mesh(Mesh<Vec4>&) const;
    virtual void process_mesh(Mesh<NVec3>&) const;

    const PtrList<PointLight>& get_light_list() const;
    const PtrList<Separator>& get_separator_list() const;
    const PerspectiveCamera& get_camera() const { return *pCamera; }

    // returns true if coordIndex and normalIndex are consistent for each separator
    bool validate_index() const;
    const std::string validate_index_msg() const;
};

class PerspectiveCamera : public CoordTransformer {
private:
    const Vec3 pos, axis; const double rot_angle;

    //              nearDist    farDist left    right   top     bottom
    const double    n,          f,      l,      r,      t,      b;

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
    PointLight(const PointLight& pl) : location(pl.location), color(pl.color) {}
};

class Separator : public MBCoord, public MBNorml, public CoordTransformer {
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

    virtual Mat44 to_left_matrix() const;
    virtual void process_mesh(Mesh<Vec4>& mesh, const std::vector<Vec4>& pMap) const;
    virtual void process_mesh(Mesh<NVec3>& mesh, const std::vector<NVec3>& pMap) const;

    const std::vector<Vec4>& get_obj_space_coord() const;
    const std::vector<Vec4> get_world_space_coord() const;
    const std::vector<Vec4> get_norm_device_coord(const PerspectiveCamera&) const;
    const std::vector<IntVec2> get_pixel_coord(const Mat44& pcMatrix, int xRes, int yRes) const;

    const std::vector<NVec3>& get_obj_space_normal() const;
    const std::vector<NVec3> get_world_space_normal() const;

    const std::vector<int>& get_coord_index() const;
    const std::vector<int>& get_normal_index() const;

    const Material& get_material() const;

    // returns true if get_coord_index() and get_normal_index() are consistent
    bool validate_index() const;
    const std::string validate_index_msg() const;
};

class Material {
public:
    const Vec3 aColor;
    const Vec3 dColor;
    const Vec3 sColor;
    const double shininess;

    Material(const Vec3& aColor, const Vec3& dColor, const Vec3& sColor, double shininess);

    Material(const Material&);
};

class Coordinate3 : public ParamEater<const Vec3&> {
private:
    std::vector<Vec4> ptVec;
public:
    Coordinate3();
    virtual ~Coordinate3();
    virtual void feed_param(const Vec3&);
    const std::vector<Vec4>& get_obj_space_coord() const;
};

class Normal3 : public ParamEater<const Vec3&> {
private:
    std::vector<NVec3> normalVec;
public:
    Normal3();
    virtual ~Normal3();
    virtual void feed_param(const Vec3&);
    const std::vector<NVec3>& get_obj_space_normal() const;
};

class IndexedFaceSet : public MBCoord, public MBNorml {
private:
    std::vector<int> * coordIndex;
    std::vector<int> * normalIndex;
public:
    IndexedFaceSet(std::vector<int> * coordIndex, std::vector<int> * normalIndex);

    virtual ~IndexedFaceSet();
    virtual void process_mesh(Mesh<Vec4>& mesh, const std::vector<Vec4>& pMap) const;
    virtual void process_mesh(Mesh<NVec3>& mesh, const std::vector<NVec3>& pMap) const;

    const std::vector<int>& get_coord_index() const;
    const std::vector<int>& get_normal_index() const;
};


#endif //   _inventor_h