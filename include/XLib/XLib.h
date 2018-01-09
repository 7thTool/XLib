/*
 * Copyright: 7thTool Open Source (i7thTool@qq.com)
 *
 * Version	: 1.1.1
 * Author	: Scott
 *
 * LICENSED UNDER THE GNU LESSER GENERAL PUBLIC LICENSE, VERSION 2.1 (THE "LICENSE");
 * YOU MAY NOT USE THIS FILE EXCEPT IN COMPLIANCE WITH THE LICENSE.
 * UNLESS REQUIRED BY APPLICABLE LAW OR AGREED TO IN WRITING, SOFTWARE
 * DISTRIBUTED UNDER THE LICENSE IS DISTRIBUTED ON AN "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.
 * SEE THE LICENSE FOR THE SPECIFIC LANGUAGE GOVERNING PERMISSIONS AND
 * LIMITATIONS UNDER THE LICENSE.
 */

#ifndef _H_XLIB_H_
#define _H_XLIB_H_

#ifdef XLIB_CONF

#include <XLibConf.h>

#else

#ifndef XLIB_DLL
#define XLIB_DLL //默认编译成动态链接库
#endif//XLIB_DLL

//VCPKG包管理器
#define USE_VCPKG	0
//XSOCKET网络库
#define USE_SOCKET	1
//LOG4CXX日志
#define USE_LOG4CXX	0
#if USE_LOG4CXX
#include<log4cxx/logger.h>
#include<log4cxx/propertyconfigurator.h>

#pragma comment(lib,"log4cxx.lib")
#endif//
//线程池
#define USE_THREADPOOL 1
//FILEEX
#define USE_FILE_EX	0

#endif//

#ifdef XLIB_DLL
#ifdef XLIB_EXPORTS
#define XLIB_API __declspec(dllexport)
#else
#define XLIB_API __declspec(dllimport)
#ifdef WIN32
#pragma comment(lib, "XLib.lib")
#endif//
#endif//XLIB_EXPORTS
#else
#define	XLIB_API 
#endif//XLIB_DLL

#ifdef WIN32

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN		// 从 Windows 头中排除极少使用的资料
#endif//WIN32_LEAN_AND_MEAN

#include <Windows.h>
#include <WindowsX.h>

#include <tchar.h>
#include <memory.h>

#else //LINUX

#include <sys/types.h> 
#include <sys/syscall.h>
#include <sys/timeb.h>
#include <pthread.h>
#include <signal.h>

#include <stdint.h>
#include <unistd.h>

#endif//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h> 
#include <ctype.h>
#include <time.h>
#include <float.h>
#include <assert.h>
#ifndef ASSERT
#define ASSERT assert
#endif//
#include <exception>
#ifndef VERIFY
#ifndef _DEBUG
#define VERIFY(f) f
#else
#define VERIFY(f) ASSERT(f)
#endif//
#endif//

#include <vector>
#include <deque>
#include <queue>
#include <queue>
#include <list>
#include <set>
#include <map>
#include <string>
#include <memory>
#include <ostream>
#include <fstream>
#include <sstream>
#include <ios>
#include <iomanip>

#include <algorithm>
#include <functional>
using namespace std;

#ifdef WIN32

#ifdef RC_INVOKED

#ifndef int8_t
typedef __int8 int8_t;
#endif//int8_t

#ifndef int16_t
typedef __int16 int16_t;
#endif//int16_t

#ifndef int32_t
typedef __int32 int32_t;
#endif//int32_t

#ifndef int64_t
typedef __int64 int64_t;
#endif//int64_t

#ifndef uint8_t
typedef unsigned __int8 uint8_t;
#endif//uint8_t

#ifndef uint16_t
typedef unsigned __int16 uint16_t;
#endif//uint16_t

#ifndef uint32_t
typedef unsigned __int32 uint32_t;
#endif//uint32_t

#ifndef uint64_t
typedef unsigned __int64 uint64_t;
#endif//uint64_t

#endif//

#else

#define __stdcall __attribute__((__stdcall__))
#define __cdecl __attribute__((__cdecl__))
//Linux下如果函数不指定调用约定，默认的情况应该是__attribute__((__cdecl__))

#ifndef FAR
#define FAR
#endif//FAR

#ifndef TCHAR
#ifdef UNICODE
typedef wchar_t TCHAR;
typedef wchar_t tchar;
#define _T(x)	L ## x 
#else
typedef char	TCHAR;
typedef char	tchar;
#define _T(x)	x
#endif//UNICODE 
#endif//TCHAR

#ifndef __int8
typedef int8_t __int8;
#endif//__int8

#ifndef __int16
typedef int16_t __int16;
#endif//__int16

#ifndef __int32
typedef int32_t __int32;
#endif//__int32

#ifndef __int64
typedef int64_t __int64;
#endif//__int64

#endif//

#ifndef byte
typedef unsigned char byte;
#endif//byte

#ifndef _countof
#define _countof(array) (sizeof(array)/sizeof(array[0]))
#endif

#ifndef _countxofy
#define _countxofy(x,y) ((y+((x)-1))/(x))
#define _countofx(x,o) _countxofy(o,x)
#endif

#define _hasallof(s,f) (((s)&(f))==(f))
#define _hasoneof(s,f) ((s)&(f))

#define _reversepos(pos,cnt) ((cnt)-((pos)+1))

