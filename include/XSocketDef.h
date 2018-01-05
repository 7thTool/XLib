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

#ifndef _H_XSOCKETDEF_H_
#define _H_XSOCKETDEF_H_

#ifdef WIN32

#include <winsock2.h>
#include <WS2tcpip.h>

#ifndef MSG_NOSIGNAL
#define MSG_NOSIGNAL 0
#endif//MSG_NOSIGNAL

/*
 * Windows Sockets errors redefined as regular Berkeley error constants.
 * These are commented out in Windows NT to avoid conflicts with errno.h.
 * Use the WSA constants instead.
 */
#ifdef EWOULDBLOCK
#undef  EWOULDBLOCK
#endif//
#ifdef EINPROGRESS
#undef EINPROGRESS             
#endif//
#ifdef EALREADY
#undef EALREADY                
#endif//
#ifdef ENOTSOCK
#undef ENOTSOCK                
#endif//
#ifdef EDESTADDRREQ
#undef EDESTADDRREQ            
#endif//
#ifdef EMSGSIZE
#undef EMSGSIZE                
#endif//
#ifdef EPROTOTYPE
#undef EPROTOTYPE              
#endif//
#ifdef ENOPROTOOPT
#undef ENOPROTOOPT             
#endif//
#ifdef EPROTONOSUPPORT
#undef EPROTONOSUPPORT         
#endif//
#ifdef ESOCKTNOSUPPORT
#undef ESOCKTNOSUPPORT         
#endif//
#ifdef EOPNOTSUPP
#undef EOPNOTSUPP              
#endif//
#ifdef EPFNOSUPPORT
#undef EPFNOSUPPORT            
#endif//
#ifdef EAFNOSUPPORT
#undef EAFNOSUPPORT            
#endif//
#ifdef EADDRINUSE
#undef EADDRINUSE              
#endif//
#ifdef EADDRNOTAVAIL
#undef EADDRNOTAVAIL           
#endif//
#ifdef ENETDOWN
#undef ENETDOWN                
#endif//
#ifdef ENETUNREACH
#undef ENETUNREACH             
#endif//
#ifdef ENETRESET
#undef ENETRESET               
#endif//
#ifdef ECONNABORTED
#undef ECONNABORTED            
#endif//
#ifdef ECONNRESET
#undef ECONNRESET              
#endif//
#ifdef ENOBUFS
#undef ENOBUFS                 
#endif//
#ifdef EISCONN
#undef EISCONN                 
#endif//
#ifdef ENOTCONN
#undef ENOTCONN                
#endif//
#ifdef ESHUTDOWN
#undef ESHUTDOWN               
#endif//
#ifdef ETOOMANYREFS
#undef ETOOMANYREFS            
#endif//
#ifdef ETIMEDOUT
#undef ETIMEDOUT               
#endif//
#ifdef ECONNREFUSED
#undef ECONNREFUSED            
#endif//
#ifdef ELOOP
#undef ELOOP                   
#endif//
#ifdef ENAMETOOLONG
#undef ENAMETOOLONG            
#endif//
#ifdef EHOSTDOWN
#undef EHOSTDOWN               
#endif//
#ifdef EHOSTUNREACH
#undef EHOSTUNREACH            
#endif//
#ifdef ENOTEMPTY
#undef ENOTEMPTY               
#endif//
#ifdef EPROCLIM
#undef EPROCLIM                
#endif//
#ifdef EUSERS
#undef EUSERS                  
#endif//
#ifdef EDQUOT
#undef EDQUOT                  
#endif//
#ifdef ESTALE
#undef ESTALE                  
#endif//
#ifdef EREMOTE
#undef EREMOTE                 
#endif//

