//
//  parser_param.h
//  
//  This helps a parser collecting and checking required parameters for constructing objects.
//
//  All pointers taken by an instance of ParserParam's children classes as parameters MUST NOT
//  be used elsewhere and these pointers are deleted when the instance is destroyed.
//
//  Created by Ying-Yu Ho on 11/05/13.
//
//

#ifndef _parser_param_h
#define _parser_param_h

#include "inventor.h"

// T is the type of object(s) to make
template <typename T>
struct ParserParam {
public:
    virtual ~ParserParam() {}
    // true if some parameters are missing
    virtual bool incomplete_param() const = 0;
    // hint what are missing
    virtual const char* incomplete_param_msg() const = 0;
    // "new" an object from stored parameters and return its pointer
    virtual T* make_object() const = 0;
};

struct InventorParam;
struct PCameraParam;
struct PLightParam;
struct SeparatorParam;
struct TransformParam;
struct MaterialParam;

struct InventorParam : public ParserParam<Inventor> { 
public:
    PerspectiveCamera* pCamPtr;
    std::list<PointLight*> plList;
    std::list<Separator*> sepList;

    InventorParam() : pCamPtr(0) {}

    virtual bool incomplete_param() const { return !pCamPtr; }

    virtual const char* incomplete_param_msg() const {
        if (incomplete_param())
            return "No PerspectiveCamera";
        else
            return "";
    }

    virtual Inventor* make_object() const {
        if (!incomplete_param()) {
            Inventor *i = new Inventor(pCamPtr);
            for (std::list<PointLight*>::const_iterator it = plList.begin(); it != plList.end(); it++)
                i->feed_param(*it);
            for (std::list<Separator*>::const_iterator it = sepList.begin(); it != sepList.end(); it++)
                i->feed_param(*it);
            return i;
        } else { return 0; }
    }
};

struct PLightParam : public ParserParam<PointLight> {
private:
    Vec3 defLocation;
    Vec3 defColor;
public:
    Vec3* location;
    Vec3* color;

    PLightParam() : location(0), color(0) {
        defLocation[0] = 0.;    defLocation[1] = 0.;    defLocation[2] = 1.;
           defColor[0] = 1.;       defColor[1] = 1.;       defColor[2] = 1.;
    }

    virtual ~PLightParam() {
        if (location) delete location;
        if (color) delete color;
    }

    virtual bool incomplete_param() const { return false; }

    virtual const char* incomplete_param_msg() const { return ""; }

    virtual PointLight* make_object() const {
        const Vec3& locV = (location) ? (*location) : defLocation;
        const Vec3& colV = (color) ? (*color) : defColor;
        return new PointLight(locV, colV);
    }
};

struct SeparatorParam : public ParserParam<Separator> { 
public:
    Coordinate3 * cPtr;
    Normal3 * nPtr;
    IndexedFaceSet * iPtr;
    ComboTransform * tPtr;
    Material * mPtr;

    SeparatorParam() : tPtr(new ComboTransform()), cPtr(0), nPtr(0), iPtr(0), mPtr(0) {}

    virtual bool incomplete_param() const {
        return !(mPtr && cPtr && nPtr && iPtr);
    }

    virtual const char* incomplete_param_msg() const {
        if (!incomplete_param()) return "";
        std::string s = "No ";
        if (!mPtr) s += "Material ";
        if (!cPtr) s += "Coordinate3 ";
        if (!nPtr) s += "Normal ";
        if (!iPtr) s += "IndexedFaceSet ";
        return s.c_str();
    }

    virtual Separator* make_object() const {
        if (!incomplete_param())
            return new Separator(tPtr, mPtr, cPtr, nPtr, iPtr);
        else
            return 0;
    }
};

struct PCameraParam : public ParserParam<PerspectiveCamera> {
public:
    Vec3 *pos;
    Vec4 *orient;
    double *n; // nearDistance
    double *f; // farDistance
    double *l; // left
    double *r; // right
    double *t; // top
    double *b; // bottom

    PCameraParam() : pos(0), orient(0), n(0), f(0), l(0), r(0), t(0), b(0) {}

    virtual ~PCameraParam() {
        if (pos) delete pos;
        if (orient) delete orient;
        if (n) delete n;
        if (f) delete f;
        if (l) delete l;
        if (r) delete r;
        if (t) delete t;
        if (b) delete b;
    }

    virtual bool incomplete_param() const { return !(pos && orient && n && f && l && r && t && b); }

