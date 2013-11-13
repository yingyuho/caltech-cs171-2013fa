//
//  inventor.h
//  
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
	template<typename T>
	static bool deleteAllPtr(T* elemPtr) { delete elemPtr; return true; }
	template<typename T> static void watch(T) {} // should optimize away
	// static void watch(const char *s) { std::cout << s; }
	// static void watch(const int i) { std::cout << i << " "; }
}

template< typename T, typename U>
class PolygonMaker;

class Inventor;
class PerspectiveCamera;
class Separator;
class Coordinate3;
class IndexedFaceSet;
class IndexedFaceLine;

template< typename T, typename U = std::vector<T> >
class PolygonMaker {
public:
	typedef T PLType;
	typedef std::list<std::list<T>*> PLList;
	typedef U PointMap;
	virtual ~PolygonMaker() {}
	virtual void build_polygon_list(PLList& pList, const U& pMap) const = 0;
};

class Coordinate3 {
private:
	std::vector<Vec4> ptVec;
public:
	Coordinate3() {}

	~Coordinate3() {}

	Coordinate3& add_point(const Vec3 &v) {
		double v4[4] = {v[0], v[1], v[2], 1};
		ptVec.push_back(Vec4(v4));
		return *this;
	}
	const std::vector<Vec4>& get_obj_space_coord() const {
		return ptVec;
	}
};

class IndexedFaceLine : public PolygonMaker<IntVec2> {
private:
	std::vector<int> indexVec;
public:
	IndexedFaceLine() {
		// TODO
	}

	~IndexedFaceLine() {
		// TODO
	}

	IndexedFaceLine& add_coord_index(int index) {
		indexVec.push_back(index);
		return *this;
	}

	void build_polygon_list(PLList& pList, const PointMap& pMap) const {
		InventorHelper::watch("IndexedFaceLine::build_polygon_list()\n");

		std::list<PLType>* polyLPtr = new std::list<PLType>();

		for ( std::vector<int>::const_iterator it = indexVec.begin(); it != indexVec.end(); it++ ) {
			InventorHelper::watch(*it);
			if ( *it == -1 ) {
				pList.push_back(polyLPtr);
				polyLPtr = new std::list<PLType>();
			} else {
				polyLPtr->push_back(pMap[*it]);
			}
		}
		InventorHelper::watch("\n");

		if ( polyLPtr->empty() )
			delete polyLPtr;
		else
			pList.push_back(polyLPtr);
	}
};

class IndexedFaceSet : public PolygonMaker<IntVec2> {
private:
	std::list<IndexedFaceLine*> ifsLineList;
	typedef std::list<IndexedFaceLine*>::const_iterator CIter;
public:
	IndexedFaceSet() {}

	~IndexedFaceSet() {
		ifsLineList.remove_if(InventorHelper::deleteAllPtr<IndexedFaceLine>);
	}

	IndexedFaceSet& add_ifsline(IndexedFaceLine* ifsline) {
		ifsLineList.push_back(ifsline);
		return *this;
	}

	void build_polygon_list(PLList& pList, const PointMap& pMap) const {
		InventorHelper::watch("IndexedFaceSet::build_polygon_list()\n");
		for ( CIter it = ifsLineList.begin(); it != ifsLineList.end(); it++ ) {
			(*it)->build_polygon_list(pList, pMap);
		}
	}
};

class PerspectiveCamera {
private:
	Vec3 pos;
	Vec3 axis;
	double rot_angle;
    double n; // nearDistance
    double f; // farDistance
    double l; // left
    double r; // right
    double t; // top
    double b; // bottom
public:
	PerspectiveCamera(Vec3 pos, Vec4 orient \
		, double n, double f, double l, double r, double t, double b) \
		: pos(pos), axis(Vec3(&(orient[0])).normalize()), rot_angle(orient[3]) \
		, n(n), f(f), l(l), r(r), t(t), b(b) {}

	Mat44 to_matrix() const {
		Translation *t1 = new Translation(-pos);
		Rotation *t2 = new Rotation(axis, -rot_angle);

		double proj[16] = 	{ 2*n/(r-l), 0, (r+l)/(r-l), 0 \
							, 0, 2*n/(t-b), (t+b)/(t-b), 0 \
							, 0, 0, -(f+n)/(f-n), -2*f*n/(f-n) \
							, 0, 0, -1, 0 };

		Mat44 matrix = Mat44(proj) * t2->to_matrix() * t1->to_matrix();

		delete t1;
		delete t2;

		return matrix;
	}
};

class Separator : public PolygonMaker<IntVec2> {
private:
	ComboTransform *tPtr;
	Coordinate3 *coord3Ptr;
	IndexedFaceSet *ifsPtr;
public:
	Separator(ComboTransform* tPtr, Coordinate3* cPtr, IndexedFaceSet* iPtr) \
	: tPtr(tPtr), coord3Ptr(cPtr), ifsPtr(iPtr) {}

	~Separator() {
		if (tPtr) delete tPtr;
		if (coord3Ptr) delete coord3Ptr;
		if (ifsPtr) delete ifsPtr;
	}

	Mat44 to_matrix() const {
		return tPtr->to_matrix();
	}

	const std::vector<Vec4>& get_obj_space_coord() const {
		return coord3Ptr->get_obj_space_coord();
	}

	const std::vector<Vec4> get_norm_device_coord(const Mat44& pcMatrix) const {
		const std::vector<Vec4>& osc = get_obj_space_coord();
		std::vector<Vec4> ndc(osc.size());
		Mat44 mat = pcMatrix * tPtr->to_matrix();

		for ( int i = 0; i < osc.size(); i++ )
			ndc[i] = mat * osc[i];

		return ndc;
	}

	const std::vector<IntVec2> get_pixel_coord(const Mat44& pcMatrix, int xRes, int yRes) const {
		const std::vector<Vec4>& osc = get_obj_space_coord();
		std::vector<IntVec2> pc(osc.size());
		Mat44 mat = pcMatrix * tPtr->to_matrix();

		for ( int i = 0; i < osc.size(); i++ ) {
			Vec4 ndc = mat * osc[i];
			pc[i][0] = (int)floor(xRes*(1 + ndc[0]/ndc[3])/2);
			pc[i][1] = (int)floor(yRes*(1 - ndc[1]/ndc[3])/2);
		}

		return pc;
	}

	void build_polygon_list(PLList& pList, const PointMap& pMap) const {
		InventorHelper::watch("Separator::build_polygon_list()\n");
		ifsPtr->build_polygon_list(pList, pMap);
	}
};

class Inventor : public PolygonMaker<IntVec2,IntVec2> {
private:
	PerspectiveCamera* pCamera;
	std::list<Separator*> sepList;
public:
	Inventor(PerspectiveCamera* pCamera) : pCamera(pCamera) {}

	~Inventor() {
		if (pCamera) delete pCamera;
		sepList.remove_if(InventorHelper::deleteAllPtr<Separator>);
	}

	Inventor& add_separator(Separator* sep) {
		sepList.push_back(sep);
		return *this;
	}

	void build_polygon_list(PLList& pList, const IntVec2& res) const {
		InventorHelper::watch("Inventor::build_polygon_list()\n");
		for ( std::list<Separator*>::const_iterator it = sepList.begin();\
			it != sepList.end(); it++ ) {

			std::vector<IntVec2> pixel_coord = \
				(*it)->get_pixel_coord(pCamera->to_matrix(), res[0], res[1]);

			(*it)->build_polygon_list(pList, pixel_coord);
		}
	}
};

#endif //   _inventor_h