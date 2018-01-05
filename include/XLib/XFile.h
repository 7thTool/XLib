#ifndef _H_FILE_H_
#define _H_FILE_H_

#include "XLib.h"
#include <WinDNS.h>
#if _MSC_VER <= 1200
#ifdef USE_FILE_EX
#undef USE_FILE_EX
#endif//
#define USE_FILE_EX 0
#else
#ifndef USE_FILE_EX
#define USE_FILE_EX 1
#endif//
#endif//
#if USE_FILE_EX
#if WINVER < 0x0600
#include <fileextd.h>
//#pragma comment(lib,"fileextd.lib")
#endif//
#endif//

#ifndef INVALID_HANDLE_VALUE
#define INVALID_HANDLE_VALUE ((HANDLE)(LONG_PTR)-1)
#endif
#ifndef INVALID_FILE_SIZE
#define INVALID_FILE_SIZE ((DWORD)0xFFFFFFFF)
#endif
#ifndef INVALID_SET_FILE_POINTER
#define INVALID_SET_FILE_POINTER ((DWORD)-1)
#endif
#ifndef INVALID_FILE_ATTRIBUTES
#define INVALID_FILE_ATTRIBUTES ((DWORD)-1)
#endif

#ifndef FILE_BEGIN
#define FILE_BEGIN           0
#endif
#ifndef FILE_CURRENT
#define FILE_CURRENT         1
#endif
#ifndef FILE_END
#define FILE_END             2
#endif

enum OpenFlags
{
	modeRead =         (int) 0x00000,
	modeWrite =        (int) 0x00001,
	modeReadWrite =    (int) 0x00002,
	shareCompat =      (int) 0x00000,
	shareExclusive =   (int) 0x00010,
	shareDenyWrite =   (int) 0x00020,
	shareDenyRead =    (int) 0x00030,
	shareDenyNone =    (int) 0x00040,
	modeNoInherit =    (int) 0x00080,
	modeCreate =       (int) 0x01000,
	modeNoTruncate =   (int) 0x02000,
	typeText =         (int) 0x04000, // typeText and typeBinary are
	typeBinary =       (int) 0x08000, // used in derived classes only
	osNoBuffer =       (int) 0x10000,
	osWriteThrough =   (int) 0x20000,
	osRandomAccess =   (int) 0x40000,
	osSequentialScan = (int) 0x80000,
};

enum SeekPosition { begin = FILE_BEGIN, current = FILE_CURRENT, end = FILE_END };

class XLIB_API File
{
public:
	static const HANDLE hFileNull;

	/*enum Attribute
	{
		normal =    0x00,
		readOnly =  0x01,
		hidden =    0x02,
		system =    0x04,
		volume =    0x08,
		directory = 0x10,
		archive =   0x20
	};

	enum SeekPosition { begin = FILE_BEGIN, current = FILE_CURRENT, end = FILE_END };*/

	// Constructors
	File();
	File(HANDLE hFile);
	File(LPCSTR lpszFileName, UINT nOpenFlags);
	File(LPCWSTR lpszFileName, UINT nOpenFlags);
#ifdef USE_FILE_EX
	File(HANDLE hVolume, QWORD qwId, UINT nOpenFlags);
#endif//
	~File();

	// Attributes
	HANDLE m_hFile;
	operator HANDLE () { return m_hFile; }

	// Operations
	BOOL Open(LPCSTR lpszFileName, UINT nOpenFlags);
	BOOL Open(LPCWSTR lpszFileName, UINT nOpenFlags);
#if USE_FILE_EX
	BOOL Open(HANDLE hVolume, QWORD qwId, UINT nOpenFlags);
#endif//
	UINT Read(void* lpBuf, UINT nCount);
	UINT Write(const void* lpBuf, UINT nCount);
	void Flush();
	void Close();
	BOOL IsOpen();

	ULONGLONG Seek(LONGLONG lOff, UINT nFrom);
	ULONGLONG SeekToEnd();
	void SeekToBegin();
	ULONGLONG GetPosition() const;
	void SetLength(ULONGLONG dwNewLen);
	ULONGLONG GetLength() const;

