#ifndef _H_ASYNCSOCKETEX_H_
#define _H_ASYNCSOCKETEX_H_

#include "XSocketEx.h"

#define FD_USER_BIT			10 //FD_MAX_EVENTS

#define FD_FAILED_BIT		(FD_USER_BIT)
#define FD_FAILED			(1 << FD_FAILED_BIT)

#define FD_FORCEREAD_BIT	(FD_USER_BIT + 1)
#define FD_FORCEREAD		(1 << FD_FORCEREAD_BIT)

#undef  FD_MAX_EVENTS
#undef	FD_ALL_EVENTS
#define FD_MAX_EVENTS		(FD_USER_BIT + 2)
#define FD_ALL_EVENTS		((1 << FD_MAX_EVENTS) - 1)
#define FD_DEF_EVENTS		(FD_READ | FD_WRITE | FD_OOB | FD_ACCEPT | FD_CONNECT | FD_CLOSE | FD_FAILED | FD_FORCEREAD)

#define WM_SOCKETEX_MSG			(WM_USER + 0x0100)					// 0X0500
#define WM_SOCKETEX_GETHOST		(WM_SOCKETEX_MSG + 1)				// 0x0501
#define WM_SOCKETEX_NOTIFY		(WM_SOCKETEX_MSG + 2)				// 0x0502

class AsyncSocketExHelperWindow;
class XLIB_API AsyncSocketEx : public SocketEx
{
	friend class AsyncSocketExHelperWindow;

public:
	AsyncSocketEx();
	virtual ~AsyncSocketEx();

	SOCKET Create(int nSocketType = SOCK_STREAM,
				long lEvent = FD_READ | FD_WRITE | FD_OOB | FD_ACCEPT |	FD_CONNECT | FD_CLOSE);
	void Close();

	SOCKET Attach(SOCKET Sock);
	SOCKET Detach();

	BOOL AsyncSelect(long lEvent = FD_READ | FD_WRITE | FD_OOB | FD_ACCEPT | FD_CONNECT | FD_CLOSE);
	BOOL TriggerEvent(long lEvent, long nErrCode = 0);

	int Connect(LPCSTR lpszHostAddress, UINT nHostPort);
	int Connect(const SOCKADDR* lpSockAddr, int nSockAddrLen);

	virtual void OnHostNameResolved(long nErrCode);
//	virtual void OnRole(int nRole);
// 	virtual void OnReceive(int nErrorCode);
// 	virtual void OnSend(int nErrorCode);
// 	virtual void OnOutOfBandData(int nErrorCode);
// 	virtual void OnAccept(int nErrorCode);
// 	virtual void OnConnect(int nErrorCode);
// 	virtual void OnClose(int nErrorCode);

protected:
	int m_nSockIndex; //Index of socket, required by AsyncSocketExHelperWindow

	//AsyncGetHostByName
	char *m_pAsyncGetHostByNameBuffer; //Buffer for hostend structure
	HANDLE m_hAsyncGetHostByNameHandle; //TaskHandle
	int m_nAsyncGetHostByNamePort; //Port to connect to

	//Critical section for thread synchronization
	static CriticalSection m_sGlobalCriticalSection;

	//Pointer to the data of the local thread
	struct t_AsyncSocketExThreadData
	{
		AsyncSocketExHelperWindow *m_pHelperWindow;
		int nInstanceCount;
		DWORD nThreadId;
	} *m_pLocalAsyncSocketExThreadData;
	HWND GetHelperWindowHandle();

	//List of the data structures for all threads
	static struct t_AsyncSocketExThreadDataList
	{
		t_AsyncSocketExThreadDataList *pNext;
		t_AsyncSocketExThreadData *pThreadData;
	} *m_spAsyncSocketExThreadDataList;

	//Initializes Thread data and helper window, fills m_pLocalAsyncSocketExThreadData
	BOOL InitAsyncSocketExInstance();

	//Destroys helper window after last instance of CAsyncSocketEx in current thread has been closed
	void FreeAsyncSocketExInstance();
};

#endif//_H_ASYNCSOCKETEX_H_
