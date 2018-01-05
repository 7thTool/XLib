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

	static long Test(long volatile *Target, long Value);			//返回Value的哪些位被设置了，都没设置就为0
	static long TestAndSet(long volatile *Target, long Value);		//返回设置了Value的哪些位
	static long TestAndReset(long volatile *Target, long Value);	//返回重置了Value的哪些位
	static long TestAndChange(long volatile *Target, long Value);	//返回异或了Value的哪些位
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
//模块间锁的处理
//1、界定好锁的功能边界，不要随便扩展锁的边界，尽量局部锁
//2、访问同一共享资源，用一把锁
//3、不要跨模块调用
//4、多个锁的进入退出顺序要刚好相反
//
//---模块1<-->模块2<-->模块3
//----锁-------锁-------锁
//这里模块1和模块3在和模块2交互时最好能先释放自己的锁，再调用模块2，模块2
//处理模块1和模块3的调用时，必须使用同一把锁
//////////////////////////////////////////////////////////////////////////

/*!
 *	@brief CriticalSection 定义.
 *
 *	封装CriticalSection，实现临界区互斥
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
 *	@brief Mutex 定义.
 *
 *	跨进程的同步锁
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
	//创建、打开、关闭
	int  Create(const char * name);
	int  Open(const char * name);
	void Close();
protected:
	//读取、写入
	void Lock();
	void UnLock();
};
/*!
 *	@brief WaitEvent 定义.
 *
 *	等待事件，虽然可以用在多个进程（必须加入事件名称），但为了兼容Linux，该类主要用于进程内部，进程间使用的请参照其他的类
 *  主要处理激活和非激活，可以用它来替代sleep函数，它能够提早响应。
 */
class XLIB_API WaitEvent
{
protected:
#ifdef WIN32
	HANDLE									m_hRecordData;
#else
	int										m_IsStartWait;
	int										m_nCount;	//	安全计数,避免thread_signal空唤醒,以及保持thread_wait被信号唤醒的处理
	pthread_cond_t							m_hRecordData;
	pthread_mutex_t							m_hMutex;
#endif
public:
	WaitEvent();
	virtual ~WaitEvent();
public:
	//等待事件
	void Wait(unsigned long lMSec = 0xFFFFFFFF);
	void Lock(unsigned long lMSec = 0xFFFFFFFF);
	//激活事件
	void Active();
	void UnLock();
};
/*!
 *	@brief Locker 定义.
 *
 *	封装Locker，实现互斥对象的操作
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
*	@brief ThreadProc 定义.
*
*	线程回调函数定义
*/
typedef unsigned int (__stdcall *pThreadFunc)(void*);
typedef void* (*thread_routine)(void*);

/*!
 *	@brief Thread 定义.
 *
 *	封装Thread，实现线程
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
 *	@brief ThreadPool 标志.
 *
 *	定义线程池标志
 */
enum
{
	thread_flag_none			= 0,				//!< 同时多线程调用
	thread_flag_once_call		= 0x00000001,		//!< 同时只有一个线程调用
};

/*!
 *	@brief MaxThreadCount.
 *
 *	定义默认最大线程池线程数目
 */
#ifndef MAX_THREAD_COUNT
#define MAX_THREAD_COUNT 300
#endif//

/*!
 *	@brief Thread 定义.
 *
 *	封装Thread，实现线程
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
	unsigned int Run(Thread *pThread);		//工作线程
	unsigned int Idle();					//维护线程
	void StopAll();							//m_bStopAll 是 true 时 停止所有工作线程 内部调用

protected:
	bool m_bStopAll;						//是否停止所有线程
	struct tagThreadParam
	{
		long id;							//注册ID
		int enter : 1;						//进入调用
		int flag : 31;						//注册标志
		pThreadFunc pFunc;
		void* pParam;
	};
	vector<tagThreadParam*> m_threadParams;	//参数
	int m_threadParamNext;					//m_threadParams 下一调用位置
	vector<Thread*> m_threads;				//id->线程
	Thread m_Idle;							//idle线程
	CriticalSection m_Section;				//锁

	static unsigned int __stdcall s_Run(void* pParam);
	static unsigned int __stdcall s_Idle(void* pParam);
};

#ifndef USE_THREADPOOL
#define USE_THREADPOOL	1
#endif//USE_THREADPOOL

#if USE_THREADPOOL
/*!
 *	@brief 全局线程池接口 定义.
 *
 *	实现全局线程池
 */
bool XLIB_API StartThreadPool();
long XLIB_API RegisterThreadPool(pThreadFunc pFunc, void* pParam, long flag = 0);
void XLIB_API UnRegisterThreadPool(long id);
void XLIB_API UnRegisterThreadPoolAll();
void XLIB_API StopThreadPool();
#endif//USE_THREADPOOL

#endif//_H_XTHREAD_H_