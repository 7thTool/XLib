#pragma once

#ifndef _H_XUSER_H_
#define _H_XUSER_H_

#include "XLib.h"

#include <ShlObj.h>
#include <Shlwapi.h>
#include <ShellAPI.h>
#include <Psapi.h>
//#pragma comment(lib, "ShlObj.lib")
#pragma comment(lib, "Shlwapi.lib")
//#pragma comment(lib, "ShellAPI.lib")
#pragma comment(lib, "Psapi.lib")

//////////////////////////////////////////////////////////////////////////
///User API
//////////////////////////////////////////////////////////////////////////
XLIB_API int ReversePos(int pos, int count);

//CMM MSG TO STRING
XLIB_API LPCTSTR Msg2Str(UINT uMsg);

//Open or Close Console 
XLIB_API long OpenConsole(LPCTSTR lpszTitle, SHORT wHeight = 300, SHORT wWidth = 80);
XLIB_API void CloseConsole();

//Copy or Move File to Clipboard
XLIB_API BOOL SHCopyClipFileA(LPBYTE lpszFiles, DWORD dwSize, BOOL bCopy = TRUE);
XLIB_API BOOL SHCopyClipFileW(LPBYTE lpszFiles, DWORD dwSize, BOOL bCopy = TRUE);
#ifdef UNICODE
#define SHCopyClipFile SHCopyClipFileW
#else
#define SHCopyClipFile SHCopyClipFileA
#endif//

//Paste Clipboard File to Path
XLIB_API BOOL SHPasteFileA(LPSTR lpszPath);
XLIB_API BOOL SHPasteFileW(LPWSTR lpszPath);
#ifdef UNICODE
#define SHPasteFile SHPasteFileW
#else
#define SHPasteFile SHPasteFileA
#endif//

//Delete File
XLIB_API BOOL SHDeleteFileA(LPBYTE lpszFiles, DWORD dwSize, BOOL bRecycle = TRUE);
XLIB_API BOOL SHDeleteFileW(LPBYTE lpszFiles, DWORD dwSize, BOOL bRecycle = TRUE);
#ifdef UNICODE
#define SHDeleteFile SHDeleteFileW
#else
#define SHDeleteFile SHDeleteFileA
#endif//

//Rename File
XLIB_API BOOL SHRenameFileA(LPCSTR lpszFile, LPCSTR lpszNewFile);
XLIB_API BOOL SHRenameFileW(LPCWSTR lpszFile, LPCWSTR lpszNewFile);
#ifdef UNICODE
#define SHRenameFile SHRenameFileW
#else
#define SHRenameFile SHRenameFileA
#endif//

//Get Spectial Folder
XLIB_API BOOL SHGetFolderPathA(int csidl, LPSTR lpszPath, BOOL bCreate = FALSE);
XLIB_API BOOL SHGetFolderPathW(int csidl, LPWSTR lpszPath, BOOL bCreate = FALSE);
#ifdef UNICODE
#define SHGetFolderPath SHGetFolderPathW
#else
#define SHGetFolderPath SHGetFolderPathA
#endif//

XLIB_API BOOL SHExecuteA(LPCSTR lpszFile);
XLIB_API BOOL SHOpenA(LPCSTR lpszFile);
XLIB_API BOOL SHOpenAsA(LPCSTR lpszFile);
XLIB_API BOOL SHExecuteW(LPCWSTR lpszFile);
XLIB_API BOOL SHOpenW(LPCWSTR lpszFile);
XLIB_API BOOL SHOpenAsW(LPCWSTR lpszFile);
#ifdef UNICODE
#define SHExecute	SHExecuteW
#define SHOpen		SHOpenW
#define SHOpenAs	SHOpenAsW
#else
#define SHExecute	SHExecuteA
#define SHOpen		SHOpenA
#define SHOpenAs	SHOpenAsA
#endif//

//Get File Name From Full Path Name
XLIB_API BOOL PathNameSpecA(LPCSTR lpszFile, LPSTR lpszName, BOOL bExt = TRUE);
XLIB_API BOOL PathNameSpecW(LPCWSTR lpszFile, LPWSTR lpszName, BOOL bExt = TRUE);
#ifdef UNICODE
#define PathNameSpec PathNameSpecW
#else
#define PathNameSpec PathNameSpecA
#endif//

//Get File Path From Full Path Name
XLIB_API BOOL PathPathSpecA(LPCSTR lpszFile, LPSTR lpszPath);
XLIB_API BOOL PathPathSpecW(LPCWSTR lpszFile, LPWSTR lpszPath);
#ifdef UNICODE
#define PathPathSpec PathPathSpecW
#else
#define PathPathSpec PathPathSpecA
#endif//

class XLIB_API CurrentDirectory
{
private:
	BOOL m_bAutoRestore;
	TCHAR m_szCurrentDirectory[MAX_PATH];
public:
	CurrentDirectory(BOOL bAutoRestore = FALSE);
	CurrentDirectory(LPCTSTR lpszCurrentDirectory, BOOL bAutoRestore = TRUE);
	~CurrentDirectory();

