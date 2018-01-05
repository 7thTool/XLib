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

#ifndef _H_XSOCKETEX_H_
#define _H_XSOCKETEX_H_

#include "XSocket.h"
#include "XThread.h"

#ifndef USE_SOCKET_THREADPOOL
#define USE_SOCKET_THREADPOOL	USE_THREADPOOL
#endif//USE_SOCKET_THREADPOOL

#define MAX_SOCKET_COUNT	(u_short)(-1)

/*!
 *	@brief Socket 角色定义.
 *
 *	定义Socket在网络中扮演的角色
 */
enum
{
	SOCKET_ROLE_NONE = 0,		//!< 未知
	SOCKET_ROLE_LISTEN,			//!< Server端监听Socket	
	SOCKET_ROLE_WORK,			//!< Server端Accept到的Socket或者UDP套接字
	SOCKET_ROLE_CONNECT,		//!< Client端连接Socket
};

#define FD_IDLE_BIT 15
#define FD_IDLE     (1 << FD_IDLE_BIT)

/*!
 *	@brief 可伸缩的Socket封装.
 *
 *	SocketEx定义了可伸缩Socket的接口和基本实现
 */
class XLIB_API SocketEx : public Socket
{
public:
	static unsigned long Ip2N(const char* Ip);
	static const char* N2Ip(unsigned long Ip);
	static const char* Url2Ip(const char* Url);
	
public:
	SocketEx();
	virtual ~SocketEx();

	//只需重载Attach，因为Create和Detach都会调用Attach
	SOCKET Create(int nSockAf = AF_INET, int nSockType = SOCK_STREAM);
	virtual SOCKET Attach(SOCKET Sock, int Role = SOCKET_ROLE_NONE);
	virtual SOCKET Detach();

	int ShutDown(int nHow = Both);
	int Close();

	SOCKET Accept(SOCKADDR* lpSockAddr, int* lpSockAddrLen);
	//int Bind(const char* lpszHostAddress, unsigned short nHostPort, PPROXYINFO pProxy);
	int Bind(const char* lpszHostAddress, unsigned short nHostPort);
	int Bind(const SOCKADDR* lpSockAddr, int nSockAddrLen);
	//int Connect(const char* lpszHostAddress, unsigned short nHostPort, PPROXYINFO pProxy);
	int Connect(const char* lpszHostAddress, unsigned short nHostPort);
	int Connect(const SOCKADDR* lpSockAddr, int nSockAddrLen);
	int Listen(int nConnectionBacklog = 5);

	int Send(const char* lpBuf, int nBufLen, int nFlags = 0);
	int Receive(char* lpBuf, int nBufLen, int nFlags = 0);
	int SendTo(const char* lpBuf, int nBufLen, const SOCKADDR* lpSockAddr, int nSockAddrLen, int nFlags = 0);
	int ReceiveFrom(char* lpBuf, int nBufLen, SOCKADDR* lpSockAddr, int* lpSockAddrLen, int nFlags = 0);

	int Role();
#define IsNoneRole()		Role()==SOCKET_ROLE_NONE
#define IsConnectSocket()	Role()==SOCKET_ROLE_CONNECT
#define IsListenSocket()	Role()==SOCKET_ROLE_LISTEN
#define IsWorkSocket()		Role()==SOCKET_ROLE_WORK

	//int Select(int Event);
	//int HasSelect(int Event);
	void Trigger(int Event);
protected:
	bool TryEvent(int Event);
	void AddEvent(int Event);
	void RemoveEvent(int Event);
	bool IsEventOf(int Event);

protected:
	/*!
	 *	@brief 通知套接字扮演指定角色.
	 *
	 *	收到这个回调，说明Socket完成了初始化了，并开始进入指定角色
	 *	@param nRole indicates this socket to play as a role (listen / accept/ connect)
	 *	SOCKET_ROLE_NONE
	 *	SOCKET_ROLE_LISTEN
	 *	SOCKET_ROLE_WORK
	 *	SOCKET_ROLE_CONNECT
	 *	other value indecate unknown error.
	 */
	virtual void OnRole(int nRole);

	/*!
	 *	@brief 通知套接字无可操作状态，正在空闲或者等待中.
	 *
	 *	OnIdle函数是后台在计算空闲的情况，给SocketEx用于做一些空闲处理动作，比如检查超时
	 *	、清理垃圾数据等等
	 *	@param nErrorCode 标示进入空闲时间，可以使用GetTickCount()-nErrorCode计算OnIdle耗时
	 */
	virtual void OnIdle(int nErrorCode);

	/*!
	 *	@brief 通知套接字可以调用Send发送数据.
	 *
	 *	
	 *	Notifies a socket that it can send data by calling Send.
	 *	@param nErrorCode The most recent error on a socket. 
	 *	The following error codes apply to the OnSend member function:
	 *	0					The function executed successfully.
	 *	ENETDOWN			The Windows Sockets implementation detected that the network subsystem failed.
	 */
	virtual void OnSend(int nErrorCode);

	virtual void OnSended(const char* lpBuf, int nBufLen, int nFlags);

	virtual void OnSendedTo(const char* lpBuf, int nBufLen, const SOCKADDR* lpSockAddr, int nSockAddrLen, int nFlags);
	
	/*!
	 *	@brief 通知套接字有数据可以通过调用Receive读取.
	 *
	 *	
	 *	Notifies a this socket that there is data to be retrieved by calling Receive.
	 *	@param nErrorCode The most recent error on a socket. 
	 *	The following error codes apply to the OnReceive member function:
	 *	0					The function executed successfully.
	 *	ENETDOWN			The Windows Sockets implementation detected that the network subsystem failed.
	 */
	virtual void OnReceive(int nErrorCode);

	virtual void OnReceived(const char* lpBuf, int nBufLen, int nFlags);

	virtual void OnReceivedFrom(const char* lpBuf, int nBufLen, const SOCKADDR* lpSockAddr, int nSockAddrLen, int nFlags);
	
	/*!
	 *	@brief 通知正在接收数据的套接字有带外数据，通常是紧急数据 要读取.
	 *
	 *	
	 *	Notifies a receiving socket that there is out-of-band data to be read on the socket, usually an urgent message.
	 *	@param nErrorCode The most recent error on a socket. 
	 *	The following error codes apply to the OnOutOfBandData member function:
	 *	0					The function executed successfully.
	 *	ENETDOWN			The Windows Sockets implementation detected that the network subsystem failed.
	 */
	virtual void OnOutOfBandData(int nErrorCode);

	virtual void OnOutOfBandedData(const char* lpBuf, int nBufLen, int nFlags);

