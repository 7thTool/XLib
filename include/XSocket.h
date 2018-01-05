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

#ifndef _H_XSOCKET_H_
#define _H_XSOCKET_H_

#include "XLib.h"
#include "XSocketDef.h"

/*!
 *	@brief Socket封装.
 *
 *	封装Socket的基本操作
 */
class XLIB_API Socket
{
public:
	static long InitNetEnv();
	static void ReleaseNetEnv();

	static u_long H2N(u_long n);
	static u_long N2H(u_long n);
	static u_short H2N(u_short n);
	static u_short N2H(u_short n);

	static SOCKET Create(int nSockAf, int nSockType, int nSockProtocol);

	static bool IsSocket(SOCKET Sock);

	enum { Receives = 0, Sends = 1, Both = 2 };
	static int ShutDown(SOCKET Sock, int nHow);
	static int Close(SOCKET Sock);
	
	static SOCKET Accept(SOCKET Sock, SOCKADDR* lpSockAddr, int* lpSockAddrLen);
	static int Bind(SOCKET Sock, const SOCKADDR* lpSockAddr, int nSockAddrLen);
	static int Connect(SOCKET Sock, const SOCKADDR* lpSockAddr, int nSockAddrLen);
	static int Listen(SOCKET Sock, int nConnectionBacklog);
	static int Send(SOCKET Sock, const char* lpBuf, int nBufLen, int nFlags = MSG_NOSIGNAL);
	static int Receive(SOCKET Sock, char* lpBuf, int nBufLen, int nFlags = MSG_NOSIGNAL);
	//static int SyncSend(SOCKET Sock, const char* lpBuf, int nBufLen, int nFlags = MSG_NOSIGNAL);
	//static int SyncReceive(SOCKET Sock, char* lpBuf, int nBufLen, int nFlags = MSG_NOSIGNAL);
	static int SendTo(SOCKET Sock, const char* lpBuf, int nBufLen,
		const SOCKADDR* lpSockAddr = 0, int nSockAddrLen = 0, int nFlags = MSG_NOSIGNAL);
	static int ReceiveFrom(SOCKET Sock, char* lpBuf, int nBufLen, 
		SOCKADDR* lpSockAddr = 0, int* lpSockAddrLen = 0, int nFlags = MSG_NOSIGNAL);

	static int IOCtl(SOCKET Sock, long lCommand, u_long* lpArgument);
	static int IOCtl(SOCKET Sock, long lCommand, u_long Argument);
	static int GetSockOpt(SOCKET Sock, int nLevel, int nOptionName, void* lpOptionValue, int* lpOptionLen);
	static int GetSockOpt(SOCKET Sock, int nLevel, int nOptionName, void* lpOptionValue, int nOptionLen);
	static int SetSockOpt(SOCKET Sock, int nLevel, int nOptionName, const void* lpOptionValue, int nOptionLen);
	static int SetSendTimeOut(SOCKET Sock, int TimeOut);
	static int SetRecvTimeOut(SOCKET Sock, int TimeOut);
	static int GetSendTimeOut(SOCKET Sock);
	static int GetRecvTimeOut(SOCKET Sock);
	static int SetKeepAlive(SOCKET Sock, u_long onoff, u_long time = 30*1000);

	static int GetPeerName(SOCKET Sock, SOCKADDR* lpSockAddr, int* lpSockAddrLen);
	static int GetSockName(SOCKET Sock, SOCKADDR* lpSockAddr, int* lpSockAddrLen);

	static int GetLastError();
	static void SetLastError(int nError);

	static int GetErrorMessageA(int nError, char* lpszMessage, int nMessageLen);
	static int GetErrorMessageW(int nError, wchar_t* lpszMessage, int nMessageLen);
#ifdef UNICODE
#define GetErrorMessage GetErrorMessageW
#else
#define GetErrorMessage GetErrorMessageA
#endif//

protected:
	SOCKET m_Sock;
public:
	Socket(SOCKET Sock = INVALID_SOCKET):m_Sock(Sock) { }
	~Socket() { }

	operator SOCKET() const { return m_Sock; }

	bool IsSocket() { 
#if 0
		if(IsSocket(m_Sock)) {
			int OptVal = 0;
			if(GetSockOpt(SO_TYPE, (void*)&OptVal, sizeof OptVal) == SOCKET_ERROR) {
				if(GetLastError() == WSAENOTSOCK) {
					m_Sock = INVALID_SOCKET;
					return false;
				}
			}
			return true;
		}
		return false;
#else
		return IsSocket(m_Sock);
#endif//
	}

