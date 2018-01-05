#ifndef _H_XSTR_H_
#define _H_XSTR_H_

#include "XChar.h"

namespace std {

	void trimleft(std::string& str);
	void trimright(std::string& str);
	void wtrimleft(std::wstring& str);
	void wtrimright(std::wstring& str);

	typedef std::vector<wstring> wstringarray;
	typedef std::vector<string> stringarray;

#ifdef UNICODE
#define _ttrimleft(str) wtrimleft(str)
#define _ttrimright(str) wtrimright(str)
#else
#define _ttrimleft(str) trimleft(str)
#define _ttrimright(str) trimright(str)
#endif//

#ifdef UNICODE
#define tstring wstring
#define tstringarray wstringarray
#else
#define tstring string
#define tstringarray stringarray
#endif//

	struct stringiequal
	{
		bool operator()(const string& s1, const string& s2) const
		{
			return stricmp(s1.c_str(), s2.c_str()) == 0;
		}
	};
	struct wstringiequal
	{
		bool operator()(const wstring& s1, const wstring& s2) const
		{
			return wcsicmp(s1.c_str(), s2.c_str()) == 0;
		}
	};
	struct stringiless
	{
		bool operator()(const string& s1, const string& s2) const
		{
			return stricmp(s1.c_str(), s2.c_str()) < 0;
		}
	};
	struct wstringiless
	{
		bool operator()(const wstring& s1, const wstring& s2) const
		{
			return wcsicmp(s1.c_str(), s2.c_str()) < 0;
		}
	};
	typedef set<string,stringiless> stringiset;
	typedef set<wstring,wstringiless> wstringiset;
#ifdef UNICODE
#define tstringiequal wstringiequal
#define tstringiless wstringiless
#else
#define tstringiequal stringiequal
#define tstringiless stringiless
#endif//
#define tstringiset set<tstring,tstringiless>

	struct striequal {  
		bool operator() (const char* src, const char* dst) const {
			return stricmp(src, dst) == 0;
		}
	};
	struct strequal {  
		bool operator() (const char* src, const char* dst) const {
			return strcmp(src, dst) == 0;
		}
	};
	struct wcsiequal {  
		bool operator() (const wchar_t* src, const wchar_t* dst) const {
			return wcsicmp(src, dst) == 0;
		}
	};
	struct wcsequal {  
		bool operator() (const wchar_t* src, const wchar_t* dst) const {
			return wcscmp(src, dst) == 0;
		}
	};
	struct striless {  
		bool operator() (const char* src, const char* dst) const {
			return stricmp(src, dst) < 0;
		}
	};
	struct strless {  
		bool operator() (const char* src, const char* dst) const {
			return strcmp(src, dst) < 0;
		}
	};
	struct wcsiless {  
		bool operator() (const wchar_t* src, const wchar_t* dst) const {
			return wcsicmp(src, dst) < 0;
		}
	};
	struct wcsless {  
		bool operator() (const wchar_t* src, const wchar_t* dst) const {
			return wcscmp(src, dst) < 0;
		}
	};
	typedef set<char*,striless> striset;
	typedef set<wchar_t*,wcsiless> wcsiset;
#ifdef UNICODE
#define tcsiequal wcsiequal
#define tcsiless wcsiless
#define tcsiset wcsiset
#else
#define tcsiequal striequal
#define tcsiless striless
#define tcsiset striset
#endif//

} 

#ifndef CP_GBK
#define CP_GBK	936
#endif//

XLIB_API bool mb2wc(const std::string& src, std::wstring& dst, unsigned int cp = CP_OEMCP);
XLIB_API bool wc2mb(const std::wstring& src, std::string& dst, unsigned int cp = CP_OEMCP);

XLIB_API bool utf82wc(const std::string& src, std::wstring& dst);
XLIB_API bool wc2utf8(const std::wstring& src, std::string& dst);

XLIB_API bool utf82mb(const std::string& src, std::string& dst);
XLIB_API bool mb2utf8(const std::string& src, std::string& dst);

XLIB_API std::wstring mb2wc(const std::string& src, unsigned int cp = CP_OEMCP);
XLIB_API std::string wc2mb(const std::wstring& src, unsigned int cp = CP_OEMCP);

XLIB_API std::wstring utf82wc(const std::string& src);
XLIB_API std::string wc2utf8(const std::wstring& src);

XLIB_API std::string utf82mb(const std::string& src);
XLIB_API std::string mb2utf8(const std::string& src);

//////////////////////////////////////////////////////////////////////////


