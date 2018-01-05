
#pragma once 

#ifndef _H_UIXML_H_
#define _H_UIXML_H_

#include "XConfig.h"
#include "XUser.h"

enum 
{ 
		XML_FLAG_NONE	= 0, 
		XML_FLAG_FILE	= 0x01,//XML FILE
		XML_FLAG_STREAM	= 0x02,//XML STREAM
		XML_FLAG_MARKUP	= 0X04,//XML Object
};

//////////////////////////////////////////////////////////////////////////
///如果传入XML指针，XML要已经找到并进入name节点，其他情况这里会Find and IntoElem
#define BEGIN_XML_MAP(name) \
	{ \
		if (lpszXml && XmlFlag) { \
			\
			BOOL bXmlOk = TRUE; \
			Markup* xml; \
			if (XmlFlag & XML_FLAG_FILE) { \
				xml = new Markup(); \
				bXmlOk = xml->Open(lpszXml); \
			} else if (XmlFlag & XML_FLAG_STREAM) { \
				xml = new Markup(); \
				xml->Set(std::tstring(lpszXml)); \
			} else if (XmlFlag & XML_FLAG_MARKUP) { \
				xml = (Markup*)lpszXml; \
			} \
			if (!(XmlFlag & XML_FLAG_MARKUP) && xml) { \
				if(!name) { \
					xml->First(); \
					xml->IntoElem(); \
				} else if (xml->Find(name)) { \
					xml->IntoElem(); \
				} else { \
					bXmlOk = FALSE; \
				} \
			} \
			if (bXmlOk) { 

#define BEGIN_XML_WRITE_MAP(name) \
	{ \
		if (lpszXml && XmlFlag) { \
			\
			BOOL bXmlOk = TRUE; \
			Markup* xml; \
			if (XmlFlag & XML_FLAG_FILE) { \
				xml = new Markup(); \
				bXmlOk = xml->Open(lpszXml,TRUE); \
			} else if (XmlFlag & XML_FLAG_STREAM) { \
				xml = new Markup(); \
				xml->Set(std::tstring(lpszXml)); \
			} else if (XmlFlag & XML_FLAG_MARKUP) { \
				xml = (Markup*)lpszXml; \
			} \
			if ((XmlFlag & XML_FLAG_FILE) && xml) { \
				if (xml->New(std::tstring(name))) { \
					xml->IntoElem(); \
				} else { \
					bXmlOk = FALSE; \
				} \
			} else if ((XmlFlag & XML_FLAG_STREAM) && xml) { \
				if (xml->Find(name)) { \
					xml->IntoElem(); \
				} else { \
					bXmlOk = FALSE; \
				} \
			} \
			if (bXmlOk) { 

#define XML_HANDLER(func) \
				func(xml);
			
#define BEGIN_XML_ATTRIBUTE_LOOP() \
				std::tstring strattr; \
				std::tstring strval; \
				for (long attrcookie = xml->FirstAttributeString(strattr, strval); attrcookie; attrcookie = xml->NextAttributeString(strattr, strval, attrcookie)) \
				{ \
					LPCTSTR lpszAttr = strattr.c_str(); \
					LPCTSTR lpszValue = strval.c_str(); \
					BOOL bOk = FALSE; 

#define BEGIN_XML_ATTRIBUTE_MAP() \
					BOOL bOk = FALSE; 
				
#define XML_ATTRIBUTE_HANDLER(name, func) \
					if (_tcsicmp(name, lpszAttr)==0 && (bOk = func(lpszAttr, lpszValue))) { \
						; \
					} else 

#define CHAIN_XML_ATTRIBUTE_HANDLER(func) \
					if ((bOk = func(lpszAttr, lpszValue))) { \
						; \
					} else

#define CHAIN_XML_ATTRIBUTE_DEFAULT()	CHAIN_XML_ATTRIBUTE_HANDLER(SetAttributeString)

#define END_XML_ATTRIBUTE_MAP() \
					{ \
					} 

#define END_XML_ATTRIBUTE_LOOP() \
					{ \
					} \
				} 

#define BEGIN_XML_ELEMENT_LOOP() \
				for(long cookie = xml->First(); cookie; cookie = xml->Next(cookie)) \
				{ \
					if(xml->GetType(true)!=node_element) { continue; } \
					xml->IntoElem(); \
					std::tstring strName; xml->GetName(strName); \
					LPCTSTR lpszElement = strName.c_str(); \
					BOOL bOk = FALSE; 
				
#define BEGIN_XML_ELEMENT_MAP() \
					BOOL bOk = FALSE; 

//#define XML_ELEMENT_HANDLER(name, func) \
//					if (_tcsicmp(name, lpszElement)==0 && (bOk = func(lpszElement, xml))) { \
//						; \
//					} else 

#define XML_ELEMENT_HANDLER(name, func) \
					if (_tcsicmp(name, lpszElement)==0 && (bOk = func((LPCTSTR)xml, XML_FLAG_MARKUP))) { \
						; \
					} else 

#define CHAIN_XML_ELEMENT_HANDLER(func) \
					if ((bOk = func(lpszElement, (LPCTSTR)xml, XML_FLAG_MARKUP))) { \
						; \
					} else

#define CHAIN_XML_HANDLER(func) \
					if ((bOk = func((LPCTSTR)xml, XML_FLAG_MARKUP))) { \
						; \
					} else

#define END_XML_ELEMENT_MAP() \
					{ \
					} 

#define END_XML_ELEMENT_LOOP() \
					{ \
					} \
					xml->OutofElem(); \
				} 

#define END_XML_MAP() \
			} \
			if (!(XmlFlag & XML_FLAG_MARKUP) && xml) { \
				if (bXmlOk) { \
					xml->OutofElem(); \
				} \
				delete xml; \
				xml = NULL; \
			} \
		} \
	}

#define END_XML_WRITE_MAP() \
	END_XML_MAP()

//////////////////////////////////////////////////////////////////////////

#define BEGIN_ATTRIBUTE_MAP(theClass) \
    LRESULT SetAttributeString(LPCTSTR lpszAttr, LPCTSTR lpszValue, UINT uFlags = 0) \
    { \
        LRESULT bOk = FALSE; 

#define CHAIN_ATTRIBUTE_OBJECT(obj) \
		if ((bOk = obj.SetAttributeString(lpszAttr, lpszValue, uFlags))) { \
            return bOk; \
        } else 

#define CHAIN_ATTRIBUTE_OBJECTPTR(objptr) \
		if ((bOk = objptr->SetAttributeString(lpszAttr, lpszValue, uFlags))) { \
			return bOk; \
		} else 

#define CHAIN_ATTRIBUTE_HANDLER(func) \
		if ((bOk = func(lpszAttr, lpszValue, uFlags))) { \
			return bOk; \
        } else 

#define CHAIN_ATTRIBUTE_MAP(classname) \
		if ((bOk = classname::SetAttributeString(lpszAttr, lpszValue, uFlags))) { \
			return bOk; \
        } else

// LRESULT OnAttribute(LPCTSTR lpszAttr, LPCTSTR lpszValue, UINT uFlags = 0)
#define ATTRIBUTE_HANDLER(name, func) \
        if (_tcsicmp(name, lpszAttr)==0) { \
            func(lpszAttr, lpszValue, uFlags); \
			bOk = TRUE; \
        } else 

// str = str
#define ATTRIBUTE_STRING(name, str) \
		if (_tcsicmp(name, lpszAttr)==0) { \
			str = lpszValue; \
            bOk = TRUE; \
        } else 

#define ATTRIBUTE_STRING_HANDLER(name, func) \
		if (_tcsicmp(name, lpszAttr)==0) { \
			func(lpszValue); \
			bOk = TRUE; \
		} else 

#define ATTRIBUTE_STRING_HANDLER_EX(name, func) \
		if (_tcsicmp(name, lpszAttr)==0) { \
			func(lpszValue, uFlags); \
			bOk = TRUE; \
		} else 

#define ATTRIBUTE_STRING_HANDLER2(name, func, func2) \
		if (_tcsicmp(name, lpszAttr)==0) { \
			func(func2(lpszValue)); \
			bOk = TRUE; \
		} else 

#define ATTRIBUTE_STRING_HANDLER2_EX(name, func, func2) \
		if (_tcsicmp(name, lpszAttr)==0) { \
			func(func2(lpszValue), uFlags); \
			bOk = TRUE; \
		} else 

// val = val
#define ATTRIBUTE_VALUE(name, type, val) \
		if (_tcsicmp(name, lpszAttr)==0) { \
			val = tcsto<type>(std::tstring(lpszValue)); \
			bOk = TRUE; \
        } else 

#define ATTRIBUTE_VALUE_HANDLER(name, type, func) \
		if (_tcsicmp(name, lpszAttr)==0) { \
			func(tcsto<type>(std::tstring(lpszValue))); \
			bOk = TRUE; \
        } else 

#define ATTRIBUTE_VALUE_POINTER(name, type, ptr) \
		if (_tcsicmp(name, lpszAttr)==0) { \
			ptr = (type*)tcsto<long>(std::tstring(lpszValue)); \
			bOk = TRUE; \
        } else 

// COLORREF = "r,g,b"
#define ATTRIBUTE_COLORREF(name, val) \
        if (_tcsicmp(name, lpszAttr)==0) { \
            val = _RGB(lpszValue); \
            bOk = TRUE; \
        } else 

// RECT = "l,t,r,b"
#define ATTRIBUTE_RECT(name, val) \
		if (_tcsicmp(name, lpszAttr)==0) { \
			val = _RECT(lpszValue); \
			bOk = TRUE; \
        } else 

#define END_ATTRIBUTE_MAP() \
		{ \
		} \
		return bOk; \
    } 

//////////////////////////////////////////////////////////////////////////

#endif//_H_UIXML_H_