	LPCTSTR SetCurrentDirectory(LPCTSTR lpszCurrentDirectory = NULL);
	LPCTSTR GetCurrentDirecttory();
	LPCTSTR RestoreCurrentDirectory();
};

//Get Icon Index From File
XLIB_API INT SHGetIconIndexA(LPCSTR lpszFile, INT IconFlag = SHGFI_SMALLICON);
XLIB_API INT SHGetIconIndexW(LPCWSTR lpszFile, INT IconFlag = SHGFI_SMALLICON);
#ifdef UNICODE
#define SHGetIconIndex SHGetIconIndexW
#else
#define SHGetIconIndex SHGetIconIndexA
#endif//

//Get a Rect in Big Rect
XLIB_API BOOL RectInRect(LPRECT lprc1, LPRECT lprc2);
#if 0
XLIB_API std::ostream& operator << (std::ostream& o,  const RECT& rc);
XLIB_API std::istream& operator >> (std::istream& i, RECT& rc);
XLIB_API std::wostream& operator << (std::wostream& o,  const RECT& rc);
XLIB_API std::wistream& operator >> (std::wistream& i, RECT& rc);
#define _RECT(x)	data_cast<RECT>(x)
#else
XLIB_API RECT String2RectA(LPCSTR lpszRect);
XLIB_API RECT String2RectW(LPCWSTR lpszRect);
XLIB_API LPSTR Rect2StringA(LPSTR lpszStr, LPCRECT lpRect);
XLIB_API LPWSTR Rect2StringW(LPWSTR lpszStr, LPCRECT lpRect);
#ifdef UNICODE
#define _RECT(x)	String2RectW(x)
#define _RECTSTR(s,x)	Rect2StringW(s,x)
#else
#define _RECT(x)	String2RectA(x)
#define _RECTSTR(s,x)	Rect2StringA(s,x)
#endif//
#endif//

#define _COLOR(x)	data_cast(COLORREF)(x)

XLIB_API COLORREF String2ColorA(LPCSTR lpszColor);
XLIB_API COLORREF String2ColorW(LPCWSTR lpszColor);
XLIB_API LPSTR Color2StringA(LPSTR lpszStr, COLORREF cr);
XLIB_API LPWSTR Color2StringW(LPWSTR lpszStr, COLORREF cr);
#ifdef UNICODE
#define _RGB(x)	String2ColorW(x)
#define _RGBSTR(s,x) Color2StringW(s,x)
#else
#define _RGB(x)	String2ColorA(x)
#define _RGBSTR(s,x) Color2StringA(s,x)
#endif//

XLIB_API ULONG HexString2ValueA(LPCSTR lpszValue);
XLIB_API ULONG HexString2ValueW(LPCWSTR lpszValue);
XLIB_API LPSTR Value2HexStringA(LPSTR lpszStr, ULONG nValue);
XLIB_API LPWSTR Value2HexStringW(LPWSTR lpszStr, ULONG nValue);
#ifdef UNICODE
#define _HEXVALUE(x)	HexString2ValueW(x)
#define _HEXSTRING(s,x)	Value2HexStringW(s,x)
#else
#define _HEXVALUE(x)	HexString2ValueA(x)
#define _HEXSTRING(s,x)	Value2HexStringA(s,x)
#endif//

XLIB_API COLORREF HexString2ColorA(LPCSTR lpszValue);
XLIB_API COLORREF HexString2ColorW(LPCWSTR lpszValue);
#ifdef UNICODE
#define _HEXCOLOR(x)	HexString2ColorW(x)
#else
#define _HEXCOLOR(x)	HexString2ColorA(x)
#endif//

XLIB_API LOGFONTA String2LogFontA(LPCSTR lpszValue);
XLIB_API LOGFONTW String2LogFontW(LPCWSTR lpszValue);
XLIB_API LPSTR String2LogFontA(LPSTR lpszStr, LOGFONTA* lgFont);
XLIB_API LPWSTR String2LogFontW(LPWSTR lpszStr, LOGFONTW* lgFont);
#ifdef UNICODE
#define _LOGFONT(x)	String2LogFontW(x)
#define _LOGFONTSTR(s,x) String2LogFontW(s,x)
#else
#define _LOGFONT(x)	String2LogFontA(x)
#define _LOGFONTSTR(s,x) String2LogFontA(s,x)
#endif//

class XLIB_API Clipboard
{
public:
	static HWND GetOwner();
protected:
	HWND m_hOwner;
public:
	Clipboard(HWND hOwner = NULL);
	~Clipboard();

	BOOL Open(HWND hOwner);
	void Close();

	BOOL IsOpen();
	BOOL IsFormatAvailable(UINT uCF);
	//UINT CountFormats();
	//UINT EnumFormats(UINT uCF);
	BOOL Empty();

	BOOL SetData(UINT uCF, LPBYTE lpData, UINT DataLen = 0);
	BOOL GetData(UINT uCF, LPBYTE lpData, UINT* lpDataLen = 0);

	BOOL SetText(LPCSTR lpszText, UINT TextLen);
	BOOL SetText(LPCWSTR lpszText, UINT TextLen);
	BOOL GetText(LPCSTR lpBuffer, UINT* lpBufferLen);
	BOOL GetText(LPCWSTR lpBuffer, UINT* lpBufferLen);

