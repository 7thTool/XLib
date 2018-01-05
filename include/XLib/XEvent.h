#ifndef _H_EVENT_H_
#define _H_EVENT_H_

#include "XLib.h"
#include "XStr.h"
#include "XSmartPtr.h"
#include "XSmartPtrEx.h"
#include "XThread.h"
#include "XMap.h"

class EvtObjecter;
class EvtFilter;
class EvtIdler;
class EvtDispatcher;

#define EVT_NONE		0
#define EVT_NULL		0
#define EVT_QUIT		1
#define EVT_IDLE		2
#define EVT_TIMER		3
#define	EVT_MAX			1024

//////////////////////////////////////////////////////////////////////////
//局部对象，不可以new的，可以包装成objPtr，调用SendEvent
class LPARAMObj : public ObjectEx
{
	typedef ObjectEx Base;
public:
	LPARAMObj(const void* d):Base(-1),data(d){}
	LPARAMObj(const Object & o):Base(-1),obj(&o){}
	union
	{
		const void* data;
		const Object* obj;
	};
};
typedef Handle<LPARAMObj> LPARAMObjPtr;
typedef LPARAMObjPtr LPVOIDObjPtr;
#define LOCALPTR(p)	LPARAMObjPtr(&LPARAMObj(p))
#define LOCALOBJ(o)	LPARAMObjPtr(&LPARAMObj(o))
class OBJPARAMObj : public ObjectEx
{
	typedef ObjectEx Base;
public:
	OBJPARAMObj():Base(-1),objPtr(ObjectPtr()){}
	OBJPARAMObj(ObjectPtr & o):Base(-1),objPtr(o){}
	ObjectPtr & objPtr;
};
typedef Handle<OBJPARAMObj> OBJPARAMObjPtr;
#define OBJPTR0 OBJPARAMObjPtr(&OBJPARAMObj())
#define OBJPTR(o) OBJPARAMObjPtr(&OBJPARAMObj(o))
//////////////////////////////////////////////////////////////////////////
//Bundle对象
class Bundle : public ObjectEx
{
	typedef ObjectEx Base;
public:
	Bundle() {}

	void putString(const std::string& key, const std::string& value) {
		mapString[key] = value;
	}
	const std::string& getString(const std::string& key) {
		std::map<std::string,std::string>::iterator it = mapString.find(key);
		if (it!=mapString.end()) {
			return it->second;
		}
		return std::string();
	}

	void putPtr(const std::string& key, void* value) {
		mapData[key] = value;
	}
	void* getPtr(const std::string& key) {
		std::map<std::string,void*>::iterator it = mapData.find(key);
		if (it!=mapData.end()) {
			return it->second;
		}
		return 0;
	}

	template<typename _Type>
	void putNumber(const std::string& key, const _Type & value) {
		mapData[key] = (void*)value;
	}
	template<typename _Type>
	_Type getNumber(const std::string& key, _Type& value) {
		std::map<std::string,void*>::iterator it = mapData.find(key);
		if (it!=mapData.end()) {
			return (_Type)it->second;
		}
		return _Type();
	}

private:
	std::map<std::string,std::string> mapString;
	std::map<std::string,void*> mapData;
};
//////////////////////////////////////////////////////////////////////////

class XLIB_API Event
{
public:
	EvtObjecter* src;			//事件源
	EvtObjecter* dst;			//事件目标

	long evt;					//事件
	long value;					//事件参数
	ObjectPtr objPtr;			//事件对象参数

	unsigned long time;			//时间发生的时间

	bool handled;				//是否处理了

	Event();
	Event(EvtObjecter* src, EvtObjecter* dst, long evt, long value, ObjectPtr objPtr);
	~Event();

	Event(const Event& o);
	Event& operator=(const Event& o);
};

