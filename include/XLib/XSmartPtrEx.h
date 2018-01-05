#ifndef _H_XSMARTPTREX_H_
#define _H_XSMARTPTREX_H_

#include "XLib.h"
#include "XSmartPtr.h"

//////////////////////////////////////////////////////////////////////////
//ObjectEx�����ڶ�ջ��ʹ�ã������ջ�ֲ����ܶ���ʹ��SendEvent��ͬ������
//������û�������
//Ҳ���Ժ�ObjectExT���ʹ���ڴ��new delete����
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
