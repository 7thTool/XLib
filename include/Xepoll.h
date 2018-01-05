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

#ifndef _H_XEPOLL_H_
#define _H_XEPOLL_H_

#include "XSocketEx.h"
#include <sys/epoll.h>

/*!
 *	@brief CompletionPortSocket 模板定义.
 *
 *	封装CompletionPortSocket
 */
template<class TBase = SocketEx>
class EPollSocket : public TBase
{
	typedef TBase Base;
protected:
	int m_epfd;
public:
	EPollSocket():Base(),m_epfd(0)
	{
		
	}

	virtual ~EPollSocket()
	{
		
	}

	void Trigger(int Event)
	{
#ifdef USE_EPOLLET
		int fd = m_Sock;
		struct epoll_event event = {0};
		event.data.ptr = (void*)this;
		event.events=EPOLLRDHUP|EPOLLPRI|EPOLLET|EPOLLONESHOT;
		if (Event&FD_WRITE) {
			event.events |= EPOLLOUT|EPOLLIN;
		}
		if (Event&FD_READ) {
			event.events |= EPOLLIN;
		}
		epoll_ctl(m_epfd,EPOLL_CTL_MOD,fd,&event);
#endif//
	}
};

/*!
 *	@brief EPollManager 模板定义.
 *
 *	封装EPollManager，实现epoll模型
 */
#ifdef WIN32
template<class TSocket = SocketEx, u_short uFD_SETSize = FD_SETSIZE>
#else
template<class TSocket, u_short uFD_SETSize>
#endif//
class EPollManager
{
protected:
	int m_epfd;
	std::vector<TSocket*> m_pSockets;
	unsigned long m_nIdleIndex;
	CriticalSection m_Section;
public:
	EPollManager(int nMaxThreadCount, int nMaxSockCount):m_epfd(0),m_nIdleIndex(0)
	{
		Create(nMaxThreadCount,nMaxSockCount);
	}
	virtual ~EPollManager() 
	{
		Close();
	}

	bool Create(int nMaxThreadCount, int nMaxSockCount)
	{
		int i = 0;
		nMaxSockCount = ((nMaxSockCount+(uFD_SETSize-1))/uFD_SETSize)*uFD_SETSize;
		int nWorkThread = nMaxSockCount/uFD_SETSize;
		PRINTF("epoll thread count is [%d], max socket count is [%d/%d].\n", nWorkThread, nMaxSockCount, uFD_SETSize);
		m_epfd = epoll_create(nMaxSockCount);
		m_pSockets.resize(nMaxSockCount,NULL);
		m_nIdleIndex = 0;
#if USE_SOCKET_THREADPOOL
		m_IdleThreads.resize(nMaxThreadCount,0);
		for (i=0;i<m_IdleThreads.size();i++)
		{
			m_IdleThreads[i] = RegisterThreadPool(IdleThread, (void*)this);
		}
		m_WorkThreads.resize(nWorkThread,0);
		for (i=0;i<nWorkThread;i++)
		{
			m_WorkThreads[i] = RegisterThreadPool(WorkThread, (void*)this);
		}
#else
		m_IdleThreads.resize(nMaxThreadCount);
		for (i=0;i<m_IdleThreads.size();i++)
		{
			m_IdleThreads[i].CreateThread(IdleThread, (void*)this);
		}
		m_WorkThreads.resize(nWorkThread);
		for (i=0;i<nWorkThread;i++)
		{
			m_WorkThreads[i].CreateThread(WorkThread, (void*)this);
		}
#endif//
		return true;
	}

	void Close(bool bClose = false)
	{
		int i = 0;
		if (m_epfd) {
			close(m_epfd);
			m_epfd = 0;
		}
#if USE_SOCKET_THREADPOOL
		for (i=0;i<m_IdleThreads.size();i++)
		{
			UnRegisterThreadPool(m_IdleThreads[i]);
			m_IdleThreads[i] = 0;
		}
		m_IdleThreads.clear();
		for (i=0;i<m_WorkThreads.size();i++)
		{
			UnRegisterThreadPool(m_WorkThreads[i]);
			m_WorkThreads[i] = 0;
		}
		m_WorkThreads.clear();
#else
		for (i=0;i<m_IdleThreads.size();i++)
		{
			m_IdleThreads[i].StopThread();
		}
		m_IdleThreads.clear();
		for (i=0;i<m_WorkThreads.size();i++)
		{
			m_WorkThreads[i].StopThread();
		}
		m_WorkThreads.clear();
#endif//

		Clear(bClose);
	}