struct EventLess
{
	bool operator()(const Event& x, const Event& y) const
	{
		if (x.src<y.src) {
			return true;
		} else if (x.src>y.src) {
			return false;
		}
		if (x.dst<y.dst) {
			return true;
		} else if (x.dst>y.dst) {
			return false;
		}
		if ((Object*)x.objPtr<(Object*)y.objPtr) {
			return true;
		} else if ((Object*)x.objPtr>(Object*)y.objPtr) {
			return false;
		}
		return false;
	}
};
//struct XLIB_API EventLess
//{
//	bool operator()(const Event& x, const Event& y) const;
//};
//
//struct XLIB_API EventEqual
//{
//	bool operator()(const Event& x, const Event& y) const;
//};
//
//XLIB_API bool operator == (const Event & x, const Event & y);
//XLIB_API bool operator != (const Event & x, const Event & y);
//XLIB_API bool operator < (const Event & x, const Event & y);
//XLIB_API bool operator > (const Event & x, const Event & y);

//STR
/*enum
{
	OBJECTER_STR_FLAG_NONE			= 0,
	OBJECTER_STR_FLAG_NAME,
	OBJECTER_STR_FLAG_USER,
	OBJECTER_STR_FLAG_STATUS,
	OBJECTER_STR_FLAG_LASTERROR,
	OBJECTER_STR_FLAG_MAX,
};*/
#define STR_NONE		NULL
#define STR_NAMEA		"Name"
#define STR_NAMEW		L"Name"
#define STR_USERA		"User"
#define STR_USERW		L"User"
#define STR_STATUSA		"Status"
#define STR_STATUSW		L"Status"
#define STR_ERRORA		"Error"
#define STR_ERRORW		L"Error"
#ifndef UNICODE
#define STR_NAME		STR_NAMEA
#define STR_USER		STR_USERA
#define STR_STATUS		STR_STATUSA
#define STR_ERROR		STR_ERRORA
#else
#define STR_NAME		STR_NAMEW
#define STR_USER		STR_USERW
#define STR_STATUS		STR_STATUSW
#define STR_ERROR		STR_ERRORW
#endif//

#define STR_APPPATHA		"AppPath"
#define STR_APPPATHW		L"AppPath"
#define STR_APPDATAA		"AppData"
#define STR_APPDATAW		L"AppData"
#define STR_MODULEDIRA		"ModuleDir"
#define STR_MODULEDIRW		L"ModuleDir"
#define STR_DATADIRA		"DataDir"
#define STR_DATADIRW		L"DataDir"
#define STR_USERDIRA		"UserDir"
#define STR_USERDIRW		L"UserDir"
#ifndef UNICODE
#define STR_APPPATH			STR_APPPATHA
#define STR_APPDATA			STR_APPDATAA
#define STR_MODULEDIR		STR_MODULEDIRA
#define STR_DATADIR			STR_DATADIRA
#define STR_USERDIR			STR_USERDIRA
#else
#define STR_APPPATH			STR_APPPATHW
#define STR_APPDATA			STR_APPDATAW
#define STR_MODULEDIR		STR_MODULEDIRW
#define STR_DATADIR			STR_DATADIRW
#define STR_USERDIR			STR_USERDIRW
#endif//

class XLIB_API EvtObjecter
{
	friend class EvtDispatcher;
public:
	static long PostThreadEvent(unsigned long ThreadId, Event& evt);
public:
	EvtObjecter();
	virtual ~EvtObjecter();

	virtual unsigned long GetThreadId();

	virtual const char* c_strof(const char* name);
	virtual const wchar_t* c_strof(const wchar_t* name);

	//发送事件
	virtual long SendEvent(Event& evt);
	virtual long PostEvent(Event& evt);
	long SendEvent(EvtObjecter* dst, Event& evt);
	long PostEvent(EvtObjecter* dst, Event& evt);
	long SendEvent(EvtObjecter* dst, long evt, long value = 0, ObjectPtr objPtr = 0);
	long PostEvent(EvtObjecter* dst, long evt, long value = 0, ObjectPtr objPtr = 0);
	long SendEvent(long evt, long value = 0, ObjectPtr objPtr = 0);
	long PostEvent(long evt, long value = 0, ObjectPtr objPtr = 0);

protected:
	//外部直接调用事件过来
	virtual long Call(Event& evt);
	//long Call(EvtObjecter* src, long evt, long value = 0, ObjectPtr objPtr = 0);
protected:
	virtual long OnEvent(Event& evt);
};

class XLIB_API EvtFilter
{
public:
	virtual bool PreTranslateEvent(Event& evt) = 0;
};

