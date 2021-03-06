//
//  transform.h
//  
//  Interface and concrete classes that describe invertible transforms on 4x1 vectors
//  and give matrix representations when needed.
//
//  Created by Ying-Yu Ho on 10/31/13.
//
//

#ifndef _transform_h
#define _transform_h

#include <string>
#include "matrix.h"
#include "ptr_container.h"

class CoordTransformer;
class NormalTransformer;

// pure abstract base class for invertible transforms
class Transform;

// subclasses
// class ComboTransform;
class Translation;
class Rotation;
class ScaleFactor;
class TRSTransform;

class CoordTransformer {
public:
    virtual ~CoordTransformer() {}
    virtual Mat44 to_left_matrix() const = 0;
};

class NormalTransformer {
public:
    virtual ~NormalTransformer() {}
    virtual Mat33 to_right_matrix() const = 0;
};

class Transform : public CoordTransformer, public NormalTransformer {
public:
    virtual ~Transform() {};
    virtual Mat44 to_left_matrix() const = 0;
    virtual Mat33 to_right_matrix() const = 0;
    // virtual Transform* inverse() const = 0;
    virtual Transform* clone() const = 0;
};

class Translation : public Transform {
public:
    double x;
    double y;
    double z;

public:
    Translation() : x(0), y(0), z(0) {}
    Translation(double x, double y, double z) : x(x), y(y), z(z) {}
    Translation(const Translation& t) : x(t.x), y(t.y), z(t.z) {}
    Translation(const Vec3 v) : x(v[0]), y(v[1]), z(v[2]) {}

    virtual Mat44 to_left_matrix() const {
        double m[16] = {1, 0, 0, x, \
                        0, 1, 0, y, \
                        0, 0, 1, z, \
                        0, 0, 0, 1 };
        return Mat44(m);
    }

    virtual Mat33 to_right_matrix() const {
        return Mat33(1.);
    }

    virtual Transform* inverse() const { return new Translation(-x, -y, -z); }
    virtual Transform* clone() const { return new Translation(*this); }
};

class Rotation : public Transform {
public:
    Vec3 axis;
    double th;
private:
    // without renormalizing the axis
    Rotation(int i, const Vec3 &v, double th) : axis(v), th(th) {}

    static Vec3 unit_vec(double x, double y, double z) {
        double triple[3] = { x, y, z };
        Vec3 v(triple);
        return v.normalize();
    }

public:
    Rotation() : axis(unit_vec(0,0,1)), th(0) {}
    Rotation(double x, double y, double z, double th) : axis(unit_vec(x,y,z)), th(th) {}
    Rotation(const Vec4 &v) : axis(unit_vec(v[0],v[1],v[2])), th(v[3]) {}
    Rotation(const Vec3 &v, double th) : axis(v.normalize()), th(th) {}

    virtual Mat44 to_left_matrix() const {
        double x = axis[0];
        double y = axis[1];
        double z = axis[2];
        double s = sin(th);
        double c = cos(th);

        double m[16] = {x*x*(1-c) +   c, x*y*(1-c) - z*s, x*z*(1-c) + y*s, 0, \
                        y*x*(1-c) + z*s, y*y*(1-c) +   c, y*z*(1-c) - x*s, 0, \
                        z*x*(1-c) - y*s, z*y*(1-c) + x*s, z*z*(1-c) +   c, 0, \
                        0,0,0,1};

        return Mat44(m);
    }

    virtual Mat33 to_right_matrix() const {
        double x = axis[0];
        double y = axis[1];
        double z = axis[2];
        double s = sin(th);
        double c = cos(th);

        double m[9] = { x*x*(1-c) +   c, y*x*(1-c) + z*s, z*x*(1-c) - y*s, \
                        x*y*(1-c) - z*s, y*y*(1-c) +   c, z*y*(1-c) + x*s, \
                        x*z*(1-c) + y*s, y*z*(1-c) - x*s, z*z*(1-c) +   c  };

        return Mat33(m);
    }

    virtual Transform* inverse() const { return new Rotation(0, axis, -th); }
    virtual Transform* clone() const { return new Rotation(*this); }
};