	BOOL GetTime(LPFILETIME lpCreationTime, LPFILETIME lpLastAccessTime, LPFILETIME lpLastWriteTime);
#define GetCreateTime(lpCreationTime)	GetTime((lpCreationTime), NULL, NULL)
#define GetAccessTime(lpLastAccessTime)	GetTime(NULL, (lpLastAccessTime), NULL)
#define GetWriteTime(lpLastWriteTime)	GetTime(NULL, NULL, (lpLastWriteTime))
	BOOL SetTime(const FILETIME* lpCreationTime, const FILETIME* lpLastAccessTime, const FILETIME* lpLastWriteTime);
#define SetCreateTime(lpCreationTime)	SetTime((lpCreationTime), NULL, NULL)
#define SetAccessTime(lpLastAccessTime)	SetTime(NULL, (lpLastAccessTime), NULL)
#define SetWriteTime(lpLastWriteTime)	SetTime(NULL, NULL, (lpLastWriteTime))

	BOOL GetFileInformation(LPBY_HANDLE_FILE_INFORMATION lpFileInformation);
#if USE_FILE_EX
	BOOL GetFileInformationEx(FILE_INFO_BY_HANDLE_CLASS FileInformationClass, LPVOID lpFileInformation, DWORD dwBufferSize);
#endif//
	
	static BOOL RenameFileA(LPCSTR lpszOldName, LPCSTR lpszNewName, BOOL bReplaceIfExists = FALSE);
	static BOOL RenameFileW(LPCWSTR lpszOldName, LPCWSTR lpszNewName, BOOL bReplaceIfExists = FALSE);
#ifdef UNICODE
#define RenameFile	RenameFileW
#else	
#define RenameFile	RenameFileA
#endif//
	static BOOL RemoveFileA(LPCSTR lpszFileName);
	static BOOL RemoveFileW(LPCWSTR lpszFileName);
#ifdef UNICODE
#define RemoveFile	RemoveFileW
#else	
#define RemoveFile	RemoveFileA
#endif//
	static BOOL MoveFileA(LPCSTR lpExistingFileName, LPCSTR lpNewFileName, DWORD dwFlags = 0);
	static BOOL MoveFileW(LPCWSTR lpExistingFileName, LPCWSTR lpNewFileName, DWORD dwFlags = 0);
	static BOOL CopyFileA(LPCSTR lpExistingFileName, LPCSTR lpNewFileName, BOOL bFailIfExists = FALSE);
	static BOOL CopyFileW(LPCWSTR lpExistingFileName, LPCWSTR lpNewFileName, BOOL bFailIfExists = FALSE);
	static BOOL DeleteAndCopyFileA(LPCSTR lpExistingFileName, LPCSTR lpNewFileName, BOOL bFailIfExists = FALSE);
	static BOOL DeleteAndCopyFileW(LPCWSTR lpExistingFileName, LPCWSTR lpNewFileName, BOOL bFailIfExists = FALSE);
	static BOOL CreateDirectoryA(LPCSTR lpszDirectory);
	static BOOL CreateDirectoryW(LPCWSTR lpszDirectory);
	static BOOL RemoveDirectoryA(LPCSTR lpszDirectory, BOOL bExcludeSelfDirectory = FALSE);
	static BOOL RemoveDirectoryW(LPCWSTR lpszDirectory, BOOL bExcludeSelfDirectory = FALSE);
	static BOOL CopyDirectoryA(LPCSTR lpszOldDir, LPCSTR lpszNewDir, BOOL bFailIfExists = FALSE);
	static BOOL CopyDirectoryW(LPCWSTR lpszOldDir, LPCWSTR lpszNewDir, BOOL bFailIfExists = FALSE);
	static BOOL DeleteAndCopyDirectoryA(LPCSTR lpszOldDir, LPCSTR lpszNewDir, BOOL bFailIfExists = FALSE);
	static BOOL DeleteAndCopyDirectoryW(LPCWSTR lpszOldDir, LPCWSTR lpszNewDir, BOOL bFailIfExists = FALSE);
	static BOOL IsFileExistA(LPCSTR lpszFileName);
	static BOOL IsFileExistW(LPCWSTR lpszFileName);
#ifdef UNICODE
#define MoveFile				MoveFileW
#define CopyFile				CopyFileW
#define DeleteAndCopyFile		DeleteAndCopyFileW
#define CreateDirectory			CreateDirectoryW
#define RemoveDirectory			RemoveDirectoryW
#define CopyDirectory			CopyDirectoryW
#define DeleteAndCopyDirectory	DeleteAndCopyDirectoryW
#define IsFileExist				IsFileExistW
#else	
#define MoveFile				MoveFileA
#define CopyFile				CopyFileA
#define DeleteAndCopyFile		DeleteAndCopyFileA
#define CreateDirectory			CreateDirectoryA
#define RemoveDirectory			RemoveDirectoryA
#define CopyDirectory			CopyDirectoryA
#define DeleteAndCopyDirectory	DeleteAndCopyDirectoryA
#define IsFileExist				IsFileExistA
#endif//
	static BOOL IsFile(HANDLE hFile);
};