class XLIB_API EvtIdler
{
public:
	virtual void OnIdle(unsigned int tick) = 0;
};

#define BEGIN_EVT_CALL(theClass) \
protected: \
	long Call(Event& evt) \
	{ \
		long ret = RLT_UNKNOWN; \

#define ON_CALL(myevt,myfunc) \
		if(evt.evt == myevt) { \
			evt.handled = true; \
			ret = myfunc(evt); \
			if(evt.handled) { \
				return ret; \
			} \
		}

#define END_EVT_CALL() \
		ret = OnEvent(evt); \
		if(!evt.handled) { \
			ret = RLT_UNKNOWN; \
		} \
		return ret; \
	}

#define BEGIN_EVT_MAP(theClass) \
protected: \
	long OnEvent(Event& evt) \
	{ \
		long ret = RLT_UNKNOWN; 

#define ON_EVT(myevt,myfunc) \
		if(evt.evt == myevt) { \
			evt.handled = true; \
			ret = myfunc(evt); \
			if(evt.handled) { \
				return ret; \
			} \
		}

#define CHAIN_EVT_MAP(theChainClass) \
		ret = theChainClass::OnEvent(evt); \
		if(evt.handled) { \
			return ret; \
		}

#define CHAIN_EVT_OBJ(theObject) \
		ret = theObject.OnEvent(evt); \
		if(evt.handled) { \
			return ret; \
		}

#define CHAIN_EVT_PTR(theObjPtr) \
		if(theObjPtr) { \
			ret = theObjPtr->OnEvent(evt); \
			if(evt.handled) { \
				return ret; \
			} \
		}

#define END_EVT_MAP() \
		return ret; \
	} 

//以下事件模型

enum
{
	PE_NOREMOVE = 0,
	PE_REMOVE,
};

//单线程DispatchEvent
class XLIB_API EvtDispatcher
{
private:
	static CriticalSection s_Section;
	static std::map<unsigned long,EvtDispatcher*> s_mapDispatcher;
public:
	static void SetEvtDispatcher(EvtDispatcher* pDispatcher, unsigned long ThreadId = Thread::GetCurrentThreadId());
	static EvtDispatcher* GetEvtDispatcher(unsigned long ThreadId = Thread::GetCurrentThreadId());
protected:
	//typedef set<EvtObjecter*> pEvtObjecters;
	//pEvtObjecters m_pEvtObjecters;

	typedef std::vector<EvtFilter*> EvtFilterArray;
	EvtFilterArray m_EvtFilterArray;
	typedef std::vector<EvtIdler*> EvtIdlerArray;
	EvtIdlerArray m_EvtIdlerArray;

	typedef std::map<long,Event> EvtTimerMap;
	EvtTimerMap m_EvtTimerMap;
	long m_nTimer;
	long m_nMaxTimer;

	typedef std::deque<Event> EventQueue;
	EventQueue m_EventQueue;
	CriticalSection m_SectionQueue;
	WaitEvent m_WaitQueue;

	Event m_evt;

public:
	EvtDispatcher();
	virtual ~EvtDispatcher();

	//virtual void AddEvtObjecter(EvtObjecter* pEvtObjecter);
	//virtual void RemoveEvtObjecter(EvtObjecter* pEvtObjecter);

	virtual void RegisterEvtFilter(EvtFilter* pEvtFilter);
	virtual void UnRegisterEvtFilter(EvtFilter* pEvtFilter);

	virtual void RegisterEvtIdler(EvtIdler* pEvtIdler);
	virtual void UnRegisterEvtIdler(EvtIdler* pEvtIdler);

	virtual long StartTimer(EvtObjecter* pEvtObjecter, unsigned long uElapse);
	virtual void StopTimer(long nIDTimer);

	//virtual long SendEvent(Event& evt);
	virtual long PostEvent(Event& evt);
	virtual long PostDelayEvent(Event& evt, unsigned long delay);
	virtual long PostQuitEvent();
	virtual long PostIdleEvent();
	virtual long GetEvent(EvtObjecter* dst, Event& evt, unsigned long timeout = 0);
	virtual long PeekEvent(EvtObjecter* dst, Event& evt, long flag = PE_NOREMOVE);
	virtual long DispatchEvent(Event& evt);
	virtual bool PreTranslateEvent(Event& evt);
	virtual bool OnTimer(long timer);
	virtual void OnIdle(unsigned int tick);
	virtual void OnQuit();

