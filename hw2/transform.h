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
#include <list>
#include <algorithm>
#include <typeinfo>
#include "matrix.h"

class Mat44Maker;

// pure abstract base class for invertible transforms
class Transform;

// subclasses
class ComboTransform;
class Translation;
class Rotation;
class ScaleFactor;

//class NonTRSBlockException;

class Mat44Maker {
public:
    virtual ~Mat44Maker() {}
    virtual Mat44 to_matrix() const = 0 ;
};

class Transform : public Mat44Maker {
public:
    typedef Matrix<double,4,4> Mat44;
    typedef Matrix<double,3,1> Vec3;
    typedef Matrix<double,4,1> Vec4;

    template<typename T>
    static bool deleteAllPtr(T* elemPtr) { delete elemPtr; return true; }

    virtual ~Transform() {};
    virtual Mat44 to_matrix() const = 0;
    virtual Transform* inverse() const = 0;
    virtual Transform* clone() const = 0;
};

class Translation : public Transform {
private:
    double x;
    double y;
    double z;

public:
    Translation(double x, double y, double z) : x(x), y(y), z(z) {}
    Translation(const Vec3 v) : x(v[0]), y(v[1]), z(v[2]) {}

    virtual Mat44 to_matrix() const {
        double m[16] = {1, 0, 0, x, \
                        0, 1, 0, y, \
                        0, 0, 1, z, \
                        0, 0, 0, 1 };
        return Mat44(m);
    }

    virtual Transform* inverse() const { return new Translation(-x, -y, -z); }
    virtual Transform* clone() const { return new Translation(x, y, z); }
};

class Rotation : public Transform {
private:
    Vec3 axis;
    double th;

    // without renormalizing the axis
    Rotation(int i, const Vec3 &v, double th) : axis(v), th(th) {}

    static Vec3 unit_vec(double x, double y, double z) {
        double triple[3] = { x, y, z };
        Vec3 v(triple);
        return v.normalize();
    }

public:
    Rotation(double x, double y, double z, double th) : axis(unit_vec(x,y,z)), th(th) {}
    Rotation(const Vec4 &v) : axis(unit_vec(v[0],v[1],v[2])), th(v[3]) {}
    Rotation(const Vec3 &v, double th) : axis(v.normalize()), th(th) {}

    virtual Mat44 to_matrix() const {
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

    virtual Transform* inverse() const { return new Rotation(0, axis, -th); }
    virtual Transform* clone() const { return new Rotation(0, axis, th); }
};

class ScaleFactor : public Transform {
private:
    double x;
    double y;
    double z;

public:
    ScaleFactor(double x, double y, double z) : x(x), y(y), z(z) {}
    ScaleFactor(const Vec3 &v) : x(v[0]), y(v[1]), z(v[2]) {}

    virtual Mat44 to_matrix() const {
        double m[16] = {x, 0, 0, 0, \
                        0, y, 0, 0, \
                        0, 0, z, 0, \
                        0, 0, 0, 1 };
        return Mat44(m);
    }

    virtual Transform* inverse() const throw (ZeroDetException) {
        if ( x*y*z == 0. || x*y*z == -0. )
            throw ZeroDetException();
        return new ScaleFactor(1/x,1/y,1/z);
    }

    virtual Transform* clone() const { return new ScaleFactor(x,y,z); }
};

/*class MatrixTransform : public Transform {
private:
    Mat44 data;
    Mat44 data_inv;
    MatrixTransform(const Mat44 &m1, const Mat44 &m2) : data(m1), data_inv(m2) {}
public:
    MatrixTransform(const Transform *t) {
        if (typeid(*t)==typeid(MatrixTransform)) {
            MatrixTransform *t2 = (MatrixTransform*) t;
            data = t2->data;
            data_inv = t2->data_inv;
        } else {
            data = t->to_matrix();
            Transform *ti = t->inverse();
            data_inv = ti->to_matrix();
            delete ti;
        }
    }

    virtual Mat44 to_matrix() const { return data; }

    virtual Transform* inverse() const { return new MatrixTransform(data_inv, data); }

    virtual Transform* clone() const { return new MatrixTransform(data, data_inv); }
};*/

class ComboTransform : public Transform {
private:
    typedef std::list<Transform*> TransformList;
    TransformList tList;

public:
    ComboTransform() {}

    ComboTransform(const TransformList *tPtrList2) {
        for (TransformList::const_iterator it = tPtrList2->begin(); it != tPtrList2->end(); it++)
            tList.push_back((*it)->clone());
    }

    ComboTransform& push_right(const Transform *t) {
        tList.push_back(t->clone());
        return *this;
    }

    virtual ~ComboTransform() {
        tList.remove_if(Transform::deleteAllPtr<Transform>);
    }

    virtual Mat44 to_matrix() const {
        // start with the identity matrix
        Mat44 m = Mat44(1.);

        // right multiplication by matrices in the list
        for (TransformList::const_iterator it = tList.begin(); it != tList.end(); it++)
            m = m * (*it)->to_matrix();

        return m;
    }

    virtual Transform* inverse() const {
        ComboTransform *t = new ComboTransform();

        for (TransformList::const_iterator it = tList.begin(); it != tList.end(); it++)
            t->tList.push_front((*it)->inverse());

        return t;
    }

    virtual Transform* clone() const {
        return new ComboTransform(&tList);
    }
};

/*class NonTRSBlockException : public std::runtime_error { 
public:
    NonTRSBlockException(const std::string& msg) : std::runtime_error(msg) {}
    ~NonTRSBlockException() throw () {}
};*/

/*class TRSTransform : public Transform {
private:
    Transform* trsArray[3];
    #define IDT typeid(Translation).hash_code()
    #define IDR typeid(Rotation).hash_code()
    #define IDS typeid(ScaleFactor).hash_code()
    ;
public:
    TRSTransform() { std::fill_n(trsArray, 3, (Transform*)(0)); }

    TRSTransform& push(const Transform *t) throw ( NonTRSBlockException ) {
        size_t id = typeid(*t).hash_code();
        int i;

        if (id == IDT)
            i = 0;
        else if (id == IDR)
            i = 1;
        else if (id == IDS)
            i = 2;
        else
            throw NonTRSBlockException(\
                "only translation, rotation, and scaling are allowed in a transform block");

        if ( trsArray[i] != 0 ) {
            throw NonTRSBlockException(\
                "at most one translation, rotation, or scaling in each transform block");
        } else {
            trsArray[i] = t->clone();
        }

        return *this;
    }

    virtual ~TRSTransform() {
        for ( int i = 0; i < 3; i++ )
            if ( trsArray[i] != 0 ) { delete trsArray[i]; }
    }

    virtual Mat44 to_matrix() const {
        Mat44 m = Mat44(1.);

        // right multiplication by matrices in the array
        for ( int i = 0; i < 3; i++ )
            if ( trsArray[i] != 0 ) { m = m * trsArray[i]->to_matrix(); }

        return m;
    }

    virtual Transform* inverse() const {
        ComboTransform *t = new ComboTransform();

        for ( int i = 3-1; i >= 0; i-- )
            if ( trsArray[i] != 0 ) { t->push_right(trsArray[i]); }

        return t;
    }

    virtual Transform* clone() const {
        TRSTransform *t = new TRSTransform();  

        for ( int i = 0; i < 3; i++ )
            if ( trsArray[i] != 0 ) { t->trsArray[i] = trsArray[i]->clone(); }

        return t;
    }
};*/


#endif //   _transform_h