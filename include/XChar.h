#ifndef _H_XCHAR_H_
#define _H_XCHAR_H_

#include "XLib.h"

//////////////////////////////////////////////////////////////////////////
///注意：反向处理的的源指针要指向最后，正向处理源指向开始位置，原则就是执行最先开始处理的位置

XLIB_API int wmemicmp(wchar_t * src, wchar_t * dst, size_t count);
XLIB_API int memrcmp(char * src, char * dst, size_t rcount);
XLIB_API int wmemrcmp(wchar_t * src, wchar_t * dst, size_t rcount);
XLIB_API int memricmp(char * src, char * dst, size_t rcount);
XLIB_API int wmemricmp(wchar_t * src, wchar_t * dst, size_t rcount);

#define strrcmp(src,dst) memrcmp(src,dst,strlen(dst))
#define strrnicmp(src,dst,len) memricmp(src,dst,len)
#define wcsrcmp(src,dst) wmemrcmp(src,dst,wcslen(dst))
#define wcsrnicmp(src,dst,len) wmemricmp(src,dst,len)
#ifdef UNICODE
#define _tcsrcmp wcsrcmp
#define _tcsrnicmp wcsrnicmp
#else
#define _tcsrcmp strrcmp
#define _tcsrnicmp strrnicmp
#endif//

XLIB_API char* memmem(char * src, size_t srclen, char * dst, size_t dstlen);
XLIB_API wchar_t* wmemmem(wchar_t * src, size_t srclen, wchar_t * dst, size_t dstlen);
XLIB_API char* memimem(char * src, size_t srclen, char * dst, size_t dstlen);
XLIB_API wchar_t* wmemimem(wchar_t * src, size_t srclen, wchar_t * dst, size_t dstlen);
XLIB_API char* memrmem(char * src, size_t srcrlen, char * dst, size_t dstrlen);
XLIB_API wchar_t* wmemrmem(wchar_t * src, size_t srcrlen, wchar_t * dst, size_t dstrlen);
XLIB_API char* memrimem(char * src, size_t srcrlen, char * dst, size_t dstrlen);
XLIB_API wchar_t* wmemrimem(wchar_t * src, size_t srcrlen, wchar_t * dst, size_t dstrlen);
XLIB_API char* memrep(char* src, int srclen, char* dst, int dstlen, char* r, int rlen, char* t, int tlen);
XLIB_API wchar_t* wmemrep(wchar_t* src, int srclen, wchar_t* dst, int dstlen, wchar_t* r, int rlen, wchar_t* t, int tlen);
XLIB_API char* memirep(char* src, int srclen, char* dst, int dstlen, char* r, int rlen, char* t, int tlen);
XLIB_API wchar_t* wmemirep(wchar_t* src, int srclen, wchar_t* dst, int dstlen, wchar_t* r, int rlen, wchar_t* t, int tlen);
XLIB_API char* memtrimleft(char* src, int srclen, char* spec, int speclen);
XLIB_API wchar_t* wmemtrimleft(wchar_t* src, int srclen, wchar_t* spec, int speclen);
XLIB_API char* memtrimright(char* src, int srclen, char* spec, int speclen);
XLIB_API wchar_t* wmemtrimright(wchar_t* src, int srclen, wchar_t* spec, int speclen);
XLIB_API char* memtrim(char* src, int srclen, char* spec, int speclen);
XLIB_API wchar_t* wmemtrim(wchar_t* src, int srclen, wchar_t* spec, int speclen);
XLIB_API char* memskip(char* src, int srclen, char* spec, int speclen);
XLIB_API wchar_t* wmemskip(wchar_t* src, int srclen, wchar_t* spec, int speclen);
XLIB_API char* memrskip(char* src, int srcrlen, char* spec, int speclen);
XLIB_API wchar_t* wmemrskip(wchar_t* src, int srcrlen, wchar_t* spec, int speclen);
XLIB_API char* membrk(char* src, int srclen, char* spec, int speclen);
XLIB_API wchar_t* wmembrk(wchar_t* src, int srclen, wchar_t* spec, int speclen);
XLIB_API char* memrbrk(char* src, int srcrlen, char* spec, int speclen);
XLIB_API wchar_t* wmemrbrk(wchar_t* src, int srcrlen, wchar_t* spec, int speclen);
XLIB_API char* memnext(char* src, int srclen, char* spec, int speclen, char* cat, int catlen, char* brk, int brklen);
XLIB_API wchar_t* wmemnext(wchar_t* src, int srclen, wchar_t* spec, int speclen, wchar_t* cat, int catlen, wchar_t* brk, int brklen);
XLIB_API char* memnexts(char* src, int srclen, char* spec, int speclen, char* cat, int catlen, char* brk, int brklen);
XLIB_API wchar_t* wmemnexts(wchar_t* src, int srclen, wchar_t* spec, int speclen, wchar_t* cat, int catlen, wchar_t* brk, int brklen);
XLIB_API char* meminexts(char* src, int srclen, char* spec, int speclen, char* cat, int catlen, char* brk, int brklen);
XLIB_API wchar_t* wmeminexts(wchar_t* src, int srclen, wchar_t* spec, int speclen, wchar_t* cat, int catlen, wchar_t* brk, int brklen);