	virtual long RunOnce(unsigned long timeout = 0);
	virtual long Run(bool bJoin = true);
};

//long XLIB_API PostThreadEvent(unsigned long ThreadId, Event& evt);
//long XLIB_API PostEvent(Event& evt);
//long XLIB_API SendEvent(Event& evt);

//多线程管理器
class XLIB_API EvtMultiThreadManager
{
public:
	EvtMultiThreadManager();
};

//支持多线程注册监听，监听接收对象收到Event时，最好放到一个缓存队列里在另外线程处理，尽量不要占住EvtHandler发送的事件线程
//如果EvtHandler模块实现了Post机制，对于PostEvent就没关系了，因为不会占住EvtHandler发送的事件线程
//class XLIB_API EvtHandler : virtual public EvtObjecter
//{
//protected:
//	typedef std::set<EvtObjecter*> EvtHandlerSet;
//	static EvtHandlerSet _EvtHandlerSet;
//	static CriticalSection _Section;
//public:
//	static bool IsEvtHandler(EvtObjecter* pEvtHandler);
//
//protected:
//	typedef std::vector<EvtObjecter*> pEvtListeners;
//	pEvtListeners m_pListeners;
//	CriticalSection m_Section; //只处理m_pListeners锁
//public:
//	EvtHandler();
//	virtual ~EvtHandler();
//
//	//注册监听
//	virtual void RegisterListener(EvtObjecter* pListener);
//	//反注册监听
//	virtual void UnRegisterListener(EvtObjecter* pListener);
//	//是否注册监听
//	virtual bool IsRegisterListener(EvtObjecter* pListener);
//
//	virtual bool IsListenerEmpty();
//
//	virtual long SendToListener(Event& evt);
//	virtual long PostToListener(Event& evt);
//	long SendToListener(EvtObjecter* dst, long evt, long value, ObjectPtr objPtr);
//	long PostToListener(EvtObjecter* dst, long evt, long value, ObjectPtr objPtr);
//
//	virtual void BroadcastSendToListener(Event& evt);
//	virtual void BroadcastPostToListener(Event& evt);
//	void BroadcastSendToListener(long evt, long value, ObjectPtr objPtr);
//	void BroadcastPostToListener(long evt, long value, ObjectPtr objPtr);
//
//	//virtual long Call(Event& evt);
//	//long Call(EvtObjecter* src, long evt, long value = 0, ObjectPtr objPtr = 0);
//	
//	BEGIN_EVT_MAP(EvtHandler)
//	END_EVT_MAP()
//};

template<class TBase = EvtObjecter>
class EvtHandlerT : virtual public TBase
{
public:
	typedef EvtHandlerT<TBase> This;
	typedef TBase Base;
protected:
	typedef std::vector<EvtObjecter*> pEvtListeners;
	pEvtListeners m_pListeners;
	CriticalSection m_SectionListeners;
public:
	EvtHandlerT()
	{
		
	}

	~EvtHandlerT()
	{
		
	}

	virtual void RegisterListener(EvtObjecter* pListener)
	{
		SectionLocker Lock(&m_SectionListeners);
		if (pListener) {
			ASSERT(!IsRegisterListener(pListener));
			m_pListeners.push_back(pListener);
		}
	}

	virtual void UnRegisterListener(EvtObjecter* pListener)
	{
		SectionLocker Lock(&m_SectionListeners);
		if (pListener) {
			pEvtListeners::iterator it = std::find(m_pListeners.begin(),m_pListeners.end(),pListener);
			if (it!=m_pListeners.end()) {
				m_pListeners.erase(it);
			}
		}
	}

	virtual bool IsRegisterListener(EvtObjecter* pListener)
	{
		if (!pListener) {
			return false;
		}
		SectionLocker Lock(&m_SectionListeners);
		pEvtListeners::iterator it = std::find(m_pListeners.begin(),m_pListeners.end(),pListener);
		if (it != m_pListeners.end()) {
			return true;
		}
		return false;
	}

	virtual bool IsListenerEmpty()
	{
		//SectionLocker Lock(&m_SectionListeners);
		return m_pListeners.empty();
	}

