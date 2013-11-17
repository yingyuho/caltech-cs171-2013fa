//
//  ptr_container.h
//  
//  
//
//  Created by Ying-Yu Ho on 11/16/13.
//
//

#ifndef _ptr_container_h
#define _ptr_container_h

#include <list>
#include <vector>
#include <algorithm>

template< typename T >
class PtrList : public std::list<T*> {
private:
	static bool deleteAllPtr(T* elemPtr) { delete elemPtr; return true; }
	static T* clone(T* t) { return new T(*t); }
public:
    PtrList() {}
    PtrList(const PtrList&);
    PtrList& operator= (const PtrList&);
    virtual ~PtrList();
};

template< typename T >
class PtrVector : public std::vector<T*> {
private:
	static T* clone(T* t) { return new T(*t); }
	void deleteAllPtr();
public:
    PtrVector() {}
    PtrVector(const PtrVector&);
    PtrVector& operator= (const PtrVector&);
    virtual ~PtrVector();
};

// PtrList

template< typename T >
PtrList<T>::PtrList(const PtrList& x) : std::list<T*>(x) {
	std::transform(this->cbegin(), this->cend(), this->begin(), clone);
}

template< typename T >
PtrList<T>& PtrList<T>::operator= (const PtrList& x) {
	if ( this != &x ) {
		this->remove_if(deleteAllPtr);
		std::list<T*>::operator= (x);
		std::transform(this->cbegin(), this->cend(), this->begin(), clone);
	}
	return *this;
}

template< typename T >
PtrList<T>::~PtrList() {
	this->remove_if(deleteAllPtr);
}

// PtrVector

template< typename T >
void PtrVector<T>::deleteAllPtr() {
	for ( typename std::vector<T*>::iterator it = this->begin(); it != this->end(); it++ )
		delete *it;
}

template< typename T >
PtrVector<T>::PtrVector(const PtrVector& x) : std::vector<T*>(x) {
	std::transform(this->cbegin(), this->cend(), this->begin(), clone);
}

template< typename T >
PtrVector<T>& PtrVector<T>::operator= (const PtrVector& x) {
	this->deleteAllPtr();
	std::vector<T*>::operator= (x);
	std::transform(this->cbegin(), this->cend(), this->begin(), clone);
}

template< typename T >
PtrVector<T>::~PtrVector() {
	this->deleteAllPtr();
}

#endif //   _ptr_container_h