extern XLIB_API const char* strnull();
extern XLIB_API const wchar_t* wcsnull();
#ifdef UNICODE
#define _tcsnull wcsnull
#else
#define _tcsnull strnull
#endif//

XLIB_API char* strnstr(char* string, int len, char* spec);
XLIB_API wchar_t* wcsnstr(wchar_t* string, int len, wchar_t* spec);
#ifdef UNICODE
#define _tcsnstr wcsnstr
#else
#define _tcsnstr strnstr
#endif//

XLIB_API char* stristr(char* str1, char* str2);
XLIB_API wchar_t* wcsistr(wchar_t * str1, wchar_t* str2);
#ifdef UNICODE
#define _tcsistr wcsistr
#else
#define _tcsistr stristr
#endif//

XLIB_API char* strnistr(char* string, int len, char* spec);
XLIB_API wchar_t* wcsnistr(wchar_t* string, int len, wchar_t* spec);
//#define strnistr memimem
//#define wcsnistr wmemimem
#ifdef UNICODE
#define _tcsnistr wcsnistr
#else
#define _tcsnistr strnistr
#endif//

XLIB_API char* strrstr(char* str1, char* str2);
XLIB_API wchar_t* wcsrstr(wchar_t * str1, wchar_t* str2);
#ifdef UNICODE
#define _tcsrstr wcsrstr
#else
#define _tcsrstr strrstr
#endif//

XLIB_API char* strnrstr(char* string, int rlen, char* spec);
XLIB_API wchar_t* wcsnrstr(wchar_t* string, int rlen, wchar_t* spec);
#ifdef UNICODE
#define _tcsnrstr wcsnrstr
#else
#define _tcsnrstr strnrstr
#endif//

XLIB_API char* strirstr(char* str1, char* str2);
XLIB_API wchar_t* wcsirstr(wchar_t * str1, wchar_t* str2);
#ifdef UNICODE
#define _tcsirstr wcsirstr
#else
#define _tcsirstr strirstr
#endif//

XLIB_API char* strnirstr(char* string, int rlen, char* spec);
XLIB_API wchar_t* wcsnirstr(wchar_t* string, int rlen, wchar_t* spec);
//#define strnistr memimem
//#define wcsnistr wmemimem
#ifdef UNICODE
#define _tcsnirstr wcsnirstr
#else
#define _tcsnirstr strnirstr
#endif//

XLIB_API char* strichr(char * string, int ch);
XLIB_API wchar_t* wcsichr(wchar_t * string, int ch);
#ifdef UNICODE
#define _tcsichr wcsichr
#else
#define _tcsichr strichr
#endif//

XLIB_API char* strnchr(char* string, int len, char ch);
XLIB_API wchar_t* wcsnchr(wchar_t* string, int len, wchar_t ch);
#ifdef UNICODE
#define _tcsnchr wcsnchr
#else
#define _tcsnchr strnchr
#endif//

XLIB_API char* strnichr(char* string, int len, char ch);
XLIB_API wchar_t* wcsnichr(wchar_t* string, int len, wchar_t ch);
#ifdef UNICODE
#define _tcsnichr wcsnichr
#else
#define _tcsnichr strnichr
#endif//

char* strtrimleft(char* string, char* spec);
wchar_t* wcstrimleft(wchar_t* string, wchar_t* spec);
#ifdef UNICODE
#define _tcstrimleft wcstrimleft
#else
#define _tcstrimleft strtrimleft
#endif//

char* strntrimleft(char* string, int len, char* spec);
wchar_t* wcsntrimleft(wchar_t* string, int len, wchar_t* spec);
#ifdef UNICODE
#define _tcsntrimleft wcsntrimleft
#else
#define _tcsntrimleft strntrimleft
#endif//