//#ifndef EWOULDBLOCK
#define EWOULDBLOCK             WSAEWOULDBLOCK
//#endif//
//#ifndef EINPROGRESS
#define EINPROGRESS             WSAEINPROGRESS
//#endif//
//#ifndef EALREADY
#define EALREADY                WSAEALREADY
//#endif//
//#ifndef ENOTSOCK
#define ENOTSOCK                WSAENOTSOCK
//#endif//
//#ifndef EDESTADDRREQ
#define EDESTADDRREQ            WSAEDESTADDRREQ
//#endif//
//#ifndef EMSGSIZE
#define EMSGSIZE                WSAEMSGSIZE
//#endif//
//#ifndef EPROTOTYPE
#define EPROTOTYPE              WSAEPROTOTYPE
//#endif//
//#ifndef ENOPROTOOPT
#define ENOPROTOOPT             WSAENOPROTOOPT
//#endif//
//#ifndef EPROTONOSUPPORT
#define EPROTONOSUPPORT         WSAEPROTONOSUPPORT
//#endif//
//#ifndef ESOCKTNOSUPPORT
#define ESOCKTNOSUPPORT         WSAESOCKTNOSUPPORT
//#endif//
//#ifndef EOPNOTSUPP
#define EOPNOTSUPP              WSAEOPNOTSUPP
//#endif//
//#ifndef EPFNOSUPPORT
#define EPFNOSUPPORT            WSAEPFNOSUPPORT
//#endif//
//#ifndef EAFNOSUPPORT
#define EAFNOSUPPORT            WSAEAFNOSUPPORT
//#endif//
//#ifndef EADDRINUSE
#define EADDRINUSE              WSAEADDRINUSE
//#endif//
//#ifndef EADDRNOTAVAIL
#define EADDRNOTAVAIL           WSAEADDRNOTAVAIL
//#endif//
//#ifndef ENETDOWN
#define ENETDOWN                WSAENETDOWN
//#endif//
//#ifndef ENETUNREACH
#define ENETUNREACH             WSAENETUNREACH
//#endif//
//#ifndef ENETRESET
#define ENETRESET               WSAENETRESET
//#endif//
//#ifndef ECONNABORTED
#define ECONNABORTED            WSAECONNABORTED
//#endif//
//#ifndef ECONNRESET
#define ECONNRESET              WSAECONNRESET
//#endif//
//#ifndef ENOBUFS
#define ENOBUFS                 WSAENOBUFS
//#endif//
//#ifndef EISCONN
#define EISCONN                 WSAEISCONN
//#endif//
//#ifndef ENOTCONN
#define ENOTCONN                WSAENOTCONN
//#endif//
//#ifndef ESHUTDOWN
#define ESHUTDOWN               WSAESHUTDOWN
//#endif//
//#ifndef ETOOMANYREFS
#define ETOOMANYREFS            WSAETOOMANYREFS
//#endif//
//#ifndef ETIMEDOUT
#define ETIMEDOUT               WSAETIMEDOUT
//#endif//
//#ifndef ECONNREFUSED
#define ECONNREFUSED            WSAECONNREFUSED
//#endif//
//#ifndef ELOOP
#define ELOOP                   WSAELOOP
//#endif//
//#ifndef ENAMETOOLONG
#define ENAMETOOLONG            WSAENAMETOOLONG
//#endif//
//#ifndef EHOSTDOWN
#define EHOSTDOWN               WSAEHOSTDOWN
//#endif//
//#ifndef EHOSTUNREACH
#define EHOSTUNREACH            WSAEHOSTUNREACH
//#endif//
//#ifndef ENOTEMPTY
#define ENOTEMPTY               WSAENOTEMPTY
//#endif//
//#ifndef EPROCLIM
#define EPROCLIM                WSAEPROCLIM
//#endif//
//#ifndef EUSERS
#define EUSERS                  WSAEUSERS
//#endif//
//#ifndef EDQUOT
#define EDQUOT                  WSAEDQUOT
//#endif//
//#ifndef ESTALE
#define ESTALE                  WSAESTALE
//#endif//
//#ifndef EREMOTE
#define EREMOTE                 WSAEREMOTE
//#endif//

#else //LINUX

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netinet/tcp.h>
#include <fcntl.h>
#include <errno.h>

#ifndef SOCKET
typedef int SOCKET;
#endif//SOCKET

typedef struct sockaddr SOCKADDR;
typedef struct sockaddr *PSOCKADDR;
typedef struct sockaddr FAR *LPSOCKADDR;

