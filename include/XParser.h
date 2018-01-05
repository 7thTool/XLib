#ifndef _H_XPARSER_H_
#define _H_XPARSER_H_

#include "XLib.h"
#include "XStr.h"

class strref : public std::pair<char*,size_t>
{
	typedef std::pair<char*,size_t> base;
public:
	strref() : base() {}

	strref(const char* str, int len = -1) 
	{
		first = (char*)str;
		second = len < 0 ? (str ? strlen(first) : 0) : len;
	}

	strref(const std::string& str) 
	{
		first = (char*)str.c_str();
		second = str.size();
	}

	operator const char* () const { return first; }

	const char* c_str() const { return first; }

	size_t size() const { return second; }
};
class wcsref : public std::pair<wchar_t*,size_t>
{
	typedef std::pair<wchar_t*,size_t> base;
public:
	wcsref() : base() {}

	wcsref(const wchar_t* str, int len = -1) 
	{
		first = (wchar_t*)str;
		second = len < 0 ? (str ? wcslen(first) : 0) : len;
	}

	wcsref(const std::wstring& str) 
	{
		first = (wchar_t*)str.c_str();
		second = str.size();
	}

	operator const wchar_t* () const { return first;}

	const wchar_t* c_str() const { return first; }

	size_t size() const { return second; }
};
struct strrefless
{
	bool operator()(const strref& s1, const strref& s2) const
	{
		if (s1.second < s2.second) {
			return true;
		} else if (s1.second > s2.second) {
			return false;
		}
		return strncmp(s1.first, s2.first, s2.second) < 0;
	}
};
struct wcsrefless
{
	bool operator()(const wcsref& s1, const wcsref& s2) const
	{
		if (s1.second < s2.second) {
			return true;
		} else if (s1.second > s2.second) {
			return false;
		}
		return wcsncmp(s1.first, s2.first, s2.second) < 0;
	}
};
struct strrefiless
{
	bool operator()(const strref& s1, const strref& s2) const
	{
		if (s1.second < s2.second) {
			return true;
		} else if (s1.second > s2.second) {
			return false;
		}
		return strnicmp(s1.first, s2.first, s2.second) < 0;
	}
};
struct wcsrefiless
{
	bool operator()(const wcsref& s1, const wcsref& s2) const
	{
		if (s1.second < s2.second) {
			return true;
		} else if (s1.second > s2.second) {
			return false;
		}
		return wcsnicmp(s1.first, s2.first, s2.second) < 0;
	}
};
typedef std::vector<strref> strrefarray;
typedef std::vector<wcsref> wcsrefarray;
typedef std::pair<strref,strref> strrefpair;
typedef std::pair<wcsref,wcsref> wcsrefpair;
struct strrefpairless
{
	bool operator()(const strrefpair& s1, const strrefpair& s2) const
	{
		if (s1.first.second < s2.first.second) {
			return true;
		} else if (s1.first.second > s2.first.second) {
			return false;
		}
		return strncmp(s1.first.first, s2.first.first, s2.first.second) < 0;
	}
};
struct wcsrefpairless
{
	bool operator()(const wcsrefpair& s1, const wcsrefpair& s2) const
	{
		if (s1.first.second < s2.first.second) {
			return true;
		} else if (s1.first.second > s2.first.second) {
			return false;
		}
		return wcsncmp(s1.first.first, s2.first.first, s2.first.second) < 0;
	}
};
struct strrefpairiless
{
	bool operator()(const strrefpair& s1, const strrefpair& s2) const
	{
		if (s1.first.second < s2.first.second) {
			return true;
		} else if (s1.first.second > s2.first.second) {
			return false;
		}
		return strnicmp(s1.first.first, s2.first.first, s2.first.second) < 0;
	}
};
struct wcsrefpairiless
{
	bool operator()(const wcsrefpair& s1, const wcsrefpair& s2) const
	{
		if (s1.first.second < s2.first.second) {
			return true;
		} else if (s1.first.second > s2.first.second) {
			return false;
		}
		return wcsnicmp(s1.first.first, s2.first.first, s2.first.second) < 0;
	}
};
typedef std::vector<strrefpair> strrefpairarray;
typedef std::vector<wcsrefpair> wcsrefpairarray;
typedef std::map<strref,strref,strrefless> strref2strref;
typedef std::map<wcsref,wcsref,wcsrefless> wcsref2wcsref;
typedef std::map<strref,strref,strrefiless> strrefi2strref;
typedef std::map<wcsref,wcsref,wcsrefiless> wcsrefi2wcsref;
#ifdef UNICODE
#define tcsref	strref
#define tcsrefarray strrefarray
#define tcsrefpair strrefpair
#define tcsref2tcsref	strref2strref
#define tcsrefi2strref strrefi2strref
#else
#define tcsref wcsref
#define tcsrefarray wcsrefarray
#define tcsrefpair wcsrefpair
#define tcsref2tcsref	wcsref2wcsref
#define tcsrefi2strref wcsrefi2wcsref
#endif//

XLIB_API void strsplit(strrefarray& arr, 
			  char* str, char* spec, char* cat = "\"");
XLIB_API void wcssplit(wcsrefarray& arr, 
			  wchar_t* str, wchar_t* spec, wchar_t* cat = L"\"");
#ifdef UNICODE
#define _tcssplit wcssplit
#else
#define _tcssplit strsplit
#endif//

XLIB_API void strnsplit(strrefarray& arr, 
			   char* str, int len, char* spec, char* cat = "\"");
XLIB_API void wcsnsplit(wcsrefarray& arr, 
			   wchar_t* str, int len, wchar_t* spec, wchar_t* cat = L"\"");
#ifdef UNICODE
#define _tcsnsplit wcsnsplit
#else
#define _tcsnsplit strnsplit
#endif//

XLIB_API void strkeyvalue(strrefpairarray& key2value, 
				 char* str, char* spec = "=", char* cat = "\"", char* brk = " \t\r\n");
XLIB_API void wcskeyvalue(wcsrefpairarray& key2value, 
				 wchar_t* str, wchar_t* spec = L"=", wchar_t* cat = L"\"", wchar_t* brk = L" \t\r\n");
#ifdef UNICODE
#define _tcskeyvalue wcskeyvalue
#else
#define _tcskeyvalue strkeyvalue
#endif//

XLIB_API void strnkeyvalue(strrefpairarray& key2value, 
				 char* str, int len, char* spec = "=", char* cat = "\"", char* brk = " \t\r\n");
XLIB_API void wcsnkeyvalue(wcsrefpairarray& key2value, 
				 wchar_t* str, int len, wchar_t* spec = L"=", wchar_t* cat = L"\"", wchar_t* brk = L" \t\r\n");
#ifdef UNICODE
#define _tcsnkeyvalue wcsnkeyvalue
#else
#define _tcsnkeyvalue strnkeyvalue
#endif//

XLIB_API void strcmdline(strref2strref& op2value, strref& val, 
				char* str, char* opspec = "/-", char* valspec = ":=", char* cat = "\"");
XLIB_API void wcscmdline(wcsref2wcsref& op2value, wcsref& val, 
				wchar_t* str, wchar_t* opspec = L"/-", wchar_t* valspec = L":=", wchar_t* cat = L"\"");
#ifdef UNICODE
#define _tcscmdline wcscmdline
#else
#define _tcscmdline strcmdline
#endif//

XLIB_API void strncmdline(strref2strref& op2value, strref& val, 
				char* str, int len, char* opspec = "/-", char* valspec = ":=", char* cat = "\"");
XLIB_API void wcsncmdline(wcsref2wcsref& op2value, wcsref& val, 
				wchar_t* str, int len, wchar_t* opspec = L"/-", wchar_t* valspec = L":=", wchar_t* cat = L"\"");
#ifdef UNICODE
#define _tcsncmdline wcsncmdline
#else
#define _tcsncmdline strncmdline
#endif//

XLIB_API void strsection(strref2strref& sec2value, char* str, char* spec = "[]");
XLIB_API void wcssection(wcsref2wcsref& sec2value, wchar_t* str, wchar_t* spec = L"[]");
#ifdef UNICODE
#define _tcssection wcssection
#else
#define _tcssection strsection
#endif//

XLIB_API void strnsection(strref2strref& sec2value, char* str, int len, char* spec = "[]");
XLIB_API void wcsnsection(wcsref2wcsref& sec2value, wchar_t* str, int len, wchar_t* spec = L"[]");
#ifdef UNICODE
#define _tcsnsection wcsnsection
#else
#define _tcsnsection strnsection
#endif//

XLIB_API void strnseckeyvalue(strrefpairarray& key2value, char* str, int len, char* cmt = ";#", char* cat = "\"");
XLIB_API void wcsnseckeyvalue(wcsrefpairarray& key2value, wchar_t* str, int len, wchar_t* cmt = L";#", wchar_t* cat = L"\"");
#ifdef UNICODE
#define _tcsnseckeyvalue wcsnseckeyvalue
#else
#define _tcsnseckeyvalue strnseckeyvalue
#endif//

enum
{
	node_element = 0,
	node_cdata_section,
	node_processing_instruction,
	node_comment,
	node_document_type,
};
template<class tbase = strref>
class strrefnode : public tbase
{
	typedef strrefnode<tbase> strrefelem;
	typedef std::vector<strrefelem*> strrefelemarray;
public:
	strrefelem* parent;

	byte type;
	strref data;
	strrefelemarray childs;

public:
	strrefnode(strrefelem* parent) { this->parent = parent; }
	~strrefnode() { clear(); parent = 0; }

	void clear()
	{
		type = 0;
		first = 0;
		second = 0;
		clearchild();
	}

	void clearchild()
	{
		data.first = 0;
		data.second = 0;
		for (size_t i = 0, j = childs.size(); i < j; ++i)
		{
			delete childs[i];
		}
		childs.clear();
	}

	char* strnode(char* str, int len)
	{
		clear();

		char* nil = str + len;

		char* next = str;
		while (next && next < nil)
		{
			next = strnskip(next, nil - next, " \t\n\r"); //ignore space
			if (!next) {
				break;
			}
			if (*next == '<') { //element
				// Started with <, could be:
				// <!--...--> comment
				// <!DOCTYPE ...> dtd
				// <?target ...?> processing instruction
				// <![CDATA[...]]> cdata section
				// <NAME ...> element
				//
				first = next;
				type = node_element;

				char* node_end = 0;

				next++;
				if (*next == '?') {
					type = node_processing_instruction;
					node_end = "?>";
				} else if (*next == '!') {
					if (*(next + 1) == '[') {
						type = node_cdata_section;
						node_end = "]]>";
					} else if (*(next + 1) == '-') {
						type = node_comment;
						node_end = "-->";
					} else {
						type = node_document_type;
					}
				} else if (*next == '/') { //parent node end </....>
					next = strnnext(next + 1, nil - next - 1, ">", "\"", "");
					assert(next);
					next++;
					//second = next - first;
					break;
				} 

				if (node_end) {
					next = strnnexts(next + 1, nil - next - 1, node_end, "\"", "");
					if (next) {
						next += strlen(node_end);
						second = next - first;
					}
					break;
				} else {
					next = strnnext(next, nil - next, ">", "\"", "");
					if (!next) {
						break;
					}
					if (type == node_element) {
						if (*(next - 1) == '/') {
							next++;
							second = next - first;
						} else {
							next++;
							data.first = next;
							do {
								strrefelem * child = new strrefelem(this);
								next = child->strnode(next, nil - next);
								if (!child->first) { //error no "<" ...
									delete child;
									break;
								}
								if (*(child->first + 1) == '/') {
									second = next ? next - first : nil - first;
									data.second = child->first - data.first;
									delete child;
									break;
								} else {
									childs.push_back(child);
								}
							} while (next);
							if (!second) { //find data end if no child
								next = strnnexts(data.first, nil - data.first, "</", "\"", "");
								if (next) {
									data.second = next - data.first;
									next = strnnext(next, nil - next, ">", "\"", "");
									if (next) {
										next++;
										second = next - first;
									}
								}
							}
						}
					} else {
						next++;
						second = next - first;
					}
				}
			} else {
				return 0;
			}
			break;
		}
		return (next && *next) ? next : 0;
	}
};
class strrefattr : public strref
{
public:
	strref name;
	strrefpairarray attrs;
};
typedef strrefnode<strrefattr> strrefelement;
template<class tbase = wcsref>
class wcsrefnode : public tbase
{
	typedef wcsrefnode<tbase> wcsrefelem;
	typedef std::vector<wcsrefelem*> wcsrefelemarray;
public:
	wcsrefelem* parent;