	/*!
	 *	@brief 通知正在监听的服务器套接字有一个连接需要调用Accept接收连接.
	 *
	 *	
	 *	Notifies a listening socket that it can accept pending connection requests by calling Accept.
	 *	@param nErrorCode The most recent error on a socket. 
	 *	The following error codes applies to the OnAccept member function:
	 *	0					The function executed successfully.
	 *	ENETDOWN			The Windows Sockets implementation detected that the network subsystem failed.
	 */
	virtual void OnAccept(int nErrorCode);

	/*!
	 *	@brief 通知正在连接的客户端套接字连接建立完成，可能成功或者失败.
	 *
	 *	
	 *	Notifies a connecting socket that the connection attempt is complete, whether successfully or in error.
	 *	@param nErrorCode The most recent error on a socket. 
	 *	The following error codes apply to the OnConnect member function:
	 *	0					The function executed successfully.
	 *	WSAEADDRINUSE		The specified address is already in use.
	 *	WSAEADDRNOTAVAIL	The specified address is not available from the local machine.
	 *	WSAEAFNOSUPPORT		Addresses in the specified family cannot be used with this socket.
	 *	WSAECONNREFUSED		The attempt to connect was forcefully rejected.
	 *	WSAEDESTADDRREQ		A destination address is required.
	 *	WSAEFAULT			The lpSockAddrLen argument is incorrect.
	 *	WSAEINVAL			The socket is already bound to an address.
	 *	WSAEISCONN			The socket is already connected.
	 *	WSAEMFILE			No more file descriptors are available.
	 *	WSAENETUNREACH		The network cannot be reached from this host at this time.
	 *	WSAENOBUFS			No buffer space is available. The socket cannot be connected.
	 *	WSAENOTCONN			The socket is not connected.
	 *	WSAENOTSOCK			The descriptor is a file, not a socket.
	 *	WSAETIMEDOUT		The attempt to connect timed out without establishing a connection.
	 */
	virtual void OnConnect(int nErrorCode);

	/*!
	 *	@brief 通知套接字连接已经关闭.
	 *
	 *	
	 *	Notifies a socket that the socket connected to it has closed.
	 *	@param nErrorCode The most recent error on a socket. 
	 *	The following error codes apply to the OnClose member function:
	 *	0					The function executed successfully.
	 *	ENETDOWN			The Windows Sockets implementation detected that the network subsystem failed.
	 *	WSAECONNRESET		The connection was reset by the remote side.
	 *	WSAECONNABORTED		The connection was aborted due to timeout or other failure.
	 */
	virtual void OnClose(int nErrorCode);

protected:
	long m_Role;
#ifdef WIN32
	/*volatile */long m_Event;
#else
	volatile long m_Event;
#endif//
	//long m_TriggerEvent;

private:
	SocketEx(const SocketEx& Sock) {};
	void operator=(const SocketEx& Sock) {};
};

/*!
 *	@brief SocketExImpl 模板定义.
 *
 *	封装SocketEx，一般用于SocketEx最终实现的包装
 */
template<class T, class TBase = SocketEx>
class SocketExImpl : public TBase
{
	typedef TBase Base;
public:
	SocketExImpl():Base()
	{

	}

	SocketExImpl(int nMaxSockCount):Base(nMaxSockCount)
	{

	}

	SocketExImpl(int nMaxThreadCount, int nMaxSockCount):Base(nMaxThreadCount,nMaxSockCount)
	{

	}

protected:
	//
	virtual void OnClose(int nErrorCode)
	{
		T* pT = static_cast<T*>(this);

		Base::OnClose(nErrorCode);

		pT->Close();
	}
};

/*!
 *	@brief SocketWorkImpl 模板定义.
 *
 *	扩展SocketWorkImpl，实现对工作线程的包装
 */
class SockWorkBase {};
template<class T, class TBase = SockWorkBase>
class SocketWorkImpl : public TBase
{
	typedef TBase Base;
public:
	SocketWorkImpl(int nMaxThreadCount):Base()
	{
		int i;
		int nWorkThread = nMaxThreadCount;
		PRINTF("work thread count is [%d].\n", nWorkThread);

#if USE_SOCKET_THREADPOOL
		m_WorkThreads.resize(nWorkThread,0);
		for (i=0;i<nWorkThread;i++)
		{
			m_WorkThreads[i] = RegisterThreadPool(WorkThread, (void*)static_cast<T*>(this));
		}
#else
		m_WorkThreads.resize(nWorkThread);
		for (i=0;i<nWorkThread;i++)
		{
			m_WorkThreads[i].CreateThread(WorkThread, (void*)static_cast<T*>(this));
		}
#endif//
	}

	SocketWorkImpl(int nMaxThreadCount, int nMaxSockCount):Base(nMaxSockCount)
	{
		int i;
		int nWorkThread = nMaxThreadCount;
		PRINTF("work thread count is [%d] max socket count is [%d].\n", nWorkThread, nMaxSockCount);

#if USE_SOCKET_THREADPOOL
		m_WorkThreads.resize(nWorkThread,0);
		for (i=0;i<nWorkThread;i++)
		{
			m_WorkThreads[i] = RegisterThreadPool(WorkThread, (void*)static_cast<T*>(this));
		}
#else
		m_WorkThreads.resize(nWorkThread);
		for (i=0;i<nWorkThread;i++)
		{
			m_WorkThreads[i].CreateThread(WorkThread, (void*)static_cast<T*>(this));
		}
#endif//
	}

	~SocketWorkImpl()
	{
		int i;

#if USE_SOCKET_THREADPOOL
		for (i=0;i<m_WorkThreads.size();i++)
		{
			UnRegisterThreadPool(m_WorkThreads[i]);
			m_WorkThreads[i] = 0;
		}
		m_WorkThreads.clear();
#else
		for (i=0;i<m_WorkThreads.size();i++)
		{
			m_WorkThreads[i].StopThread();
		}
		m_WorkThreads.clear();
#endif//
	}

protected:
	//
#if USE_SOCKET_THREADPOOL
	std::vector<long> m_WorkThreads;
	static unsigned int __stdcall WorkThread(void* pParam)
	{
		T* pThis = (T*)pParam;
		ASSERT(pThis);
		return pThis->OnWork();
		return 0;
	}
#else
	std::vector<Thread> m_WorkThreads;
	static unsigned int __stdcall WorkThread(void* pParam)
	{
		T* pThis = (T*)pParam;
		ASSERT(pThis);
		int i=0;
		unsigned long ThreadId = Thread::GetCurrentThreadId();
		const std::vector<Thread> & Threads = pThis->m_WorkThreads;
		for (i=0;i<Threads.size();i++)
		{
			if (Threads[i].GetThreadId()==ThreadId) {
				break;
			}
		}
		ASSERT(i<Threads.size());
		while (!Threads[i].IsNeedStop())
		{
			Thread::Sleep(1);
			pThis->OnWork();
		}
		return 0;
	}
#endif//
};

