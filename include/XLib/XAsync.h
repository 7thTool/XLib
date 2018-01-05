#ifndef _H_ASYNC_H_
#define _H_ASYNC_H_

#include "XLib.h"
#include "XStat.h"
#include "XThread.h"

class XLIB_API AsyncWork : public XStatusEx
{
protected:
	long m_AsyncPoolId;				//�첽����ID
	CriticalSection m_Section;		//��

public:
	AsyncWork();
	virtual ~AsyncWork();

	virtual void StartWork(long flag = 0);
	virtual void StopWork();

	virtual void Work();
};

#define FIND_STATUS_FIND			0X00000002
#define FIND_STATUS_FINDING			0X00000004
#define FIND_STATUS_STOPFIND		0X00000008

class AsyncFinder;

class XLIB_API FindKey
{
protected:
	AsyncFinder* m_pFinder;
public:
	FindKey(AsyncFinder* pFinder);

	bool IsNeedStopFind();
};

class XLIB_API AsyncFinder : public AsyncWork
{
public:
	typedef AsyncWork Base;
protected:
	long m_TickSpan;
public:
	AsyncFinder();
	virtual ~AsyncFinder();

	virtual void Find();
	virtual void StopFind();
	virtual bool IsFinding();
	virtual bool IsNeedStopFind();

	virtual long GetTickSpan();

	virtual void Work();
	//����
	virtual void OnIdle();
	//��ʼ����
	virtual long OnFind();
	//�������
	virtual void OnFindComplete(long err);
};

template<class T, UINT uMsg = WM_USER>
class AsyncWndFinder : public AsyncFinder
{
public:
	typedef AsyncFinder Base;
public:
	AsyncWndFinder()
	{

	}

	//�������
	virtual void OnFindComplete(long err)
	{
		T* pT = static_cast<T*>(this);
		pT->PostMessage(uMsg, (WPARAM)err, (LPARAM)pT);
	}
};

#endif//_H_ASYNC_H_