	byte type;
	wcsref data;
	wcsrefelemarray childs;

public:
	wcsrefnode(wcsrefelem* parent) { this->parent = parent; }
	~wcsrefnode() { clear(); parent = 0; }

	void clear()
	{
		type = 0;
		first = 0;
		second = 0;
		clearchild();
	}

	void clearchild()
	{
		data.first = 0;
		data.second = 0;
		for (size_t i = 0, j = childs.size(); i < j; ++i)
		{
			delete childs[i];
		}
		childs.clear();
	}

	wchar_t* wcsnode(wchar_t* wcs, int len)
	{
		clear();

		wchar_t* nil = wcs + len;

		wchar_t* next = wcs;
		while (next && next < nil)
		{
			next = wcsnskip(next, nil - next, L" \t\n\r"); //ignore space
			if (!next) {
				break;
			}
			if (*next == L'<') { //element
				// Started with <, could be:
				// <!--...--> comment
				// <!DOCTYPE ...> dtd
				// <?target ...?> processing instruction
				// <![CDATA[...]]> cdata section
				// <NAME ...> element
				//
				first = next;
				type = node_element;

				wchar_t* node_end = 0;

				next++;
				if (*next == L'?') {
					type = node_processing_instruction;
					node_end = L"?>";
				} else if (*next == L'!') {
					if (*(next + 1) == L'[') {
						type = node_cdata_section;
						node_end = L"]]>";
					} else if (*(next + 1) == '-') {
						type = node_comment;
						node_end = L"-->";
					} else {
						type = node_document_type;
					}
				} else if (*next == L'/') { //parent node end </....>
					next = wcsnnext(next + 1, nil - next - 1, L">", L"\"", L"");
					assert(next);
					next++;
					//second = next - first;
					break;
				} 

				if (node_end) {
					next = wcsnnexts(next + 1, nil - next - 1, node_end, L"\"", L"");
					if (next) {
						next += wcslen(node_end);
						second = next - first;
					}
					break;
				} else {
					next = wcsnnext(next, nil - next, L">", L"\"", L"");
					if (!next) {
						break;
					}
					if (type == node_element) {
						if (*(next - 1) == L'/') {
							next++;
							second = next - first;
						} else {
							next++;
							data.first = next;
							do {
								wcsrefelem* child = new wcsrefelem(this);
								next = child->wcsnode(next, nil - next);
								if (!child->first) { //error no "<" ...
									delete child;
									break;
								}
								if (*(child->first + 1) == L'/') {
									second = next ? next - first : nil - first;
									data.second = child->first - data.first;
									delete child;
									break;
								} else {
									childs.push_back(child);
								}
							} while (next);
							if (!second) { //find data end if no child
								next = wcsnnexts(data.first, nil - data.first, L"</", L"\"", L"");
								if (next) {
									data.second = next - data.first;
									next = wcsnnext(next, nil - next, L">", L"\"", L"");
									if (next) {
										next++;
										second = next - first;
									}
								}
							}
						}
					} else {
						next++;
						second = next - first;
					}
				}
			} else {
				return 0;
			}
			break;
		}
		return (next && *next) ? next : 0;
	}
};
class wcsrefattr : public wcsref
{
public:
	wcsref name;
	wcsrefpairarray attrs;
};
typedef wcsrefnode<wcsrefattr> wcsrefelement;
XLIB_API void strelement(strrefelement& elem);
XLIB_API void wcselement(wcsrefelement& elem);
#ifdef UNICODE
#define _tcselement wcselement
#else
#define _tcselement strelement
#endif//
XLIB_API void stresc2value(char* str, int len, std::string& val);
XLIB_API void wcsesc2value(wchar_t* str, int len, std::wstring& val);
#ifdef UNICODE
#define _tcsesc2value wcsesc2value
#else
#define _tcsesc2value stresc2value
#endif//
XLIB_API void strelement2value(strrefelement& elem, std::string& val);
XLIB_API void wcselement2value(wcsrefelement& elem, std::wstring& val);
#ifdef UNICODE
#define _tcselement2value wcselement2value
#else
#define _tcselement2value strelement2value
#endif//
typedef std::vector<strrefelement*> strrefelementarray;
typedef std::vector<wcsrefelement*> wcsrefelementarray;
XLIB_API void strmarkup(strrefelementarray& elements, char* str, int len);
XLIB_API void wcsmarkup(wcsrefelementarray& elements, wchar_t* str, int len);
#ifdef UNICODE
#define _tcsmarkup wcsmarkup
#else
#define _tcsmarkup strmarkup
#endif//

#ifdef UNICODE
#define tcsrefnode wcsrefnode
#define tcsrefattr wcsrefattr
#define tcsrefelement wcsrefelement
#define tcsrefelementarray wcsrefelementarray
#else
#define tcsrefnode strrefnode
#define tcsrefattr strrefattr
#define tcsrefelement strrefelement
#define tcsrefelementarray strrefelementarray
#endif//

class ParserA
{
protected:
	std::string m_str;
public:
	const std::string& Set(std::string& str) {
		m_str = str;
		return m_str;
	}
	const std::string& Get() {
		return m_str;
	}
};

class ParserW
{
protected:
	std::wstring m_str;
public:
	const std::wstring& Set(std::wstring& str) {
		m_str = str;
		return m_str;
	}
	const std::wstring& Get() {
		return m_str;
	}
};

class KeyValueParserA : public ParserA
{
	typedef ParserA Base;
protected:
	strrefpairarray m_key2value;
public:
	void Set(char* keyvalue, char* spec = "=", char* cat = "\"", char* brk = " \r\n")
	{
		m_key2value.clear();
		strkeyvalue(m_key2value, (char*)Base::Set(std::string(keyvalue)).c_str(), spec, cat, brk);
	}

	bool Get(strref & key, strref & val)
	{
		return Find(key, val);
	}

	bool Get(std::string & key, std::string val)
	{
		strref strkey(key);
		strref strval;
		if (Get(strkey, strval)) {
			val.assign(strval.first, strval.second);
			return true;
		}
		return false;
	}

	std::string& GetKeyValue(std::string & key, std::string& val, std::string& def = std::string("")) 
	{ 
		if(!Get(key, val)) {
			val = def;
		}
		return val;
	}

	char* GetKeyValue(char* key, char* val, int len, char* def = "")
	{
		strref pstrkey(key,strlen(key));
		strref strval;
		if(Get(pstrkey, strval)) {
			len = min(strval.second,len);
			strncpy(val, strval.first, len);
			val[len] = 0;
		} else {
			strcpy(val, def);
		}
		return val;
	}

	bool Find(strref& str, strref& val)
	{
		for (size_t i = 0, j = m_key2value.size(); i < j; ++i)
		{
			strrefpair & keyvalue = m_key2value[i];
			if (keyvalue.first.second == str.second) {
				if (strnicmp(keyvalue.first.first, str.first, str.second) == 0) {
					val = keyvalue.second;
					return true;
				}
			}
		}
		return false;
	}
};

class KeyValueParserW : public ParserW
{
	typedef ParserW Base;
protected:
	wcsrefpairarray m_key2value;
public:
	void Set(wchar_t* keyvalue, wchar_t* spec = L"=", wchar_t* cat = L"\"", wchar_t* brk = L" \r\n")
	{
		m_key2value.clear();
		wcskeyvalue(m_key2value, (wchar_t*)Base::Set(std::wstring(keyvalue)).c_str(), spec, cat, brk);
	}

	bool Get(wcsref & key, wcsref & val)
	{
		return Find(key, val);
	}

	bool Get(std::wstring & key, std::wstring val)
	{
		wcsref strkey(key);
		wcsref strval;
		if (Get(strkey, strval)) {
			val.assign(strval.first, strval.second);
			return true;
		}
		return false;
	}

	std::wstring& GetKeyValue(std::wstring & key, std::wstring& val, std::wstring& def = std::wstring(L"")) 
	{ 
		if(!Get(key, val)) {
			val = def;
		}
		return val;
	}

	wchar_t* GetKeyValue(wchar_t* key, wchar_t* val, int len, wchar_t* def = L"")
	{
		wcsref pstrkey(key,wcslen(key));
		wcsref strval;
		if(Get(pstrkey, strval)) {
			len = min(strval.second,len);
			wcsncpy(val, strval.first, len);
			val[len] = 0;
		} else {
			wcscpy(val, def);
		}
		return val;
	}

	bool Find(wcsref& str, wcsref& val)
	{
		for (size_t i = 0, j = m_key2value.size(); i < j; ++i)
		{
			wcsrefpair & keyvalue = m_key2value[i];
			if (keyvalue.first.second == str.second) {
				if (wcsnicmp(keyvalue.first.first, str.first, str.second) == 0) {
					val = keyvalue.second;
					return true;
				}
			}
		}
		return false;
	}
};

#ifdef UNICODE
#define KeyValueParser KeyValueParserW
#else
#define KeyValueParser KeyValueParserA
#endif//

class CommandLineParserA : public ParserA
{
	typedef ParserA Base;
protected:
	strref2strref m_op2value;
	strref m_value;
	
public:
	void Set(char* cmdline, char* opspec = "/-", char* valspec = ":=", char* cat = "\"")
	{
		m_op2value.clear();
		strcmdline(m_op2value, m_value, (char*)Base::Set(std::string(cmdline)).c_str(), opspec, valspec, cat);
	}
	
	bool Get(strref & val)
	{
		val = m_value;
		return true;
	}
	bool Get(std::string& val)
	{
		if (m_value.first && m_value.second) {
			val.assign(m_value.first, m_value.second);
			return true;
		}
		return false;
	}

	bool Get(strref & op, strref & val)
	{
		strref2strref::iterator it = m_op2value.find(op);
		if (it != m_op2value.end()) {
			val = it->second;
			return true;
		}
		return false;
	}
	bool Get(std::string& op, std::string& val)
	{
		strref2strref::iterator it = m_op2value.find(strref((char*)op.c_str(),op.size()));
		if (it != m_op2value.end()) {
			val.assign(it->second.first, it->second.second);
			return true;
		}
		return false;
	}

	std::string& GetValue(std::string& val, std::string& def = std::string(""))
	{
		if(!Get(val)) {
			val = def;
		}
		return val;
	}
	char* GetValue(char* val, int len, char* def = "")
	{
		if (m_value.first && m_value.second) {
			len = min(m_value.second,len);
			strncpy(val, m_value.first, len);
			val[len] = 0;
		} else {
			strcpy(val, def);
		}
		return val;
	}
	std::string& GetOpValue(std::string& op, std::string& val, std::string& def = std::string(""))
	{
		if(!Get(op, val)) {
			val = def;
		}
		return val;
	}
	char* GetOpValue(char* op, char* val, int len, char* def = "")
	{
		strref pstrop(op,strlen(op));
		strref strval;
		if (Get(pstrop, strval)) {
			len = min(strval.second,len);
			strncpy(val, strval.first, len);
			val[len] = 0;
		} else {
			strcpy(val, def);
		}
		return val;
	}
};

class CommandLineParserW : public ParserW
{
	typedef ParserW Base;
protected:
	wcsref2wcsref m_op2value;
	wcsref m_value;

public:
	void Set(wchar_t* cmdline, wchar_t* opspec = L"/-", wchar_t* valspec = L":=", wchar_t* cat = L"\"")
	{
		m_op2value.clear();
		wcscmdline(m_op2value, m_value, (wchar_t*)Base::Set(std::wstring(cmdline)).c_str(), opspec, valspec, cat);
	}

