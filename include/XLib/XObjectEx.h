#ifndef _H_OBJECTEX_H_
#define _H_OBJECTEX_H_

#include "XLib.h"
#include "XObject.h"

#define DECLARE_DYNCCREATE_FACTORY_EX(object,base) \
	class object; \
	DECLARE_DYNCCREATE_FACTORY(object) \
	class object : public base\
	{ \
	public: \
		template<class TObject> \
		static HWND Create##object(TObject* & pObject, LPCTSTR lpszName, HWND hWndParent, LPCTSTR lpszXml, UINT XmlFlag) \
		{ \
			object* objptr = DYNCREATE_OBJECT(object,lpszName); \
			if(objptr) { \
				pObject = dynamic_cast<TObject*>(objptr); \
				if (pObject) { \
					return pObject->Create(hWndParent, lpszXml, XmlFlag); \
				} else { \
					DYNDELETE_OBJECT(objptr); \
				} \
			} \
			return NULL; \
		} \
		template<class TObject> \
		static void Destroy##object(TObject* & pObject) \
		{ \
			if (pObject) { \
				pObject->Destroy(); \
				DYNDELETE_OBJECT(pObject); \
				pObject = NULL; \
			} \
		} \
	public: \
		object():base(){} \
		object(HWND hWnd):base(hWnd){} \
		virtual LPCTSTR GetThisClassName() { return NULL; } \
		\
		virtual HWND Create(HWND hWndParent, LPCTSTR lpszXml = NULL, UINT XmlFlag = XML_FLAG_FILE) { return NULL; } \
		virtual void Destroy() { }; \
	};

#define DECLARE_DYNCCREATE_FACTORY_EX_EXPORT(ex,object,base) \
	class object; \
	DECLARE_DYNCCREATE_FACTORY_EXPORT(ex,object) \
	class ex object : public base\
	{ \
	public: \
		template<class TObject> \
		static HWND Create##object(TObject* & pObject, LPCTSTR lpszName, HWND hWndParent, LPCTSTR lpszXml, UINT XmlFlag) \
		{ \
			object* objptr = DYNCREATE_OBJECT(object,lpszName); \
			if(objptr) { \
				pObject = dynamic_cast<TObject*>(objptr); \
				if (pObject) { \
					return pObject->Create(hWndParent, lpszXml, XmlFlag); \
				} else { \
					DYNDELETE_OBJECT(objptr); \
				} \
			} \
			return NULL; \
		} \
		template<class TObject> \
		static void Destroy##object(TObject* & pObject) \
		{ \
			if (pObject) { \
				pObject->Destroy(); \
				DYNDELETE_OBJECT(pObject); \
				pObject = NULL; \
			} \
		} \
	public: \
		object():base(){} \
		object(HWND hWnd):base(hWnd){} \
		virtual LPCTSTR GetThisClassName() { return NULL; } \
		\
		virtual HWND Create(HWND hWndParent, LPCTSTR lpszXml = NULL, UINT XmlFlag = XML_FLAG_FILE) { return NULL; } \
		virtual void Destroy() { } \
	};

#define BEGIN_DYNCREATE_OBJECT_MAP(theClass) \
	HWND Create(HWND hWndParent, LPCTSTR lpszXml = NULL, UINT XmlFlag = XML_FLAG_FILE) \
	{ \
		HWND Res = NULL;

#define END_DYNCREATE_OBJECT_MAP() \
		return Res; \
	}

#define DECLARE_OBJECTER(theclass) \
	public: \
	virtual LPCTSTR GetThisClassName() { return theclass::GetClassName(); } 

#define DECLARE_DYNCREATE_OBJECTER(theclass,classname,object) \
	DECLARE_DYNCCREATE_CLASS(classname) \
	DECLARE_DYNCREATE_OBJECT(theclass,object) \
	DECLARE_OBJECTER(theclass)

#define DECLARE_DYNCREATE_OBJECTER_EX(theclass,classname,object1,object2) \
	DECLARE_DYNCCREATE_CLASS(classname) \
	DECLARE_DYNCREATE_OBJECT(theclass,object1) \
	DECLARE_DYNCREATE_OBJECT(theclass,object2) \
	DECLARE_OBJECTER(theclass)

#define IMPLEMENT_DYNCREATE_OBJECTER(theclass,object) \
	IMPLEMENT_DYNCREATE_OBJECT(theclass,object) \
	IMPLEMENT_DYNCREATE_REGISTER(theclass,object)

#define IMPLEMENT_DYNCREATE_OBJECTER_EX(theclass,object1,object2) \
	IMPLEMENT_DYNCREATE_OBJECT(theclass,object1) \
	IMPLEMENT_DYNCREATE_REGISTER(theclass,object1) \
	IMPLEMENT_DYNCREATE_OBJECT(theclass,object2) \
	IMPLEMENT_DYNCREATE_REGISTER(theclass,object2)


#define DECLARE_DYNCCREATE_WND_CLASS(theclass) \
	DECLARE_DYNCCREATE_CLASS(theclass::GetWndClassName())

#define DECLARE_WND_OBJECTER(theclass) \
	public: \
	virtual HWND GetThisHwnd() { return m_hWnd; } 

#define DECLARE_DYNCREATE_WND_OBJECTER(theclass,object) \
	DECLARE_DYNCREATE_OBJECTER(theclass,theclass::GetWndClassName(),object) \
	DECLARE_WND_OBJECTER(theclass)

#define DECLARE_DYNCREATE_WND_OBJECTER_EX(theclass,object1,object2) \
	DECLARE_DYNCREATE_OBJECTER_EX(theclass,theclass::GetWndClassName(),object1,object2) \
	DECLARE_WND_OBJECTER(theclass)

#define IMPLEMENT_DYNCREATE_WND_OBJECTER(theclass,object) \
	IMPLEMENT_DYNCREATE_OBJECTER(theclass,object) 

#define IMPLEMENT_DYNCREATE_WND_OBJECTER_EX(theclass,object1,object2) \
	IMPLEMENT_DYNCREATE_OBJECTER_EX(theclass,object1,object2) 

#endif//_H_OBJECTEX_H_