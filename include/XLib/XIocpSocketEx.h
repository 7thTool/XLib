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

#ifndef _H_XIOCPSOCKETEX_H_
#define _H_XIOCPSOCKETEX_H_

#include "XSocketEx.h"
#include <MSWSock.h>

/*!
 *	@brief CompletionPort OVERLAPPED 定义.
 *
 *	ERROR_PORT_UNREACHABLE	1234	No service is operating at the destination network endpoint on the remote system.
 *	ERROR_OPERATION_ABORTED	995		The I/O operation has been aborted because of either a thread exit or an application request.
 */
enum 
{
	IOCP_OPERATION_NONE = 0,
	IOCP_OPERATION_ACCEPT,
	IOCP_OPERATION_CONNECT,
	IOCP_OPERATION_RECEIVE,
	IOCP_OPERATION_SEND,
};
#define IOCP_OPERATION_EXIT	ULONG_PTR(-1)
typedef struct _PER_IO_OPERATION_DATA
{ 
	WSAOVERLAPPED	Overlapped;
	WSABUF			Buffer;
	long			Flags;
	long			OperationType;
	union
	{
		long		NumberOfBytesCompleted;
		long		NumberOfBytesReceived;
		long		NumberOfBytesSended;
	};
}PER_IO_OPERATION_DATA, *LPPER_IO_OPERATION_DATA;

/*!
 *	@brief CompletionPortSocket 模板定义.
 *
 *	封装CompletionPortSocket
 */
template<class TBase = SocketEx>
class CompletionPortSocket : public TBase
{
	typedef TBase Base;
protected:
	PER_IO_OPERATION_DATA m_SendOverlapped;
	PER_IO_OPERATION_DATA m_ReceiveOverlapped;
public:
	CompletionPortSocket():Base()
	{
		memset(&m_SendOverlapped,0,sizeof(PER_IO_OPERATION_DATA));
		memset(&m_ReceiveOverlapped,0,sizeof(PER_IO_OPERATION_DATA));
	}

	virtual ~CompletionPortSocket()
	{
		
	}
	