/*!
 *	@brief SocketWorkImpl 模板定义.
 *
 *	扩展SocketExImpl，实现对工作线程的包装
 */
template<class T, class TBase = SocketEx>
class SocketExWorkImpl 
	: public SocketWorkImpl<T>
	, public SocketExImpl<T,TBase>
{
	typedef SocketWorkImpl<T> WorkBase;
	typedef SocketExImpl<T,TBase> SockBase;
public:
	SocketExWorkImpl(int nMaxThreadCount):WorkBase(nMaxThreadCount),SockBase()
	{
		
	}

	SocketExWorkImpl(int nMaxThreadCount, int nMaxSockCount):WorkBase(nMaxThreadCount),SockBase(nMaxSockCount)
	{
		
	}
};

/*!
 *	@brief ConnectSocket 模板定义.
 *
 *	封装ConnectSocket，适用于客户端连接Socket
 */
template<class TBase = SocketEx>
class ConnectSocket : public TBase
{
	typedef ConnectSocket<TBase> This;
	typedef TBase Base;
protected:
	bool m_bConnected;
	unsigned long m_ConnectTime;
	unsigned long m_ConnectTimeOut;

public:
	ConnectSocket():Base(), m_bConnected(false), m_ConnectTime(0), m_ConnectTimeOut(0) {}
	//virtual ~SocketConnectTimeOut() {}

	int Close()
	{
		int rlt = Base::Close();
		m_bConnected = false;
		m_ConnectTime = 0;
		return rlt;
	}

	bool IsConnecting()
	{
		if(IsConnected()) {
			return false;
		}
		return m_ConnectTime;
	}

	bool IsConnected()
	{
		return m_bConnected;
	}

	void SetConnectTimeOut(unsigned long TimeOut)
	{
		m_ConnectTimeOut = TimeOut;
	}

	unsigned long GetConnectTimeOut()
	{
		return m_ConnectTimeOut;
	}

	bool IsConnectTimeOut() 
	{ 
		if(m_ConnectTimeOut && (GetTickCount() >= (m_ConnectTime + m_ConnectTimeOut))) {
			return true;
		}
		return false;
	}

	unsigned long GetConnectTime()
	{
		return m_ConnectTime;
	}

protected:
	virtual void OnIdle(int nErrorCode)
	{
		Base::OnIdle(nErrorCode);

		//ASSERT(IsConnectSocket());
		if(m_ConnectTimeOut && IsEventOf(FD_CONNECT)) {
			if(IsConnectTimeOut()) {
				OnConnect(ETIMEDOUT);
			}
		}
	}

	virtual void OnRole(int nRole)
	{
		//ASSERT(nRole==SOCKET_ROLE_CONNECT);
		m_ConnectTime = GetTickCount();

		Base::OnRole(nRole);
	}

	virtual void OnConnect(int nErrorCode)
	{
		if(!nErrorCode) {
			m_bConnected = true;
			m_ConnectTime = GetTickCount() - m_ConnectTime; //记住连接耗时
		}

		Base::OnConnect(nErrorCode);
	}
};

/*!
 *	@brief WorkSocket 模板定义.
 *
 *	封装WorkSocket，适用于服务端工作Socket
 */
template<class TBase = SocketEx>
class WorkSocket : public TBase
{
	typedef TBase Base;
public:
	WorkSocket():Base()
	{

	}
	virtual ~WorkSocket()
	{

	}
};

/*!
 *	@brief ListenSocket 模板定义.
 *
 *	封装ListenSocket，适用于服务端监听Socket
 */
template<class TBase = SocketEx>
class ListenSocket : public TBase
{
	typedef TBase Base;
public:
	ListenSocket():Base()
	{

	}
	virtual ~ListenSocket()
	{

	}

protected:
	virtual void OnAddPeer(SocketEx* pSocket)
	{
		
	}

	virtual void OnRemovePeer(SocketEx* pSocket)
	{
		
	}
};

//////////////////////////////////////////////////////////////////////////

//DECLARE_HANDLE(HSOCKEX);		// An HSOCK	Handle

//////////////////////////////////////////////////////////////////////////

/*!
 *	@brief SelectSocket 模板定义.
 *
 *	封装SelectSocket，实现对select模型管理一个客户端连接Socket
 */
template<class TBase = SocketEx>
class SelectSocket : public TBase
{
	typedef TBase Base;
public:
	SelectSocket() : Base()
	{
#if USE_SOCKET_THREADPOOL
		m_WorkThread = RegisterThreadPool(WorkThread, (void*)this);

		m_WriteThread = RegisterThreadPool(WriteThread, (void*)this);
		m_ReadThread = RegisterThreadPool(ReadThread, (void*)this);
#else
		m_WorkThread.CreateThread(WorkThread, (void*)this);

		m_WriteThread.CreateThread(WriteThread, (void*)this);
		m_ReadThread.CreateThread(ReadThread, (void*)this);
#endif//
	}
	virtual ~SelectSocket() 
	{
		Close();
#if USE_SOCKET_THREADPOOL
		UnRegisterThreadPool(m_WriteThread);
		m_WriteThread = 0;
		UnRegisterThreadPool(m_ReadThread);
		m_ReadThread = 0;

		UnRegisterThreadPool(m_WorkThread);
		m_WorkThread = 0;
#else
		m_WriteThread.StopThread();
		m_ReadThread.StopThread();
		m_WorkThread.StopThread();
#endif//
	}

protected:
	//
	int OnWorkHandle()
	{
		if(!IsSocket()) {
			return 0;
		}

		unsigned long nTickCount = GetTickCount();

		while(1) {

			if (!IsSocket()) {
				break;
			}

			int nfds = 0;
			int maxfds = 0;
			fd_set exceptfds;
			FD_ZERO(&exceptfds);
			FD_SET(Base::m_Sock, &exceptfds);
			struct timeval tv = {0, 0};
			if(IsEventOf(FD_CONNECT)) {
				fd_set writefds;
				FD_ZERO(&writefds);
				maxfds = Base::m_Sock+1;
				FD_SET(Base::m_Sock, &writefds);
				nfds = select(maxfds, NULL, &writefds, &exceptfds, &tv);
				if (nfds > 0) {
					if (FD_ISSET(Base::m_Sock,&writefds)) {
						int nErrorCode = 0;
						GetSockOpt(SOL_SOCKET, SO_ERROR, (char *)&nErrorCode, sizeof(nErrorCode));	
						OnConnect(nErrorCode);
						if(nErrorCode==0) {
							RemoveEvent(FD_CONNECT);
						}
					}
				}
			} else {
				nfds = select(maxfds, NULL, NULL, &exceptfds, &tv);
			}
			if (nfds > 0) {
				if (FD_ISSET(Base::m_Sock,&exceptfds)) {
					int nErrorCode = GetLastError();
					/*GetSockOpt(SOL_SOCKET, SO_ERROR, (char *)&nErrorCode, sizeof(nErrorCode));*/
					if (nErrorCode==0) {
						nErrorCode = ENETDOWN;
						SetLastError(nErrorCode);
					}
					OnClose(nErrorCode);
				}
			} else if (nfds == 0) {
				OnIdle(GetTickCount());
			} else {
				if (IsSocket()) {
					OnClose(GetLastError());
				}
			}

			if (!IsSocket()) {
				break;
			}

			unsigned long nLastTickCount = GetTickCount();
			if ((nLastTickCount-nLastTickCount)<20) {
				Thread::Sleep(1);
				nTickCount = GetTickCount();
			} else {
				nTickCount = nLastTickCount;
			}
		}
		return 0;
	}

