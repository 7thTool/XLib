/*
 * Copyright: 7thTool Open Source (i7thTool@qq.com)
 *
 * Version	: 1.1.1
 * Author	: Scott
 * Project	: http://git.oschina.net/7thTool/XSocket
 * Blog		: http://blog.csdn.net/zhangzq86
 *
 * LICENSED UNDER THE GNU LESSER GENERAL PUBLIC LICENSE, VERSION 2.1 (THE "LICENSE");
 * YOU MAY NOT USE THIS FILE EXCEPT IN COMPLIANCE WITH THE LICENSE.
 * UNLESS REQUIRED BY APPLICABLE LAW OR AGREED TO IN WRITING, SOFTWARE
 * DISTRIBUTED UNDER THE LICENSE IS DISTRIBUTED ON AN "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.
 * SEE THE LICENSE FOR THE SPECIFIC LANGUAGE GOVERNING PERMISSIONS AND
 * LIMITATIONS UNDER THE LICENSE.
 */

#ifndef _H_XTHREAD_H_
#define _H_XTHREAD_H_

#include "XLib.h"

//////////////////////////////////////////////////////////////////////////

class XLIB_API Atomic
{
public:
	static long Exchange(long volatile *Target, long Value);
	static void* ExchangePointer(void* *Target, void* Value);

	static long CompareExchange(long volatile *Target, long Value, long Comparand);
	static void* CompareExchangePointer(void* *Target, void* Value, void* Comparand);

	static long Add(long volatile *Target, long Value);
	static long Minus(long volatile *Target, long Value);

	static long Increment(long volatile *Target);
	static long Decrement(long volatile *Target);

	static long And(long volatile *Target, long Value);
	static long Or(long volatile *Target, long Value);
	static long Xor(long volatile *Target, long Value);

	static long TestBit(long volatile *Base, long Bit);
	static long TestAndSetBit(long volatile *Base, long Bit);
	static long TestAndResetBit(long volatile *Base, long Bit);
	static long TestAndChangeBit(long volatile *Base, long Bit);

	static long Test(long volatile *Target, long Value);			//����Value����Щλ�������ˣ���û���þ�Ϊ0
	static long TestAndSet(long volatile *Target, long Value);		//����������Value����Щλ
	static long TestAndReset(long volatile *Target, long Value);	//����������Value����Щλ
	static long TestAndChange(long volatile *Target, long Value);	//���������Value����Щλ
};

//////////////////////////////////////////////////////////////////////////
///Thread

struct SingleThreadTag {};
struct MultiThreadTag {};

template <typename LockType>
class ThreadingModel
{
public:
	typedef MultiThreadTag ThreadTag;

	class Lock
	{
		Lock(const Lock&);
		void operator=(const Lock&);

	public:
		explicit Lock(const ThreadingModel& host)
			: m_host(host)
		{
			m_host.m_mutex.Lock();
		}
		~Lock()
		{
			m_host.m_mutex.UnLock();
		}

	private:
		const ThreadingModel& m_host;
	};

	friend class Lock;

	ThreadingModel() {}

private:
	ThreadingModel(const ThreadingModel&);
	ThreadingModel& operator=(const ThreadingModel&);

private:
	mutable LockType    m_mutex;
};

template <>
class ThreadingModel<void>
{
public:
	typedef SingleThreadTag ThreadTag;

	struct Lock
	{
		explicit Lock(const ThreadingModel&) {}
	};

	ThreadingModel() {}

private:
	ThreadingModel(const ThreadingModel&);
	ThreadingModel& operator=(const ThreadingModel&);
};

//////////////////////////////////////////////////////////////////////////
//ģ������Ĵ���
//1���綨�����Ĺ��ܱ߽磬��Ҫ�����չ���ı߽磬�����ֲ���
//2������ͬһ������Դ����һ����
//3����Ҫ��ģ�����
//4��������Ľ����˳�˳��Ҫ�պ��෴
//
//---ģ��1<-->ģ��2<-->ģ��3
//----��-------��-------��
//����ģ��1��ģ��3�ں�ģ��2����ʱ��������ͷ��Լ��������ٵ���ģ��2��ģ��2
//����ģ��1��ģ��3�ĵ���ʱ������ʹ��ͬһ����
//////////////////////////////////////////////////////////////////////////

/*!
 *	@brief CriticalSection ����.
 *
 *	��װCriticalSection��ʵ���ٽ�������
 */
class XLIB_API CriticalSection
{
public:
	CriticalSection();
	~CriticalSection();

public:
	void Lock();
	void UnLock();

protected:
#ifdef WIN32
	CRITICAL_SECTION m_Section;
#else
	pthread_mutex_t m_Section;
#endif
};
/*!
 *	@brief Mutex ����.
 *
 *	����̵�ͬ����
 */
class XLIB_API Mutex
{
protected:
#ifdef WIN32
	HANDLE								m_hRecordData;
#else
	int									m_iRecordData;
#endif
protected:
	__inline int  inner_createkeyfromstring(const char * str);
public:
	Mutex(void);
	virtual ~Mutex();
public:
	//�������򿪡��ر�
	int  Create(const char * name);
	int  Open(const char * name);
	void Close();
protected:
	//��ȡ��д��
	void Lock();
	void UnLock();
};
/*!
 *	@brief WaitEvent ����.
 *
 *	�ȴ��¼�����Ȼ�������ڶ�����̣���������¼����ƣ�����Ϊ�˼���Linux��������Ҫ���ڽ����ڲ������̼�ʹ�õ��������������
 *  ��Ҫ������ͷǼ���������������sleep���������ܹ�������Ӧ��
 */
