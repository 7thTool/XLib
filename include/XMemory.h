#ifndef _H_UTIL_MEMORY_H_
#define _H_UTIL_MEMORY_H_

#include "XLib.h"
#include "XThread.h"
#include "XFile.h"

//////////////////////////////////////////////////////////////////////////
#define SAFE_DELETE(p)		do{if(p){delete p; p = NULL;}}while(0)
#define SAFE_DELETE_EX(p)	do{if(p){delete []p; p = NULL;}}while(0)

//////////////////////////////////////////////////////////////////////////
///Buffer
//////////////////////////////////////////////////////////////////////////
template <class T, int nStackCount = 512>
class BufferT
{
protected:
	T* m_pT;
	int m_nCount;
	T m_T[nStackCount];
public:
	BufferT() : m_pT(m_T), m_nCount(nStackCount) { }
	BufferT(int nCount) : m_pT(m_T), m_nCount(nStackCount) { Alloc(nCount); }
	BufferT(int nCount, const T & Val) : m_pT(m_T), m_nCount(nStackCount) { Alloc(nCount, Val); }
	BufferT(T* pT, int nCount) : m_pT(m_T), m_nCount(nStackCount) { Alloc(pT, nCount); }
	~BufferT() { Free(); }

	operator T* () const { return m_pT; }
	T& operator[](int pos) const { assert(pos>0&&pos<m_nCount) return m_pT[pos]; }

	T* Buf() { return m_pT; }
	T* Bufffer() { return m_pT; }

	int Count() { return m_nCount; }
	int Size() { return m_nCount*sizeof(T); }

	T* Alloc(int nCount)
	{
		if(m_nCount >= nCount) {
			if (m_nCount > nStackCount) {
				delete []m_pT;
				m_pT = m_T;
			}
		} else {
			if(m_nCount > nStackCount) {
				delete []m_pT;
				m_pT = new T[nCount];
			} else if(nCount > nStackCount) {
				m_pT = new T[nCount];
			} else {
				m_pT = m_T;
			}
		}
		m_nCount = nCount;
		return m_pT;
	}

	T* Alloc(int nCount, T Val)
	{
		T* pThis = Alloc(nCount); 
		for(int i = 0; i < nCount; i++)
		{
			pThis[i] = Val;
		}
		return pThis;
	}

	T* Alloc(T* pT, int nCount)
	{
		T* pThis = Alloc(nCount); 
		for(int i = 0; i < nCount; i++)
		{
			pThis[i] = pT[i];
		}
		return pThis;
	}

	void Free()
	{
		if(m_nCount > nStackCount) {
			delete []m_pT;
			m_pT = m_T;
		}
		m_nCount = nStackCount;
	}
};

template <class T, int nStackLength = 512>
class CBufferT : public BufferT<T, nStackLength>
{
	typedef BufferT<T, nStackLength> Base;
public:
	CBufferT() : Base() { }
	CBufferT(int nBufferLength) : Base(nBufferLength) { }
	CBufferT(int nBufferLength, const T & Val) : Base(nBufferLength, Val) { }
	CBufferT(T* pBuffer, int nBufferLength) : Base(pBuffer,nBufferLength) { }

	T* GetBuffer() { return (T*)(*this); }
	T* GetBuffer(int nNewBufferLength) { return Alloc(nNewBufferLength); }
	void ReleaseBuffer() { Free(); }
};

//////////////////////////////////////////////////////////////////////////
///Memory
//////////////////////////////////////////////////////////////////////////
class XLIB_API ShareMemory : public MemFile
{
public:
	ShareMemory();
	ShareMemory(LPCTSTR lpszShareMemoryName);
	ShareMemory(LPCTSTR lpszShareMemoryName, DWORD dwMemorySize);
	~ShareMemory();

	BOOL Create(LPCTSTR lpszShareMemoryName, DWORD dwMemorySize);
};