	virtual long SendToListener(Event& evt)
	{
		SectionLocker Lock(&m_SectionListeners);
		if(IsRegisterListener(evt.dst)) {
			return SendEvent(evt);
		}
		return RLT_UNKNOWN;
	}

	virtual long PostToListener(Event& evt)
	{
		SectionLocker Lock(&m_SectionListeners);
		if(IsRegisterListener(evt.dst)) {
			return PostEvent(evt);
		}
		return RLT_UNKNOWN;
	}

	long SendToListener(EvtObjecter* dst, long evt, long value, ObjectPtr objPtr)
	{
		return SendToListener(Event(this, dst, evt, value, objPtr));
	}

	long PostToListener(EvtObjecter* dst, long evt, long value, ObjectPtr objPtr)
	{
		return PostToListener(Event(this, dst, evt, value, objPtr));
	}

	virtual void BroadcastSendToListener(Event& evt)
	{
		//EvtListener不要占住EvtHandler发送的事件线程
		SectionLocker Lock(&m_SectionListeners);
		for (pEvtListeners::iterator it = m_pListeners.begin(); it != m_pListeners.end(); ++it)
		{
			evt.dst = *it;
			SendEvent(evt);
		}
	}

	virtual void BroadcastPostToListener(Event& evt)
	{
		//EvtListener不要占住EvtHandler发送的事件线程
		SectionLocker Lock(&m_SectionListeners);
		for (pEvtListeners::iterator it = m_pListeners.begin(); it != m_pListeners.end(); ++it)
		{
			evt.dst = *it;
			PostEvent(evt);
		}
	}

	void BroadcastSendToListener(long evt, long value, ObjectPtr objPtr)
	{
		BroadcastSendToListener(Event(this, 0, evt, value, objPtr));
	}

	void BroadcastPostToListener(long evt, long value, ObjectPtr objPtr)
	{
		BroadcastPostToListener(Event(this, 0, evt, value, objPtr));
	}
};

typedef EvtHandlerT<EvtObjecter> EvtHandler;

//////////////////////////////////////////////////////////////////////////

//注意事项同EvtHandler
//class XLIB_API EvtListener : virtual public EvtObjecter
//{
//protected:
//	typedef std::set<EvtObjecter*> EvtListenerSet;
//	static EvtListenerSet _EvtListenerSet;
//	static CriticalSection _Section;
//public:
//	static bool IsEvtListener(EvtObjecter* pListener);
//protected:
//	//typedef std::vector<EvtHandler*> pEvtHandlers;
//	//pEvtHandlers m_pHandlers;
//public:
//	EvtListener();
//	virtual ~EvtListener();
//
//	/*virtual void AddHandler(EvtHandler* pHandler);
//	virtual void RemoveHandler(EvtHandler* pHandler);
//	virtual bool IsHandler(EvtHandler* pHandler);
//
//	virtual bool IsHandlerEmpty();
//
//	virtual long SendToHandler(Event& evt);
//	virtual long PostToHandler(Event& evt);
//	long SendToHandler(EvtHandler* dst, long evt, long value, ObjectPtr objPtr);
//	long PostToHandler(EvtHandler* dst, long evt, long value, ObjectPtr objPtr);
//
//	virtual void BroadcastSendToHandler(Event& evt);
//	virtual void BroadcastPostToHandler(Event& evt);
//	void BroadcastSendToHandler(long evt, long value, ObjectPtr objPtr);
//	void BroadcastPostToHandler(long evt, long value, ObjectPtr objPtr);*/
//
//	BEGIN_EVT_MAP(EvtListener)
//	END_EVT_MAP()
//};

template<class THandler = EvtObjecter, class TBase = EvtObjecter>
class EvtListenerT : virtual public TBase
{
	typedef EvtListenerT<THandler,TBase> This;
	typedef TBase Base;
	typedef THandler Handler;
protected:
	typedef std::vector<THandler*> pEvtHandlers;
	pEvtHandlers m_pHandlers;
	CriticalSection m_SectionHandlers;
public:

	virtual void AddHandler(THandler* pHandler)
	{
		if (pHandler) {
			ASSERT(!IsHandler(pHandler));
			pHandler->RegisterListener(this);

			SectionLocker Lock(&m_SectionHandlers);
			m_pHandlers.push_back(pHandler);
		}
	}

	virtual void RemoveHandler(THandler* pHandler)
	{
		if (pHandler) {
			pHandler->UnRegisterListener(this);

			SectionLocker Lock(&m_SectionHandlers);
			pEvtHandlers::iterator it = std::find(m_pHandlers.begin(),m_pHandlers.end(),pHandler);
			if (it!=m_pHandlers.end()) {
				m_pHandlers.erase(it);
			}
		}
	}

	virtual void RemoveAllHandler()
	{
		SectionLocker Lock(&m_SectionHandlers);
		for (pEvtHandlers::iterator it = m_pHandlers.begin(); it!=m_pHandlers.end(); ++it)
		{
			(*it)->UnRegisterListener(this);
		}
		m_pHandlers.clear();
	}

	virtual bool IsHandler(THandler* pHandler)
	{
		SectionLocker Lock(&m_SectionHandlers);
		pEvtHandlers::iterator it = std::find(m_pHandlers.begin(),m_pHandlers.end(),pHandler);
		if (it!=m_pHandlers.end()) {
			return true;
		}
		return false;
	}

	virtual bool IsHandlerEmpty()
	{
		//SectionLocker Lock(&m_SectionHandlers);
		return m_pHandlers.empty();
	}

	virtual long SendToHandler(Event& evt)
	{
		SectionLocker Lock(&m_SectionHandlers);
		if(IsHandler(dynamic_cast<THandler*>(evt.dst))) {
			return SendEvent(evt);
		}
		return RLT_UNKNOWN;
	}

	virtual long PostToHandler(Event& evt)
	{
		SectionLocker Lock(&m_SectionHandlers);
		if(IsHandler(dynamic_cast<THandler*>(evt.dst))) {
			return PostEvent(evt);
		} 
		return RLT_UNKNOWN;
	}

	long SendToHandler(THandler* dst, long evt, long value, ObjectPtr objPtr)
	{
		return SendToHandler(Event(this, dst, evt, value, objPtr));
	}

	long PostToHandler(THandler* dst, long evt, long value, ObjectPtr objPtr)
	{
		return PostToHandler(Event(this, dst, evt, value, objPtr));
	}

	virtual void BroadcastSendToHandler(Event& evt)
	{
		SectionLocker Lock(&m_SectionHandlers);
		for (pEvtHandlers::iterator it = m_pHandlers.begin(); it!=m_pHandlers.end(); ++it)
		{
			evt.dst = *it;
			SendEvent(evt);
		}
	}

	virtual void BroadcastPostToHandler(Event& evt)
	{
		SectionLocker Lock(&m_SectionHandlers);
		for (pEvtHandlers::iterator it = m_pHandlers.begin(); it!=m_pHandlers.end(); ++it)
		{
			evt.dst = *it;
			PostEvent(evt);
		}
	}

	void BroadcastSendToHandler(long evt, long value, ObjectPtr objPtr)
	{
		BroadcastSendToHandler(Event(this, 0, evt, value, objPtr));
	}

	void BroadcastPostToHandler(long evt, long value, ObjectPtr objPtr)
	{
		BroadcastPostToHandler(Event(this, 0, evt, value, objPtr));
	}
};

typedef EvtListenerT<EvtHandler,EvtObjecter> EvtListener;

//////////////////////////////////////////////////////////////////////////

template<class THandler = EvtObjecter, class TListener = EvtObjecter>
class EvtProcesserT : public TListener, public THandler
{
	typedef EvtProcesserT<TListener,THandler> This;
public:

	BEGIN_EVT_MAP(This)
		if (IsRegisterListener(evt.src)) {
			ret = THandler::OnEvent(evt);	//收到监听者调用
		} else {
			ret = TListener::OnEvent(evt);	//收到处理者回调
		}
		if(evt.handled) {
			return ret;
		}
	END_EVT_MAP()
};