	bool Get(wcsref & val)
	{
		val = m_value;
		return true;
	}
	bool Get(std::wstring& val)
	{
		if (m_value.first && m_value.second) {
			val.assign(m_value.first, m_value.second);
			return true;
		}
		return false;
	}
	bool Get(wcsref & op, wcsref & val)
	{
		wcsref2wcsref::iterator it = m_op2value.find(op);
		if (it != m_op2value.end()) {
			val = it->second;
			return true;
		}
		return false;
	}
	bool Get(std::wstring& op, std::wstring& val)
	{
		wcsref2wcsref::iterator it = m_op2value.find(wcsref((wchar_t*)op.c_str(),op.size()));
		if (it != m_op2value.end()) {
			val.assign(it->second.first, it->second.second);
			return true;
		}
		return false;
	}

	std::wstring& GetValue(std::wstring& val, std::wstring& def = std::wstring(L""))
	{
		if(!Get(val)) { 
			val = def;
		}
		return val;
	}
	wchar_t* GetValue(wchar_t* val, int len, wchar_t* def = L"")
	{
		if (m_value.first && m_value.second) {
			len = min(m_value.second,len);
			wcsncpy(val, m_value.first, len);
			val[len] = 0;
		} else {
			wcscpy(val, def);
		}
		return val;
	}
	std::wstring& GetOpValue(std::wstring& op, std::wstring& val, std::wstring& def = std::wstring(L""))
	{
		if(!Get(op, val)) {
			val = def;
		}
		return val;
	}
	wchar_t* GetOpValue(wchar_t* op, wchar_t* val, int len, wchar_t* def = L"")
	{
		wcsref pstrop(op,wcslen(op));
		wcsref strval;
		if (Get(pstrop, strval)) {
			len = min(strval.second,len);
			wcsncpy(val, strval.first, len);
			val[len] = 0;
		} else {
			wcscpy(val, def);
		}
		return val;
	}
};

#ifdef UNICODE
#define CommandLineParser CommandLineParserW
#else
#define CommandLineParser CommandLineParserA
#endif//

//////////////////////////////////////////////////////////////////////////
//INI
//////////////////////////////////////////////////////////////////////////
class ProfileParserA : public ParserA
{
	typedef ParserA Base;
protected:
	typedef strref2strref strrefsec2value;
	strrefsec2value m_sec2value;
	typedef strrefpairarray strrefkey2value;
	typedef std::map<strref,strrefkey2value,strrefless> strrefsec2key2value;
	strrefsec2key2value m_sec2key2value;

public:
	const std::string& Set(std::string& str)
	{
		m_sec2key2value.clear();
		m_sec2value.clear();

		strsection(m_sec2value, (char*)Base::Set(str).c_str());
		return str;
	}

	const std::string& Get()
	{
		return Base::Get();
	}

	bool Get(strrefarray & secs)
	{
		strrefsec2value::iterator it = m_sec2value.begin();
		for ( ; it != m_sec2value.end(); ++it) {
			secs.push_back(it->first);
		}
		return true;
	}
	bool Get(std::stringarray & secs)
	{
		strrefsec2value::iterator it = m_sec2value.begin();
		for ( ; it != m_sec2value.end(); ++it) {
			secs.push_back(std::string(it->first.first,it->first.second));
		}
		return true;
	}

	bool Get(const strref & sec, strref & val)
	{
		strrefsec2value::iterator it = m_sec2value.find(sec);
		if (it != m_sec2value.end()) {
			val = it->second;
			return true;
		}
		return false;
	}
	bool Get(const std::string & sec, std::string & val)
	{
		strref pstrsec((char*)sec.c_str(),sec.size());
		strref strval;
		if(Get(pstrsec, strval)) {
			val.assign(strval.first,strval.second);
			return true;
		}
		return false;
	}

	bool Get(const strref & sec, const strref & key, strref & val)
	{
		bool ret = false;
		strrefsec2key2value::iterator iter_sec = m_sec2key2value.find(sec);
		if (iter_sec == m_sec2key2value.end()) {
			strref sec_value;
			if(Get(sec, sec_value)) {
				strrefkey2value & key2value = m_sec2key2value[sec];
				strnseckeyvalue(key2value, sec_value.first, sec_value.second);
				for (size_t i = 0, j = key2value.size(); i < j; ++i)
				{
					strrefpair & keyvalue = key2value[i];
					if (keyvalue.first.second == key.second) {
						if (strnicmp(keyvalue.first.first, key.first, key.second) == 0) {
							val = keyvalue.second;
							ret = true;
						}
					}
				}
			}
		} else {
			strrefkey2value & key2value = iter_sec->second;
			for (size_t i = 0, j = key2value.size(); i < j; ++i)
			{
				strrefpair & keyvalue = key2value[i];
				if (keyvalue.first.second == key.second) {
					if (strnicmp(keyvalue.first.first, key.first, key.second) == 0) {
						val = keyvalue.second;
						ret = true;
					}
				}
			}
		}
		return ret;
	}
	bool Get(const std::string & sec, const std::string & key, std::string & val)
	{
		strref pstrsec((char*)sec.c_str(),sec.size());
		strref pstrkey((char*)key.c_str(),key.size());
		strref strval;
		if(Get(pstrsec, pstrkey, strval)) {
			val.assign(strval.first,strval.second);
			return true;
		}
		return false;
	}

	bool GetProfileSectionNames(std::stringarray& secs)
	{
		return Get(secs);
	}
	bool GetProfileSection(std::string& sec, std::string val)
	{
		return Get(sec, val);
	}
	bool GetProfileSection(char* sec, char* val, int len)
	{
		strref pstrsec(sec,strlen(sec));
		strref strval;
		if (Get(pstrsec, strval)) {
			len = min(strval.second,len);
			strncpy(val, strval.first, len);
			val[len] = 0;
			return true;
		}
		return false;
	}
	std::string& GetProfileString(std::string& sec, std::string& key, std::string& val, std::string& def = std::string(""))
	{
		if(!Get(sec, key, val)) {
			val = def;
		}
		return val;
	}
	char* GetProfileString(char* sec, char* key, char* val, int len, char* def = "")
	{
		strref pstrsec(sec,strlen(sec));
		strref pstrkey(key,strlen(key));
		strref strval;
		if (Get(pstrsec,pstrkey,strval)) {
			len = min(strval.second,len);
			strncpy(val, strval.first, len);
			val[len] = 0;
		} else {
			strcpy(val, def);
		}
		return val;
	}
};

class ProfileParserW : public ParserW
{
	typedef ParserW Base;
protected:
	typedef wcsref2wcsref wcsrefsec2value;
	wcsrefsec2value m_sec2value;
	typedef wcsrefpairarray wcsrefkey2value;
	typedef std::map<wcsref,wcsrefkey2value,wcsrefless> wcsrefsec2key2value;
	wcsrefsec2key2value m_sec2key2value;

public:
	const std::wstring& Set(std::wstring& str)
	{
		m_sec2key2value.clear();
		m_sec2value.clear();

		wcssection(m_sec2value, (wchar_t*)Base::Set(str).c_str());
		return str;
	}

	const std::wstring& Get()
	{
		return Base::Get();
	}

	bool Get(wcsrefarray & secs)
	{
		wcsrefsec2value::iterator it = m_sec2value.begin();
		for ( ; it != m_sec2value.end(); ++it) {
			secs.push_back(it->first);
		}
		return true;
	}

	bool Get(std::wstringarray & secs)
	{
		wcsrefsec2value::iterator it = m_sec2value.begin();
		for ( ; it != m_sec2value.end(); ++it) {
			secs.push_back(std::wstring(it->first.first,it->first.second));
		}
		return true;
	}

	bool Get(const wcsref & sec, wcsref & val)
	{
		wcsrefsec2value::iterator it = m_sec2value.find(sec);
		if (it != m_sec2value.end()) {
			val = it->second;
			return true;
		}
		return false;
	}

	bool Get(const std::wstring & sec, std::wstring & val)
	{
		wcsref strsec((wchar_t*)sec.c_str(),sec.size());
		wcsref strval;
		if(Get(strsec, strval)) {
			val.assign(strval.first,strval.second);
			return true;
		}
		return false;
	}

	bool Get(const wcsref & sec, const wcsref & key, wcsref & val)
	{
		bool ret = false;
		wcsrefsec2key2value::iterator iter_sec = m_sec2key2value.find(sec);
		if (iter_sec == m_sec2key2value.end()) {
			wcsref sec_value;
			if(Get(sec, sec_value)) {
				wcsrefkey2value & key2value = m_sec2key2value[sec];
				wcsnseckeyvalue(key2value, sec_value.first, sec_value.second);
				for (size_t i = 0, j = key2value.size(); i < j; ++i)
				{
					wcsrefpair & keyvalue = key2value[i];
					if (keyvalue.first.second == key.second) {
						if (wcsnicmp(keyvalue.first.first, key.first, key.second) == 0) {
							val = keyvalue.second;
							ret = true;
						}
					}
				}
			}
		} else {
			wcsrefkey2value & key2value = iter_sec->second;
			for (size_t i = 0, j = key2value.size(); i < j; ++i)
			{
				wcsrefpair & keyvalue = key2value[i];
				if (keyvalue.first.second == key.second) {
					if (wcsnicmp(keyvalue.first.first, key.first, key.second) == 0) {
						val = keyvalue.second;
						ret = true;
					}
				}
			}
		}
		return ret;
	}

	bool Get(const std::wstring & sec, const std::wstring & key, std::wstring & val)
	{
		wcsref strsec((wchar_t*)sec.c_str(),sec.size());
		wcsref strkey((wchar_t*)key.c_str(),key.size());
		wcsref strval;
		if(Get(strsec, strkey, strval)) {
			val.assign(strval.first,strval.second);
			return true;
		}
		return false;
	}

	bool GetProfileSectionNames(std::wstringarray& secs)
	{
		return Get(secs);
	}
	bool GetProfileSection(std::wstring& sec, std::wstring val)
	{
		return Get(sec, val);
	}
	bool GetProfileSection(wchar_t* sec, wchar_t* val, int len)
	{
		wcsref pstrsec(sec,wcslen(sec));
		wcsref strval;
		if (Get(pstrsec, strval)) {
			len = min(strval.second,len);
			wcsncpy(val, strval.first, len);
			val[len] = 0;
			return true;
		}
		return false;
	}
	std::wstring& GetProfileString(std::wstring& sec, std::wstring& key, std::wstring& val, std::wstring& def = std::wstring(L""))
	{
		if(!Get(sec, key, val)) {
			val = def;
		}
		return val;
	}
	wchar_t* GetProfileString(wchar_t* sec, wchar_t* key, wchar_t* val, int len, wchar_t* def = L"")
	{
		wcsref pstrsec(sec,wcslen(sec));
		wcsref pstrkey(key,wcslen(key));
		wcsref strval;
		if (Get(pstrsec,pstrkey,strval)) {
			len = min(strval.second,len);
			wcsncpy(val, strval.first, len);
			val[len] = 0;
		} else {
			wcscpy(val, def);
		}
		return val;
	}
};

#ifdef UNICODE
#define ProfileParser ProfileParserW
#else
#define ProfileParser ProfileParserA
#endif//

class MarkupParserA : public ParserA
{
	typedef ParserA Base;
protected:
	int m_cp;
	strrefelementarray m_elements;
	strrefelement* m_pcurelement;	//current element
	strrefelement* m_pcurchild;		//cur child

public:
	MarkupParserA()
	{
		m_cp = 0;
		//m_elements;
		m_pcurelement = 0;	//current element
		m_pcurchild = 0;	//cur child
	}
	~MarkupParserA()
	{
		m_cp = 0;
		for (size_t i = 0, j = m_elements.size(); i < j; ++i)
		{
			delete m_elements[i];
		}
		m_elements.clear();
		m_pcurelement = 0;	//current element
		m_pcurchild = 0;	//cur child
	}

