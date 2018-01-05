#ifndef _H_OBJECT_H_
#define _H_OBJECT_H_

#include "XLib.h"
#include "XStr.h"
#include <map>

typedef LPVOID (*pfnCreateObject)();
typedef BOOL (*pfnEnumClass)(LPCTSTR, LPVOID);

#define DECLARE_DYNCCREATE_FACTORY(object) \
	class X##object##Factory \
	{ \
	public: \
		static void Register(LPCTSTR, pfnCreateObject); \
		static void UnRegister(LPCTSTR); \
		static pfnCreateObject EnumClass(pfnEnumClass, LPVOID); \
		static object* CreateObject(LPCTSTR); \
	};

#define DECLARE_DYNCCREATE_FACTORY_EXPORT(ex,object) \
	class ex X##object##Factory \
	{ \
	public: \
		static void Register(LPCTSTR, pfnCreateObject); \
		static void UnRegister(LPCTSTR); \
		static pfnCreateObject EnumClass(pfnEnumClass, LPVOID); \
		static object* CreateObject(LPCTSTR); \
	};

#define IMPLEMENT_DYNCCREATE_FACTORY(object) \
	typedef std::map<std::tstring,pfnCreateObject,std::tstringiless> X##object2pfnCreate; \
	X##object2pfnCreate _##object##2pfnCreate; \
	void X##object##Factory::Register(LPCTSTR name, pfnCreateObject fn) \
	{ \
		if(!name) { return; } \
		if (fn) { \
			_##object##2pfnCreate[name] = fn; \
		} else { \
			_##object##2pfnCreate.erase(name); \
		} \
	} \
	void X##object##Factory::UnRegister(LPCTSTR name) \
	{ \
		Register(name, 0); \
	} \
	pfnCreateObject X##object##Factory::EnumClass(pfnEnumClass fn, LPVOID param) \
	{ \
		X##object2pfnCreate::iterator it = _##object##2pfnCreate.begin(); \
		for (; it != _##object##2pfnCreate.end(); ++it) \
		{ \
			if(fn(it->first.c_str(), param)) { \
				break; \
			} \
		} \
		if(it != _##object##2pfnCreate.end()) { \
			return it->second; \
		} \
		return 0; \
	} \
	object* X##object##Factory::CreateObject(LPCTSTR name) \
	{ \
		X##object2pfnCreate::iterator it = _##object##2pfnCreate.find(name); \
		if (it != _##object##2pfnCreate.end()) { \
			return (object*)it->second(); \
		} \
		return 0; \
	}

#define DECLARE_DYNCCREATE_CLASS(classname) \
	public:  \
	static LPCTSTR GetClassName() { return classname; }

#define DECLARE_DYNCREATE_OBJECT(theclass,object) \
	public: \
	static BOOL Enum##object##Func(LPCTSTR, LPVOID); \
	static object* Create##object();

#define IMPLEMENT_DYNCREATE_OBJECT(theclass,object) \
	BOOL theclass::Enum##object##Func(LPCTSTR name, LPVOID param) \
	{ \
		if(_tcsicmp(name,theclass::GetClassName()) == 0) { \
			return TRUE; \
		} \
		return FALSE; \
	} \
	object* theclass::Create##object() \
	{ \
		return dynamic_cast<object*>(new theclass()); \
	}

#define IMPLEMENT_DYNCREATE_REGISTER(theclass,object) \
	class X##theclass##object \
	{ \
	public: \
		X##theclass##object() { X##object##Factory::Register(theclass::GetClassName(),(pfnCreateObject)theclass::Create##object); } \
		~X##theclass##object() { X##object##Factory::UnRegister(theclass::GetClassName()); } \
	}; \
	X##theclass##object _##theclass##object; 

#define DYNCREATE_REGISTER(theclass,classname,object) \
	X##object##Factory::Register(classname,(pfnCreateObject)theclass::Create##object)

#define DYNCREATE_UNREGISTER(theclass,classname,object) \
	X##object##Factory::UnRegister(classname)

#define DYNCREATE_ENUM(object,f,p) \
	X##object##Factory::EnumClass(f,p)

//#define DYNCREATE_FUNC(object,name) \
//	X##object##Factory::EnumClass(Enum##object##Func,0)

#define DYNCREATE_OBJECT(object,name) \
	X##object##Factory::CreateObject(name)

#define DYNDELETE_OBJECT(object) \
	delete object

#endif//_H_OBJECT_H_