template<typename Target>
Target strto(const std::string& arg, const Target& def = Target())
{
	if (!arg.empty()) {
		try
		{
			Target o;
			std::stringstream ss;
			ss << arg;
			ss >> o;
			return o;
		}
		catch(std::exception& e)
		{

		}
		catch (...)
		{

		}
	}
	return def;
}

template<typename Target>
Target wcsto(const std::wstring& arg, const Target& def = Target())
{
	if (!arg.empty()) {
		try 
		{
			Target o;
			std::wstringstream ss;
			ss << arg;
			ss >> o;
			return o;
		}
		catch(std::exception& e)
		{

		}
		catch (...)
		{

		}
	}
	return def;
}

template<typename Source>
std::string tostr(const Source& arg)
{
	std::ostringstream ss;
	ss << arg;
	return ss.str();
}

template<typename Source>
std::wstring towcs(const Source& arg)
{
	std::wostringstream ss;
	ss << arg;
	return ss.str();
}

template<typename Source>
std::string tostrex(const Source& arg, int p = -1, int w = -1, char c = '0')
{
	std::ostringstream ss;
	if (p>=0) {
		ss.setf(ios::fixed);
		ss.precision(p);
	}
	if (w>=0) {
		ss.width(w);
		ss.fill(c);
	}
	ss << arg;
	return ss.str();
}

template<typename Source>
std::wstring towcsex(const Source& arg, int p = -1, int w = -1, wchar_t c = '0')
{
	std::wostringstream ss;
	if (p>=0) {
		ss.setf(ios::fixed);
		ss.precision(p);
	}
	if (w>=0) {
		ss.width(w);
		ss.fill(c);
	}
	ss << arg;
	return ss.str();
}

#ifdef UNICODE
#define tcsto wcsto
#define ttocs towcs
#define ttocsex towcsex
#else
#define tcsto strto
#define ttocs tostr
#define ttocsex tostrex
#endif//

////#ifndef data_cast
//#include <boost/lexical_cast.hpp>
////#define  data_cast boost::lexical_cast
////#endif//
//template<typename Target, typename Source>
//Target data_cast(const Source& arg, const Target& def = Target())
//{
//	try
//	{
//		return boost::lexical_cast<Target>(arg);
//	}
//	catch(boost::bad_lexical_cast& e)
//	{
//
//	}
//	catch(std::bad_cast& e)
//	{
//
//	}
//	catch(std::exception& e)
//	{
//
//	}
//	catch (...)
//	{
//		
//	}
//	return def;
//}

// #ifdef __T
// #undef __T
// #endif//
// #ifdef UNICODE
// #define __T L ## x
// #else
// #define __T x
// #endif//__T
// 
// #ifdef _T
// #undef _T
// #endif//
// #define _T __T(x)

#define _STRA(x)		tostr<std::string>(x)
#define _STRW(x)		towcs<std::wstring>(x)
#define _SZA(x)			_STRA(x).c_str()
#define _SZW(x)			_STRW(x).c_str()
#ifdef UNICODE
#define _STR(x)		_STRW(x)
#define _SZ(x)		_SZW(x)
#else
#define _STR(x)		_STRA(x)
#define _SZ(x)		_SZA(x)
#endif//

#define _NA(x)		strto<int>(x)
#define _N1A(x)		strto<char)(x)
#define _N2A(x)		strto<short>(x)
#define _N4A(x)		strto<int>(x)
#define _N8A(x)		strto<__int64>(x)
#define _FA(x)		strto<float>(x)
#define _F2A(x)		strto<double>(x)

#define _NW(x)		wcsto<int>(x)
#define _N1W(x)		wcsto<char)(x)
#define _N2W(x)		wcsto<short>(x)
#define _N4W(x)		wcsto<int>(x)
#define _N8W(x)		wcsto<__int64>(x)
#define _FW(x)		wcsto<float>(x)
#define _F2W(x)		wcsto<double>(x)

#ifdef UNICODE
#define _N(x)	_NW(x)
#define _N1(x)	_N1W(x)
#define _N2(x)	_N2W(x)
#define _N4(x)	_N4W(x)
#define _N8(x)	_N8W(x)
#define _F(x)	_FW(x)
#define _F2(x)	_F2W(x)
#else
#define _N(x)	_NA(x)
#define _N1(x)	_N1A(x)
#define _N2(x)	_N2A(x)
#define _N4(x)	_N4A(x)
#define _N8(x)	_N8A(x)
#define _F(x)	_FA(x)
#define _F2(x)	_F2A(x)
#endif//

#endif//_H_XSTR_H_