char* strtrimright(char* string, char* spec);
wchar_t* wcstrimright(wchar_t* string, wchar_t* spec);
#ifdef UNICODE
#define _tcstrimright wcstrimright
#else
#define _tcstrimright strtrimright
#endif//

char* strntrimright(char* string, int len, char* spec);
wchar_t* wcsntrimright(wchar_t* string, int len, wchar_t* spec);
//#define strntrimright memtrimright
//#define wcsntrimright wmemtrimright
#ifdef UNICODE
#define _tcsntrimright wcsntrimright
#else
#define _tcsntrimright strntrimright
#endif//

char* strtrim(char* string, char* spec);
wchar_t* wcstrim(wchar_t* string, wchar_t* spec);
#ifdef UNICODE
#define _tcstrim wcstrim
#else
#define _tcstrim strtrim
#endif//

char* strntrim(char* string, int len, char* spec);
wchar_t* wcsntrim(wchar_t* string, int len, wchar_t* spec);
#ifdef UNICODE
#define _tcsntrim wcsntrim
#else
#define _tcsntrim strntrim
#endif//

XLIB_API char* strrep(char* string, char* r, char* t);
XLIB_API wchar_t* wcsrep(wchar_t* string, wchar_t* r, wchar_t* t);
#ifdef UNICODE
#define _tcsrep wcsrep
#else
#define _tcsrep strrep
#endif//

XLIB_API char* strnrep(char* string, int len, char* r, char* t);
XLIB_API wchar_t* wcsnrep(wchar_t* string, int len, wchar_t* r, wchar_t* t);
#ifdef UNICODE
#define _tcsnrep wcsnrep
#else
#define _tcsnrep strnrep
#endif//

XLIB_API char* strirep(char* string, char* r, char* t);
XLIB_API wchar_t* wcsirep(wchar_t* string, wchar_t* r, wchar_t* t);
#ifdef UNICODE
#define _tcsirep wcsirep
#else
#define _tcsirep strirep
#endif//

XLIB_API char* strnirep(char* string, int len, char* r, char* t);
XLIB_API wchar_t* wcsnirep(wchar_t* string, int len, wchar_t* r, wchar_t* t);
#ifdef UNICODE
#define _tcsnirep wcsnirep
#else
#define _tcsnirep strnirep
#endif//

XLIB_API char* strrepchr(char* string, char r, char t);
XLIB_API wchar_t* wcsrepchr(wchar_t* string, wchar_t r, wchar_t t);
#ifdef UNICODE
#define _tcsrepchr wcsrepchr
#else
#define _tcsrepchr strrepchr
#endif//

XLIB_API char* strnrepchr(char* string, int len, char r, char t);
XLIB_API wchar_t* wcsnrepchr(wchar_t* string, int len, wchar_t r, wchar_t t);
#ifdef UNICODE
#define _tcsnrepchr wcsnrepchr
#else
#define _tcsnrepchr strnrepchr
#endif//

XLIB_API char* strirepchr(char* string, char r, char t);
XLIB_API wchar_t* wcsirepchr(wchar_t* string, wchar_t r, wchar_t t);
#ifdef UNICODE
#define _tcsirepchr wcsirepchr
#else
#define _tcsirepchr strirepchr
#endif//

XLIB_API char* strnirepchr(char* string, int len, char r, char t);
XLIB_API wchar_t* wcsnirepchr(wchar_t* string, int len, wchar_t r, wchar_t t);
#ifdef UNICODE
#define _tcsnirepchr wcsnirepchr
#else
#define _tcsnirepchr strnirepchr
#endif//

//skip left spec
XLIB_API char* strskipchr(char* string, char spec);
XLIB_API wchar_t* wcsskipchr(wchar_t* string, wchar_t spec);
#ifdef UNICODE
#define _tcsskipchr wcsskipchr
#else
#define _tcsskipchr strskipchr
#endif//

XLIB_API char* strnskipchr(char* string, int len, char spec);
XLIB_API wchar_t* wcsnskipchr(wchar_t* string, int len, wchar_t spec);
#ifdef UNICODE
#define _tcsnskipchr wcsnskipchr
#else
#define _tcsnskipchr strnskipchr
#endif//