	const std::string& Set(std::string& str, int cp = CP_UTF8)
	{
		m_cp = CP_UTF8;
		char* strencode = strnexts((char*)str.c_str(), "encoding=\"", "", "\r\n");
		if (strencode) {
			strencode += 10;
			char* strencodeend = strchr(strencode, '\"');
			if(strencodeend) {
				m_cp = strn2encode(strencode, strencodeend - strencode, m_cp);
			}
		}
		if (m_cp != cp) {
			std::wstring wcs;
			mb2wc(str, wcs, cp);
			wc2mb(wcs, str, m_cp);
		}
		m_pcurelement = 0;
		m_pcurchild = 0;
		for (size_t i = 0, j = m_elements.size(); i < j; ++i)
		{
			delete m_elements[i];
		}
		m_elements.clear();

		Base::Set(str);
		strmarkup(m_elements, (char*)m_str.c_str(), m_str.length());
		return m_str;
	}

	const std::string& Set(std::wstring& wcs)
	{
		m_cp = CP_UTF8;
		wchar_t* strencode = wcsnexts((wchar_t*)wcs.c_str(), L"encoding=\"", L"", L"\r\n");
		if (strencode) {
			strencode += 10;
			wchar_t* strencodeend = wcschr(strencode, L'\"');
			if(strencodeend) {
				m_cp = wcsn2encode(strencode, strencodeend - strencode, m_cp);
			}
		}
		std::string str;
		wc2mb(wcs, str, m_cp);
		return Set(str, m_cp);
	}

	const std::string& Get()
	{
		return Base::Get();
	}

	int GetCP() 
	{
		return m_cp;
	}

	int GetSize()
	{
		if (m_pcurelement) {
			return m_pcurelement->childs.size();
		}
		return m_elements.size();
	}

	int GetType(bool child = false)
	{
		if (!child) {
			if (m_pcurelement) {
				return m_pcurelement->type;
			}
		} else {
			if (m_pcurchild) {
				return m_pcurchild->type;
			}
		}
		return node_element;
	}

	bool GetName(strref& name, bool child = false)
	{
		if (!child) {
			if (m_pcurelement) {
				name = m_pcurelement->name;
				return true;
			}
		} else {
			if (m_pcurchild) {
				name = m_pcurchild->name;
				return true;
			}
		}
		return false;
	}

	bool GetName(std::string& name, bool child = false)
	{
		strref strname;
		if (GetName(strname, child)) {
			name.assign(strname.first, strname.second);
			return true;
		}
		return false;
	}

	std::string& GetTagName(std::string& name, bool child = false)
	{
		GetName(name, child);
		return name;
	}

	char* GetTagName(char* name, int len, bool child = false)
	{
		strref strname;
		if(GetName(strname, child)) {
			len = min(strname.second,len);
			strncpy(name, strname.first, len);
			name[len] = 0;
		}
		return name;
	}

	//get data
	bool Get(const strref& name, strref& val)
	{
		bool ret = false;
		if (Find(name)) {
			if (m_pcurchild->childs.size() == 1) {
				switch (m_pcurchild->childs[0]->type)
				{
				case node_cdata_section:
					// <![CDATA[...]]> cdata section
					val.first = m_pcurchild->data.first + 9;
					val.second = m_pcurchild->data.second - 12;
					ret = true;
					break;
				case node_processing_instruction:
					// <?target ...?> processing instruction
					val.first = m_pcurchild->data.first + 8;
					val.second = m_pcurchild->data.second - 10;
					ret = true;
					break;
				case node_comment: 
					//<!--...--> comment
					val.first = m_pcurchild->data.first + 4;
					val.second = m_pcurchild->data.second - 7;
					ret = true;
					break;
				case node_document_type: 
					// <!DOCTYPE ...> dtd
					val.first = m_pcurchild->data.first + 9;
					val.second = m_pcurchild->data.second - 10;
					ret = true;
					break;
				default:
					break;
				}
			} 
			if (!ret) {
				val = m_pcurchild->data;
				ret = true;
			}
		}
		return ret;
	}

	bool Get(const std::string& name, std::string& val)
	{
		strref strname((char*)name.c_str(),name.size());
		strref strval;
		if (Get(strname, strval)) {
			val.assign(strval.first, strval.second);
			return true; 
		}
		return false;
	}

	std::string& GetDataString(const std::string& name, std::string& val, std::string& def = std::string(""))
	{
		if(!Get(name,val)) {
			val = def;
		}
		return val;
	}

	char* GetDataString(const char* name, char* val, int len, char* def = "")
	{
		strref strname(name,strlen(name));
		strref strval;
		if (Get(strname, strval)) {
			len = min(strval.second,len);
			strncpy(val, strval.first, len);
			val[len] = 0;
		} else {
			strcpy(val, def);
		}
		return val;
	}
#if _MSC_VER<=1200
#else
	template<class _Ty>
	_Ty GetDataT(std::string& name, const _Ty& def = _Ty())
	{
		std::string val;
		if (Get(name, val)) {
			return strto<_Ty>(val,def);
		}
		return def;
	}
	template<class _Ty>
	_Ty GetDataT(char* name, const _Ty& def = _Ty())
	{
		strref strname(name,strlen(name));
		strref strval;
		if (Get(strname, strval)) {
			return strto<_Ty>(std::string(strval.first,strval.second),def);
		}
		return def;
	}
#endif//

	bool Get(const strref& name, const strref& attr, strref& val)
	{
		if (Find(name)) {
			strrefpairarray& attrs = m_pcurchild->attrs;
			for (size_t i = 0, j = attrs.size(); i < j; ++i)
			{
				strrefpair & keyvalue = attrs[i];
				if (keyvalue.first.second == attr.second) {
					if (strnicmp(keyvalue.first.first, attr.first, attr.second) == 0) {
						val = keyvalue.second;
						return true;
					}
				}
			}
		}
		return false;
	}

	bool Get(const std::string& name, const std::string attr, std::string& val)
	{
		strref strname((char*)name.c_str(),name.length());
		strref strattr((char*)attr.c_str(),attr.length());
		strref strval;
		if (Get(strname, strattr, strval)) {
			stresc2value(strval.first, strval.second, val);
			//val.assign(strval.first, strval.second);
			return true; 
		}
		return false;
	}

	void ResetPos(strrefelement* pelement = 0)
	{
		strrefelementarray& childs = pelement ? pelement->childs : m_elements;
		size_t i = 0, j = childs.size();
		for (; i < j; ++i)
		{
			strrefelement & child = *childs[i];
			if (child.type != node_element) {
				continue;
			}
			if (!child.name.second) {
				strelement(child);
			}
		}
		m_pcurelement = pelement;
		m_pcurchild = 0;
	}

	void IntoElem()
	{
		assert(m_pcurchild);
		ResetPos(m_pcurchild);
	}

	void OutofElem()
	{
		if (m_pcurelement) {
			strrefelement* pelement = m_pcurelement;
			ResetPos(m_pcurelement->parent);
			m_pcurchild = pelement;
		}
	}

	bool Find(const strref& name)
	{
		strrefelement* ptempelement = m_pcurelement;
		if (!name.first || !name.second) {
			//return m_pcurelement;
		} else {
			char* str = strnbrk(name.first, name.second, "\\/");
			if (!str) { //if no sec break find direct.
				ptempelement = FindChild(name, ptempelement);
			} else { //else find child
				strref sec(name.first, str - name.first);
				strref next(str + 1, name.second - sec.second - 1);
				do {
					assert(sec.first && sec.second);
					ptempelement = FindChild(sec, ptempelement);
					if (!ptempelement) {
						break;
					}
					if (!next.first || !next.second) {
						break;
					}
					sec.first = next.first;
					sec.second = next.second;
					str = strnbrk(next.first, next.second, "\\/");
					if (str) {
						sec.second = str - sec.first;
						next.first = str + 1;
						next.second = next.second - sec.second - 1;
					} else {
						next.first = 0;
						next.second = 0;
					}
				} while (true);
			}
		}
		m_pcurchild = ptempelement;
		return (m_pcurchild!=0);
	}

	long First()
	{
		return Next(0);
	}

	long Next(long cookie)
	{
		if (m_pcurelement == 0) {
			if (cookie < m_elements.size()) {
				m_pcurchild = m_elements[cookie];
				++cookie;
				return cookie;
			}
		} else {
			if (cookie < m_pcurelement->childs.size()) {
				m_pcurchild = m_pcurelement->childs[cookie];
				++cookie;
				return cookie;
			}
		}
		return 0;
	}

	long Next()
	{
		long cookie = 0;
		if (m_pcurelement == 0) {
			if (!m_elements.empty()) {
				m_pcurchild = m_elements[cookie];
				++cookie;
				return cookie;
			}
		} else {
			if (m_pcurchild) {
				for (; cookie<m_pcurelement->childs.size(); ++cookie)
				{
					if (m_pcurchild == m_pcurelement->childs[cookie]) {
						++cookie;
						return cookie;
					}
				}
			}
		}
		return 0;
	}

	long FirstAttribute(strref& attr, strref& val)
	{
		return NextAttribute(attr, val, 0);
	}

	long NextAttribute(strref& attr, strref& val, long cookie)
	{
		if (m_pcurelement) {
			if (cookie < m_pcurelement->attrs.size()) {
				attr = m_pcurelement->attrs[cookie].first;
				val = m_pcurelement->attrs[cookie].second;
				++cookie;
				return cookie;
			}
		}
		return 0;
	}

	long FirstAttributeString(std::string& attr, std::string& val)
	{
		long cookie = 0;
		strref strattr, strval;
		if((cookie = FirstAttribute(strattr, strval))) {
			attr.assign(strattr.first, strattr.second);
			val.assign(strval.first, strval.second);
			return cookie;
		}
		return 0;
	}

	long NextAttributeString(std::string& attr, std::string& val, long cookie)
	{
		strref strattr, strval;
		if((cookie = NextAttribute(strattr, strval, cookie))) {
			attr.assign(strattr.first, strattr.second);
			val.assign(strval.first, strval.second);
			return cookie;
		}
		return 0;
	}

	long FirstAttributeString(char* attr, int attrlen, char* val, int vallen)
	{
		long cookie = 0;
		strref strattr, strval;
		if((cookie = FirstAttribute(strattr, strval))) {
			attrlen = min(strattr.second,attrlen);
			strncpy(attr,strattr.first,attrlen);
			attr[attrlen] = 0;
			vallen = min(strval.second,vallen);
			strncpy(val, strval.first, vallen);
			val[vallen] = 0;
			return cookie;
		}
		return 0;
	}
	
	long NextAttributeString(char* attr, int attrlen, char* val, int vallen, long cookie)
	{
		strref strattr, strval;
		if((cookie = NextAttribute(strattr, strval, cookie))) {
			attrlen = min(strattr.second,attrlen);
			strncpy(attr,strattr.first,attrlen);
			attr[attrlen] = 0;
			vallen = min(strval.second,vallen);
			strncpy(val, strval.first, vallen);
			val[vallen] = 0;
			return cookie;
		}
		return 0;
	}