//////////////////////////////////////////////////////////////////////////
template<class T, class _Ty, class _Pr=less<_Ty>>
class EvtRequestT
{
protected:
	typedef multi2rmap<Event,_Ty,EventLess,_Pr> MapRequests;
	MapRequests m_Requests;

public:
	EvtRequestT()
	{

	}
	~EvtRequestT()
	{
		ASSERT(m_Requests.left.empty()&&m_Requests.right.empty());
	}

	void Clear(bool bNotify = true)
	{
		T* pT = static_cast<T*>(this);
		if (bNotify) {
			while (m_Requests.right.size())
			{
				MapRequests::right_iterator it = m_Requests.right.begin();
				pT->UpdateRequest(it->first);
			}
		}
		m_Requests.clear();
	}

	//取消请求
	void OnRemoveRequest(const Event& Req,const _Ty& SubReq)
	{

	}
	void RemoveRequest(const Event& Req)
	{ 
		T* pT = static_cast<T*>(this);
		MapRequests::_left_Paircc pr = m_Requests.left.equal_range(Req);
		MapRequests::const_left_iterator it = pr.first;
		for (; it!=pr.second; ++it)
		{
			pT->OnRemoveRequest(Req,it->second);
		}
		m_Requests.erase(Req);
	}

	//增加请求
	void OnAddRequest(const Event& Req, const _Ty& SubReq)
	{

	}
	void AddRequest(const Event& Req, const _Ty& SubReq)
	{
		T* pT = static_cast<T*>(this);
		MapRequests::left_iterator it = m_Requests.insert(MapRequests::value_left_type(Req,SubReq));
		if (it!=m_Requests.left.end()) {
			pT->OnAddRequest(Req,SubReq);
		}
	}

	//更新请求
	void OnUpdateRequest(const Event& Req, const _Ty& SubReq, long SubReqCount)
	{

	}
	bool UpdateRequest(const _Ty& SubReq, Event& Req)
	{
		T* pT = static_cast<T*>(this);
		long size = 0;
		MapRequests::right_iterator it = m_Requests.right.find(SubReq);
		if (it!=m_Requests.right.end()) {
			Req = it->second;
			size = m_Requests.left.count(Req);
			m_Requests.rerase(it);
			size -= 1;
			pT->OnUpdateRequest(Req,SubReq,size);
			return true;
		}
		return false;
	}
	bool UpdateRequest(const _Ty& SubReq)
	{
		T* pT = static_cast<T*>(this);
		Event Req;
		return pT->UpdateRequest(SubReq,Req);
	}

	//查找请求
	bool FindRequest(const _Ty& SubReq, Event& Req)
	{
		//T* pT = static_cast<T*>(this);
		long size = 0;
		MapRequests::right_iterator it = m_Requests.right.find(SubReq);
		if (it!=m_Requests.right.end()) {
			Req = it->second;
			return true;
		}
		return false;
	}
};

//////////////////////////////////////////////////////////////////////////

//多通道(多线程)支持,一般用于事件频繁，一个线程处理不过来，或者提高事件分发效率的情况
template<class T>
class EvtChannelT
{
protected:
	//后台通知事件
	typedef std::deque<Event> EventQueue;
	EventQueue m_EventQueue;
	CriticalSection m_SectionQueue;
public:
	EvtChannelT(int nEvtChannelCount = 1)
	{
		int i,j;
		//m_EventQueue.reserve(nEvtChannelCount * 100);
		m_ChannelThreads.resize(nEvtChannelCount);
		for(i = 0, j = m_ChannelThreads.size(); i < j; i++)
		{
			m_ChannelThreads[i] = RegisterThreadPool(ChannelThread, (void*)this);
		}
	}
	~EvtChannelT()
	{
		int i,j;
		for(i = 0, j = m_ChannelThreads.size(); i < j; i++)
		{
			UnRegisterThreadPool(m_ChannelThreads[i]);
			m_ChannelThreads[i] = 0;
		}
		m_ChannelThreads.clear();

		SectionLocker Lock(&m_SectionQueue);
		m_EventQueue.clear();
	}

	long AddToChannel(Event& evt)
	{
		SectionLocker Lock(&m_SectionQueue);
		m_EventQueue.push_back(evt);
		return RLT_OK;
	}

