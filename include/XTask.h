#ifndef _H_XTASK_H_
#define _H_XTASK_H_

#include "XLib.h"
#include "XLogic.h"

//任务小于0出错，大于0成功，0没有处理
#define TASK_RESULT_TIMEOUT		-2		//任务超时
#define TASK_RESULT_ERROR		-1		//任务普通错误
#define TASK_RESULT_NULL		0		//任务没有处理
#define TASK_RESULT_FALSE		0		//任务没有处理
#define TASK_RESULT_TRUE		1		//任务执行完成
#define TASK_RESULT_OK			1		//任务执行完成
#define TASK_RESULT_COMPLETE	1		//任务执行完成
#define TASK_RESULT_X			2		//任务无需执行完成

//Task日志只需注册顶层任务的日志
template<class TTask>
class TaskLogerT
{
public:
	typedef TaskLogerT<TTask> TTaskLoger;
private:
	static std::map<TTask*,TTaskLoger*> s_pTask2pLoger;
public:
	static void RegisterLoger(TTaskLoger* pLoger,TTask* pObj)
	{
		s_pTask2pLoger[pObj] = pLoger;
	}
	static void UnRegisterLoger(TTaskLoger* pLoger,TTask* pObj) 
	{
		std::map<TTask*,TTaskLoger*>::iterator it = s_pTask2pLoger.find(pObj);
		if (it!=s_pTask2pLoger.end())
		{
			s_pTask2pLoger.erase(it);
		}
	}
	static void Log(TTask* pObj) 
	{
		if (!pObj) {
			return;
		}

		std::map<TTask*,TTaskLoger*>::iterator it = s_pTask2pLoger.begin();
		for (; it!=s_pTask2pLoger.end(); ++it)
		{
			if (it->first==pObj || it->first->IsChild(pObj,TRUE))
			{
				it->second->OnLog(pObj->GetResult());
			}
		}
	}
	static void Log(TTask* pObj, LPCTSTR lpszStr) 
	{
		std::map<TTask*,TTaskLoger*>::iterator it = s_pTask2pLoger.begin();
		for (; it!=s_pTask2pLoger.end(); ++it)
		{
			if ((it->first==pObj) || (it->first && it->first->IsChild(pObj,TRUE)))
			{
				it->second->OnLog(lpszStr);
			}
		}
	}

public:

	virtual void OnLog(LPCTSTR lpszStr) { };
};

template<class TTask>
std::map<TTask*,TaskLogerT::TTaskLoger*> TaskLogerT<TTask>::s_pTask2pLoger;

//////////////////////////////////////////////////////////////////////////
//后台保持任务运行者，需要主动注册

template<class TTask>
class TaskRunnerT
{
private:
	static TaskRunnerT<TTask>* s_pRunner;
public:
	static void SetRunner(TaskRunnerT<TTask>* pRunner)
	{
		s_pRunner = pRunner;
	}
	static TaskRunnerT<TTask>* GetRunner() 
	{
		return s_pRunner;
	}
	static void RegisterRunTask(TTask* pObj) 
	{
		TaskRunnerT<TTask>* pRunner = GetRunner();
		if (pRunner)
		{
			pRunner->OnRegisterRunTask(pObj);
		}
	}
	static void UnRegisterRunTask(TTask* pObj)
	{
		TaskRunnerT<TTask>* pRunner = GetRunner();
		if (pRunner)
		{
			pRunner->OnUnRegisterRunTask(pObj);
		}
	}
public:
	TaskRunnerT()
	{
		SetRunner(this);
	}
	~TaskRunnerT()
	{
		SetRunner(NULL);
	}

protected:
	virtual void OnRegisterRunTask(TTask* pObj) 
	{
		m_pRunObjs.insert(pObj);
	}

	virtual void OnUnRegisterRunTask(TTask* pObj)
	{
		m_pRunObjs.erase(pObj);
	}

protected:
	std::set<TTask*> m_pRunObjs;
};

template<class TTask>
TaskRunnerT<TTask>* TaskRunnerT<TTask>::s_pRunner = NULL;

//////////////////////////////////////////////////////////////////////////
//具体任务需要具体实现

//任务对象
template<class T,class TFactory>
class XLIB_API TaskT : public LogicT<T,TFactory>
{
public:
	TaskT()
	{
		
	}

	void SetDescription(LPCTSTR lpszStr) { }
	LPCTSTR GetDescription() { return NULL; }

	//重置
	void Reset(bool bComplete = false) 
	{
		if(IsNode()) {
			T* pC = FirstChild();
			for (;pC;pC=pC->Next())
			{
				pC->Reset(bComplete);
			}
		}
		if (bComplete) {
			ModifyFlags(LOGIC_FLAG_EXECUTED,0);
		} else {
			ModifyFlags(0,LOGIC_FLAG_EXECUTED|LOGIC_FLAG_EXECUTING);
		}
	}

	void Add(T* pObj)
	{
		if (nFlags&LOGIC_FLAG_SEQUENCE) {
			pObj->Free();
			LastChild(pObj);
		} else if (nFlags&LOGIC_FLAG_DEPENDENT) {
			pObj->Free();
			LastChild(pObj);
		} else {
			T* pT = dynamic_cast<T*>(this);
			T* pParentObj = pObj->Parent();
			if (pParentObj==pThis) {
				//已经在列表里,不用处理
			} else {
				pObj->Free();
				LastChild(pObj);
			}
		}
	}

	void Remove(T* pObj)
	{
		pObj->Free();
	}

	bool IsProcessing()
	{
		return IsExecuting();
	}

	bool IsComplete()
	{
		return IsExecuted();
	}

	void SetResult(LPCTSTR lpszStr) 
	{ 
		
	}
	LPCTSTR GetResult() 
	{ 
		if (IsNode()) {
			//保持和IsComplete判断一致，只有出错时使用出错对象的Result
			int nFlags = GetFlags();
			if (nFlags&LOGIC_FLAG_DEPENDENT) {
				BOOL bError = FALSE;
				TTask* pC = FirstChild();
				for(; pC; pC=pC->Next())
				{
					bError = pC->IsError();
					if (bError) {
						break;
					}
				}
				if (bError) {
					return pC->GetResult();
				}
			} else {
				if (IsError()) {
					TTask* pC = LastChild();
					if(pC) {
						return pC->GetResult();
					}
				}
			}
			return Base::GetResult();
		}
		return NULL; 
	}
};

#endif//_H_XTASK_H_