	std::string& GetAttributeString(std::string& name, std::string& attr, std::string& val, std::string& def = std::string(""))
	{
		if (!Get(name, attr, val)) {
			val = def;
		}
		return val;
	}
	char* GetAttributeString(char* name, char* attr, char* val, int len, char* def = "")
	{
		strref strname(name,strlen(name));
		strref strattr(attr,strlen(attr));
		strref strval;
		if (Get(strname, strattr, strval)) {
			len = min(strval.second,len);
			strncpy(val, strval.first, len);
			val[len] = 0;
		} else {
			strcpy(val, def);
		}
		return val;
	}

#if _MSC_VER<=1200

/*#define USES_XML_CONVERSION \
	strref __strxmlval

#define GetAttributeT(t,n,a,d) \
	(Get(strref(n), strref(a), __strxmlval) ? data_cast<t>(std::string(__strxmlval.first,__strxmlval.second),def) : def) */
	
#else
	template<class _Ty>
	_Ty GetAttributeT(std::string& name, std::string& attr, const _Ty& def = _Ty())
	{
		std::string val;
		if (Get(name, attr, val)) {
			return strto<_Ty>(val,def);
		}
		return def;
	}
	template<class _Ty>
	_Ty GetAttributeT(char* name, char* attr, const _Ty& def = _Ty())
	{
		strref strname(name,strlen(name));
		strref strattr(attr,strlen(attr));
		strref strval;
		if (Get(strname, strattr, strval)) {
			return strto<_Ty>(std::string(strval.first,strval.second),def);
		}
		return def;
	}
#endif//

protected:
	strrefelement* FindChild(const strref& name, strrefelement* parent)
	{
		//first find cur child
		if (m_pcurchild && m_pcurchild->parent == parent) {
			if (m_pcurchild->name.second == name.second && 
				!strnicmp(m_pcurchild->name.first, name.first, name.second)) {
				return m_pcurchild;
			}
		}
		//second find all child
		strrefelementarray& childs = parent ? parent->childs : m_elements;
		size_t i = 0, j = childs.size();
		for (; i < j; ++i)
		{
			strrefelement & child = *childs[i];
			if (child.type != node_element) {
				continue;
			}
			if (!child.name.second) {
				strelement(child);
			}
			if (child.name.second == name.second && 
				!strnicmp(child.name.first, name.first, name.second)) {
					return childs[i];
			}
		}
		return 0;
	}
};

class MarkupParserW : public ParserW
{
	typedef ParserW Base;
protected:
	int m_cp;
	wcsrefelementarray m_elements;
	wcsrefelement* m_pcurelement; //cur element
	wcsrefelement* m_pcurchild; //cur child

public:
	MarkupParserW()
	{
		m_cp = 0;
		//m_elements;
		m_pcurelement = 0;	//current element
		m_pcurchild = 0;	//cur child
	}
	~MarkupParserW()
	{
		m_cp = 0;
		for (size_t i = 0, j = m_elements.size(); i < j; ++i)
		{
			delete m_elements[i];
		}
		m_elements.clear();
		m_pcurelement = 0;	//current element
		m_pcurchild = 0;	//cur child
	}

	const std::wstring& Set(std::wstring& str)
	{
		m_cp = CP_UTF8;
		wchar_t* strencode = wcsnexts((wchar_t*)str.c_str(), L"encoding=\"", L"", L"\r\n");
		if (strencode) {
			strencode += 10;
			wchar_t* strencodeend = wcschr(strencode, L'\"');
			if(strencodeend) {
				m_cp = wcsn2encode(strencode, strencodeend - strencode, m_cp);
			}
		}
		m_pcurelement = 0;
		m_pcurchild = 0;
		for (size_t i = 0, j = m_elements.size(); i < j; ++i)
		{
			delete m_elements[i];
		}
		m_elements.clear();

		Base::Set(str);
		wcsmarkup(m_elements, (wchar_t*)m_str.c_str(), m_str.length());
		return m_str;
	}

	const std::wstring& Set(std::string& str, int cp = CP_UTF8)
	{
		std::wstring wcs;
		mb2wc(str, wcs, cp);
		return Set(wcs);
	}

	const std::wstring& Get()
	{
		return Base::Get();
	}

	int GetCP() 
	{
		return m_cp;
	}

	int GetSize()
	{
		if (m_pcurelement) {
			return m_pcurelement->childs.size();
		}
		return m_elements.size();
	}

	int GetType(bool child = false)
	{
		if (!child) {
			if (m_pcurelement) {
				return m_pcurelement->type;
			}
		} else {
			if (m_pcurchild) {
				return m_pcurchild->type;
			}
		}
		return node_element;
	}

	bool GetName(wcsref& name, bool child = false)
	{
		if (!child) {
			if (m_pcurelement) {
				name = m_pcurelement->name;
				return true;
			}
		} else {
			if (m_pcurchild) {
				name = m_pcurchild->name;
				return true;
			}
		}
		return false;
	}

	bool GetName(std::wstring& name, bool child = false)
	{
		wcsref strname;
		if (GetName(strname, child)) {
			name.assign(strname.first, strname.second);
			return true;
		}
		return false;
	}

	std::wstring& GetTagName(std::wstring& name, bool child = false)
	{
		GetName(name, child);
		return name;
	}

	wchar_t* GetTagName(wchar_t* name, int len, bool child = false)
	{
		wcsref strname;
		if(GetName(strname, child)) {
			len = min(strname.second,len);
			wcsncpy(name, strname.first, len);
			name[len] = 0;
		}
		return name;
	}

	//get data
	bool Get(const wcsref& name, wcsref& val)
	{
		bool ret = false;
		if (Find(name)) {
			if (m_pcurchild->childs.size() == 1) {
				switch (m_pcurchild->childs[0]->type)
				{
				case node_cdata_section:
					// <![CDATA[...]]> cdata section
					val.first = m_pcurchild->data.first + 9;
					val.second = m_pcurchild->data.second - 12;
					ret = true;
					break;
				case node_processing_instruction:
					// <?target ...?> processing instruction
					val.first = m_pcurchild->data.first + 8;
					val.second = m_pcurchild->data.second - 10;
					ret = true;
					break;
				case node_comment: 
					//<!--...--> comment
					val.first = m_pcurchild->data.first + 4;
					val.second = m_pcurchild->data.second - 7;
					ret = true;
					break;
				case node_document_type: 
					// <!DOCTYPE ...> dtd
					val.first = m_pcurchild->data.first + 9;
					val.second = m_pcurchild->data.second - 10;
					ret = true;
					break;
				default:
					break;
				}
			} 
			if (!ret) {
				val = m_pcurchild->data;
				ret = true;
			}
		}
		return ret;
	}

	bool Get(const std::wstring& name, std::wstring& val)
	{
		wcsref strname((wchar_t*)name.c_str(),name.size());
		wcsref strval;
		if (Get(strname, strval)) {
			val.assign(strval.first, strval.second);
			return true; 
		}
		return false;
	}

	bool Get(const wcsref& name, const wcsref& attr, wcsref& val)
	{
		if (Find(name)) {
			wcsrefpairarray& attrs = m_pcurchild->attrs;
			for (size_t i = 0, j = attrs.size(); i < j; ++i)
			{
				wcsrefpair & keyvalue = attrs[i];
				if (keyvalue.first.second == attr.second) {
					if (wcsnicmp(keyvalue.first.first, attr.first, attr.second) == 0) {
						val = keyvalue.second;
						return true;
					}
				}
			}
		}
		return false;
	}

	std::wstring& GetDataString(const std::wstring& name, std::wstring& val, std::wstring& def = std::wstring(L""))
	{
		if(!Get(name,val)) {
			val = def;
		}
		return val;
	}

	wchar_t* GetDataString(const wchar_t* name, wchar_t* val, int len, wchar_t* def = L"")
	{
		wcsref strname(name,wcslen(name));
		wcsref strval;
		if (Get(strname, strval)) {
			len = min(strval.second,len);
			wcsncpy(val, strval.first, len);
			val[len] = 0;
		} else {
			wcscpy(val, def);
		}
		return val;
	}

#if _MSC_VER<=1200
#else
	template<class _Ty>
	_Ty GetDataT(std::wstring& name, const _Ty& def = _Ty())
	{
		std::wstring val;
		if (Get(name, val)) {
			return wcsto<_Ty>(val,def);
		}
		return def;
	}
	template<class _Ty>
	_Ty GetDataT(wchar_t* name, const _Ty& def = _Ty())
	{
		wcsref strname(name,wcslen(name));
		wcsref strval;
		if (Get(strname, strval)) {
			return wcsto<_Ty>(std::wstring(strval.first,strval.second),def);
		}
		return def;
	}
#endif//

	bool Get(const std::wstring& name, const std::wstring attr, std::wstring& val)
	{
		wcsref strname((wchar_t*)name.c_str(),name.size());
		wcsref wcsrefattr((wchar_t*)attr.c_str(),attr.size());
		wcsref strval;
		if (Get(strname, wcsrefattr, strval)) {
			wcsesc2value(strval.first, strval.second, val);
			//val.assign(strval.first, strval.second);
			return true; 
		}
		return false;
	}

	void ResetPos(wcsrefelement* pelement = 0)
	{
		wcsrefelementarray& childs = pelement ? pelement->childs : m_elements;
		size_t i = 0, j = childs.size();
		for (; i < j; ++i)
		{
			wcsrefelement & child = *childs[i];
			if (child.type != node_element) {
				continue;
			}
			if (!child.name.second) {
				wcselement(child);
			}
		}
		m_pcurelement = pelement;
		m_pcurchild = 0;
	}

	void IntoElem()
	{
		assert(m_pcurchild);
		ResetPos(m_pcurchild);
	}

	void OutofElem()
	{
		if (m_pcurelement) {
			wcsrefelement* pelement = m_pcurelement;
			ResetPos(m_pcurelement->parent);
			m_pcurchild = pelement;
		}
	}


	bool Find(const wcsref& name)
	{
		wcsrefelement* ptempelement = m_pcurelement;
		if (!name.first || !name.second) {
			//return m_pcurelement;
		} else {
			wchar_t* str = wcsnbrk(name.first, name.second, L"\\/");
			if (!str) { //if no sec break find direct.
				ptempelement = FindChild(name, ptempelement);
			} else { //else find child
				wcsref sec(name.first, str - name.first);
				wcsref next(str + 1, name.second - sec.second - 1);
				do {
					assert(sec.first && sec.second);
					ptempelement = FindChild(sec, ptempelement);
					if (!ptempelement) {
						break;
					}
					if (!next.first || !next.second) {
						break;
					}
					sec.first = next.first;
					sec.second = next.second;
					str = wcsnbrk(next.first, next.second, L"\\/");
					if (str) {
						sec.second = str - sec.first;
						next.first = str + 1;
						next.second = next.second - sec.second - 1;
					} else {
						next.first = 0;
						next.second = 0;
					}
				} while (true);
			}
		}
		m_pcurchild = ptempelement;
		return (m_pcurchild!=0);
	}

	long First()
	{
		return Next(0);
	}

	long Next(long cookie)
	{
		if (m_pcurelement == 0) {
			if (cookie < m_elements.size()) {
				m_pcurchild = m_elements[cookie];
				++cookie;
				return cookie;
			}
		} else {
			if (cookie < m_pcurelement->childs.size()) {
				m_pcurchild = m_pcurelement->childs[cookie];
				++cookie;
				return cookie;
			}
		}
		return 0;
	}

	long Next()
	{
		long cookie = 0;
		if (m_pcurelement == 0) {
			if (!m_elements.empty()) {
				m_pcurchild = m_elements[cookie];
				++cookie;
				return cookie;
			}
		} else {
			if (m_pcurchild) {
				for (; cookie<m_pcurelement->childs.size(); ++cookie)
				{
					if (m_pcurchild == m_pcurelement->childs[cookie]) {
						++cookie;
						return cookie;
					}
				}
			}
		}
		return 0;
	}

	long FirstAttribute(wcsref& attr, wcsref& val)
	{
		return NextAttribute(attr, val, 0);
	}
	
	long NextAttribute(wcsref& attr, wcsref& val, long cookie)
	{
		if (m_pcurelement) {
			if (cookie < m_pcurelement->attrs.size()) {
				attr = m_pcurelement->attrs[cookie].first;
				val = m_pcurelement->attrs[cookie].second;
				++cookie;
				return cookie;
			}
		}
		return 0;
	}
	
	long FirstAttributeString(std::wstring& attr, std::wstring& val)
	{
		long cookie = 0;
		wcsref wcsattr, wcsval;
		if((cookie = FirstAttribute(wcsattr, wcsval))) {
			attr.assign(wcsattr.first, wcsattr.second);
			val.assign(wcsval.first, wcsval.second);
			return cookie;
		}
		return 0;
	}
	