typedef struct sockaddr_storage SOCKADDR_STORAGE;
typedef struct sockaddr_storage *PSOCKADDR_STORAGE;
typedef struct sockaddr_storage FAR *LPSOCKADDR_STORAGE;

typedef struct sockaddr_in SOCKADDR_IN;
typedef struct sockaddr_in *PSOCKADDR_IN;
typedef struct sockaddr_in FAR *LPSOCKADDR_IN;

typedef struct linger LINGER;
typedef struct linger *PLINGER;
typedef struct linger FAR *LPLINGER;

typedef struct in_addr IN_ADDR;
typedef struct in_addr *PIN_ADDR;
typedef struct in_addr FAR *LPIN_ADDR;

typedef fd_set FD_SET;
typedef fd_set *PFD_SET;
typedef fd_set FAR *LPFD_SET;

typedef struct hostent HOSTENT;
typedef struct hostent *PHOSTENT;
typedef struct hostent FAR *LPHOSTENT;

typedef struct servent SERVENT;
typedef struct servent *PSERVENT;
typedef struct servent FAR *LPSERVENT;

typedef struct protoent PROTOENT;
typedef struct protoent *PPROTOENT;
typedef struct protoent FAR *LPPROTOENT;

typedef struct timeval TIMEVAL;
typedef struct timeval *PTIMEVAL;
typedef struct timeval FAR *LPTIMEVAL;

#ifndef SOCKET_ERROR
#define SOCKET_ERROR  (-1)  
#endif//SOCKET_ERROR

#ifndef INVALID_SOCKET
#define INVALID_SOCKET  0  
#endif//INVALID_SOCKET

/*
 * WinSock 2 extension -- bit values and indices for FD_XXX network events
 */
#define FD_READ_BIT      0
#define FD_READ          (1 << FD_READ_BIT)

#define FD_WRITE_BIT     1
#define FD_WRITE         (1 << FD_WRITE_BIT)

#define FD_OOB_BIT       2
#define FD_OOB           (1 << FD_OOB_BIT)

#define FD_ACCEPT_BIT    3
#define FD_ACCEPT        (1 << FD_ACCEPT_BIT)

#define FD_CONNECT_BIT   4
#define FD_CONNECT       (1 << FD_CONNECT_BIT)

#define FD_CLOSE_BIT     5
#define FD_CLOSE         (1 << FD_CLOSE_BIT)

#define FD_QOS_BIT       6
#define FD_QOS           (1 << FD_QOS_BIT)

#define FD_GROUP_QOS_BIT 7
#define FD_GROUP_QOS     (1 << FD_GROUP_QOS_BIT)

#define FD_ROUTING_INTERFACE_CHANGE_BIT 8
#define FD_ROUTING_INTERFACE_CHANGE     (1 << FD_ROUTING_INTERFACE_CHANGE_BIT)

#define FD_ADDRESS_LIST_CHANGE_BIT 9
#define FD_ADDRESS_LIST_CHANGE     (1 << FD_ADDRESS_LIST_CHANGE_BIT)

#define FD_MAX_EVENTS    10
#define FD_ALL_EVENTS    ((1 << FD_MAX_EVENTS) - 1)

#endif//

/*!
 *	@brief 代理类型.
 *
 *	定义支持的代理类型
 */
enum
{
	PROXYTYPE_NONE			= 0,	//!< 不使用代理服务器
	PROXYTYPE_SOCKS4,				//!< 使用SOCKS V4代理
	PROXYTYPE_SOCKS4A,				//!< 使用SOCKS V4A代理
	PROXYTYPE_SOCKS5,				//!< 使用SOCKS V5代理
	PROXYTYPE_HTTP10,				//!< 使用HTTP V1.0代理
	PROXYTYPE_HTTP11,				//!< 使用HTTP V1.1代理
};

/*!
 *	@brief 代理服务信息结构.
 *
 *	定义代理信息结构
 */
typedef struct  tagPROXYINFO
{
	int eType; 
	char szHost[260];
	u_short nPort;
	bool bAuth;
	char szUser[260];
	char szPwd[260];
}PROXYINFO,*PPROXYINFO;

#endif//_H_XSOCKETDEF_H_