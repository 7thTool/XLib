#ifndef _H_STAT_H_
#define _H_STAT_H_

#include "XLib.h"
//#include "XFile.h"
//#include "XThread.h"

#define X_STATUS_NONE				0
#define X_STATUS_OK			0x00000001

class XLIB_API XStatus
{
protected:
	unsigned long m_Status;		//�Ѿ���ɵ�״̬
public:
	XStatus();
	~XStatus();

	unsigned long GetStatus();
	void SetStatus(unsigned long status);

	//flag=0 ��ʾֻҪ��һ��״̬�ͷ���true
	//flag=1 ��ʾҪ��statusָ����ȫ��״̬�ŷ���true
	//flag=2 ��ʾֻ��״̬��Ȳŷ���true
	//���򷵻�false
	bool HasStatus(unsigned long status, unsigned long flag = 0);
};

class XLIB_API XStatusEx : public XStatus
{
protected:
	unsigned long m_InStatus;	//���ڴ����״̬
public:
	XStatusEx();
	~XStatusEx();

	unsigned long GetStatus();
	void SetStatus(unsigned long status);
	unsigned long GetInStatus();
	void SetInStatus(unsigned long status);

	//flag=0 ��ʾֻҪ��һ��״̬�ͷ���true
	//flag=1 ��ʾҪ��statusָ����ȫ��״̬�ŷ���true
	//flag=2 ��ʾֻ��״̬��Ȳŷ���true
	//���򷵻�false
	bool HasStatus(unsigned long status, unsigned long flag = 0);
	bool HasInStatus(unsigned long status, unsigned long flag = 0);
};

#pragma pack(1)

enum LogLevel
{ 
	loglevel_debug	= 0,
	loglevel_info,
	loglevel_warn,
	loglevel_error,
	loglevel_fatal,
};
XLIB_API void LogOutputA(LogLevel level, const char* str);
XLIB_API void LogOutputW(LogLevel level, const wchar_t* str);
#ifndef UNICODE
#define LogOutput	LogOutputA
#else
#define LogOutput	LogOutputW
#endif//
#define LogDebug(str)	LogOutput(loglevel_debug,str)
#define LogInfo(str)	LogOutput(loglevel_info,str)
#define LogWarn(str)	LogOutput(loglevel_warn,str)
#define LogError(str)	LogOutput(loglevel_error,str)
#define LogFatal(str)	LogOutput(loglevel_fatal,str)
#if USE_LOG4CXX
extern XLIB_API log4cxx::LoggerPtr logger;
#else
//#define MAX_LOG_RECORD_LENGTH	300
//typedef struct tagLogFileHeadInfo
//{
//	char szName[20];					//�ļ�ͷ����ʶ
//	char szVerName[10];					//�汾��������
//	char szVer[10];						//�汾
//	char szMaxRecordName[20];			//����¼��������
//	char szMaxRecord[10];				//����¼����
//	char szFirstRecordName[20];			//��һ����¼����
//	char szFirstRecord[10];				//��һ����¼λ��
//	char szLastRecordName[20];			//���һ����¼����
//	char szLastRecord[10];				//���һ����¼λ��
//	char szReserved[MAX_LOG_RECORD_LENGTH-20-10-10-20-10-20-10-20-10-2];
//	char szReturn[2];					//����
//}LOGFILEHEADINFO,*PLOGFILEHEADINFO;
//typedef struct tagLogFileRecordInfo
//{
//	char szType[6];						//��־����[��Ϣ]��[����]��[����]
//	char szDateTime[21];				//����ʱ��(2006-06-09 14:08:33)
//	char szSource[23];					//��־��Դ
//#define MAX_LOG_RECORD_CONTENT_LENGTH	(MAX_LOG_RECORD_LENGTH-6-21-23-2)
//	char szContent[MAX_LOG_RECORD_CONTENT_LENGTH];
//	char szReturn[2];						//����
//} LOGFILERECORDINFO,*PLOGFILERECORDINFO;
//#pragma pack()
//class LogFile
//{
//protected:
//	tagLogFileHeadInfo m_HeadInfo;
//	long m_MaxRecord;
//	long m_FirstRecord;
//	long m_LastRecord;
//	long m_CacheRecordCount;
//	std::vector<tagLogFileRecordInfo> m_CacheRecords;
//	File m_File;
//	CriticalSection m_Section;
//public:
//	LogFile(const char* lpszFileName = NULL, tagLogFileHeadInfo* pHeadInfo = NULL);
//	LogFile(const wchar_t* lpszFileName = NULL, tagLogFileHeadInfo* pHeadInfo = NULL);
//
//	BOOL Open(const char* lpszFileName, tagLogFileHeadInfo* pHeadInfo = NULL);
//	BOOL Open(const wchar_t* lpszFileName, tagLogFileHeadInfo* pHeadInfo = NULL);
//	void Close();
//
//	BOOL IsOpen();
//
//	void Flush();
//
//	//��־��Ϊ��Ϣ�����桢����
//	BOOL Write(const char * lpszType,const char * lpszSource, const char * lpszContent);
//	BOOL WriteInfo(const char * lpszSource,const char * lpszFormat, ...);
//	BOOL WriteWarning(const char * lpszSource,const char * lpszFormat, ...);
//	BOOL WriteError(const char * lpszSource,const char * lpszFormat, ...);
//
//protected:
//	BOOL OpenFile(tagLogFileHeadInfo* pHeadInfo);
//
//	//BOOL Write(const void* lpBuf, UINT nCount);
//};
#endif

//////////////////////////////////////////////////////////////////////////

/*class Stat
{
protected:

public:
	Stat();
	~Stat();

	long GetFreq(int n);
};*/

/*enum
{
	LOG_LEVEL_DEBUG = 0,
	LOG_LEVEL_INFO,
	LOG_LEVEL_ERROR,
};
#define LOG_LEVEL_MASK	0X000000FF

enum
{
	LOG_TYPE_NORMAL = 0,
	LOG_TYPE_SUPPORT,
	LOG_TYPE_PROGRAMS,
};
#define LOG_TYPE_MASK	0X0000FF00

enum
{
	LOG_FORMAT_NORMAL = 0,
	LOG_FORMAT_XML,
};
#define LOG_MODULE_MASK	0XFF000000

#define	MAKELOGFLAG(l,t,o,m)	MAKELONG(MAKEWORD(l,t),MAKEWORD(o,m))
#define LOGLEVEL(f)				LOBYTE(LOWORD(f))
#define LOGTYPE(f)				HIBYTE(LOWORD(f))
#define LOGOUT(f)				LOBYTE(HIWORD(f))
#define LOGMODULE(f)			HIBYTE(HIWORD(f))

class Log
{
public:
	//static void Report(long flag, char*);
};*/

#endif//_H_STAT_H_