	int OnWriteHandle()
	{
		if(!IsSocket()) {
			return 0;
		}

		if(IsNoneRole()) {
			return 0;
		}

		if (IsEventOf(FD_CONNECT)) {
			//连接中...
			return 0;
		}
		
		int nfds = 0;
		int maxfds = 0;
		fd_set writefds;
		FD_ZERO(&writefds);
		maxfds = Base::m_Sock+1;
		FD_SET(Base::m_Sock, &writefds);
		struct timeval tv = {0, 50*1000};
		nfds = select(maxfds, NULL, &writefds, NULL, &tv);
		if (nfds > 0) {
			if (FD_ISSET(Base::m_Sock,&writefds)) {
				if (IsEventOf(FD_CONNECT)) {
					//连接中...
				} else {
					//AddEvent(FD_WRITE);
					OnSend(0);
					//RemoveEvent(FD_WRITE);
				}
			}
		}

		return 0;
	}

	int OnReadHandle()
	{
		if(!IsSocket()) {
			return 0;
		}

		if(IsNoneRole()) {
			return 0;
		}

		if (IsEventOf(FD_CONNECT)) {
			//连接中...
			return 0;
		}

		int nfds = 0;
		int maxfds = 0;
		fd_set readfds;
		FD_ZERO(&readfds);
		maxfds = Base::m_Sock+1;
		FD_SET(Base::m_Sock, &readfds);
		struct timeval tv = {0, 50*1000};
		nfds = select(maxfds, &readfds, NULL, NULL, &tv);
		if (nfds > 0) {
			if (FD_ISSET(Base::m_Sock,&readfds)) {
				int err = 0;
				GetSockOpt(SOL_SOCKET, SO_OOBINLINE, &err, sizeof(err));
				if (err) {
					//AddEvent(FD_OOB);
					OnOutOfBandData(0);
					//RemoveEvent(FD_OOB);
				} else {
					//AddEvent(FD_READ);
					OnReceive(0);
					//RemoveEvent(FD_READ);
				}
			}
		}

		return 0;
	}

protected:
#if USE_SOCKET_THREADPOOL
	long m_WorkThread;
	long m_WriteThread;
	long m_ReadThread;
	static unsigned int __stdcall WorkThread(void* pParam)
	{
		SelectSocket* pThis = (SelectSocket*)pParam;
		ASSERT(pThis);
		return pThis->OnWorkHandle();
		return 0;
	}
	static unsigned int __stdcall WriteThread(void* pParam)
	{
		SelectSocket* pThis = (SelectSocket*)pParam;
		ASSERT(pThis);
		return pThis->OnWriteHandle();
		return 0;
	}
	static unsigned int __stdcall ReadThread(void* pParam)
	{
		SelectSocket* pThis = (SelectSocket*)pParam;
		ASSERT(pThis);
		return pThis->OnReadHandle();
		return 0;
	}
#else
	Thread m_WorkThread;
	Thread m_WriteThread;
	Thread m_ReadThread;
	static unsigned int __stdcall WorkThread(void* pParam)
	{
		SelectSocket* pThis = (SelectSocket*)pParam;
		ASSERT(pThis);
		while (!pThis->m_WorkThread.IsNeedStop())
		{
			Thread::Sleep(1);
			pThis->OnWorkHandle();
		}
		return 0;
	}
	static unsigned int __stdcall WriteThread(void* pParam)
	{
		SelectSocket* pThis = (SelectSocket*)pParam;
		ASSERT(pThis);
		while (!pThis->m_WriteThread.IsNeedStop())
		{
			Thread::Sleep(1);
			pThis->OnWriteHandle();
		}
		return 0;
	}
	static unsigned int __stdcall ReadThread(void* pParam)
	{
		SelectSocket* pThis = (SelectSocket*)pParam;
		ASSERT(pThis);
		while (!pThis->m_ReadThread.IsNeedStop())
		{
			Thread::Sleep(1);
			pThis->OnReadHandle();
		}
		return 0;
	}
#endif//
};

template<class TSocket, u_short uFD_SETSize>
class SelectSet;
template<class TSocket, u_short uFD_SETSize>
class SelectManager;

#ifndef USE_SELECTSET_WORKHANDLE
#define USE_SELECTSET_WORKHANDLE 0
#endif//

/*!
 *	@brief SelectSet 模板定义.
 *
 *	封装SelectSet，实现对select模型封装，最多管理uFD_SETSize数Socket
 */
template<class TSocket = SocketEx, u_short uFD_SETSize = FD_SETSIZE>
class SelectSet
{
	friend class SelectManager<TSocket,uFD_SETSize>;
protected:
	TSocket* m_pSockets[uFD_SETSize];
#if !USE_SELECTSET_WORKHANDLE
	long m_nIdleIndex;
#endif//
	CriticalSection m_Section;
public:
	SelectSet()
	{
		Create();
	}
	virtual ~SelectSet() 
	{
		Close();
	}

	bool Create()
	{
		memset(m_pSockets,0,sizeof(m_pSockets));
#if USE_SOCKET_THREADPOOL
#if USE_SELECTSET_WORKHANDLE
		m_WorkThread = RegisterThreadPool(WorkThread, (void*)this);
#else
		m_nIdleIndex = 0;
#endif//
		m_WriteThread = RegisterThreadPool(WriteThread, (void*)this);
		m_ReadThread = RegisterThreadPool(ReadThread, (void*)this);
#else
#if USE_SELECTSET_WORKHANDLE
		m_WorkThread.CreateThread(WorkThread, (void*)this);
#else
		m_nIdleIndex = 0;
#endif//
		m_WriteThread.CreateThread(WriteThread, (void*)this);
		m_ReadThread.CreateThread(ReadThread, (void*)this);
#endif//
		return true;
	}