class XLIB_API WaitEvent
{
protected:
#ifdef WIN32
	HANDLE									m_hRecordData;
#else
	int										m_IsStartWait;
	int										m_nCount;	//	��ȫ����,����thread_signal�ջ���,�Լ�����thread_wait���źŻ��ѵĴ���
	pthread_cond_t							m_hRecordData;
	pthread_mutex_t							m_hMutex;
#endif
public:
	WaitEvent();
	virtual ~WaitEvent();
public:
	//�ȴ��¼�
	void Wait(unsigned long lMSec = 0xFFFFFFFF);
	void Lock(unsigned long lMSec = 0xFFFFFFFF);
	//�����¼�
	void Active();
	void UnLock();
};
/*!
 *	@brief Locker ����.
 *
 *	��װLocker��ʵ�ֻ������Ĳ���
 */
template<class T>
class Locker
{
public:
	Locker():m_pLock(NULL){}
	Locker(const T * pLock):m_pLock(NULL){Attach(pLock);}
	~Locker(){Detach();}

	void Attach(const T * pLock) {
		m_pLock = (T *)pLock;
		if (m_pLock != NULL) {
			m_pLock->Lock();
		}
	}
	void Detach() {
		if (m_pLock != NULL) {
			m_pLock->UnLock();
			m_pLock = NULL;
		}
	}

protected:
	T * m_pLock;
};
typedef Locker<CriticalSection> SectionLocker;

/*!
*	@brief ThreadProc ����.
*
*	�̻߳ص���������
*/
typedef unsigned int (__stdcall *pThreadFunc)(void*);
typedef void* (*thread_routine)(void*);

/*!
 *	@brief Thread ����.
 *
 *	��װThread��ʵ���߳�
 */
class XLIB_API Thread
{
public:
	static void Sleep(unsigned long lMSec);
	static unsigned long GetCurrentThreadId();
public:
	Thread();
	~Thread();

public:
#ifdef WIN32
	HANDLE operator() () {return m_hThread;}
#endif//

	bool CreateThread(pThreadFunc pFunc, void* pParam = NULL);
	void StopThread(long lTimeOut = INFINITE);

	unsigned long GetThreadId();

	bool IsNeedStop();
	bool IsRun();

	void Join();

protected:
#ifdef WIN32
	HANDLE			m_hThread;
	DWORD			m_dwThreadId;
#else
	pthread_t		m_hThread;
#endif
	bool			m_bNeedStop;
};

/*!
 *	@brief ThreadPool ��־.
 *
 *	�����̳߳ر�־
 */
enum
{
	thread_flag_none			= 0,				//!< ͬʱ���̵߳���
	thread_flag_once_call		= 0x00000001,		//!< ͬʱֻ��һ���̵߳���
};

/*!
 *	@brief MaxThreadCount.
 *
 *	����Ĭ������̳߳��߳���Ŀ
 */
#ifndef MAX_THREAD_COUNT
#define MAX_THREAD_COUNT 300
#endif//

/*!
 *	@brief Thread ����.
 *
 *	��װThread��ʵ���߳�
 */
class XLIB_API ThreadPool
{
protected:
	static int _MaxThreadCount;
public:
	void SetMaxThreadCount(int MaxThreadCount);
	int GetMaxThreadCount();
public:
	ThreadPool();
	~ThreadPool();

public:
	bool Start();
	long Register(pThreadFunc pFunc, void* pParam, long flag = thread_flag_none);
	void UnRegister(long id);
	void UnRegisterAll();
	void Stop();

protected:
	unsigned int Run(Thread *pThread);		//�����߳�
	unsigned int Idle();					//ά���߳�
	void StopAll();							//m_bStopAll �� true ʱ ֹͣ���й����߳� �ڲ�����

protected:
	bool m_bStopAll;						//�Ƿ�ֹͣ�����߳�
	struct tagThreadParam
	{
		long id;							//ע��ID
		int enter : 1;						//�������
		int flag : 31;						//ע���־
		pThreadFunc pFunc;
		void* pParam;
	};
	vector<tagThreadParam*> m_threadParams;	//����
	int m_threadParamNext;					//m_threadParams ��һ����λ��
	vector<Thread*> m_threads;				//id->�߳�
	Thread m_Idle;							//idle�߳�
	CriticalSection m_Section;				//��

	static unsigned int __stdcall s_Run(void* pParam);
	static unsigned int __stdcall s_Idle(void* pParam);
};

#ifndef USE_THREADPOOL
#define USE_THREADPOOL	1
#endif//USE_THREADPOOL

#if USE_THREADPOOL
/*!
 *	@brief ȫ���̳߳ؽӿ� ����.
 *
 *	ʵ��ȫ���̳߳�
 */
bool XLIB_API StartThreadPool();
long XLIB_API RegisterThreadPool(pThreadFunc pFunc, void* pParam, long flag = 0);
void XLIB_API UnRegisterThreadPool(long id);
void XLIB_API UnRegisterThreadPoolAll();
void XLIB_API StopThreadPool();
#endif//USE_THREADPOOL

#endif//_H_XTHREAD_H_