//////////////////////////////////////////////////////////////////////////
///MemoryPool
//////////////////////////////////////////////////////////////////////////

class MemoryBlock;
class XLIB_API MemoryPool 
{ 
private: 
	size_t m_nUnitSize;		//一个可分配单元的大小
	size_t m_nInitSize;		//第一个可分配空间数目
	size_t m_nGrowSize;		//新增的可分配空间数目
	MemoryBlock* m_pFirst;	//指向第一个内存块

public: 
	//单元大小，第一个内存块的可分配空间数目，第二个内存块之后的可分配空间数目 
	MemoryPool(const size_t& nUnitSize, const size_t& nInitSize = 32, const size_t& nGrowSize = 32); 
	~MemoryPool(void);
	void* Alloc();			//分配内存 
	void Free(void* pfree);	//回收内存

private:
	void FreeMemoryBlock();//销毁内存
};

//////////////////////////////////////////////////////////////////////////
///VarMemoryPool
//////////////////////////////////////////////////////////////////////////

class VarMemoryBlock;
class XLIB_API VarMemoryPool
{
private: 
	size_t m_nInitSize;	//第一个可分配空间数目
	size_t m_nGrowSize;	//新增的可分配空间数目
	VarMemoryBlock* m_pFirst;//指向第一个内存块
public: 
	//第一个内存块的可分配空间大小，单位BYTE，第二个内存块之后的可分配空间数目，单位BYTE 
	VarMemoryPool(const size_t& nInitSize = 32, const size_t& nGrowSize = 32);
	~VarMemoryPool(void);
public:
	void* Alloc(const size_t& nSize);	//分配内存
	//void Free(void* pfree);			//释放内存,没有释放，只增不减内存池
	size_t SaveToFile(HANDLE hFile);	//将内存序列化到磁盘,返回写入文件大小
	void* LoadFromFile(HANDLE hFile);	//读取序列化内存，返回内存指针

private:
	void FreeMemoryBlock();
};

//////////////////////////////////////////////////////////////////////////

class XLIB_API memoryset
{
public:
	memoryset();
	~memoryset();

	void* Alloc(size_t size);
	void Free(void* pfree, size_t size);

protected:
	MemoryPool m_MemPool4;
	MemoryPool m_MemPool8;
	MemoryPool m_MemPool16;
	MemoryPool m_MemPool32;
	MemoryPool m_MemPool64;
	MemoryPool m_MemPool128;
	MemoryPool m_MemPool256;
	MemoryPool m_MemPool512;
	MemoryPool m_MemPool1024;
	MemoryPool m_MemPool2048;
	MemoryPool m_MemPool4096;
	MemoryPool m_MemPool6144;
	MemoryPool m_MemPool8192;
	MemoryPool m_MemPool10240;
};

class XLIB_API objmemoryset
{
public:
	objmemoryset() {
	}
	~objmemoryset() {
	}

	template<class _Ty>
	void Alloc(_Ty*& ty) {//分配内存 
		SectionLocker Lock(&m_Section);
		size_t size = sizeof(_Ty);
		MemoryPool*& pMemPool = m_Size2MemPool[size];
		if (pMemPool == NULL) {
			pMemPool = new MemoryPool(size, 32, 32);
		}
		ty = new (pMemPool->Alloc())_Ty();
	}
	template<class _Ty>
	void Free(_Ty* ty, size_t size = 0) {//回收内存
		SectionLocker Lock(&m_Section);
		if (size == 0) {
			size = sizeof(_Ty);
		}
		MemoryPool*& pMemPool = m_Size2MemPool[size];
		if (pMemPool) {
			ty->~_Ty();
			pMemPool->Free((void*)ty);
		}
	}

protected:
	map<size_t,MemoryPool*>	 m_Size2MemPool; //固定大小内存到内存池的映射
	CriticalSection	m_Section; //锁
};

#endif//_H_UTIL_MEMORY_H_