	SOCKET Create(int nSockAf = AF_INET, int nSockType = SOCK_STREAM)
	{
		int nRole = SOCKET_ROLE_NONE;
		SOCKET Sock = WSASocket(nSockAf, nSockType, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
		return Attach(Sock, nRole);
	}

	int Close()
	{
		int rlt = Base::Close();
		
		return rlt;
	}

	/*SOCKET Accept(SOCKADDR* lpSockAddr, int* lpSockAddrLen)
	{
		SOCKET Sock = INVALID_SOCKET;
		if(AcceptEx(m_Sock,Sock)) {
			return Sock;
		}
		return INVALID_SOCKET;
	}*/

	int Connect(const char* lpszHostAddress, unsigned short nHostPort)
	{
		SOCKADDR_IN sockAddr = {0};
		if (lpszHostAddress == NULL) {
			sockAddr.sin_addr.s_addr = H2N((u_long)INADDR_ANY);
		} else {
			sockAddr.sin_addr.s_addr = Ip2N(Url2Ip((char*)lpszHostAddress));
			if (sockAddr.sin_addr.s_addr == INADDR_NONE) {
				//SetLastError(EINVAL);
				//return SOCKET_ERROR;
			}
		}
		sockAddr.sin_family = AF_INET;
		sockAddr.sin_port = H2N((u_short)nHostPort);
		return Connect((SOCKADDR*)&sockAddr, sizeof(sockAddr));
	}

	int Connect(const SOCKADDR* lpSockAddr, int nSockAddrLen)
	{
		ASSERT(IsSocket());

		//获得ConnectEx 函数的指针
		LPFN_CONNECTEX ConnectEx = NULL;
		DWORD dwBytes = 0;
		GUID GuidConnectEx = WSAID_CONNECTEX;
		if (SOCKET_ERROR == WSAIoctl(m_Sock, SIO_GET_EXTENSION_FUNCTION_POINTER,
			&GuidConnectEx, sizeof(GuidConnectEx ),
			&ConnectEx, sizeof (ConnectEx), &dwBytes, 0, 0)) {
			PRINTF("WSAIoctl ConnectEx is failed. Error=%d\n", GetLastError());
			return SOCKET_ERROR;
		}

		OnRole(SOCKET_ROLE_CONNECT);
		m_Role = SOCKET_ROLE_CONNECT;
		m_Event = FD_CONNECT;
		IOCtl(FIONBIO, 1);//设为非阻塞模式

		//MSDN说The parameter s is an unbound or a listening socket，
		//还是诡异两个字connect操作干嘛要绑定？不知道，没人给解释，那绑定就对了，那么绑哪个？
		//最好把你的地址结构像下面这样设置
		SOCKADDR_IN temp;
		temp.sin_family = AF_INET;
		temp.sin_port = htons(0);
		temp.sin_addr.s_addr = htonl(ADDR_ANY);
		//为什么端口这个地方用0，原因很简单，你去查查MSDN，这样表示他会在1000-4000这个范围（可能记错，想了解的话去查MSDN）找一个没被用到的port，
		//这样的话最大程度保证你bind的成功，然后再把socket句柄丢给IOCP，然后调用ConnectEx这样就会看到熟悉的WSA_IO_PENDING了！
		Bind((const SOCKADDR*)&temp,sizeof(SOCKADDR_IN));
		
		PER_IO_OPERATION_DATA* pOverlapped = &m_SendOverlapped;
		memset(&pOverlapped->Overlapped, 0, sizeof(WSAOVERLAPPED));
		pOverlapped->Buffer.buf	= NULL;
		pOverlapped->Buffer.len	= 0;
		pOverlapped->Flags = 0;
		pOverlapped->OperationType = IOCP_OPERATION_CONNECT;
		pOverlapped->NumberOfBytesSended = 0;
		if(ConnectEx(m_Sock, 
			lpSockAddr, 
			nSockAddrLen, 
			pOverlapped->Buffer.buf,
			pOverlapped->Buffer.len,
			(LPDWORD)&pOverlapped->NumberOfBytesSended, 
			&pOverlapped->Overlapped)) {
			ASSERT(0);
			return 0;
		} else {
			//int nError = GetLastError();
			//PRINTF("ConnectEx is failed. Error=%d\n", nError);
		}
		return SOCKET_ERROR;
	}

	int Send(const char* lpBuf, int nBufLen, int nFlags = 0)
	{
		PER_IO_OPERATION_DATA* pOverlapped = &m_SendOverlapped;
		memset(&pOverlapped->Overlapped, 0, sizeof(WSAOVERLAPPED));
		pOverlapped->Buffer.buf	= (char*)lpBuf;
		pOverlapped->Buffer.len	= nBufLen; 
		pOverlapped->Flags = nFlags;
		pOverlapped->OperationType = IOCP_OPERATION_SEND;
		pOverlapped->NumberOfBytesSended = 0;
		int nSend = WSASend(m_Sock, 
			&pOverlapped->Buffer, 
			1, 
			(LPDWORD)&pOverlapped->NumberOfBytesSended, 
			(DWORD)pOverlapped->Flags, 
			&pOverlapped->Overlapped,
			NULL);
		return nSend;
	}

	int Receive(char* lpBuf, int nBufLen, int nFlags = 0)
	{
		PER_IO_OPERATION_DATA* pOverlapped = &m_ReceiveOverlapped;
		memset(&pOverlapped->Overlapped,0,sizeof(WSAOVERLAPPED));
		pOverlapped->Buffer.buf = lpBuf;
		pOverlapped->Buffer.len = nBufLen;
		pOverlapped->Flags = nFlags;
		pOverlapped->OperationType = IOCP_OPERATION_RECEIVE;
		pOverlapped->NumberOfBytesReceived = 0;
		int nRecv = WSARecv(m_Sock, 
			&pOverlapped->Buffer, 
			1, 
			(LPDWORD)&pOverlapped->NumberOfBytesReceived, 
			(LPDWORD)&pOverlapped->Flags, 
			&pOverlapped->Overlapped,
			NULL);
		return nRecv;
	}

	void Trigger(int Event)
	{
		if (Event&FD_WRITE) {
#if 1
			if (TryEvent(FD_WRITE)) {
				OnSend(0);
			}
#else
			PER_IO_OPERATION_DATA* pOverlapped = &m_SendOverlapped;
			if (pOverlapped->NumberOfBytesSended>=pOverlapped->Buffer.len) {
				OnSend(0);
			}
#endif//
		}
		if (Event&FD_READ) {
#if 1
			if (TryEvent(FD_READ)) {
				OnReceive(0);
			}
#else
			PER_IO_OPERATION_DATA* pOverlapped = &m_ReceiveOverlapped;
			if (pOverlapped->NumberOfBytesReceived>0) {
				OnReceive(0);
			}
#endif//
		}
	}
};


/*!
 *	@brief CompletionPort 模板定义.
 *
 *	封装CompletionPortManager，实现CompletionPort模型
 */
template<class TSocket = SocketEx, u_short uFD_SETSize = FD_SETSIZE>
class CompletionPortManager
{
protected:
	HANDLE m_hIocp;
	std::vector<TSocket*> m_pSockets;
	volatile long m_nIdleIndex;
	CriticalSection m_Section;
public:
	CompletionPortManager(int nMaxThreadCount, int nMaxSockCount):m_hIocp(INVALID_HANDLE_VALUE),m_nIdleIndex(0)
	{
		Create(nMaxThreadCount, nMaxSockCount);
	}
	virtual ~CompletionPortManager()
	{
		Close();
	}