class XLIB_API FileEx : public File
{
public:
	// Constructors
	FileEx();
	FileEx(HANDLE hFile, DWORD SizeOfHeader);
	FileEx(LPCSTR lpszFileName, UINT nOpenFlags, DWORD SizeOfHeader);
	FileEx(LPCWSTR lpszFileName, UINT nOpenFlags, DWORD SizeOfHeader);
#if USE_FILE_EX
	FileEx(HANDLE hVolume, QWORD qwId, UINT nOpenFlags, DWORD SizeOfHeader);
#endif//
	~FileEx();

	// Operations
	BOOL Open(LPCSTR lpszFileName, UINT nOpenFlags, DWORD SizeOfHeader);
	BOOL Open(LPCWSTR lpszFileName, UINT nOpenFlags, DWORD SizeOfHeader);
#if USE_FILE_EX
	BOOL Open(HANDLE hVolume, QWORD qwId, UINT nOpenFlags, DWORD SizeOfHeader);
#endif//
	void Close();

	UINT ReadHeader(void* lpBuf, UINT nCount);
	UINT WriteHeader(const void* lpBuf, UINT nCount);

	ULONGLONG Seek(LONGLONG lOff, UINT nFrom);
	ULONGLONG SeekToEnd();
	void SeekToBegin();
	ULONGLONG GetPosition() const;
	void SetLength(ULONGLONG dwNewLen);
	ULONGLONG GetLength() const;

protected:
	// Attributes
	DWORD m_SizeOfHeader;
};

class XLIB_API FileIO : public File
{
public:
	// Constructors
	FileIO();
	//FileIO(FILE* pStream);
	FileIO(LPCSTR lpszFileName, UINT nOpenFlags);
	FileIO(LPCWSTR lpszFileName, UINT nOpenFlags);
#ifdef USE_FILE_EX
	FileIO(HANDLE hVolume, QWORD qwId, UINT nOpenFlags);
#endif//
	FileIO(LPCSTR lpszFileName, LPCSTR lpszMode = NULL);
	FileIO(LPCWSTR lpszFileName, LPCWSTR lpszMode = NULL);
	~FileIO();

	// Attributes
	FILE* m_pStream;    // stdio FILE
	FILE* operator()() {return m_pStream; }

	// Operations
	//BOOL Open(FILE* pStream);
	BOOL Open(LPCSTR lpszFileName, UINT nOpenFlags);
	BOOL Open(LPCWSTR lpszFileName, UINT nOpenFlags);
#if USE_FILE_EX
	BOOL Open(HANDLE hVolume, QWORD qwId, UINT nOpenFlags);
#endif//
	BOOL Open(LPCSTR lpszFileName, LPCSTR lpszMode = NULL);
	BOOL Open(LPCWSTR lpszFileName, LPCWSTR lpszMode = NULL);
	void Flush();
	void Close();

	ULONGLONG Seek(LONGLONG lOff, UINT nFrom);
	ULONGLONG SeekToEnd();
	void SeekToBegin();
	ULONGLONG GetPosition() const;
	ULONGLONG GetLength() const;

	// Operations
	UINT Read(void* lpBuf, UINT nCount);
	UINT Write(const void* lpBuf, UINT nCount);
	// reading and writing strings
	BOOL WriteLine(LPCSTR lpszString);
	BOOL WriteLine(LPCWSTR lpszString);
	LPSTR ReadLine(LPSTR lpszString, UINT nMax);
	LPWSTR ReadLine(LPWSTR lpszString, UINT nMax);

protected:
	FILE* OpenHandle(HANDLE hFile, UINT nOpenFlags);
	HANDLE OpenStream(FILE* pStream);
};