	void Close(bool bClose = false)
	{
#if USE_SOCKET_THREADPOOL
#if USE_SELECTSET_WORKHANDLE
		if (m_WorkThread) {
			UnRegisterThreadPool(m_WorkThread);
			m_WorkThread = 0;
		}
#else
		//m_nIdleIndex = 0;
#endif//
		if (m_ReadThread) {
			UnRegisterThreadPool(m_ReadThread);
			m_ReadThread = 0;
		}
		if (m_WriteThread) {
			UnRegisterThreadPool(m_WriteThread);
			m_WriteThread = 0;
		}
#else
#if USE_SELECTSET_WORKHANDLE
		if(m_WorkThread.IsRun()) {
			m_WorkThread.StopThread();
		}
#else
		//m_nIdleIndex = 0;
#endif//
		if(m_ReadThread.IsRun()) {
			m_ReadThread.StopThread();
		}
		if(m_WriteThread.IsRun()) {
			m_WriteThread.StopThread();
		}
#endif//
		Clear(bClose);
	}

	bool AddSocket(TSocket* pSocket)
	{
		SectionLocker Lock(&m_Section);
		int i;
		for (i=0;i<uFD_SETSize;i++)
		{
			if(m_pSockets[i]==NULL) {
				if (pSocket) {
					m_pSockets[i] = pSocket;
					Lock.Detach();
					OnAddSocket(pSocket);
					return true;
				} else {
					//测试可不可以增加Socket，返回true表示可以增加
					return true;
				}
				break;
			}
		}
		return false;
	}

	bool RemoveSocket(TSocket* pSocket)
	{
		ASSERT(pSocket);
		//SectionLocker Lock(&m_Section);
		int i;
		for (i=0;i<uFD_SETSize;i++)
		{
			if(m_pSockets[i]==pSocket) {
				SectionLocker Lock(&m_Section);
				TSocket* pTempSocket = m_pSockets[i];
				if (pTempSocket) {
					m_pSockets[i] = NULL;
					Lock.Detach();
					OnRemoveSocket(pSocket);
					return true;
				} else {
					return true;
				}
				break;
			}
		}
		return false;
	}

	bool RemoveInvalidSocket(TSocket* & pSocket)
	{
		//SectionLocker Lock(&m_Section);
		int i;
		for (i=0;i<uFD_SETSize;i++)
		{
			if(m_pSockets[i]) {
				SectionLocker Lock(&m_Section);
				TSocket* pTempSocket = m_pSockets[i];
				if (pTempSocket) {
					if (!m_pSockets[i]->IsSocket()) {
						pSocket = m_pSockets[i];
						m_pSockets[i] = NULL;
						Lock.Detach();
						OnRemoveSocket(pSocket);
						return true;
						break;
					}
				}
			}
		}
		return false;
	}

	void Clear(bool bClose = false)
	{
		int i;
		for (i=0;i<uFD_SETSize;i++)
		{
			SectionLocker Lock(&m_Section);
			if (m_pSockets[i]) {
				TSocket* pSocket = m_pSockets[i];
				m_pSockets[i] = NULL;
				Lock.Detach();
				if (pSocket->IsSocket()) {
					if (bClose) {
						pSocket->OnClose(0);
					}
				}
				OnRemoveSocket(pSocket);
			}
		}
	}

protected:
	virtual void OnAddSocket(TSocket* pSocket)
	{

	}

	virtual void OnRemoveSocket(TSocket* pSocket)
	{

	}

	virtual void OnIdle(int nErrorCode)
	{
		int i;
		for (i=0; i<uFD_SETSize; i++)
		{
			if (m_pSockets[i]) {
				//SectionLocker Lock(&m_Section);
				TSocket* pTempSocket = m_pSockets[i];
				//Lock.Detach();
				if (pTempSocket) {
					if (!pTempSocket->IsSocket()) {
						//
					} else {
						pTempSocket->OnIdle(GetTickCount());
					}
				}
			}
		}
	}


	int OnWorkHandle()
	{
		SectionLocker Lock(&m_Section);
#if !USE_SELECTSET_WORKHANDLE
		if (m_nIdleIndex!=0) {
			return 0;
		} 
		m_nIdleIndex = 1;
#endif//
		int i = 0;
		int nfds = 0;
		int maxfds = 0;
		fd_set exceptfds;
		FD_ZERO(&exceptfds);
		struct timeval tv = {0, 0};
		for (i=0; i<uFD_SETSize; ++i)
		{
			if (m_pSockets[i]) {
				if(m_pSockets[i]->IsSocket()) {
					if(maxfds<(m_pSockets[i]->m_Sock+1)) {
						maxfds = (m_pSockets[i]->m_Sock+1);
					}
					FD_SET(m_pSockets[i]->m_Sock, &exceptfds);
				}
			}
		}
		Lock.Detach();
		if (maxfds > 0) {
			nfds = select(maxfds, NULL, NULL, &exceptfds, &tv);
			if(nfds > 0) {
				for (i=0; i<uFD_SETSize; ++i)
				{
					if (m_pSockets[i]) {
						Lock.Attach(&m_Section);
						TSocket* pSocket = m_pSockets[i];
						Lock.Detach();
						if (pSocket) {
							if (FD_ISSET(pSocket->m_Sock,&exceptfds)) {
								int nErrorCode = pSocket->GetLastError();
								/*int nOptLen = sizeof(nErrorCode);
								pSocket->GetSockOpt(SOL_SOCKET, SO_ERROR, (char *)&nErrorCode, nOptLen);*/
								if (nErrorCode==0) {
									nErrorCode = ENETDOWN;
									pSocket->SetLastError(nErrorCode);
								}
								pSocket->OnClose(nErrorCode);
							}
						}
					}
				}
			} else {
				OnIdle(GetTickCount());
			}
		} else {
			OnIdle(GetTickCount());
		}
#if !USE_SELECTSET_WORKHANDLE
		m_nIdleIndex = 0;
#endif//
		return 0;
	}
	int OnWriteHandle()
	{
		int i = 0;
		int nfds = 0;
		int maxfds = 0;
		fd_set writefds;
		FD_ZERO(&writefds);
		struct timeval tv = {0, 50*1000};
		SectionLocker Lock(&m_Section);
		for (i=0; i<uFD_SETSize; ++i)
		{
			if (m_pSockets[i]) {
				if(m_pSockets[i]->IsSocket()) {
					if(maxfds<(m_pSockets[i]->m_Sock+1)) {
						maxfds = (m_pSockets[i]->m_Sock+1);
					}
					FD_SET(m_pSockets[i]->m_Sock, &writefds);
				}
			}
		}
		Lock.Detach();
		if (maxfds > 0) {
			nfds = select(maxfds, NULL, &writefds, NULL, &tv);
			if(nfds > 0) {
				for (i=0; i<uFD_SETSize; ++i)
				{
					if (m_pSockets[i]) {
						//Lock.Attach(&m_Section);
						TSocket* pSocket = m_pSockets[i];
						//Lock.Detach();
						if (pSocket) {
							if (FD_ISSET(pSocket->m_Sock,&writefds)) {
								if (pSocket->IsEventOf(FD_CONNECT)) {
									int nErrorCode = 0;
									pSocket->GetSockOpt(SOL_SOCKET, SO_ERROR, &nErrorCode, sizeof(nErrorCode));
									pSocket->OnConnect(nErrorCode);
									if(nErrorCode == 0)
										pSocket->RemoveEvent(FD_CONNECT);
								} else {
									//pSocket->AddEvent(FD_WRITE);
									pSocket->OnSend(0);
									//pSocket->RemoveEvent(FD_WRITE);
								}
							}
						}
					}
				}
			}
		}
		return 0;
	}

