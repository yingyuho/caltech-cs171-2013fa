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

struct InventorParam : public ParserParam<Inventor> { 
public:
    PerspectiveCamera* pCamPtr;
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
            for (std::list<Separator*>::const_iterator it = sepList.begin(); it != sepList.end(); it++)
                i->feed_param(*it);
            return i;
        } else { return 0; }
    }
};

struct SeparatorParam : public ParserParam<Separator> { 
public:
    Coordinate3* cPtr;
    IndexedFaceSet* iPtr;
    ComboTransform* tPtr;

    SeparatorParam() : tPtr(new ComboTransform()), cPtr(0), iPtr(0) {}

    virtual bool incomplete_param() const { return !(cPtr && iPtr); }

    virtual const char* incomplete_param_msg() const {
        if (!incomplete_param()) return "";
        std::string s = "No ";
        if (!cPtr) s += "Coordinate3 ";
        if (!iPtr) s += "IndexedFaceSet ";
        return s.c_str();
    }

    virtual Separator* make_object() const {
        if (!incomplete_param())
            return new Separator(tPtr, cPtr, iPtr);
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

#endif //   _parser_param_h