	long NextAttributeString(std::wstring& attr, std::wstring& val, long cookie)
	{
		wcsref wcsattr, wcsval;
		if((cookie = NextAttribute(wcsattr, wcsval, cookie))) {
			attr.assign(wcsattr.first, wcsattr.second);
			val.assign(wcsval.first, wcsval.second);
			return cookie;
		}
		return 0;
	}
	
	long FirstAttributeString(wchar_t* attr, int attrlen, wchar_t* val, int vallen)
	{
		long cookie = 0;
		wcsref wcsattr, wcsval;
		if((cookie = FirstAttribute(wcsattr, wcsval))) {
			attrlen = min(wcsattr.second,attrlen);
			wcsncpy(attr,wcsattr.first,attrlen);
			attr[attrlen] = 0;
			vallen = min(wcsval.second,vallen);
			wcsncpy(val, wcsval.first, vallen);
			val[vallen] = 0;
			return cookie;
		}
		return 0;
	}
	
	long NextAttributeString(wchar_t* attr, int attrlen, wchar_t* val, int vallen, long cookie)
	{
		wcsref wcsattr, wcsval;
		if((cookie = NextAttribute(wcsattr, wcsval, cookie))) {
			attrlen = min(wcsattr.second,attrlen);
			wcsncpy(attr,wcsattr.first,attrlen);
			attr[attrlen] = 0;
			vallen = min(wcsval.second,vallen);
			wcsncpy(val, wcsval.first, vallen);
			val[vallen] = 0;
			return cookie;
		}
		return 0;
	}

	std::wstring& GetAttributeString(std::wstring& name, std::wstring& attr, std::wstring& val, std::wstring& def = std::wstring(L""))
	{
		if (!Get(name, attr, val)) {
			val = def;
		}
		return val;
	}
	wchar_t* GetAttributeString(wchar_t* name, wchar_t* attr, wchar_t* val, int len, wchar_t* def = L"")
	{
		wcsref strname(name,wcslen(name));
		wcsref strattr(attr,wcslen(attr));
		wcsref strval;
		if (Get(strname, strattr, strval)) {
			len = min(strval.second,len);
			wcsncpy(val, strval.first, len);
			val[len] = 0;
		} else {
			wcscpy(val, def);
		}
		return val;
	}

#if _MSC_VER<=1200
#else
	template<typename _Ty>
	_Ty GetAttributeT(std::wstring& name, std::wstring& attr, const _Ty& def = _Ty())
	{
		std::wstring val;
		if (Get(name, attr, val)) {
			return wcsto<_Ty>(val,def);
		}
		return def;
	}
	template<typename _Ty>
	_Ty GetAttributeT(wchar_t* name, wchar_t* attr, const _Ty& def = _Ty())
	{
		wcsref strname(name,wcslen(name));
		wcsref strattr(attr,wcslen(attr));
		wcsref strval;
		if (Get(strname, strattr, strval)) {
			return wcsto<_Ty>(std::wstring(strval.first,strval.second),def);
		}
		return def;
	}
#endif//

protected:
	wcsrefelement* FindChild(const wcsref& name, wcsrefelement* parent)
	{
		//first find cur child
		if (m_pcurchild && m_pcurchild->parent == parent) {
			if (m_pcurchild->name.second == name.second && 
				!wcsnicmp(m_pcurchild->name.first, name.first, name.second)) {
					return m_pcurchild;
			}
		}
		//second find all child
		wcsrefelementarray& childs = parent ? parent->childs : m_elements;
		size_t i = 0, j = childs.size();
		for (; i < j; ++i)
		{
			wcsrefelement & child = *childs[i];
			if (child.type != node_element) {
				continue;
			}
			if (!child.name.second) {
				wcselement(child);
			}
			if (child.name.second == name.second && 
				!wcsnicmp(child.name.first, name.first, name.second)) {
					return childs[i];
			}
		}
		return 0;
	}
};

#ifdef UNICODE
#define MarkupParser MarkupParserW
#else
#define MarkupParser MarkupParserA
#endif//

typedef KeyValueParserA KeyValueA;
typedef KeyValueParserW KeyValueW;
typedef CommandLineParserA CommandLineA;
typedef CommandLineParserW CommandLineW;
typedef ProfileParserA ProfileReaderA;
typedef ProfileParserW ProfileReaderW;
typedef MarkupParserA MarkupReaderA;
typedef MarkupParserW MarkupReaderW;
#define KeyValue KeyValueParser
#define CommandLine CommandLineParser
#define ProfileReader ProfileParser
#define MarkupReader MarkupParser

class ProfileWriterA : public ProfileReaderA
{
	typedef ProfileReaderA Base;
protected:
	typedef std::map<std::string,std::string,stringiless> strsec2value;
	strsec2value m_strsec2value;
	typedef std::map<std::string,std::string,stringiless> strkey2value;
	typedef std::map<std::string,strkey2value,stringiless> strsec2key2value;
	strsec2key2value m_strsec2key2value;

public:
	const std::string& Set(std::string& str)
	{
		m_strsec2key2value.clear();
		m_strsec2value.clear();
		return Base::Set(str);
	}

	const std::string& Get() 
	{
		if (IsModify()) {
			std::stringstream ss;
			strrefsec2value::iterator it_sv = m_sec2value.begin();
			for (; it_sv != m_sec2value.end(); ++it_sv)
			{
				const strref& sec = it_sv->first;
				const strref& sec_value = it_sv->second;
				ss << "\r\n[";
				ss.write(sec.first, sec.second);
				ss << "]\r\n";
				ss.write(sec_value.first, sec_value.second);
				ss << "\r\n";
			}
			return Set(ss.str());
		} else {
			return Base::Get();
		}
	}

	bool IsModify()
	{
		return (m_strsec2key2value.empty() && m_strsec2value.empty()) ? false : true;
	}

	bool Get(const strref & sec, strref & val)
	{
		strrefsec2key2value::iterator iter_skv = m_sec2key2value.find(sec);
		if (iter_skv != m_sec2key2value.end()) {
			std::stringstream ss;
			strrefkey2value& key2value = iter_skv->second;
			strrefkey2value::iterator it_kv = key2value.begin();
			for (; it_kv != key2value.end(); ++it_kv)
			{
				ss.write(it_kv->first.first, it_kv->first.second);
				ss << "=";
				ss.write(it_kv->second.first, it_kv->second.second);
				ss << "\r\n";
			}
			Set(std::string(sec.first,sec.second), ss.str());
		}
		return Base::Get(sec, val);
	}

	bool Get(const std::string & sec, std::string & val)
	{
		strref pstrsec((char*)sec.c_str(),sec.size());
		strref strval;
		if(Get(pstrsec, strval)) {
			val.assign(strval.first,strval.second);
			return true;
		}
		return false;
	}

	bool Get(const strref & sec, const strref & key, strref & val)
	{
		return Base::Get(sec, key, val);
	}

	bool Get(const std::string & sec, const std::string & key, std::string & val)
	{
		return Base::Get(sec, key, val);
	}

	bool Set(const strref & sec, const strref & val)
	{
		m_sec2key2value.erase(sec);
		m_sec2value[sec] = val;
		return true;
	}

	bool Set(const std::string & sec, const std::string & val)
	{
		std::stringstream ss;
		ss << "[" << sec << "]\r\n" << val;
		m_strsec2value[sec] = ss.str();
		std::string & strvalue  = m_strsec2value[sec];
		strref pstrsec((char*)strvalue.c_str() + 1, sec.size());
		strref strval((char*)strvalue.c_str() + 1 + sec.size() + 1, val.size());
		return Set(pstrsec, strval);
	}

	bool Set(const strref & sec, const strref & key, const strref & val)
	{
		strrefsec2key2value::iterator iter_skv = m_sec2key2value.find(sec);
		if (iter_skv == m_sec2key2value.end()) {
			strref sec_value;
			if (Get(sec, sec_value)) {
				strnkeyvalue(m_sec2key2value[sec], sec_value.first, sec_value.second);
			}
		}
		strrefpair keyvalue(key,val);
		strrefkey2value & key2value = m_sec2key2value[sec];
		size_t i = 0, j = key2value.size();
		for (; i < j; ++i)
		{
			strrefpair & keyvalue = key2value[i];
			if (keyvalue.first.second == key.second) {
				if (strnicmp(keyvalue.first.first, key.first, key.second) == 0) {
					break;
				}
			}
		}
		if (i < j) {
			key2value[i] = keyvalue;
		} else {
			key2value.push_back(keyvalue);
		}
		return true;
	}

	bool Set(const std::string & sec, const std::string & key, const std::string & val)
	{
		m_strsec2key2value[sec][key] = val;
		strsec2key2value::iterator iter_skv = m_strsec2key2value.find(sec);
		const std::string & strsec  = iter_skv->first;
		strkey2value & key2value = iter_skv->second;
		strkey2value::iterator it_kv = key2value.find(key);
		const std::string & strkey  = it_kv->first;
		const std::string & strvalue  = it_kv->second;
		strref pstrsec((char*)strsec.c_str(), strsec.size());
		strref pstrkey((char*)strkey.c_str(), strkey.size());
		strref strval((char*)strvalue.c_str(), strvalue.size());
		return Set(pstrsec, pstrkey, strval);
	}

	bool WriteProfileSection(std::string& sec, std::string& val)
	{
		return Set(sec, val);
	}
	bool WriteProfileSection(char* sec, char* val)
	{
		return Set(std::string(sec), std::string(val));
	}
	bool WriteProfileString(std::string& sec, std::string& key, std::string& val)
	{
		return Set(sec, key, val);
	}
	bool WriteProfileString(char* sec, char* key, char* val)
	{
		return Set(std::string(sec), std::string(key), std::string(val));
	}
};

class ProfileWriterW : public ProfileReaderW
{
	typedef ProfileReaderW Base;
protected:
	typedef std::map<std::wstring,std::wstring,wstringiless> wcssec2value;
	wcssec2value m_wcssec2value;
	typedef std::map<std::wstring,std::wstring,wstringiless> wcskey2value;
	typedef std::map<std::wstring,wcskey2value,wstringiless> wcssec2key2value;
	wcssec2key2value m_wcssec2key2value;

public:
	const std::wstring& Set(std::wstring& wcs)
	{
		m_wcssec2key2value.clear();
		m_wcssec2value.clear();
		return Base::Set(wcs);
	}

	const std::wstring& Get() 
	{
		if (IsModify()) {
			std::wstringstream ss;
			wcsrefsec2value::iterator it_sv = m_sec2value.begin();
			for (; it_sv != m_sec2value.end(); ++it_sv)
			{
				const wcsref& sec = it_sv->first;
				const wcsref& sec_value = it_sv->second;
				ss << L"\r\n[";
				ss.write(sec.first, sec.second);
				ss << L"]\r\n";
				ss.write(sec_value.first, sec_value.second);
				ss << L"\r\n";
			}
			return Set(ss.str());
		} else {
			return Base::Get();
		}
	}

	bool IsModify() 
	{
		return (m_wcssec2key2value.empty() && m_wcssec2value.empty()) ? false : true;
	}

	bool Get(const wcsref & sec, wcsref & val)
	{
		wcsrefsec2key2value::iterator iter_skv = m_sec2key2value.find(sec);
		if (iter_skv != m_sec2key2value.end()) {
			std::wstringstream ss;
			wcsrefkey2value& key2value = iter_skv->second;
			wcsrefkey2value::iterator it_kv = key2value.begin();
			for (; it_kv != key2value.end(); ++it_kv)
			{
				ss.write(it_kv->first.first, it_kv->first.second);
				ss << L"=";
				ss.write(it_kv->second.first, it_kv->second.second);
				ss << L"\r\n";
			}
			Set(std::wstring(sec.first,sec.second), ss.str());
		}
		return Base::Get(sec, val);
	}

	bool Get(const std::wstring & sec, std::wstring & val)
	{
		wcsref strsec((wchar_t*)sec.c_str(),sec.size());
		wcsref strval;
		if(Get(strsec, strval)) {
			val.assign(strval.first,strval.second);
			return true;
		}
		return false;
	}