	int OnReadHandle()
	{
		int i = 0;
		int nfds = 0;
		int maxfds = 0;
		fd_set readfds;
		FD_ZERO(&readfds);
		struct timeval tv = {0, 50*1000};
		SectionLocker Lock(&m_Section);
		for (i=0; i<uFD_SETSize; ++i)
		{
			if (m_pSockets[i]) {
				if(m_pSockets[i]->IsSocket()) {
					if(maxfds<(m_pSockets[i]->m_Sock+1)) {
						maxfds = (m_pSockets[i]->m_Sock+1);
					}
					FD_SET(m_pSockets[i]->m_Sock, &readfds);
				}
			}
		}
		Lock.Detach();
		if (maxfds > 0) {
			nfds = select(maxfds, &readfds, NULL, NULL, &tv);
			if(nfds > 0) {
				for (i=0; i<uFD_SETSize; ++i)
				{
					if (m_pSockets[i]) {
						//Lock.Attach(&m_Section);
						TSocket* pSocket = m_pSockets[i];
						//Lock.Detach();
						if (pSocket) {
							if (FD_ISSET(pSocket->m_Sock,&readfds)) {
								if (pSocket->IsEventOf(FD_ACCEPT)) {
									pSocket->OnAccept(0);
								} else {
									int err = 0;
									pSocket->GetSockOpt(SOL_SOCKET, SO_OOBINLINE, &err, sizeof(err));
									if (err) {
										//pSocket->AddEvent(FD_OOB)
										pSocket->OnOutOfBandData(0);
										//pSocket->RemoveEvent(FD_OOB);
									} else {
										//pSocket->AddEvent(FD_READ)
										pSocket->OnReceive(0);
										//pSocket->RemoveEvent(FD_READ);
									}
								}
							}
						}
					}
				}
			}
		}
		return 0;
	}

protected:
#if USE_SOCKET_THREADPOOL
#if USE_SELECTSET_WORKHANDLE
	long m_WorkThread;
#endif//
	long m_WriteThread;
	long m_ReadThread;
#if USE_SELECTSET_WORKHANDLE
	static unsigned int __stdcall WorkThread(void* pParam)
	{
		SelectSet* pThis = (SelectSet*)pParam;
		ASSERT(pThis);
		return pThis->OnWorkHandle();
		return 0;
	}
#endif//
	static unsigned int __stdcall WriteThread(void* pParam)
	{
		SelectSet* pThis = (SelectSet*)pParam;
		ASSERT(pThis);
		return pThis->OnWriteHandle();
		return 0;
	}
	static unsigned int __stdcall ReadThread(void* pParam)
	{
		SelectSet* pThis = (SelectSet*)pParam;
		ASSERT(pThis);
		return pThis->OnReadHandle();
		return 0;
	}
#else
#if USE_SELECTSET_WORKHANDLE
	Thread m_WorkThread;
#endif//
	Thread m_WriteThread;
	Thread m_ReadThread;
#if USE_SELECTSET_WORKHANDLE
	static unsigned int __stdcall WorkThread(void* pParam)
	{
		SelectSet* pThis = (SelectSet*)pParam;
		ASSERT(pThis);
		while (!pThis->m_WorkThread.IsNeedStop())
		{
			pThis->OnWorkHandle();
		}
		return 0;
	}
#endif//
	static unsigned int __stdcall WriteThread(void* pParam)
	{
		SelectSet* pThis = (SelectSet*)pParam;
		ASSERT(pThis);
		while (!pThis->m_WriteThread.IsNeedStop())
		{
			pThis->OnWriteHandle();
			Thread::Sleep(1);
		}
		return 0;
	}
	static unsigned int __stdcall ReadThread(void* pParam)
	{
		SelectSet* pThis = (SelectSet*)pParam;
		ASSERT(pThis);
		while (!pThis->m_ReadThread.IsNeedStop())
		{
			pThis->OnReadHandle();
			Thread::Sleep(1);
		}
		return 0;
	}
#endif//
};

/*!
 *	@brief SelectManager 模板定义.
 *
 *	封装SelectManager，实现对select模型管理多个SelectSet，可以支持任意数Socket
 */
template<class TSocket = SocketEx, u_short uFD_SETSize = FD_SETSIZE>
class SelectManager
{
	typedef SelectSet<TSocket,uFD_SETSize> SockSet;
protected:
	std::vector<SockSet*> m_pSockSets;
public:
	SelectManager(int nMaxThreadCount, int nMaxSockCount)
	{
		Create(nMaxThreadCount,nMaxSockCount);
	}

	virtual ~SelectManager() 
	{
		Close();
	}