#define	RLT_UNKNOWN		-1
#define RLT_NULL		0
#define RLT_FAILED		0
#define RLT_FALSE		0
#define RLT_OK			1
#define RLT_TRUE		1

#ifndef PRINTF
#ifdef _DEBUG
#define PRINTF(format,...) printf(format, ##__VA_ARGS__)
#define WPRINTF(format,...) wprintf(format, ##__VA_ARGS__)
#define PRINTFL(format,...) printf(format, ##__VA_ARGS__);printf("\n")
#define WPRINTFL(format,...) wprintf(format, ##__VA_ARGS__);wprintf(L"\n")
#else
#define PRINTF(format,...) 
#define WPRINTF(format,...) 
#define PRINTFL(format,...)
#define WPRINTFL(format,...)
#endif//
#endif//
//#ifdef __DEBUG__  
//#define DEBUG(format,...) printf("File: "__FILE__", Line: %05d: "format"\n", __LINE__, ##__VA_ARGS__)  
//#else  
//#define DEBUG(format,...) 
//#endif 

#define MAKEINT16(l, h)     ((__int16)(((__int8)(((__int16)(l)) & 0xff)) | ((__int16)((__int8)(((__int16)(h)) & 0xff))) << 8))
#define LOINT8(x)           ((__int8)(((__int16)(x)) & 0xff))
#define HIINT8(x)           ((__int8)((((__int16)(x)) >> 8) & 0xff))
#define MAKESHORT			MAKEINT16			
//#define MAKEWORD			MAKEINT16
//#define LOBYTE			LOINT8
//#define HIBYTE			HIINT8
#define MAKEINT32(l,h)		((__int32)(((__int16)(((__int32)(l)) & 0xffff)) | ((__int32)((__int16)(((__int32)(h)) & 0xffff))) << 16))
#define LOINT16(l)          ((__int16)(((__int32)(l)) & 0xffff))
#define HIINT16(l)          ((__int16)((((__int32)(l)) >> 16) & 0xffff))
//#define MAKEDWORD			MAKEINT32
#define MAKEINT				MAKEINT32
//#define MAKELONG			MAKEINT32
//#define LOWORD			LOINT16
//#define HIWORD			HIINT16
#define LOSHORT				LOINT16
#define HISHORT				HIINT16
#define MAKEINT64(l,h)		((__int64)(((__int32)((__int64)(l) & 0xffffffff)) | ((__int64)((__int32)(((__int64)(h)) & 0xffffffff))) << 32))
#define LOINT32(x)          ((__int32)(((__int64)(x)) & 0xffffffff))
#define HIINT32(x)          ((__int32)((((__int64)(x)) >> 32) & 0xffffffff))
//#define MAKEDWORDLONG		MAKEINT64
#define MAKELONGLONG		MAKEINT64
//#define LODWORD			LOINT32
//#define HIDWORD			HIINT32
#define LOINT				LOINT32
#define HIINT				HIINT32
#define LOLONG				LOINT32
#define HILONG				HIINT32

#define	_E					(2.7182818285)
#define _e					_E
#define	_PI					(3.1415926536)
#define _pi					_PI
#define	_zero_ctrl_			(1e-10)
#define	_over_ctrl_			(1e15)
#define _precision_			(1e-6)
#define _decimaldigits_		(7)
#define _decimaldigitsex_	(15)
#define _error_float_		(1e16)
#define _4_5_				(0.500001) // 四舍五入校正值，0.5是为了四舍五入，0.000001是为了校正浮点数值

#define IsZeroFloat(v)		((v)>(-_zero_ctrl_) && (v)<_zero_ctrl_)
#define IsOverFloat(v)		((v)<(-_over_ctrl_) || (v)>_over_ctrl_)
#define IsEqualFloat(x,y)	((x)>((y)-_zero_ctrl_) && (x)<((y)+_zero_ctrl_))

#ifndef MAX
#define MAX(a,b)	(((a) > (b)) ? (a) : (b))
#endif//

#ifndef MIN
#define MIN(a,b)	(((a) < (b)) ? (a) : (b))
#endif//

#ifndef MID
#define MID(a,b,c)	((a)>(b)&&(a)<(c)?(a):((b)>(a)&&(b)<(c)?(b):(c)))
//#define MID(a,b,c)	MAX(MIN(a,b),MIN(MAX(a,b),c))
#endif//

#ifndef ABS
#define ABS(x)		((x) >=0 ? (x) : -(x))
#endif//

#ifndef INFINITE
#define INFINITE            0xFFFFFFFF  // Infinite timeout
#endif//INFINITE

#ifdef WIN32

#ifndef snprintf
#define snprintf _snprintf
#endif//snprintf

#else

#ifndef gettid
#define gettid() syscall(__NR_gettid)
#endif//

#ifndef stricmp
#define stricmp strcasecmp 
#endif//stricmp

#ifndef strnicmp
#define strnicmp strncasecmp 
#endif//strnicmp

#ifndef _snprintf
#define _snprintf snprintf
#endif//_snprintf

unsigned long XLIB_API GetTickCount();

#endif//

/*!
 *	@brief XLib 全局初始化对象.
 *
 *	使用XLib之前需要先创建全局的XLibInit对象
 *	eg: XLibInit g_XLibInit;
 */
class XLIB_API XLibInit
{
private:
	static long s_bInit;
public:
	static void Init();
	static void Release();

public:
	XLibInit();
	~XLibInit();
};

#endif//_H_XLIB_H_