	enum 
	{
		REMOVE_EVT_FLAG_NONE	= 0,
		REMOVE_EVT_FLAG_SRC		= 0X01,
		REMOVE_EVT_FLAG_DST		= 0X02,
		REMOVE_EVT_FLAG_EVT		= 0X04,
		REMOVE_EVT_FLAG_VALUE	= 0X08,
		REMOVE_EVT_FLAG_OBJPTR	= 0X10,
	};

	void RemoveFromChannel(Event& evt, long flags)
	{
		SectionLocker Lock(&m_SectionQueue);
		for(EventQueue::iterator it = m_EventQueue.begin(); it != m_EventQueue.end(); )
		{
			bool bRemove = true;
			if (bRemove && ((flags & REMOVE_EVT_FLAG_SRC) && (it->src != evt.src))) {
				bRemove = false;
			}
			if (bRemove && ((flags & REMOVE_EVT_FLAG_DST) && (it->dst != evt.dst))) {
				bRemove = false;
			}
			if (bRemove && ((flags & REMOVE_EVT_FLAG_EVT) && (it->evt != evt.evt))) {
				bRemove = false;
			}
			if (bRemove && ((flags & REMOVE_EVT_FLAG_VALUE) && (it->value != evt.value))) {
				bRemove = false;
			}
			if (bRemove && ((flags & REMOVE_EVT_FLAG_OBJPTR) && (it->objPtr != evt.objPtr))) {
				bRemove = false;
			}
			if(bRemove) {
				it = m_EventQueue.erase(it);
			} else {
				++it;
			}
		}
	}

	void RemoveAllFromChannel()
	{
		SectionLocker Lock(&m_SectionQueue);
		m_EventQueue.clear();
	}

protected:
	void OnChannel()
	{
		//T* pT = static_cast<T*>(this);
		if (m_EventQueue.empty()) {
			return ;
		}
		SectionLocker Lock(&m_SectionQueue);
		if (m_EventQueue.empty()) {
			return ;
		}
		Event evt;
		evt = m_EventQueue.front();
		m_EventQueue.pop_front();
		Lock.Detach();
		ASSERT(evt.dst);
		evt.dst->SendEvent(evt);
	}

protected:
	std::vector<long> m_ChannelThreads;
	static unsigned int __stdcall ChannelThread(void* pParam)
	{
		T* pT = static_cast<T*>((EvtChannelT<T>*)pParam);
		ASSERT(pT);
		pT->OnChannel();
		return 0;
	}
};

//////////////////////////////////////////////////////////////////////////

//需要支持Post异步的地方可以用这个实现
template<class T, class TBase = EvtObjecter>
class EvtChannelObjecterT : public TBase, public EvtChannelT<T>
{
	typedef EvtChannelObjecterT<T,TBase> This;
	typedef TBase Base;
	typedef EvtChannelT<T> EvtChannel;
public:
	EvtChannelObjecterT(int nAsyncChannelCount = 1):Base(),EvtChannel(nAsyncChannelCount)
	{

	}

	virtual long PostEvent(Event& evt)
	{
		AddToChannel(evt);
		return RLT_OK;
	}
	long PostEvent(EvtObjecter* dst, Event& evt)
	{
		return Base::PostEvent(dst,evt);
	}
	long PostEvent(EvtObjecter* dst, long evt, long value, ObjectPtr objPtr)
	{
		return Base::PostEvent(dst,evt,value,objPtr);
	}
	long PostEvent(long evt, long value, ObjectPtr objPtr)
	{
		return Base::PostEvent(evt, value, objPtr);
	}

public:
	//BEGIN_EVT_MAP(This)
	//END_EVT_MAP()
};

////////////////////////////////////////////////////////////////////////////////////
//以上事件模型支持两种模型
//模型1: 使用EvtDispatcher事件分发器，只需从EvtObjecter派生，然后使用EvtDispatcher的
//SendEvent[直接发送事件]/PostEvent[异步发送事件]
//模型2: 使用EvtListenner/EvtHandler监听处理模型，监听者向处理者注册监听，然后监听者
//使用OnEvent让处理者处理相关处理相关请求，处理者当事件发生时直接回调监听者OnEvent
//因为都使用OnEvent,所以两者都支持使用BEGIN_EVT_MAP/END_EVT_MAP
//提高吞吐率可以使用EvtChannel

#endif//_H_EVENT_H_