	bool Create(int nMaxThreadCount, int nMaxSockCount)
	{
		int i,j;
		nMaxSockCount  = ((nMaxSockCount+(uFD_SETSize-1))/uFD_SETSize)*uFD_SETSize;
		int nSockSetCount = nMaxSockCount/uFD_SETSize;
		PRINTF("select set count is [%d], max socket count is [%d/%d].\n", nSockSetCount, nMaxSockCount, uFD_SETSize);
		m_pSockSets.resize(nSockSetCount,NULL);
		for (i=0;i<nSockSetCount;i++)
		{
			m_pSockSets[i] = new SockSet();
		}
#if !USE_SELECTSET_WORKHANDLE
#if USE_SOCKET_THREADPOOL
		m_IdleThreads.resize(nMaxThreadCount,0);
		for (i=0;i<m_IdleThreads.size();i++)
		{
			m_IdleThreads[i] = RegisterThreadPool(IdleThread, (void*)this);
		}
#else
		m_IdleThreads.resize(nMaxThreadCount);
		for (i=0;i<m_IdleThreads.size();i++)
		{
			m_IdleThreads[i].CreateThread(IdleThread, (void*)this);
		}
#endif//
#endif//
		return true;
	}

	void Close(bool bClose = false)
	{
		int i,j;
#if !USE_SELECTSET_WORKHANDLE
#if USE_SOCKET_THREADPOOL
		for (i=0;i<m_IdleThreads.size();i++)
		{
			UnRegisterThreadPool(m_IdleThreads[i]);
			m_IdleThreads[i] = 0;
		}
		m_IdleThreads.clear();
#else
		for (i=0;i<m_IdleThreads.size();i++)
		{
			m_IdleThreads[i].StopThread();
		}
		m_IdleThreads.clear();
#endif//
#endif//
		for (i=0,j=m_pSockSets.size();i<j;i++)
		{
			m_pSockSets[i]->Close(bClose);
			delete m_pSockSets[i];
		}
		m_pSockSets.clear();
	}

	bool AddSocket(TSocket* pSocket)
	{
		int i,j;
		for (i=0,j=m_pSockSets.size();i<j;i++)
		{
			if (m_pSockSets[i]->AddSocket(pSocket)) {
				if (pSocket) {
					OnAddSocket(pSocket);
				}
				return true;
				break;
			}
		}
		return false;
	}

	bool RemoveSocket(TSocket* pSocket)
	{
		int i,j;
		for (i=0,j=m_pSockSets.size();i<j;i++)
		{
			if (m_pSockSets[i]->RemoveSocket(pSocket)) {
				OnRemoveSocket(pSocket);
				return true;
				break;
			}
		}
		return false;
	}

	bool RemoveInvalidSocket(TSocket* & pSocket)
	{
		int i,j;
		for (i=0,j=m_pSockSets.size();i<j;i++)
		{
			if (m_pSockSets[i]->RemoveInvalidSocket(pSocket)) {
				OnRemoveSocket(pSocket);
				return true;
				break;
			}
		}
		return false;
	}

	void Clear(bool bClose = false)
	{
		int i,j;
		for (i=0,j=m_pSockSets.size();i<j;i++)
		{
			m_pSockSets[i]->Clear(bClose);
		}
	}

protected:
	virtual void OnAddSocket(TSocket* pSocket)
	{

	}

	virtual void OnRemoveSocket(TSocket* pSocket)
	{

	}
#if !USE_SELECTSET_WORKHANDLE
	virtual void OnIdle(int nErrorCode)
	{
		int i;
		for (i=0;i<m_pSockSets.size();i++)
		{
			m_pSockSets[i]->OnWorkHandle();
		}
	}

	int OnIdleHandle()
	{
		OnIdle(GetTickCount());
		return 0;
	}

protected:
#if USE_SOCKET_THREADPOOL
	std::vector<long> m_IdleThreads;
	static unsigned int __stdcall IdleThread(void* pParam)
	{
		SelectManager* pThis = (SelectManager*)pParam;
		ASSERT(pThis);
		return pThis->OnIdleHandle();
		return 0;
	}
#else
	std::vector<Thread> m_IdleThreads;
	static unsigned int __stdcall IdleThread(void* pParam)
	{
		SelectManager* pThis = (SelectManager*)pParam;
		ASSERT(pThis);
		int i=0;
		unsigned long ThreadId = Thread::GetCurrentThreadId();
		const std::vector<Thread> & Threads = pThis->m_IdleThreads;
		for (i=0;i<Threads.size();i++)
		{
			if (Threads[i].GetThreadId()==ThreadId) {
				break;
			}
		}
		ASSERT(i<Threads.size());
		while (!Threads[i].IsNeedStop())
		{
			Thread::Sleep(1);
			pThis->OnIdleHandle();
		}
		return 0;
	}
#endif//
#endif//
};

//////////////////////////////////////////////////////////////////////////

/*!
 *	@brief SelectClient 模板定义.
 *
 *	封装SelectClient，实现对select模型管理一个客户端连接Socket
 */
template<class TBase = SocketEx>
class SelectClient : public SelectSocket<TBase>
{
	typedef SelectSocket<TBase> Base;
public:
	SelectClient():Base()
	{
		
	}
	virtual ~SelectClient()
	{
		
	}

protected:
};

/*!
 *	@brief SelectServer 模板定义.
 *
 *	封装SelectServer，实现对select模型管理监听Socket连接，依赖SelectSet/SelectManager
 */
template<class TBase = ListenSocket<SocketEx>, class TWorkSocket = WorkSocket<SocketEx>, u_short uFD_SETSize = FD_SETSIZE>
class SelectServer : public TBase
{
	typedef SelectServer<TBase,TWorkSocket,uFD_SETSize> This;
	typedef TBase Base;
	class SockWorker : public TWorkSocket
	{
		typedef TWorkSocket Base;
		friend class SelectSet<SockWorker,uFD_SETSize>;
		friend class SelectManager<SockWorker,uFD_SETSize>;
		friend class SelectServer<TBase,TWorkSocket,uFD_SETSize>;
	public:

	protected:
		//TWorkSocket会使用SocketExImpl封装的
		/*virtual void OnClose(int nErrorCode)
		{
			Base::OnClose(nErrorCode);

			//if (nErrorCode) {
				Close();
			//}
		}*/
	};
	typedef SelectManager<SockWorker,uFD_SETSize> SockWorkerManager;
protected:
	std::vector<SockWorker*> m_pSockets;
	SockWorkerManager m_SockManager;
public:
	SelectServer(int nMaxThreadCount, int nMaxSockCount) : Base(),m_SockManager(nMaxThreadCount,nMaxSockCount)
	{
		m_pSockets.reserve(((nMaxSockCount+uFD_SETSize-1)/uFD_SETSize)*uFD_SETSize);
#if USE_SOCKET_THREADPOOL
		m_WorkThread = RegisterThreadPool(WorkThread, (void*)this);
#else
		m_WorkThread.CreateThread(WorkThread, (void*)this);
#endif//
	}
	virtual ~SelectServer() 
	{
		int i;

		Close();

#if USE_SOCKET_THREADPOOL
		UnRegisterThreadPool(m_WorkThread);
		m_WorkThread = 0;
#else
		m_WorkThread.StopThread();
#endif//

		m_SockManager.Close(true);

		for (i=0; i<m_pSockets.size(); i++)
		{
			DeletePeer(m_pSockets[i]);
		}
		m_pSockets.clear();
	}

protected:
	virtual SockWorker* NewPeer()
	{
		return new SockWorker();
	}

