#ifndef _H_XSMARTPTREX_H_
#define _H_XSMARTPTREX_H_

#include "XLib.h"
#include "XSmartPtr.h"

//////////////////////////////////////////////////////////////////////////
//ObjectEx可以在堆栈上使用，比如堆栈局部智能对象，使用SendEvent等同步方法
//调用是没有问题的
//也可以和ObjectExT配合使用内存池new delete对象
//

class XLIB_API ObjectEx : public Object
{
public:
	static void* Alloc(size_t size);
	static void Free(void* p, size_t size);

public:
	ObjectEx(long ref = 0);
	virtual ~ObjectEx();

	void* operator new(size_t size);
	void operator delete(void *p);
};
typedef Handle<ObjectEx> ObjectExPtr;
template<class T, class TBase = ObjectEx>
class ObjectExT : public TBase
{
	typedef ObjectExT<T> This;
	typedef TBase Base;
public:
	void* operator new(size_t size)
	{
		ASSERT(size==sizeof(T));
		return Base::Alloc(size);
	}

	void operator delete(void *p)
	{
		Base::Free(p, sizeof(T));
	}
};

#endif//_H_XSMARTPTREX_H_