enum TextFileEncode
{ 
	MBCS = 0,
	UTF_8,
	UNICODE_LE,
	UNICODE_BE,
};

class XLIB_API TextFile : public FileIO
{
public:
	// Constructors
	TextFile();
	TextFile(LPCSTR lpszFileName, UINT nOpenFlags);
	TextFile(LPCWSTR lpszFileName, UINT nOpenFlags);
#ifdef USE_FILE_EX
	TextFile(HANDLE hVolume, QWORD qwId, UINT nOpenFlags);
#endif//
	TextFile(LPCSTR lpszFileName, LPCSTR lpszMode = NULL);
	TextFile(LPCWSTR lpszFileName, LPCWSTR lpszMode = NULL);
	~TextFile();

	WORD GetCP() { return m_CodePage; }

	// Operations
	BOOL Open(LPCSTR lpszFileName, UINT nOpenFlags);
	BOOL Open(LPCWSTR lpszFileName, UINT nOpenFlags);
#if USE_FILE_EX
	BOOL Open(HANDLE hVolume, QWORD qwId, UINT nOpenFlags);
#endif//
	BOOL Open(LPCSTR lpszFileName, LPCSTR lpszMode = NULL);
	BOOL Open(LPCWSTR lpszFileName, LPCWSTR lpszMode = NULL);
	void Close();

	ULONGLONG Seek(LONGLONG lOff, UINT nFrom);
	ULONGLONG SeekToEnd();
	void SeekToBegin();
	ULONGLONG GetPosition() const;
	void SetLength(ULONGLONG dwNewLen);
	ULONGLONG GetLength() const;

	// Operations
	UINT Write(LPCSTR lpszString, UINT nCount = 0, int nCodePage = CP_OEMCP);
	UINT Write(LPCWSTR lpszString, UINT nCount = 0);
	LPSTR Read(LPSTR lpszString, UINT nMax, int nCodePage = CP_OEMCP);
	LPWSTR Read(LPWSTR lpszString, UINT nMax);
	
	BOOL WriteLine(LPCSTR lpszString, int nCodePage = CP_OEMCP);
	BOOL WriteLine(LPCWSTR lpszString);
	LPSTR ReadLine(LPSTR lpszString, UINT nMax, int nCodePage = CP_OEMCP);
	LPWSTR ReadLine(LPWSTR lpszString, UINT nMax);

protected:
	BOOL OpenFile();
	// Attributes
	WORD m_CodePage;
};

class XLIB_API MemFile : public File
{
public:
	MemFile();
	MemFile(LPCSTR lpszFileName);
	MemFile(LPCWSTR lpszFileName);
	MemFile(LPCSTR lpszFileName, DWORD dwMemorySize);
	MemFile(LPCWSTR lpszFileName, DWORD dwMemorySize);
	~MemFile();

	BOOL Create(LPCSTR lpszFileName, DWORD dwMemorySize);
	BOOL Create(LPCWSTR lpszFileName, DWORD dwMemorySize);
	BOOL Open(LPCSTR lpszFileName, DWORD dwDesiredAccess = FILE_MAP_ALL_ACCESS);
	BOOL Open(LPCWSTR lpszFileName, DWORD dwDesiredAccess = FILE_MAP_ALL_ACCESS);
	void Flush();
	void Close();

	UINT Read(void* lpBuf, UINT nCount);
	UINT Write(const void* lpBuf, UINT nCount);

	ULONGLONG Seek(LONGLONG lOff, UINT nFrom);
	ULONGLONG SeekToEnd();
	void SeekToBegin();
	ULONGLONG GetPosition() const;

	LPVOID MapBuffer(DWORD dwStartAddr, DWORD dwBufferSize);
protected:
	void UnMapBuffer();
	// Attributes
	HANDLE	m_hMapFile;
	DWORD	m_dwDesiredAccess;
	LPVOID	m_lpBuf;
	DWORD	m_dwPosition;
};

#endif//_H_FILE_H_