	BOOL SetHtml(LPCSTR lpszHtml, UINT HtmlLen);
	BOOL SetHtml(LPCWSTR lpszHtml, UINT HtmlLen);
};

class XLIB_API COMRegister
{
protected:
	HMODULE m_hModule;
public:
	COMRegister(LPCSTR lpszFile);
	COMRegister(LPCWSTR lpszFile);
	~COMRegister();
};

#include "dbghelp.h"

typedef BOOL (WINAPI *MINIDUMPWRITEDUMP)
(
 IN HANDLE hProcess,
 IN DWORD ProcessId,
 IN HANDLE hFile,
 IN MINIDUMP_TYPE DumpType,
 IN CONST PMINIDUMP_EXCEPTION_INFORMATION ExceptionParam,
 OPTIONAL IN CONST PMINIDUMP_USER_STREAM_INFORMATION UserStreamParam,
 OPTIONAL IN CONST PMINIDUMP_CALLBACK_INFORMATION CallbackParam OPTIONAL
 );

class XLIB_API MiniDumper
{
public:
	MiniDumper(LPCTSTR lpszDumpFile = NULL, BOOL bAutoRestart = FALSE);

public:
	static void SetDumpFile(LPCTSTR lpszDumpFile, BOOL bAutoRestart = FALSE);

protected:
	static TCHAR s_szDumpFile[MAX_PATH];
	static BOOL s_AutoRestart;
	static LPTOP_LEVEL_EXCEPTION_FILTER s_pPrevFilter;
	static long WINAPI  UnhandledExceptionFilter(struct _EXCEPTION_POINTERS *pExceptionInfo);
};

//根据文件路径提取其Thumbnail
HBITMAP XLIB_API SHExtractThumbnail(LPCTSTR lpszFile, UINT nWidth, UINT nHeight);

#ifdef UNICODE

//创建快捷方式
HRESULT XLIB_API SHCreateShortcut(LPCTSTR lpszFile, LPCTSTR lpszLinkFile);

HRESULT XLIB_API SHResolveShortcut(LPCTSTR lpLinkFile, LPTSTR lpszFile);

#endif//

//根据文件后缀得到其默认的打开方式（如.txt对应Notepad.exe）
HRESULT XLIB_API SHGetDefaultAssocApp(LPCTSTR lpszExt, LPTSTR lpszAssocApp);

typedef struct tagAssocAppInfo
{
	WCHAR szAppPath[MAX_PATH];
	WCHAR szDispName[MAX_PATH];
	BOOL bRecomended;
}ASSOCAPPINFO,*LPASSOCAPPINFO;
HRESULT XLIB_API SHGetFileAssocAppEx(LPCTSTR lpszExt, LPASSOCAPPINFO lpAssocAppInfo, LPDWORD lpCount);

//给控件或窗体加ToolTip
BOOL XLIB_API SetToolTip(HINSTANCE hInst, HWND hWnd, LPCTSTR lpszToolTip);

//显示Taskbar and Start Menu Properties对话框
BOOL XLIB_API SHShowTrayPropertiesDialog();

//判断当前用户是否是Admin
BOOL XLIB_API IsUserAdministrator();

//判断当前用户是否打开了UAC（Vista or Win7 OS）
BOOL XLIB_API IsEnableUAC();

//得到当前系统时间，返回特定格式字符串
/*BOOL GetCurrentSystemTime(OUT LPWSTR pszSystemTime)
{
FILETIME fileTime;
SYSTEMTIME systemTime;
SYSTEMTIME localTime;
TIME_ZONE_INFORMATION tzinfo;
GetSystemTimeAsFileTime(&fileTime);
FileTimeToSystemTime(&fileTime, &systemTime);
GetTimeZoneInformation(&tzinfo);
BOOL bSuccess = SystemTimeToTzSpecificLocalTime(&tzinfo, &systemTime, &localTime);
if (bSuccess)
{
wsprintf(pszSystemTime, _T("%04d/%02d/%02d  %02d:%02d:%02d"),
localTime.wYear, localTime.wMonth, localTime.wDay, localTime.wHour, localTime.wMinute, localTime.wSecond);
}
return FALSE;
}*/

template<class Ty>
int BinarySearch(const Ty* x, const Ty* ptr, int max)
{
	ASSERT(x && ptr);
	bool bfind = false;
	int low = 0, high = max-1, mid = 0;
	while(low <= high)
	{
		mid = (high + low) / 2;
		if (ptr[mid] < *x) {
			low = mid + 1;//mid往右移动
		} else if(ptr[mid] > *x) {
			high = mid - 1;//mid往左移动
		} else {
			bfind = true;
			break;
		}
	}
	if (bfind) {
		//找到处理
		return mid;
	} else {
		//没有找到，这里如果处理插入low表示插入位置
		//可以做如下处理
		if(low >= max) {
			//最后追加数据
		} else {
			//前面插入数据，包括最开始位置
		}
	}
	return -1;
}

#endif//_H_XUSER_H_