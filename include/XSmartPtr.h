#ifndef _H_XSMARTPTR_H_
#define _H_XSMARTPTR_H_

#include "XLib.h"

//////////////////////////////////////////////////////////////////////////
///ObjectPtr

class XLIB_API Object
{
public:
	Object(long ref = 0); //ref<0:stack alloc ref==0:heap alloc ref>0:error
	virtual ~Object();

	long __getRef();
	long __incRef();
	long __decRef();

	bool operator==(const Object& o) const;
	bool operator!=(const Object& o) const;
	bool operator<(const Object& o) const;

	/*Object():__ref_count_(0){}
	virtual ~Object(){ASSERT(this->__ref_count_ == 0);}

	int __incRef() {
		ASSERT(this->__ref_count_ >= 0);
		return InterlockedIncrement(&__ref_count_);
	}
	int __decRef() {
		ASSERT(this->__ref_count_ > 0);
		long ref = InterlockedDecrement(&__ref_count_);
		if( ref == 0 ) {
			delete this;
		}
		return ref;
	}

	bool operator==(const Object& o) const {
		return this == &o;
	}
	bool operator!=(const Object& o) const {
		return this != &o;
	}
	bool operator<(const Object& o) const {
		return this < &o;
	}*/

private:
	volatile long __ref_count_;
};

template<typename T>
class Handle
{
public:
	Handle( T * ptr = 0 ) {
		this->auto_ptr_ = ptr;
		if( this->auto_ptr_ )
			this->auto_ptr_->__incRef();
	}

	template<typename Y>
	Handle( const Handle<Y> & r ) {
		this->auto_ptr_ = (Y*)r;
		if( this->auto_ptr_ )
			this->auto_ptr_->__incRef();
	}

	Handle( const Handle & ptr ) {
		this->auto_ptr_ = ptr;
		if( this->auto_ptr_ )
			this->auto_ptr_->__incRef();
	}

	~Handle() {
		if( this->auto_ptr_ )
			this->auto_ptr_->__decRef();
	}

	operator T*() const {
		return this->auto_ptr_;
	}
	/*T* get() const {
		return this->auto_ptr_;
	}*/

	Handle & operator= ( T * ptr ) {
		if( this->auto_ptr_ != ptr ) {
			if( ptr ) {
				ptr->__incRef();
			}

			T * p = this->auto_ptr_;
			this->auto_ptr_ = ptr;

			if( p ) {
				p->__decRef();
			}
		}

		return *this;
	}

	template<typename Y>
	Handle & operator= ( const Handle<Y> & r ) {
		T* r_auto_ptr = (Y*)r;
		if( this->auto_ptr_ != r_auto_ptr) {
			if( r_auto_ptr ) {
				r_auto_ptr->__incRef();
			}

			T * ptr = this->auto_ptr_;
			this->auto_ptr_ = r_auto_ptr;

			if( ptr ) {
				ptr->__decRef();
			}
		}

		return *this;
	}

	Handle& operator=(const Handle& r) {
		T* r_auto_ptr = (T*)r;
		if( this->auto_ptr_ != r_auto_ptr ) {
			if( r_auto_ptr ) {
				r_auto_ptr->__incRef();
			}

			T * ptr = this->auto_ptr_;
			this->auto_ptr_ = r_auto_ptr;

			if( ptr ) {
				ptr->__decRef();
			}
		}

		return *this;
	}

	T* operator ->() const {
		ASSERT(this->auto_ptr_);
		return this->auto_ptr_;
	}

	T& operator *() const {
		ASSERT(this->auto_ptr_);
		return *this->auto_ptr_;
	}

	operator bool() const {
		return auto_ptr_ ? true : false;
	}

	template<class Y>
	static Handle dynamicCast(const Handle<Y>& r) {
		T * ptr = dynamic_cast<T*>((Y*)r);
		return Handle(ptr);
	}

	template<class Y>
	static Handle dynamicCast(Y* p) {
		return Handle(dynamic_cast<T*>(p));
	}

private:
	T * auto_ptr_;
};

template<typename T, typename U>
inline bool operator!=(const Handle<T>& lhs, const Handle<U>& rhs)
{
	T* l = lhs;
	U* r = rhs;
	if(l && r) {
		return *l != *r;
	} else {
		return !l || !r;
	}
}

template<typename T, typename U>
inline bool operator==(const Handle<T>& lhs, const Handle<U>& rhs)
{
	T* l = lhs;
	U* r = rhs;
	if(l && r) {
		return *l == *r;
	} else {
		return !l && !r;
	}
}

template<typename T, typename U>
inline bool operator<(const Handle<T>& lhs, const Handle<U>& rhs)
{
	T* l = lhs;
	U* r = rhs;
	if(l && r) {
		return *l < *r;
	} else {
		return !l && r;
	}
}

typedef Handle<Object> ObjectPtr;

#endif//_H_XSMARTPTR_H_
