#ifndef _H_CONFIG_H_
#define _H_CONFIG_H_

#include "XParser.h"
#include "XFile.h"

class XLIB_API ProfileA : public ProfileWriterA
{
public:
	ProfileA();
	ProfileA(LPCSTR lpszFileName, BOOL bCreateIfNotExist = FALSE);
	ProfileA(LPCWSTR lpszFileName, BOOL bCreateIfNotExist = FALSE);
	~ProfileA();

	BOOL Open(LPCSTR lpszFileName, BOOL bCreateIfNotExist = FALSE);
	BOOL Open(LPCWSTR lpszFileName, BOOL bCreateIfNotExist = FALSE);
	void Save(LPCSTR lpszFileName = NULL);
	void Save(LPCWSTR lpszFileName);
	void Close();

	LPCSTR GetFileName(LPSTR lpszFileName, int FileNameLength = MAX_PATH);
	LPCSTR GetFilePath(LPSTR lpszFilePath, int FilePathLength = MAX_PATH);

protected:
	BOOL OpenFile(BOOL bCreateIfNotExist);

	CHAR m_szFileName[MAX_PATH];
};

class XLIB_API ProfileW : public ProfileWriterW
{
public:
	ProfileW();
	ProfileW(LPCSTR lpszFileName, BOOL bCreateIfNotExist = FALSE);
	ProfileW(LPCWSTR lpszFileName, BOOL bCreateIfNotExist = FALSE);
	~ProfileW();

	BOOL Open(LPCSTR lpszFileName, BOOL bCreateIfNotExist = FALSE);
	BOOL Open(LPCWSTR lpszFileName, BOOL bCreateIfNotExist = FALSE);
	void Save(LPCSTR lpszFileName);
	void Save(LPCWSTR lpszFileName = NULL);
	void Close();

	LPCWSTR GetFileName(LPWSTR lpszFileName, int FileNameLength = MAX_PATH);
	LPCWSTR GetFilePath(LPWSTR lpszFilePath, int FilePathLength = MAX_PATH);

protected:
	BOOL OpenFile(BOOL bCreateIfNotExist);

	WCHAR m_szFileName[MAX_PATH];
};

#ifdef UNICODE
#define Profile	ProfileW
#else
#define Profile ProfileA
#endif//

//////////////////////////////////////////////////////////////////////////

class XLIB_API MarkupA : public MarkupWriterA
{
public:
	MarkupA();
	MarkupA(LPCSTR lpszFileName, BOOL bCreateIfNotExist = FALSE);
	MarkupA(LPCWSTR lpszFileName, BOOL bCreateIfNotExist = FALSE);
	~MarkupA();

	BOOL Open(LPCSTR lpszFileName, BOOL bCreateIfNotExist = FALSE);
	BOOL Open(LPCWSTR lpszFileName, BOOL bCreateIfNotExist = FALSE);
	void Save(LPCSTR lpszFileName = NULL);
	void Save(LPCWSTR lpszFileName);
	void Close();

	LPCSTR GetFileName(LPSTR lpszFileName, int FileNameLength = MAX_PATH);
	LPCSTR GetFilePath(LPSTR lpszFilePath, int FilePathLength = MAX_PATH);

protected:
	BOOL OpenFile(BOOL bCreateIfNotExist);

	CHAR m_szFileName[MAX_PATH];
};

class XLIB_API MarkupW : public MarkupWriterW
{
public:
	MarkupW();
	MarkupW(LPCSTR lpszFileName, BOOL bCreateIfNotExist = FALSE);
	MarkupW(LPCWSTR lpszFileName, BOOL bCreateIfNotExist = FALSE);
	~MarkupW();

	BOOL Open(LPCSTR lpszFileName, BOOL bCreateIfNotExist = FALSE);
	BOOL Open(LPCWSTR lpszFileName, BOOL bCreateIfNotExist = FALSE);
	void Save(LPCSTR lpszFileName);
	void Save(LPCWSTR lpszFileName = NULL);
	void Close();

	LPCWSTR GetFileName(LPWSTR lpszFileName, int FileNameLength = MAX_PATH);
	LPCWSTR GetFilePath(LPWSTR lpszFilePath, int FilePathLength = MAX_PATH);

protected:
	BOOL OpenFile(BOOL bCreateIfNotExist);

	WCHAR m_szFileName[MAX_PATH];
};

#ifdef UNICODE
#define Markup	MarkupW
#else
#define Markup	MarkupA
#endif//

#endif//_H_CONFIG_H_