    virtual const char* incomplete_param_msg() const {
        if (!incomplete_param()) return "";
        std::string s = "No ";
        if (!pos) s += "position ";
        if (!orient) s += "orientation ";
        if (!n) s += "nearDistance ";
        if (!f) s += "farDistance ";
        if (!l) s += "left ";
        if (!r) s += "right ";
        if (!t) s += "top ";
        if (!b) s += "bottom ";
        return s.c_str();
    }

    virtual PerspectiveCamera* make_object() const {
        if (!incomplete_param())
            return new PerspectiveCamera(*pos, *orient, *n, *f, *l, *r, *t, *b);
        else
            return 0;
    }

};

// this enforces the TRS order of transforms
struct TransformParam : public ParserParam<ComboTransform> {
public:
    Translation *t;
    Rotation *r;
    ScaleFactor *s;

    TransformParam() : t(0), r(0), s(0) {}

    virtual ~TransformParam() {
        if (t) delete t;
        if (r) delete r;
        if (s) delete s;
    }

    virtual bool incomplete_param() const { return false; }

    virtual const char* incomplete_param_msg() const { return ""; }

    virtual ComboTransform* make_object() const {
        ComboTransform *tr = new ComboTransform();
        if (t) tr->push_right(t);
        if (r) tr->push_right(r);
        if (s) tr->push_right(s);
        return tr;
    }
};

/*struct NormalTransformParam : public ParserParam<ComboTransform> {
private:
    Transform *r;
    Transform *s;
public:
    NormalTransformParam(const TransformParam* tp) : r(0), s(0) {
        if (tp->r) r = tp->r->inverse();
        if (tp->s) s = tp->s->inverse();
    }

    virtual ~NormalTransformParam() {
        if (r) delete r;
        if (s) delete s;
    }

    virtual bool incomplete_param() const { return false; }

    virtual const char* incomplete_param_msg() const { return ""; }

    virtual ComboTransform* make_object() const {
        ComboTransform *tr = new ComboTransform();
        if (s) tr->push_right(s);
        if (r) tr->push_right(r);
        return tr;
    }
};*/

struct MaterialParam : public ParserParam<Material> {
private:
    Vec3 defAColor;
    Vec3 defDColor;
    Vec3 defSColor;
    const double defShininess;
public:
    Vec3 * aColor;
    Vec3 * dColor;
    Vec3 * sColor;
    double * shininess;

    MaterialParam() : aColor(0), dColor(0), sColor(0), shininess(0), defShininess(0.2) {
        defAColor[0] = .2; defAColor[1] = .2; defAColor[2] = .2;
        defDColor[0] = .8; defDColor[1] = .8; defDColor[2] = .8;
        defSColor[0] = .0; defSColor[1] = .0; defSColor[2] = .0;
    }

    virtual ~MaterialParam() {
        if (aColor) delete aColor;
        if (dColor) delete dColor;
        if (sColor) delete sColor;
        if (shininess) delete shininess;
    }

    virtual bool incomplete_param() const { return false; }

    virtual const char* incomplete_param_msg() const { return ""; }

    virtual Material* make_object() const {
        Vec3 ac = (aColor) ? (*aColor) : defAColor;
        Vec3 dc = (dColor) ? (*dColor) : defDColor;
        Vec3 sc = (sColor) ? (*sColor) : defSColor;

        for (int i = 0; i < 3; i++) {
            if (ac[i] > 1.) ac[i] = 1.; if (ac[i] < 0.) ac[i] = 0.;
            if (dc[i] > 1.) dc[i] = 1.; if (dc[i] < 0.) dc[i] = 0.;
            if (sc[i] > 1.) sc[i] = 1.; if (sc[i] < 0.) sc[i] = 0.;
        }

        double s = (shininess) ? (*shininess) : defShininess;
        if (s < 0.) s = 0.;

        return new Material(ac, dc, sc, s);
    }
};

struct IFSParam : public ParserParam<IndexedFaceSet> {
public:
    std::vector<int> * coordIndex;
    std::vector<int> * normalIndex;

    IFSParam() : coordIndex(0), normalIndex(0) {}

    virtual ~IFSParam() {}

    virtual bool incomplete_param() const { return !(coordIndex && normalIndex); }

    virtual const char* incomplete_param_msg() const {
        if (!incomplete_param()) return "";
        std::string s = "No ";
        if (!coordIndex) s += "coordIndex ";
        if (!normalIndex) s += "normalIndex ";
        return s.c_str();
    }

    virtual IndexedFaceSet* make_object() const {
        if (!incomplete_param())
            return new IndexedFaceSet(coordIndex, normalIndex);
        else
            return 0;
    }

};

#endif //   _parser_param_h