	virtual void DeletePeer(SockWorker* pSocket)
	{
		delete pSocket;
	}

	virtual void OnAddPeer(SocketEx* pSocket)
	{
		m_pSockets.push_back((SockWorker*)pSocket);
	}

	virtual void OnRemovePeer(SocketEx* pSocket)
	{
		m_pSockets.erase(std::find(m_pSockets.begin(),m_pSockets.end(),pSocket));
	}

protected:
	//网络事件回调
	virtual void OnIdle(int nErrorCode)
	{
		Base::OnIdle(nErrorCode);

		SockWorker* pSocket = NULL;
		if (m_SockManager.RemoveInvalidSocket(pSocket)) {
			OnRemovePeer(pSocket);
			DeletePeer(pSocket);
		}
	}

	virtual void OnAccept(int nErrorCode)
	{
		//bool bConitnue = false;
		//do {
		//	bConitnue = false;
			SOCKADDR_IN Addr = {0};
			int AddrLen = sizeof(SOCKADDR_IN);
			SOCKET Sock = Accept((SOCKADDR*)&Addr, &AddrLen);
			if(IsSocket(Sock)) {
				//测试下还能不能再接收SOCKET
				if(!m_SockManager.AddSocket(NULL)) {
					PRINTF("The connection was refused by the computer running select server because the maximum number of sessions has been exceeded.\n");
					Socket::Close(Sock);
					return;
				}
				SockWorker* pSocket = NewPeer();
				pSocket->Attach(Sock,SOCKET_ROLE_WORK);
				
	#ifdef WIN32
				pSocket->IOCtl(FIONBIO, 1);//设为非阻塞模式
	#else
				int flags = pSocket->IOCtl(F_GETFL,(u_long)0); 
				pSocket->IOCtl(F_SETFL, (u_long)(flags|O_NONBLOCK)); //设为非阻塞模式
				//pSocket->IOCtl(F_SETFL, (u_long)(flags&~O_NONBLOCK)); //设为阻塞模式
	#endif//
				if(m_SockManager.AddSocket(pSocket)) {
					OnAddPeer(pSocket);
				} else {
					PRINTF("The connection was refused by the computer running select server because the maximum number of sessions has been exceeded.\n");
					pSocket->Close();
					DeletePeer(pSocket);
				}
				//bConitnue = true;
			} else {
				nErrorCode = GetLastError();
				switch(nErrorCode)
				{
				case EWOULDBLOCK:
					break;
	#ifdef WIN32
				case WSA_IO_PENDING:
					break;
	#else
				case EINTR:
					//bConitnue = true;
					break;
	#endif//
				default:
					OnClose(nErrorCode);
					break;
				}
			}
		//} while (bConitnue);
	}

	int OnWorkHandle()
	{
		if(!IsSocket()) {
			return 0;
		}

		if (!IsListenSocket()) {
			return 0;
		}

		unsigned long nTickCount = GetTickCount();

		while(1) {

		if (!IsSocket()) {
			break;
		}

		int nfds = 0;
		int maxfds = 0;
		fd_set readfds, exceptfds;
		FD_ZERO(&readfds);
		FD_ZERO(&exceptfds);
		maxfds = Base::m_Sock+1;
		FD_SET(Base::m_Sock, &readfds);
		FD_SET(Base::m_Sock, &exceptfds);
		struct timeval tv = {0, 0};
		nfds = select(maxfds, &readfds, NULL, &exceptfds, &tv);
		if (nfds > 0) {
			do 
			{
				if (FD_ISSET(Base::m_Sock,&readfds)) {
					if(IsEventOf(FD_ACCEPT)) {
						OnAccept(0);
					}
				}
				if (FD_ISSET(Base::m_Sock,&exceptfds)) {
					int nErrorCode = GetLastError();
					/*GetSockOpt(SOL_SOCKET, SO_ERROR, (char *)&nErrorCode, sizeof(nErrorCode));*/
					if (nErrorCode==0) {
						nErrorCode = ENETDOWN;
						SetLastError(nErrorCode);
					}
					OnClose(nErrorCode);
				}
			} while (0);
		} else if (nfds == 0) {
			OnIdle(GetTickCount());
		} else {
			if (IsSocket()) {
				OnClose(GetLastError());
			}
		}
		
		if(!IsSocket()) {
			break;
		}

		unsigned long nLastTickCount = GetTickCount();
		if ((nLastTickCount-nLastTickCount)<20) {
			Thread::Sleep(1);
			nTickCount = GetTickCount();
		} else {
			nTickCount = nLastTickCount;
		}
		}
		return 0;
	}

protected:
#if USE_SOCKET_THREADPOOL
	long m_WorkThread;
	static unsigned int __stdcall WorkThread(void* pParam)
	{
		SelectServer* pThis = (SelectServer*)pParam;
		ASSERT(pThis);
		return pThis->OnWorkHandle();
		return 0;
	}
#else
	Thread m_WorkThread;
	static unsigned int __stdcall WorkThread(void* pParam)
	{
		SelectServer* pThis = (SelectServer*)pParam;
		ASSERT(pThis);
		while (!pThis->m_WorkThread.IsNeedStop())
		{
			Thread::Sleep(1);
			pThis->OnWorkHandle();
		}
		return 0;
	}
#endif//
};

/*!
 *	@brief SelectUdpClient 模板定义.
 *
 *	封装SelectUdpClient，实现对select模型管理一个客户端Udp Socket
 */
template<class TBase = SocketEx> 
class SelectUdpClient : public SelectSocket<TBase>
{
	typedef SelectSocket<TBase> Base;
public:
	SelectUdpClient():Base()
	{

	}
	virtual ~SelectUdpClient()
	{

	}

protected:
};

/*!
 *	@brief SelectUdpServer 模板定义.
 *
 *	封装SelectUdpServer，实现对select模型管理一个服务端Udp Socket
 */
template<class TBase = SocketEx>
class SelectUdpServer : public SelectSocket<TBase>
{
	typedef SelectSocket<TBase> Base;
public:
	SelectUdpServer():Base()
	{

	}
	virtual ~SelectUdpServer()
	{

	}

protected:
};

#endif//_H_XSOCKETEX_H_