	bool Get(const wcsref & sec, const wcsref & key, wcsref & val)
	{
		return Base::Get(sec, key, val);
	}

	bool Get(const std::wstring & sec, const std::wstring & key, std::wstring & val)
	{
		return Base::Get(sec, key, val);
	}

	bool Set(const wcsref & sec, const wcsref & val)
	{
		m_sec2key2value.erase(sec);
		m_sec2value[sec] = val;
		return true;
	}

	bool Set(const std::wstring & sec, const std::wstring & val)
	{
		std::wstringstream ss;
		ss << L"[" << sec << L"]\r\n" << val;
		m_wcssec2value[sec] = ss.str();
		std::wstring & wcsvalue  = m_wcssec2value[sec];
		wcsref strsec((wchar_t*)wcsvalue.c_str() + 1, sec.size());
		wcsref strval((wchar_t*)wcsvalue.c_str() + 1 + sec.size() + 1, val.size());
		return Set(strsec, strval);
	}

	bool Set(const wcsref & sec, const wcsref & key, const wcsref & val)
	{
		wcsrefsec2key2value::iterator iter_skv = m_sec2key2value.find(sec);
		if (iter_skv == m_sec2key2value.end()) {
			wcsref sec_value;
			if (Get(sec, sec_value)) {
				wcsnkeyvalue(m_sec2key2value[sec], sec_value.first, sec_value.second);
			}
		}
		wcsrefpair keyvalue(key,val);
		wcsrefkey2value & key2value = m_sec2key2value[sec];
		size_t i = 0, j = key2value.size();
		for (; i < j; ++i)
		{
			wcsrefpair & keyvalue = key2value[i];
			if (keyvalue.first.second == key.second) {
				if (wcsnicmp(keyvalue.first.first, key.first, key.second) == 0) {
					break;
				}
			}
		}
		if (i < j) {
			key2value[i] = keyvalue;
		} else {
			key2value.push_back(keyvalue);
		}
		return true;
	}

	bool Set(const std::wstring & sec, const std::wstring & key, const std::wstring & val)
	{
		m_wcssec2key2value[sec][key] = val;
		wcssec2key2value::iterator iter_skv = m_wcssec2key2value.find(sec);
		const std::wstring & wcssec  = iter_skv->first;
		wcskey2value & key2value = iter_skv->second;
		wcskey2value::iterator it_kv = key2value.find(key);
		const std::wstring & wcskey  = it_kv->first;
		const std::wstring & wcsvalue  = it_kv->second;
		wcsref strsec((wchar_t*)wcssec.c_str(), wcssec.size());
		wcsref strkey((wchar_t*)wcskey.c_str(), wcskey.size());
		wcsref strval((wchar_t*)wcsvalue.c_str(), wcsvalue.size());
		return Set(strsec, strkey, strval);
	}

	bool WriteProfileSection(std::wstring& sec, std::wstring& val)
	{
		return Set(sec, val);
	}
	bool WriteProfileSection(wchar_t* sec, wchar_t* val)
	{
		return Set(std::wstring(sec), std::wstring(val));
	}
	bool WriteProfileString(std::wstring& sec, std::wstring& key, std::wstring& val)
	{
		return Set(sec, key, val);
	}
	bool WriteProfileString(wchar_t* sec, wchar_t* key, wchar_t* val)
	{
		return Set(std::wstring(sec), std::wstring(key), std::wstring(val));
	}
};

#ifdef UNICODE
#define ProfileWriter ProfileWriterA
#else
#define ProfileWriter ProfileWriterW
#endif//

class MarkupWriterA : public MarkupReaderA
{//所有的SET之后，要GET的话，最好调用Update更新XML结构，这样才能保证Get正确数据
	typedef MarkupReaderA Base;
protected:
	typedef std::map<std::string,std::string,std::stringiless> strname2value;
	strname2value m_name2data;
	typedef std::map<std::string,std::string,std::stringiless> strattr2value;
	typedef std::map<std::string,strattr2value> strname2attr2value;
	strname2attr2value m_name2attr2value;
	bool m_modify;
public:
	MarkupWriterA() : m_modify(false) {}

	const std::string& Set(std::string& str)
	{
		m_modify = false;
		m_name2data.clear();
		m_name2attr2value.clear();
		return Base::Set(str);
	}

	const std::string& Set(std::wstring& str)
	{
		m_modify = false;
		m_name2data.clear();
		m_name2attr2value.clear();
		return Base::Set(str);
	}

	bool IsModify()
	{
		return m_modify;
	}

	void Modify(bool modify)
	{
		m_modify = modify;
	}

	void Update()
	{
		if (IsModify()) {
			std::stringstream ss;
			strrefelementarray& childs = m_elements;
			for (size_t i = 0, j = childs.size(); i < j; ++i)
			{
				std::string val;
				strelement2value(*childs[i], val);
				ss << val;
			}
			Set(ss.str());
		}
	}

	/*bool Get(const strref& name, strref& val)
	{
		if(Find(name)) {
			if (m_pcurchild->childs.empty()) {
				val = m_pcurchild->data;
			}
		}
		return false;
	}

	bool Get(const std::string& name, std::string& val)
	{
		return Base::Get(name, val);
	}

	bool Get(const strref& name, const strref& attr, strref& val)
	{
		return Base::Get(name, attr, val);
	}

	bool Get(const std::string& name, const std::string attr, std::string& val)
	{
		return Base::Get(name, attr, val);
	}*/

	bool Set(const strref& name, const strref& val)
	{
		Modify(true);
		FindNew(name);
		m_pcurchild->clearchild();
		m_pcurchild->data = val;
		strmarkup(m_pcurchild->childs, m_pcurchild->data.first, m_pcurchild->data.second);
		return true;
	}

	bool Set(const std::string& name, const std::string& val)
	{
		std::string fullname(name);
		if (m_pcurelement) {
			strrefelement* pelem = m_pcurelement;
			while(pelem) 
			{
				if (fullname.length() > 0) {
					fullname.insert(0, "\\");
				}
				fullname.insert(0, pelem->name.first, pelem->name.second);
				pelem = pelem->parent;
			}
		}

		std::string fullnamex = fullname + tostr(m_pcurelement);
		strname2value::iterator it = m_name2data.find(fullnamex);
		assert(it != m_name2data.end());
		const std::string& strname = it->first;
		std::string& strval = it->second;
		return Set(
			strref((char*)strname.c_str() + strname.length() - ((fullnamex.length()-fullname.length())+name.length()), name.length()), 
			strref((char*)strval.c_str(), strval.length()));
	}

	bool Set(const strref& name, const strref& attr, const strref& val)
	{
		Modify(true);
		FindNew(name);
		strrefpair attrvalue(attr,val);
		strrefpairarray& attrs = m_pcurchild->attrs;
		size_t i = 0, j = attrs.size();
		for (; i < j; ++i)
		{
			strrefpair & keyvalue = attrs[i];
			if (keyvalue.first.second == attr.second) {
				if (strnicmp(keyvalue.first.first, attr.first, attr.second) == 0) {
					break;
				}
			}
		}
		if (i < j) {
			attrs[i] = attrvalue;
		} else {
			attrs.push_back(attrvalue);
		}
		return true;
	}

	bool Set(const std::string& name, const std::string attr, const std::string& val)
	{
		std::string fullname(name);
		if (m_pcurelement) {
			strrefelement* pelem = m_pcurelement;
			while(pelem) 
			{
				if (fullname.length() > 0) {
					fullname.insert(0, "\\");
				}
				fullname.insert(0, pelem->name.first, pelem->name.second);
				pelem = pelem->parent;
			}
		}

		std::string fullnamex = fullname + tostr(m_pcurelement);
		strname2attr2value::iterator iter_nav = m_name2attr2value.find(fullnamex);
		assert (iter_nav != m_name2attr2value.end());
		strattr2value& attr2value = iter_nav->second;
		attr2value[attr] = val;
		strattr2value::iterator iter_av = attr2value.find(attr);
		const std::string& strname = iter_nav->first;
		const std::string& strattr = iter_av->first;
		const std::string& strval = iter_av->second;
		return Set(strref((char*)strname.c_str() + strname.length() - ((fullnamex.length()-fullname.length())+name.length()), name.length()),
			strref((char*)strattr.c_str(), strattr.length()), strref((char*)strval.c_str(), strval.length()));
	}

	bool New(const std::string& name)
	{
		Modify(true);
		strrefelement* elem = new strrefelement(m_pcurelement);
		elem->type = node_element;
		//elem->name = name;
		if (!m_pcurelement) {
			m_elements.push_back(elem);
			m_pcurchild = m_elements.back();
		} else {
			m_pcurelement->childs.push_back(elem);
			m_pcurchild = m_pcurelement->childs.back();
		}

		std::string fullname(name);
		if (m_pcurelement) {
			strrefelement* pelem = m_pcurelement;
			while(pelem) 
			{
				if (fullname.length() > 0) {
					fullname.insert(0, "\\");
				}
				fullname.insert(0, pelem->name.first, pelem->name.second);
				pelem = pelem->parent;
			}
		}

		std::string fullnamex;
#if 1
		fullnamex = fullname + tostr(m_pcurchild);
		assert(m_name2data.find(fullnamex)==m_name2data.end());
		assert(m_name2attr2value.find(fullnamex)==m_name2attr2value.end());
		strname2attr2value::iterator it = m_name2attr2value.end();
#else
		strname2attr2value::iterator it = m_name2attr2value.end();
		do 
		{
			fullnamex = fullname + tostr(m_pcurchild);
			it = m_name2attr2value.find(fullnamex);
		} while (it != m_name2attr2value.end());
#endif//
		m_name2data[fullnamex];
		m_name2attr2value[fullnamex];
		it = m_name2attr2value.find(fullnamex);
		elem->name = strref(it->first.c_str() + it->first.length() - ((fullnamex.length()-fullname.length())+name.length()), name.length());
		return true;
		//return New(strref(it->first.c_str() + it->first.length() - ((fullnamex.length()-fullname.length())+name.length()), name.length()));
	}

	bool Delete()
	{
		Modify(true);
		if (m_pcurchild) {
			strrefelement* parent = m_pcurchild->parent;
			strrefelementarray& childs = parent ? parent->childs : m_elements;
			for (size_t i = 0, j = childs.size(); i < j; ++i)
			{
				if (childs[i] == m_pcurchild) {
					childs.erase(childs.begin() + i);
					delete m_pcurchild;
					m_pcurchild = 0;
					break;
				}
			}
		}
	}

	bool SetAttributeString(std::string& name, std::string& attr, std::string& val)
	{
		return Set(name, attr, val);
	}
	bool SetAttributeString(char* name, char* attr, char* val)
	{
		return Set(std::string(name), std::string(attr), std::string(val));
	}

	template<class _Ty>
	_Ty SetAttributeT(std::string& name, std::string& attr, _Ty val)
	{
		return Set(name, attr, tostr(val));
	}
	template<class _Ty>
	_Ty SetAttributeT(char* name, char* attr, _Ty val)
	{
		return Set(std::string(name), std::string(attr), tostr(val));
	}

protected:
	//find or new all element form parent
	bool FindNew(const strref& name)
	{
		strrefelement* ptempelement = m_pcurelement;
		if (!name.first || !name.second) {
			//return m_pcurelement;
		} else {
			char* str = strnbrk(name.first, name.second, "\\/");
			if (!str) { //if no sec break find direct.
				ptempelement = FindChildNew(name, ptempelement);
			} else { //else find child
				strref sec(name.first, str - name.first);
				strref next(str + 1, name.second - sec.second - 1);
				do {
					assert(sec.first && sec.second);
					ptempelement = FindChildNew(sec, ptempelement);
					if (!ptempelement) {
						break;
					}
					if (!next.first || !next.second) {
						break;
					}
					sec.first = next.first;
					sec.second = next.second;
					str = strnbrk(next.first, next.second, "\\/");
					if (str) {
						sec.second = str - sec.first;
						next.first = str + 1;
						next.second = next.second - sec.second - 1;
					} else {
						next.first = 0;
						next.second = 0;
					}
				} while (true);
			}
		}
		m_pcurchild = ptempelement;
		return (m_pcurchild!=0);
	}