	bool Create(int nMaxThreadCount, int nMaxSockCount)
	{
		int i = 0;
		nMaxSockCount = ((nMaxSockCount+(uFD_SETSize-1))/uFD_SETSize)*uFD_SETSize;
		SYSTEM_INFO SystemInfo = {0};
		GetSystemInfo(&SystemInfo);
		int nWorkThread = SystemInfo.dwNumberOfProcessors*2;
		m_hIocp = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, SystemInfo.dwNumberOfProcessors);
		ASSERT(m_hIocp!=INVALID_HANDLE_VALUE);
		PRINTF("Iocp thread count is [%d], max socket count is [%d/%d].\n", nWorkThread, nMaxSockCount, uFD_SETSize);
		m_pSockets.resize(nMaxSockCount);
		m_nIdleIndex = 0;
#if USE_SOCKET_THREADPOOL
		m_IdleThreads.resize(nMaxThreadCount,0);
		for (i=0;i<m_IdleThreads.size();i++)
		{
			m_IdleThreads[i] = RegisterThreadPool(IdleThread, (void*)this);
		}
		m_WorkThreads.resize(nWorkThread,0);
		for (i=0;i<m_WorkThreads.size();i++)
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
		for (i=0;i<m_WorkThreads.size();i++)
		{
			m_WorkThreads[i].CreateThread(WorkThread, (void*)this);
		}
#endif//
		return true;
	}