	bool AddSocket(TSocket* pSocket)
	{
		SectionLocker Lock(&m_Section);
		int i;
		for (i=0;i<m_pSockets.size();i++)
		{
			if(m_pSockets[i]==NULL) {
				if (pSocket) {
					pSocket->m_epfd = m_epfd;
					int fd = pSocket->m_Sock;
					struct epoll_event event = {0};
					event.data.ptr = (void*)pSocket;
					//LT(默认)，LT+EPOLLONESHOT最可靠
					//ET，EPOLLET最高效,ET+EPOLLONESHOT高效可靠（这里使用）
					event.events = 0
						| EPOLLIN			//表示对应的文件描述符可以读（包括对端SOCKET正常关闭）；
						| EPOLLOUT			//表示对应的文件描述符可以写；
						| EPOLLRDHUP		//Stream socket peer closed connection, or shut down writing  half of connection.
						| EPOLLPRI			//表示对应的文件描述符有紧急的数据可读（这里应该表示有带外数据到来）；
						//| EPOLLERR		//表示对应的文件描述符发生错误；不用注册，会自动触发
						//| EPOLLHUP		//表示对应的文件描述符被挂断；不用注册，会自动触发
#ifdef USE_EPOLLET
						| EPOLLET			//将EPOLL设为边缘触发(Edge Triggered)模式，这是相对于水平触发(LevelTriggered)来说的；
#endif//
						| EPOLLONESHOT		//只监听一次事件，当监听完这次事件之后，如果还需要继续监听这个socket的话，需要再次把这个socket加入到EPOLL队列里
						;
					if(SOCKET_ERROR != epoll_ctl(m_epfd,EPOLL_CTL_ADD,fd,&event)) {
						m_pSockets[i] = pSocket;
						Lock.Detach();
						OnAddSocket(pSocket);
						return true;
					} else {
						PRINTF("epoll_ctl err:%d\n", ::GetLastError());
					}
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
		for (i=0;i<m_pSockets.size();i++)
		{
			if(m_pSockets[i]==pSocket) {
				SectionLocker Lock(&m_Section);
				TSocket* pTempSocket = m_pSockets[i];
				if (pTempSocket) {
					if(pSocket->IsSocket()) {
						int fd = pSocket->m_Sock;
						struct epoll_event event = {0};
						event.data.ptr = (void*)pSocket;
						if(SOCKET_ERROR != epoll_ctl(m_epfd,EPOLL_CTL_DEL,fd,&event)) {
							m_pSockets[i] = NULL;
							Lock.Detach();
							OnRemoveSocket(pSocket);
							return true;
						}
					} else {
						m_pSockets[i] = NULL;
						Lock.Detach();
						OnRemoveSocket(pSocket);
						return true;
					}
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
		for (i=0;i<m_pSockets.size();i++)
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
		for (i=0;i<m_pSockets.size();i++)
		{
			SectionLocker Lock(&m_Section);
			if (m_pSockets[i]) {
				TSocket* pSocket = m_pSockets[i];
				m_pSockets[i] = NULL;
				Lock.Detach();
				if (pSocket) {
					if(pSocket->IsSocket()) {
						int fd = pSocket->m_Sock;
						struct epoll_event event = {0};
						event.data.ptr = (void*)pSocket;
						epoll_ctl(m_epfd,EPOLL_CTL_DEL,fd,&event);
						if (bClose) {
							pSocket->OnClose(0);
						}
					}
					OnRemoveSocket(pSocket);
				}
			}
		}
	}

protected:
	//
	virtual void OnAddSocket(TSocket* pSocket)
	{
		
	}

	virtual void OnRemoveSocket(TSocket* pSocket)
	{
		
	}

	virtual void OnIdle(int nErrorCode)
	{
		unsigned long i,j;
		i = (unsigned long)Atomic::Add(&m_nIdleIndex,m_pSockets.size()/m_IdleThreads.size());
		i %= m_pSockets.size();
		j = i + m_IdleThreads.size();
		for (; i<j && i<m_pSockets.size(); i++)
		{
			if (m_pSockets[i]) {
				//SectionLocker Lock(&m_Section);
				TSocket* pTempSocket = m_pSockets[i];
				//Lock.Detach();
				if (pTempSocket) {
					if (!pTempSocket->IsSocket()) {
						//
					} else {
						if (pTempSocket->TryEvent(FD_IDLE)) {
							pTempSocket->OnIdle(GetTickCount());
							pTempSocket->RemoveEvent(FD_IDLE);
						}
					}
				}
			}
		}
	}

	int OnIdleHandle()
	{
		OnIdle(GetTickCount());
		return 0;
	}

	int OnWorkHandle()
	{
		if (m_epfd==0) {
			return 0;
		}

		while(1) {

			if (m_epfd==0) {
				break;
			}

			int i;
			int nfds = 0;
			struct epoll_event events[uFD_SETSize] = {0};
			//Specifying a timeout of -1 makes epoll_wait wait indefinitely, while specifying a timeout equal to zero makes epoll_wait to return immediately even if no events are available (return code equal to zero).
			nfds = epoll_wait(m_epfd,events,uFD_SETSize,-1);
			if (nfds>0) {
				for(i=0;i<nfds;++i)
				{
					struct epoll_event event = events[i];
					TSocket* pSocket = (TSocket*)event.data.ptr;
					unsigned int evt = event.events;
					ASSERT(pSocket);
					int fd = pSocket->m_Sock;
					int nErrorCode = 0;
					//参考NGIX逻辑...
					//if(evt&(EPOLLRDHUP|EPOLLERR|EPOLLHUP)) {
					//	PRINTF("epoll_wait error: fd=%d event=%04XD\n" , fd, evt);
					//}
					//if ((evt&(EPOLLRDHUP|EPOLLERR|EPOLLHUP)) && (evt&(EPOLLIN|EPOLLOUT))==0) {
					//	/*
					//	 * if the error events were returned without EPOLLIN or EPOLLOUT,
					//	 * then add these flags to handle the events at least in one
					//	 * active handler
					//	 */
					//	evt |= EPOLLIN|EPOLLOUT;
					//} 
					if(evt&EPOLLPRI) {
						//有紧急数据
						if(pSocket->TryEvent(FD_OOB)) {
							do {
								pSocket->OnOutOfBandData(0);
								nErrorCode = pSocket->IsSocket()?pSocket->GetLastError():ENOTSOCK;
							} while(!nErrorCode);
							if (nErrorCode==0) {
								pSocket->RemoveEvent(FD_OOB);
							}
						}
						if (!pSocket->IsSocket()) {
							continue;
						}
					} 
					if(evt&EPOLLIN) {
						//有新的可读数据
						if (pSocket->IsEventOf(FD_ACCEPT)) {
							do {
								pSocket->OnAccept(0);
								nErrorCode = pSocket->IsSocket()?pSocket->GetLastError():ENOTSOCK;
							} while(!nErrorCode);
						} else {
							if(pSocket->TryEvent(FD_READ)) {
								do {
									pSocket->OnReceive(0);
									nErrorCode = pSocket->IsSocket()?pSocket->GetLastError():ENOTSOCK;
								} while(!nErrorCode);
								if (nErrorCode==0) {
									pSocket->RemoveEvent(FD_READ);
								}
							}
						}
						if (!pSocket->IsSocket()) {
							continue;
						}
					}
					if(evt&EPOLLOUT) {
						//有新的可写数据
						if (pSocket->IsEventOf(FD_CONNECT)) {
							if (nErrorCode==0) {
								int nOptLen = sizeof(nErrorCode);
								pSocket->GetSockOpt(SOL_SOCKET, SO_ERROR, (char *)&nErrorCode, &nOptLen);
							}
							pSocket->OnConnect(nErrorCode);
							if(nErrorCode==0) {
								pSocket->RemoveEvent(FD_CONNECT);
							}
						}// else {
						if(pSocket->TryEvent(FD_WRITE)) {
							pSocket->OnSend(0);
							pSocket->RemoveEvent(FD_WRITE);
						}
						//}
						if (!pSocket->IsSocket()) {
							continue;
						}
					}
					if(evt&(EPOLLRDHUP|EPOLLERR|EPOLLHUP)) {
						nErrorCode = pSocket->GetLastError();
						pSocket->OnClose(nErrorCode);
						if (!pSocket->IsSocket()) {
							continue;
						}
					}
#ifndef USE_EPOLLET
					ASSERT(pSocket->IsSocket());
					if (pSocket->IsSocket()) {
						event.data.ptr = (void*)pSocket;
						event.events=EPOLLIN|EPOLLOUT|EPOLLRDHUP|EPOLLPRI
							|EPOLLONESHOT;
						epoll_ctl(m_epfd,EPOLL_CTL_MOD,fd,&event);
					}
#endif//
				}
			} else {
				//
			}

			if (m_epfd==0) {
				break;
			}
		}
		return 0;
	}
protected:
#if USE_SOCKET_THREADPOOL
	std::vector<long> m_IdleThreads;
	std::vector<long> m_WorkThreads;
	static unsigned int __stdcall IdleThread(void* pParam)
	{
		CompletionPortManager* pThis = (CompletionPortManager*)pParam;
		ASSERT(pThis);
		return pThis->OnIdleHandle();
		return 0;
	}
	static unsigned int __stdcall WorkThread(void* pParam)
	{
		EPollManager* pThis = (EPollManager*)pParam;
		ASSERT(pThis);
		return pThis->OnWorkHandle();
		return 0;
	}
#else
	std::vector<Thread> m_IdleThreads;
	std::vector<Thread> m_WorkThreads;
	static unsigned int __stdcall IdleThread(void* pParam)
	{
		CompletionPortManager* pThis = (CompletionPortManager*)pParam;
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
	static unsigned int __stdcall WorkThread(void* pParam)
	{
		EPollManager* pThis = (EPollManager*)pParam;
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
			pThis->OnWorkHandle();
		}
		return 0;
	}
#endif//
};

/*!
 *	@brief EPollServer 模板定义.
 *
 *	封装EPollServer，实现对epoll模型管理监听Socket连接，依赖EPollManager
 */
#ifdef WIN32
template<class TBase = ListenSocket<SocketEx>, class TWorkSocket = WorkSocket<SocketEx>, u_short uFD_SETSize = FD_SETSIZE>
#else
template<class TBase, class TWorkSocket, u_short uFD_SETSize>
#endif//
class EPollServer : public TBase
{
	typedef TBase Base;
	class SockWorker : public TWorkSocket
	{
		typedef TWorkSocket Base;
		friend class EPollManager<SockWorker,uFD_SETSize>;
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
	typedef EPollManager<SockWorker,uFD_SETSize> SockWorkerManager;
protected:
	std::vector<SockWorker*> m_pSockets;
	SockWorkerManager m_SockManager;
public:
	EPollServer(int nMaxThreadCount, int nMaxSockCount) : Base(),m_SockManager(nMaxThreadCount,nMaxSockCount)
	{
		m_pSockets.reserve(((nMaxSockCount+uFD_SETSize-1)/uFD_SETSize)*uFD_SETSize);
#if USE_SOCKET_THREADPOOL
		m_WorkThread = RegisterThreadPool(WorkThread, (void*)this);
#else
		m_WorkThread.CreateThread(WorkThread, (void*)this);
#endif//
	}
	virtual ~EPollServer() 
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
					PRINTF("The connection was refused by the computer running epoll server because the maximum number of sessions has been exceeded.\n");
					Socket::Close(Sock);
					return;
				}
				SockWorker* pSocket = NewPeer();
				pSocket->Attach(Sock,SOCKET_ROLE_WORK);

				int flags = pSocket->IOCtl(F_GETFL,(u_long)0); 
				pSocket->IOCtl(F_SETFL, (u_long)(flags|O_NONBLOCK)); //设为非阻塞模式
				//pSocket->IOCtl(F_SETFL, (u_long)(flags&~O_NONBLOCK)); //设为阻塞模式
				if(m_SockManager.AddSocket(pSocket)) {
					OnAddPeer(pSocket);
				} else {
					PRINTF("The connection was refused by the computer running epoll server because the maximum number of sessions has been exceeded.\n");
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
				case EINTR:
					//bConitnue = true;
					break;
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
						/*int nOptLen = sizeof(nErrorCode);
						GetSockOpt(SOL_SOCKET, SO_ERROR, (char *)&nErrorCode, &nOptLen);*/
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
				Thread::Sleep(20);
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
		EPollServer* pThis = (EPollServer*)pParam;
		ASSERT(pThis);
		return pThis->OnWorkHandle();
		return 0;
	}
#else
	Thread m_WorkThread;
	static unsigned int __stdcall WorkThread(void* pParam)
	{
		EPollServer* pThis = (EPollServer*)pParam;
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

#endif//_H_XEPOLL_H_