	SOCKET Create(int nSockAf = AF_INET, int nSockType = SOCK_STREAM) { return Attach(Create(nSockAf, nSockType, 0)); }
	SOCKET Attach(SOCKET Sock) { SOCKET oSock = m_Sock; m_Sock = Sock; return oSock; }
	SOCKET Detach() { return Attach(INVALID_SOCKET); }
	int ShutDown(int nHow = Both) { return ShutDown(m_Sock, nHow); }
	int Close() { if (IsSocket()) { return Close(Detach()); } return 0; }

	SOCKET Accept(SOCKADDR* lpSockAddr, int* lpSockAddrLen) { return Accept(m_Sock, lpSockAddr, lpSockAddrLen); }
	int Bind(const SOCKADDR* lpSockAddr, int nSockAddrLen) { return Bind(m_Sock, lpSockAddr, nSockAddrLen); }
	int Connect(const SOCKADDR* lpSockAddr, int nSockAddrLen) { return Connect(m_Sock, lpSockAddr, nSockAddrLen); }
	int Listen(int nConnectionBacklog = 5) { return Listen(m_Sock, nConnectionBacklog); }
	int Send(const char* lpBuf, int nBufLen, int nFlags = MSG_NOSIGNAL) { return Send(m_Sock, lpBuf, nBufLen, nFlags); }
	int Receive(char* lpBuf, int nBufLen, int nFlags = MSG_NOSIGNAL) { return Receive(m_Sock, lpBuf, nBufLen, nFlags); }
	//int SyncSend(const char* lpBuf, int nBufLen, int nFlags = MSG_NOSIGNAL) { return SyncSend(m_Sock, lpBuf, nBufLen, nFlags); }
	//int SyncReceive(char* lpBuf, int nBufLen, int nFlags = MSG_NOSIGNAL) { return SyncReceive(m_Sock, lpBuf, nBufLen, nFlags); }
	int SendTo(const char* lpBuf, int nBufLen, const SOCKADDR* lpSockAddr, int nSockAddrLen, int nFlags = MSG_NOSIGNAL) 
	{ return SendTo(m_Sock, lpBuf, nBufLen, lpSockAddr, nSockAddrLen, nFlags); }
	int ReceiveFrom(char* lpBuf, int nBufLen, SOCKADDR* lpSockAddr, int* lpSockAddrLen, int nFlags = MSG_NOSIGNAL)
	{ return ReceiveFrom(m_Sock, lpBuf, nBufLen, lpSockAddr, lpSockAddrLen, nFlags); }

	int GetPeerName(SOCKADDR* lpSockAddr, int* lpSockAddrLen) { return GetPeerName(m_Sock, lpSockAddr, lpSockAddrLen); }
	int GetSockName(SOCKADDR* lpSockAddr, int* lpSockAddrLen) { return GetSockName(m_Sock, lpSockAddr, lpSockAddrLen); }

	int IOCtl(long lCommand, u_long* lpArgument) { return IOCtl(m_Sock, lCommand, lpArgument); }
	int IOCtl(long lCommand, u_long Argument)  { return IOCtl(m_Sock, lCommand, Argument); }
	int GetSockOpt(int nLevel, int nOptionName, void* lpOptionValue, int nOptionLen) 
	{ return GetSockOpt(m_Sock, nLevel, nOptionName, lpOptionValue, &nOptionLen); }
	int SetSockOpt(int nLevel, int nOptionName, const void* lpOptionValue, int nOptionLen)
	{	return SetSockOpt(m_Sock, nLevel, nOptionName, lpOptionValue, nOptionLen); }
	int SetSockOpt(int nLevel, int nOptionName, u_long OptionValue)
	{ return SetSockOpt(m_Sock, nLevel, nOptionName, &OptionValue, sizeof(OptionValue)); }
	int SetSendTimeOut(int TimeOut) { return SetSendTimeOut(m_Sock, TimeOut); }
	int SetRecvTimeOut(int TimeOut) { return SetRecvTimeOut(m_Sock, TimeOut); }
	int GetSendTimeOut() { return GetSendTimeOut(m_Sock); }
	int GetRecvTimeOut() { return GetRecvTimeOut(m_Sock); }
	int SetKeepAlive(u_long onoff, u_long time = 30*1000) { return SetKeepAlive(m_Sock, onoff, time); }
};

#endif//_H_XSOCKET_H_