	void Close(bool bClose = false)
	{
		int i = 0;
		if(m_hIocp!=INVALID_HANDLE_VALUE) {
			for(size_t i = 0, j = m_WorkThreads.size(); i < j; i++)
			{
				PostQueuedCompletionStatus(m_hIocp, IOCP_OPERATION_EXIT, 0, NULL);
			}
			CloseHandle(m_hIocp);
			m_hIocp = INVALID_HANDLE_VALUE;
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
					HANDLE hIocp = CreateIoCompletionPort((HANDLE)pSocket->m_Sock, m_hIocp, (ULONG_PTR)(i+1), 0);
					ASSERT(hIocp);
					if (hIocp != INVALID_HANDLE_VALUE) {
						PRINTF("CreateIoCompletionPort: %ld by %ld\n", hIocp, m_hIocp);
						m_pSockets[i] = pSocket;
						Lock.Detach();
						OnAddSocket(pSocket);
						return true;
					} else {
						PRINTF("CreateIoCompletionPort Error:%d\n", ::GetLastError());
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
				if(pSocket->IsSocket()) {
					//PostQueuedCompletionStatus(m_hIocp, IOCP_OPERATION_EXIT, (ULONG_PTR)&pSocket, NULL);
					//PostQueuedCompletionStatus(m_hIocp, IOCP_OPERATION_EXIT, (ULONG_PTR)&pSocket, NULL);
					//WSAOVERLAPPED & SendOverlapped = pSocket->m_SendOverlapped.Overlapped;
					//SendOverlapped.hEvent = (HANDLE)((DWORD)SendOverlapped.hEvent | 0x1); //haha
					//WSAOVERLAPPED & ReceiveOverlapped = pSocket->m_ReceiveOverlapped.Overlapped;
					//ReceiveOverlapped.hEvent = (HANDLE)((DWORD)ReceiveOverlapped.hEvent | 0x1); //haha
					if (bClose) {
						pSocket->OnClose(0); //主动关闭
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
		if (m_hIocp==INVALID_HANDLE_VALUE) {
			return 0;
		}

		while(1) {

			int Pos = 0;
			DWORD  dwTransfer = 0;
			PER_IO_OPERATION_DATA* lpOverlapped = NULL;
			BOOL bStatus = GetQueuedCompletionStatus( 
				m_hIocp, 
				&dwTransfer, 
				(PULONG_PTR)&Pos, 
				(LPOVERLAPPED *)&lpOverlapped, 
				INFINITE);
			if (!bStatus) {
				PRINTF("GetQueuedCompletionStatus Error:%d \n", ::GetLastError());
				//break;
			}
			if (dwTransfer == IOCP_OPERATION_EXIT) {//
				PRINTF("GetQueuedCompletionStatus Eixt\n");
				break; 
			}
			if (Pos>0 && Pos<=m_pSockets.size()) {
				SectionLocker Lock(&m_Section);
				TSocket* pSocket = m_pSockets[Pos-1];
				Lock.Detach();
				if(!bStatus) {
					if (pSocket) {
						if (pSocket->IsEventOf(FD_CONNECT)) {
							int nErrorCode = ::GetLastError();
							pSocket->OnConnect(nErrorCode);
							if (nErrorCode==0) {
								pSocket->RemoveEvent(FD_CONNECT);
							}
						} else {
							int nErrorCode = ::GetLastError();
							pSocket->OnClose(nErrorCode);
						}
					}
				} else {
					if (pSocket) {
						switch (lpOverlapped->OperationType)
						{
						case IOCP_OPERATION_CONNECT:
							{
								pSocket->SetSockOpt( 
									SOL_SOCKET, 
									SO_UPDATE_CONNECT_CONTEXT, 
									NULL, 
									0 );
								pSocket->OnConnect(pSocket->GetLastError());
								if (pSocket->IsSocket()) {
									if(pSocket->TryEvent(FD_WRITE)) {
										pSocket->OnSend(0);
									}
									if(pSocket->TryEvent(FD_READ)) {
										pSocket->OnReceive(0);
									}
								}
							}
							break;
						case IOCP_OPERATION_RECEIVE:
							{ 
								lpOverlapped->NumberOfBytesReceived = dwTransfer;
								pSocket->OnReceived(lpOverlapped->Buffer.buf,lpOverlapped->NumberOfBytesReceived, 0);
								if (dwTransfer) {
									pSocket->OnReceive(0); //继续投递接收操作
									//pSocket->OnSend(0);
								} else {
									PRINTF("GetQueuedCompletionStatus Recv Error:%d WSAError:%d\n", ::GetLastError(), pSocket->GetLastError());
									pSocket->OnClose(pSocket->GetLastError());
								}
							}
							break;
						case IOCP_OPERATION_SEND:
							{
								lpOverlapped->NumberOfBytesSended = dwTransfer;
								pSocket->OnSended(lpOverlapped->Buffer.buf, lpOverlapped->NumberOfBytesSended, 0);
								if (dwTransfer) {
									pSocket->OnSend(0); //继续投递接收操作
									//pSocket->OnReceive(0); //继续投递接收操作
								} else {
									PRINTF("GetQueuedCompletionStatus Send Error:%d WSAError:%d\n", ::GetLastError(), pSocket->GetLastError());
									pSocket->OnClose(pSocket->GetLastError());
								}
							}
							break;
						default:
							{
								ASSERT(0);
							}
							break;
						}
					}
				}
			}

			if (m_hIocp==INVALID_HANDLE_VALUE) {
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
		CompletionPortManager* pThis = (CompletionPortManager*)pParam;
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
		CompletionPortManager* pThis = (CompletionPortManager*)pParam;
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
 *	@brief CompletionPortServer 模板定义.
 *
 *	封装CompletionPortServer，实现对CompletionPort模型管理监听Socket连接，依赖CompletionPortManager
 */
template<class TBase = ListenSocket<SocketEx>, class TWorkSocket = WorkSocket<CompletionPortSocket<SocketEx>>, u_short uFD_SETSize = FD_SETSIZE>
class CompletionPortServer : public TBase
{
	typedef CompletionPortServer<TBase,TWorkSocket,uFD_SETSize> This;
	typedef TBase Base;
	class SockWorker : public TWorkSocket
	{
		typedef TWorkSocket Base;
		friend class CompletionPortManager<SockWorker,uFD_SETSize>;
		friend class CompletionPortServer<TBase,TWorkSocket,uFD_SETSize>;
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
	typedef CompletionPortManager<SockWorker,uFD_SETSize> SockWorkerManager;
protected:
	std::vector<SockWorker*> m_pSockets;
	SockWorkerManager m_SockManager;
public:
	CompletionPortServer(int nMaxThreadCount, int nMaxSockCount) : Base(),m_SockManager(nMaxThreadCount,nMaxSockCount)
	{
		m_pSockets.reserve(((nMaxSockCount+uFD_SETSize-1)/uFD_SETSize)*uFD_SETSize);
#if USE_SOCKET_THREADPOOL
		m_WorkThread = RegisterThreadPool(WorkThread, (void*)this);
#else
		m_WorkThread.CreateThread(WorkThread, (void*)this);
#endif//
	}
	virtual ~CompletionPortServer() 
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
					PRINTF("The connection was refused by the computer running iocp server because the maximum number of sessions has been exceeded.\n");
					Socket::Close(Sock);
					return;
				}
				SockWorker* pSocket = NewPeer();
				pSocket->Attach(Sock,SOCKET_ROLE_WORK);
				pSocket->IOCtl(FIONBIO, 1);//设为非阻塞模式
				if(m_SockManager.AddSocket(pSocket)) {
					OnAddPeer(pSocket);
					pSocket->OnReceive(0);//投递接收操作
				} else {
					PRINTF("The connection was refused by the computer running iocp server because the maximum number of sessions has been exceeded.\n");
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
				case WSA_IO_PENDING:
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

			OnIdle(nTickCount);

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
						}
						SetLastError(nErrorCode);
						OnClose(nErrorCode);
					}
				} while (0);
			} else if (nfds == 0) {
				//
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
		CompletionPortServer* pThis = (CompletionPortServer*)pParam;
		ASSERT(pThis);
		return pThis->OnWorkHandle();
		return 0;
	}
#else
	Thread m_WorkThread;
	static unsigned int __stdcall WorkThread(void* pParam)
	{
		CompletionPortServer* pThis = (CompletionPortServer*)pParam;
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

#endif//_H_XIOCPSOCKETEX_H_