XLIB_API char* strskip(char* string, char* spec);
XLIB_API wchar_t* wcsskip(wchar_t* string, wchar_t* spec);
#ifdef UNICODE
#define _tcsskip wcsskip
#else
#define _tcsskip strskip
#endif//

XLIB_API char* strnskip(char* string, int len, char* spec);
XLIB_API wchar_t* wcsnskip(wchar_t* string, int len, wchar_t* spec);
#ifdef UNICODE
#define _tcsnskip wcsnskip
#else
#define _tcsnskip strnskip
#endif//

XLIB_API char* strrskip(char* string, int rlen, char* spec);
XLIB_API wchar_t* wcsrskip(wchar_t* string, int rlen, wchar_t* spec);
#ifdef UNICODE
#define _tcsrskip wcsrskip
#else
#define _tcsrskip strrskip
#endif//

XLIB_API char* strbrkchr(char* string, char spec);
XLIB_API wchar_t* wcsbrkchr(wchar_t* string, wchar_t spec);
#ifdef UNICODE
#define _tcsbrkchr wcsbrkchr
#else
#define _tcsbrkchr strbrkchr
#endif//

XLIB_API char* strnbrkchr(char* string, int len, char spec);
XLIB_API wchar_t* wcsnbrkchr(wchar_t* string, int len, wchar_t spec);
#ifdef UNICODE
#define _tcsnbrkchr wcsnbrkchr
#else
#define _tcsnbrkchr strnbrkchr
#endif//

XLIB_API char* strrbrkchr(char* string, int rlen, char spec);
XLIB_API wchar_t* wcsrbrkchr(wchar_t* string, int rlen, wchar_t spec);
#ifdef UNICODE
#define _tcsrbrkchr wcsrbrkchr
#else
#define _tcsrbrkchr strrbrkchr
#endif//

XLIB_API char* strbrk(char* string, char* spec);
XLIB_API wchar_t* wcsbrk(wchar_t* string, wchar_t* spec);
#ifdef UNICODE
#define _tcsbrk wcsbrk
#else
#define _tcsbrk strbrk
#endif//

XLIB_API char* strnbrk(char* string, int len, char* spec);
XLIB_API wchar_t* wcsnbrk(wchar_t* string, int len, wchar_t* spec);
#ifdef UNICODE
#define _tcsnbrk wcsnbrk
#else
#define _tcsnbrk strnbrk
#endif//

XLIB_API char* strrbrk(char* string, char* spec);
XLIB_API wchar_t* wcsrbrk(wchar_t* string, wchar_t* spec);
#ifdef UNICODE
#define _tcsrbrk wcsrbrk
#else
#define _tcsrbrk strrbrk
#endif//

XLIB_API char* strnrbrk(char* string, int rlen, char* spec);
XLIB_API wchar_t* wcsnrbrk(wchar_t* string, int rlen, wchar_t* spec);
#ifdef UNICODE
#define _tcsnrbrk wcsnrbrk
#else
#define _tcsnrbrk strnrbrk
#endif//

XLIB_API char* strnext(char* string, char* spec, char* cat, char* brk);
XLIB_API wchar_t* wcsnext(wchar_t* string, wchar_t* spec, wchar_t* cat, wchar_t* brk);
#ifdef UNICODE
#define _tcsnext wcsnext
#else
#define _tcsnext strnext
#endif//

XLIB_API char* strnnext(char* string, int len, char* spec, char* cat, char* brk);
XLIB_API wchar_t* wcsnnext(wchar_t* string, int len, wchar_t* spec, wchar_t* cat, wchar_t* brk);
#ifdef UNICODE
#define _tcsnnext wcsnnext
#else
#define _tcsnnext strnnext
#endif//

XLIB_API char* strnexts(char* string, char* spec, char* cat, char* brk);
XLIB_API wchar_t* wcsnexts(wchar_t* string, wchar_t* spec, wchar_t* cat, wchar_t* brk);
#ifdef UNICODE
#define _tcsnexts wcsnexts
#else
#define _tcsnexts strnexts
#endif//

XLIB_API char* strinexts(char* string, char* spec, char* cat, char* brk);
XLIB_API wchar_t* wcsinexts(wchar_t* string, wchar_t* spec, wchar_t* cat, wchar_t* brk);
#ifdef UNICODE
#define _tcsinexts wcsinexts
#else
#define _tcsinexts strinexts
#endif//