	//just find or new child
	strrefelement* FindChildNew(const strref& name, strrefelement* parent)
	{
		//first find cur child
		if (m_pcurchild && m_pcurchild->parent == parent) {
			if (m_pcurchild->name.second == name.second && 
				!strnicmp(m_pcurchild->name.first, name.first, name.second)) {
					return m_pcurchild;
			}
		}
		//second find all child
		strrefelementarray& childs = parent ? parent->childs : m_elements;
		size_t i = 0, j = childs.size();
		for (; i < j; ++i)
		{
			strrefelement & child = *childs[i];
			if (child.type != node_element) {
				continue;
			}
			if (!child.name.second) {
				strelement(child);
			}
			if (child.name.second == name.second && 
				!strnicmp(child.name.first, name.first, name.second)) {
					return childs[i];
			}
		}
		//new
		strrefelement* elem = new strrefelement(parent);
		elem->type = node_element;
		elem->name = name;
		childs.push_back(elem);
		return childs.back();
	}
};

class MarkupWriterW : public MarkupReaderW
{//所有的SET之后，要GET的话，最好调用Update更新XML结构，这样才能保证Get正确数据
	typedef MarkupReaderW Base;
protected:
	typedef std::map<std::wstring,std::wstring,std::wstringiless> wcsname2value;
	wcsname2value m_name2data;
	typedef std::map<std::wstring,std::wstring,std::wstringiless> wcsattr2value;
	typedef std::map<std::wstring,wcsattr2value> wcsname2attr2value;
	wcsname2attr2value m_name2attr2value;
	bool m_modify;
public:
	MarkupWriterW() : m_modify(false) {}

	const std::wstring& Set(std::wstring& wcs)
	{
		m_modify = false;
		m_name2data.clear();
		m_name2attr2value.clear();
		return Base::Set(wcs);
	}

	const std::wstring& Set(std::string& str, int cp = CP_UTF8)
	{
		m_modify = false;
		m_name2data.clear();
		m_name2attr2value.clear();
		return Base::Set(str, cp);
	}

	bool IsModify()
	{
		return m_modify;
	}

	void Modify(bool modify)
	{
		m_modify = modify;
	}

	void Update()
	{
		if (IsModify()) {
			std::wstringstream ss;
			wcsrefelementarray& childs = m_elements;
			for (size_t i = 0, j = childs.size(); i < j; ++i)
			{
				std::wstring val;
				wcselement2value(*childs[i], val);
				ss << val;
			}
			Set(ss.str());
		} 
	}

	/*bool Get(const wcsref& name, wcsref& val)
	{
		if (Find(name)) {
			if (m_pcurchild->childs.empty()) {
				val = m_pcurchild->data;
			}
		}
		return false;
	}

	bool Get(const std::wstring& name, std::wstring& val)
	{
		return Base::Get(name, val);
	}

	bool Get(const wcsref& name, const wcsref& attr, wcsref& val)
	{
		return Base::Get(name, attr, val);
	}

	bool Get(const std::wstring& name, const std::wstring attr, std::wstring& val)
	{
		return Base::Get(name, attr, val);
	}*/

	bool Set(const wcsref& name, const wcsref& val)
	{
		Modify(true);
		FindNew(name);
		m_pcurchild->childs.clear();
		m_pcurchild->data = val;
		return true;
	}

	bool Set(const std::wstring& name, const std::wstring& val)
	{//这里注意name必须为空，暂时不支持name
		std::wstring fullname(name);
		if (m_pcurelement) {
			wcsrefelement* pelem = m_pcurelement;
			while(pelem) 
			{
				if (fullname.length() > 0) {
					fullname.insert(0, L"\\");
				}
				fullname.insert(0, pelem->name.first, pelem->name.second);
				pelem = pelem->parent;
			}
		}
		
		//生成节点唯一名字，防止相同名字的节点冲突
		std::wstring fullnamex = fullname + towcs(m_pcurelement);
		wcsname2value::iterator it = m_name2data.find(fullnamex);
		assert(it != m_name2data.end());
		const std::wstring& wcsname = it->first;
		std::wstring& wcsval = it->second;
		return Set(
			wcsref((wchar_t*)wcsname.c_str() + wcsname.length() - ((fullnamex.length()-fullname.length())+name.length()), name.length()),
			wcsref((wchar_t*)wcsval.c_str(), wcsval.length()));
	}

	bool Set(const wcsref& name, const wcsref& attr, const wcsref& val)
	{
		Modify(true);
		FindNew(name);
		wcsrefpair attrvalue(attr,val);
		wcsrefpairarray& attrs = m_pcurchild->attrs;
		size_t i = 0, j = attrs.size();
		for (; i < j; ++i)
		{
			wcsrefpair & keyvalue = attrs[i];
			if (keyvalue.first.second == attr.second) {
				if (wcsnicmp(keyvalue.first.first, attr.first, attr.second) == 0) {
					break;
				}
			}
		}
		if (i < j) {
			attrs[i] = attrvalue;
		} else {
			attrs.push_back(attrvalue);
		}
		return true;
	}

	bool Set(const std::wstring& name, const std::wstring attr, const std::wstring& val)
	{//这里注意name必须为空，暂时不支持name
		std::wstring fullname(name);
		if (m_pcurelement) {
			wcsrefelement* pelem = m_pcurelement;
			while(pelem) 
			{
				if (fullname.length() > 0) {
					fullname.insert(0, L"\\");
				}
				fullname.insert(0, pelem->name.first, pelem->name.second);
				pelem = pelem->parent;
			}
		}

		//生成节点唯一名字，防止相同名字的节点冲突
		std::wstring fullnamex = fullname + towcs(m_pcurelement);
		wcsname2attr2value::iterator iter_nav = m_name2attr2value.find(fullnamex);
		assert (iter_nav != m_name2attr2value.end());
		wcsattr2value& attr2value = iter_nav->second;
		attr2value[attr] = val;
		wcsattr2value::iterator iter_av = attr2value.find(attr);
		const std::wstring& wcsname = iter_nav->first;
		const std::wstring& wcsattr = iter_av->first;
		const std::wstring& wcsval = iter_av->second;
		return Set(wcsref((wchar_t*)wcsname.c_str() + wcsname.length() - ((fullnamex.length()-fullname.length())+name.length()), name.length()),
			wcsref((wchar_t*)wcsattr.c_str(), wcsattr.length()), wcsref((wchar_t*)wcsval.c_str(), wcsval.length()));
	}

	bool New(const std::wstring& name)
	{
		Modify(true);
		wcsrefelement* elem = new wcsrefelement(m_pcurelement);
		elem->type = node_element;
		//elem->name = name;
		if (!m_pcurelement) {
			m_elements.push_back(elem);
			m_pcurchild = m_elements.back();
		} else {
			m_pcurelement->childs.push_back(elem);
			m_pcurchild = m_pcurelement->childs.back();
		}

		std::wstring fullname(name);
		if (m_pcurelement) {
			wcsrefelement* pelem = m_pcurelement;
			while(pelem) 
			{
				if (fullname.length() > 0) {
					fullname.insert(0, L"\\");
				}
				fullname.insert(0, pelem->name.first, pelem->name.second);
				pelem = pelem->parent;
			}
		}

		//生成节点唯一名字，防止相同名字的节点冲突
		std::wstring fullnamex;
#if 1
		fullnamex = fullname + towcs(m_pcurchild);
		assert(m_name2data.find(fullnamex)==m_name2data.end());
		assert(m_name2attr2value.find(fullnamex)==m_name2attr2value.end());
		wcsname2attr2value::iterator it = m_name2attr2value.end();
#else
		wcsname2attr2value::iterator it = m_name2attr2value.end();
		do 
		{
			fullnamex = fullname + towcs(m_pcurchild);
			it = m_name2attr2value.find(fullnamex);
		} while (it != m_name2attr2value.end());
#endif//
		m_name2data[fullnamex];
		m_name2attr2value[fullnamex];
		it = m_name2attr2value.find(fullnamex);

		elem->name = wcsref(it->first.c_str() + it->first.length() - ((fullnamex.length()-fullname.length())+name.length()), name.length());
		return true;
		//return New(wcsref(it->first.c_str() + it->first.length() - ((fullnamex.length()-fullname.length())+name.length()), name.length()));
	}

	bool Delete()
	{
		Modify(true);
		if (m_pcurchild) {
			wcsrefelement* parent = m_pcurchild->parent;
			wcsrefelementarray& childs = parent ? parent->childs : m_elements;
			for (size_t i = 0, j = childs.size(); i < j; ++i)
			{
				if (childs[i] == m_pcurchild) {
					childs.erase(childs.begin() + i);
					delete m_pcurchild;
					m_pcurchild = 0;
					break;
				}
			}
		}
	}

	bool SetAttributeString(std::wstring& name, std::wstring& attr, std::wstring& val)
	{
		return Set(name, attr, val);
	}

	bool SetAttributeString(wchar_t* name, wchar_t* attr, wchar_t* val)
	{
		return Set(std::wstring(name), std::wstring(attr), std::wstring(val));
	}

	template<class _Ty>
	_Ty SetAttributeT(std::wstring& name, std::wstring& attr, _Ty val)
	{
		return Set(name, attr, towcs(val));
	}
	template<class _Ty>
	_Ty SetAttributeT(wchar_t* name, wchar_t* attr, _Ty val)
	{
		return Set(std::wstring(name), std::wstring(attr), towcs(val));
	}

protected:
	//find or new all element form parent
	wcsrefelement* FindNew(const wcsref& name)
	{
		wcsrefelement* ptempelement = m_pcurelement;
		if (!name.first || !name.second) {
			//return m_pcurelement;
		} else {
			wchar_t* str = wcsnbrk(name.first, name.second, L"\\/");
			if (!str) { //if no sec break find direct.
				ptempelement = FindChildNew(name, ptempelement);
			} else { //else find child
				wcsref sec(name.first, str - name.first);
				wcsref next(str + 1, name.second - sec.second - 1);
				do {
					assert(sec.first && sec.second);
					ptempelement = FindChildNew(sec, ptempelement);
					if (!ptempelement) {
						break;
					}
					if (!next.first || !next.second) {
						break;
					}
					sec.first = next.first;
					sec.second = next.second;
					str = wcsnbrk(next.first, next.second, L"\\/");
					if (str) {
						sec.second = str - sec.first;
						next.first = str + 1;
						next.second = next.second - sec.second - 1;
					} else {
						next.first = 0;
						next.second = 0;
					}
				} while (true);
			}
		}
		m_pcurchild = ptempelement;
		return m_pcurchild;
	}

	//just find or new child
	wcsrefelement* FindChildNew(const wcsref& name, wcsrefelement* parent)
	{
		//first find cur child
		if (m_pcurchild && m_pcurchild->parent == parent) {
			if (m_pcurchild->name.second == name.second && 
				!wcsnicmp(m_pcurchild->name.first, name.first, name.second)) {
					return m_pcurchild;
			}
		}
		//second find all child
		wcsrefelementarray& childs = parent ? parent->childs : m_elements;
		size_t i = 0, j = childs.size();
		for (; i < j; ++i)
		{
			wcsrefelement & child = *childs[i];
			if (child.type != node_element) {
				continue;
			}
			if (!child.name.second) {
				wcselement(child);
			}
			if (child.name.second == name.second && 
				!wcsnicmp(child.name.first, name.first, name.second)) {
					return childs[i];
			}
		}
		//new
		wcsrefelement* elem = new wcsrefelement(parent);
		elem->type = node_element;
		elem->name = name;
		childs.push_back(elem);
		return childs.back();
	}
};

#ifdef UNICODE
#define MarkupWriter MarkupWriterA
#else
#define MarkupWriter MarkupWriterW
#endif//

#endif//_H_XCONFIG_H_