class ScaleFactor : public Transform {
public:
    double x;
    double y;
    double z;

public:
    ScaleFactor() : x(1), y(1), z(1) {}
    ScaleFactor(double x, double y, double z) : x(x), y(y), z(z) {}
    ScaleFactor(const ScaleFactor& t) : x(t.x), y(t.y), z(t.z) {}
    ScaleFactor(const Vec3 &v) : x(v[0]), y(v[1]), z(v[2]) {}

    virtual Mat44 to_left_matrix() const {
        double m[16] = {x, 0, 0, 0, \
                        0, y, 0, 0, \
                        0, 0, z, 0, \
                        0, 0, 0, 1 };
        return Mat44(m);
    }

    virtual Mat33 to_right_matrix() const {
        double m[9 ] = { 1/x,  0.,  0., \
                          0., 1/y,  0., \
                          0.,  0., 1/z, };
        return Mat33(m);
    }

    virtual Transform* inverse() const throw (ZeroDetException) {
        if ( x*y*z == 0. || x*y*z == -0. )
            throw ZeroDetException();
        return new ScaleFactor(1/x,1/y,1/z);
    }

    virtual Transform* clone() const { return new ScaleFactor(*this); }
};

/*class ComboTransform : public Transform {
private:
    typedef PtrList<Transform> TList;
    TList tList;
    void append(const TList* tList2) {
        for (TList::const_iterator it = tList2->begin(); it != tList2->end(); it++)
            tList.push_back((*it)->clone());
    }

public:
    ComboTransform() {}
    ComboTransform(const TList& tList2) { append(&tList2); }
    ComboTransform(const TList* tList2) { append(tList2); }
    // ComboTransform(const ComboTransform& t) { append(&(t.tList)); }

    ComboTransform& push_right(const Transform *t) {
        tList.push_back(t->clone());
        return *this;
    }

    // virtual ~ComboTransform() {}

    virtual Mat44 to_left_matrix() const {
        // start with the identity matrix
        Mat44 m = Mat44(1.);

        // right multiplication by matrices in the list
        for (TList::const_iterator it = tList.begin(); it != tList.end(); it++)
            m = m * (*it)->to_left_matrix();

        return m;
    }

    virtual Mat33 to_right_matrix() const {
        // start with the identity matrix
        Mat33 m = Mat33(1.);

        // left multiplication by matrices in the list
        for (TList::const_iterator it = tList.begin(); it != tList.end(); it++)
            m = (*it)->to_right_matrix() * m;

        return m;
    }

    virtual Transform* inverse() const {
        ComboTransform *t = new ComboTransform();

        for (TList::const_iterator it = tList.begin(); it != tList.end(); it++)
            t->tList.push_front((*it)->inverse());

        return t;
    }

    virtual Transform* clone() const {
        return new ComboTransform(*this);
    }
};*/

class TRSTransform : public Transform {
public:
    Translation t;
    Rotation r;
    ScaleFactor s;

public:
    TRSTransform() {}

    TRSTransform(const Translation& t, const Rotation& r, const ScaleFactor& s)\
    : t(t), r(r), s(s) {}

    virtual Mat44 to_left_matrix() const {
        Mat44 m = Mat44(1.);

        // right multiplication by matrices in TRS order
        m = m * t.to_left_matrix();
        m = m * r.to_left_matrix();
        m = m * s.to_left_matrix();

        return m;
    }

    virtual Mat33 to_right_matrix() const {
        // start with the identity matrix
        Mat33 m = Mat33(1.);

        // left multiplication by matrices in TRS order
        m = t.to_right_matrix() * m;
        m = r.to_right_matrix() * m;
        m = s.to_right_matrix() * m;

        return m;
    }

    /*virtual Transform* inverse() const {
        ComboTransform *tr = new ComboTransform();

        tr->push_right(&s);
        tr->push_right(&r);
        tr->push_right(&t);

        return tr;
    }*/

    virtual Transform* clone() const {
        return new TRSTransform(*this); 
    }
};


#endif //   _transform_h