XLIB_API char* strnnexts(char* string, int len, char* spec, char* cat, char* brk);
XLIB_API wchar_t* wcsnnexts(wchar_t* string, int len, wchar_t* spec, wchar_t* cat, wchar_t* brk);
#ifdef UNICODE
#define _tcsnnexts wcsnnexts
#else
#define _tcsnnexts strnnexts
#endif//

XLIB_API char* strninexts(char* string, int len, char* spec, char* cat, char* brk);
XLIB_API wchar_t* wcsninexts(wchar_t* string, int len, wchar_t* spec, wchar_t* cat, wchar_t* brk);
#ifdef UNICODE
#define _tcsninexts wcsninexts
#else
#define _tcsninexts strninexts
#endif//

#if _MSC_VER>1200
#ifdef USE_REGEX
XLIB_API bool strnregex(char*src, int srclen, char *dst, int dstlen);
XLIB_API bool wcsnregex(wchar_t*src, int srclen, wchar_t *dst, int dstlen);
#ifdef UNICODE
#define _tcsnregex wcsnregex
#else
#define _tcsnregex strnregex
#endif//

XLIB_API bool strregex(char*src, char *dst);
XLIB_API bool wcsregex(wchar_t*src, wchar_t *dst);
#ifdef UNICODE
#define _tcsregex wcsregex
#else
#define _tcsregex strregex
#endif//
#endif//
#endif//

XLIB_API unsigned int strn2encode(char* str, int len, unsigned int cp_default = CP_UTF8);
XLIB_API unsigned int wcsn2encode(wchar_t* str, int len, unsigned int cp_default = CP_UTF8);
XLIB_API unsigned int str2encode(char* str, unsigned int cp_default = CP_UTF8);
XLIB_API unsigned int wcs2encode(wchar_t* str, unsigned int cp_default = CP_UTF8);
#ifdef UNICODE
#define _tcsn2encode wcsn2encode
#define _tcs2encode wcs2encode
#else
#define _tcsn2encode strn2encode
#define _tcs2encode str2encode
#endif//

//XLIB_API int mb2wc(char* src, int srclen, wchar_t** dst, int* dstlen, unsigned int cp = 0);
//XLIB_API int wc2mb(wchar_t* src, int srclen, char** dst, int* dstlen, unsigned int cp = 0);

XLIB_API int mb2wc(const char* src, int srclen, wchar_t* dst, int dstLen, unsigned int cp = CP_OEMCP);
XLIB_API int wc2mb(const wchar_t* src, int srclen, char* dst, int dstLen, unsigned int cp = CP_OEMCP);

/*
// SDBM Hash Function
unsigned long strnSDBMHash(char *str, int len);
unsigned long strSDBMHash(char *str);
// RS Hash Function
unsigned long strnRSHash(char *str, int len);
unsigned long strRSHash(char *str);
// JS Hash Function
unsigned long strnJSHash(char *str, int len);
unsigned long strJSHash(char *str);
// P. J. Weinberger Hash Function
unsigned long strnPJWHash(char *str, int len);
unsigned long strPJWHash(char *str);
// ELF Hash Function
unsigned long strnELFHash(char *str, int len);
unsigned long strELFHash(char *str);
// BKDR Hash Function
unsigned long strnBKDRHash(char *str, int len);
unsigned long strBKDRHash(char *str);
// DJB Hash Function
unsigned long strnDJBHash(char *str, int len);
unsigned long strDJBHash(char *str);
// AP Hash Function
unsigned long strnAPHash(char *str, int len);
unsigned long strAPHash(char *str);*/

XLIB_API unsigned long strnhashlong(char* str, int len);
XLIB_API unsigned long strhashlong(char* str);
XLIB_API unsigned long wcsnhashlong(wchar_t* str, int len);
XLIB_API unsigned long wcshashlong(wchar_t* str);
XLIB_API unsigned long strnihashlong(char* str, int len);
XLIB_API unsigned long strihashlong(char* str);
XLIB_API unsigned long wcsnihashlong(wchar_t* str, int len);
XLIB_API unsigned long wcsihashlong(wchar_t* str);
#ifdef UNICODE
#define _tcsnhashlong wcsnhashlong
#define _tcshashlong wcshashlong
#define _tcsnihashlong wcsnihashlong
#define _tcsihashlong wcsihashlong
#else
#define _tcsnhashlong strnhashlong
#define _tcshashlong strhashlong
#define _tcsnihashlong strnihashlong
#define _tcsihashlong strihashlong
#endif//

#endif//_H_XCHAR_H_