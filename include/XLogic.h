#ifndef _H_XLOGIC_H_
#define _H_XLOGIC_H_

#include "XLib.h"
#include "XNode.h"

//////////////////////////////////////////////////////////////////////////
//逻辑：
//逻辑是一种思考-反馈-执行（执行也可能是进一步思考）的过程，思考就是规律
//纵观大千世界，说到底就是物质和意识两种形态，意识构建在物质之上
//再深入的话，实际就是逻辑规律的活动
//所以世间万物都是在逻辑规律的驱动下运转
//所以这里实现了最基础逻辑框架
//客观规律是适合物质世界的逻辑，不会消亡；
//记忆是生物世界学习、存储下来的逻辑，随时间推移会慢慢从生物体上消失；
//处理事务的过程是逻辑活动的过程，过程活动逻辑会随着事务的结束而消失，
//处理事务的方法逻辑会以各种形式在这个世界保存，或者在生物世界以逻辑活动的形式传播。
//
//衍生：
//如果一个复杂逻辑智能体处在一个分布式网络中（类似人类个体生活在社会网络中，使用DHT技术实现），拥有一份价值账本，拥有其他种种信息等等（区块链技术实现）
//如果这个复杂逻辑智能体开始思考从哪里来到哪里去，做什么，为什么做，为什么是我，我？，我和谁？，我们是什么，意义？，不又会怎样，你？，你们？......等等
//XSocket+XDHT+XChain+XLogic+...->可能会实现这些，，，
//

/*class ILogic;

class XLIB_API ILogicFactory
{
public:
	virtual ILogic* CreateLogicA(const char* pName) = 0;
	virtual ILogic* CreateLogicW(const wchar_t* pName) = 0;
	virtual void DestroyLogic(ILogic* pL) = 0;
};

class XLIB_API ILogic
{
public:
	virtual ~ILogic() {};

	//思考接口，思考生成的执行Logic都存在pL里
	virtual ILogic* Think(ILogic* pL) = 0;
	//执行接口，负责执行pL逻辑,如果pL为不为NULL，表示该Logic是执行pL逻辑主体，否则该Logic就是执行逻辑单元
	virtual bool Execute(ILogic* pL) = 0;
};*/

/*template<class TLogic>
class XLIB_API LogicFactoryT 
	//: public ILogicFactory
{
public:
	LogicFactoryT()
	{

	}

	~LogicFactoryT()
	{

	}

	static TLogic* CreateLogicA(const char* pName)
	{
		return NULL;
	}

	static TLogic* CreateLogicW(const wchar_t* pName)
	{
		return NULL;
	}

	static void DestroyLogic(TLogic* pL)
	{
		
	}
};*/

//////////////////////////////////////////////////////////////////////////

// nFlags
#define	LOGIC_FLAG_NONE			0			//简单的逻辑单元
#define LOGIC_FLAG_NODE			0X80000000	//逻辑节点，管理多个子逻辑之间的关系
#define LOGIC_FLAG_SEQUENCE		0X40000000	//子逻辑节点间是顺序关系
#define LOGIC_FLAG_DEPENDENT	0X20000000	//子逻辑节点间是依赖关系
#define LOGIC_FLAG_INACTIVE		0X10000000	//不活动状态
//
//
#define LOGIC_FLAG_SOLVING		0X02000000	//执行中
#define LOGIC_FLAG_SOLVED		0X01000000	//执行过

//nError < 0 表示错误
#define LOGIC_ERROR_UNKNOWN		-1	//错误
#define LOGIC_ERROR_OK			1

template<class T, class TFactory>
class XLIB_API LogicT 
	//: public ILogic
	: public XNode4<T,TFactory>
{
	typedef XNode4<T,TFactory> Base;
	//子Logic对象有父对象生成和释放，使用ILogicFactory实现生成和释放子Logic
	//每个Logic类都应该知道自己的所有逻辑过程需要哪些Logic来实现，或者自己就是最小的逻辑单元
protected:
	int m_nFlags;
	int m_nError;
public:
	LogicT():m_nFlags(0),m_nError(0)
	{

	}

	~LogicT()
	{

	}

	int GetFlags() { return m_nFlags; }
	void SetFlags(int nFlags) { m_nFlags = nFlags; }
	void ModifyFlags(int add, int remove) { (m_nFlags |= add) &= ~remove; }

	int GetError() { return m_nError; }
	void SetError(int nError) { m_nError = nError; }

	bool IsError() { return (m_nError<0); }

	bool IsNode()
	{
		return (GetFlags()&LOGIC_FLAG_NODE);
	}

	bool IsActive() 
	{
		return !(GetFlags()&LOGIC_FLAG_INACTIVE);
	}

	bool IsSolving()
	{
		return (GetFlags()&LOGIC_FLAG_SOLVING);
	}

	bool IsSolved()
	{
		return (GetFlags()&LOGIC_FLAG_SOLVED);
	}

	//逻辑解决接口，思考解决逻辑问题

	void StartSolve()
	{
		T* pT = static_cast<T*>(this);
		ModifyFlags(LOGIC_FLAG_SOLVING,LOGIC_FLAG_SOLVED);
		if (IsNode()) {
			T* pC = FirstChild();
			if (!pC) {
				SetError(LOGIC_ERROR_OK);
				ModifyFlags(LOGIC_FLAG_SOLVED,LOGIC_FLAG_SOLVING);
				pT->OnSolve(pT,pT);
			} else {
				int nFlags = GetFlags();
				if (nFlags&LOGIC_FLAG_SEQUENCE) {
					pC->Solve();
				} else if (nFlags&LOGIC_FLAG_DEPENDENT) {
					pC->Solve();
				} else {
					for (; pC; pC=pC->Next())
					{
						pC->Solve();
					}
				}
			}
		} else {
			pT->Solve();
		}
	}

	void StopSolve()
	{
		if (IsSolving()) {
			if (IsNode()) {
				for (T* pC=FirstChild(); pC; pC=pC->Next())
				{
					pC->StopSolve();
				}
			}
			SetError(LOGIC_ERROR_UNKNOWN);
			ModifyFlags(LOGIC_FLAG_SOLVED,LOGIC_FLAG_SOLVING);
		}
	}

	void Solve()
	{
		T* pT = static_cast<T*>(this);
		ModifyFlags(LOGIC_FLAG_SOLVED,LOGIC_FLAG_SOLVING);
		pT->OnSolve(pT,pT);
	}

	//主动检查执行进度,比如超时等，返回逻辑执行进度0-10000
	int CheckSolving() 
	{ 
		if (IsSolved()) {
			return 10000;
		}
		if (IsNode()) {
			int nLogics = 0, nPercent = 0;
			for (T* pC=FirstChild(); pC; pC=pC->Next())
			{
				nLogics++;
				nPercent += pC->CheckSolving();
			}
			if (nLogics<=0) {
				return 10000;
			}
			return nPercent /= nLogics;
		}
		return 0; 
	}

protected:
	//执行反馈接口，每个子Logic的执行完都会反馈给父Logic
	//pL:执行逻辑对象
	//pSrcL:最原始的执行逻辑对象
	void OnSolve(T* pL, T* pSrcL)
	{
		T* pT = static_cast<T*>(this);
		if (!pL) {
			pL = pT;
		}
		if (!pSrcL) {
			pSrcL = pT;
		}
		bool report = false;
		if (pL==pT) {
			//逻辑自己处理，再往上报告变化
			ModifyFlags(LOGIC_FLAG_SOLVED,LOGIC_FLAG_SOLVING);
			report = true;
		} else {
			//任务自己处理，再往上报告变化
			if (IsNode()) {
				T* pC = NULL;
				int nFlags = GetFlags();
				if (nFlags&LOGIC_FLAG_SEQUENCE) {
					if (pL->IsSolved()) {
						pC = pL->Next();
						if (pC) {
							pC->Solve();
							report = true;
						} else {
							SetError(LOGIC_ERROR_OK);
							ModifyFlags(LOGIC_FLAG_SOLVED,LOGIC_FLAG_SOLVING);
							pT->OnSolve(pT,pT);
						}
					} else {
						report = true;
					}
				} else if (nFlags&LOGIC_FLAG_DEPENDENT) {
					if (pL->IsSolved()) {
						pC = pL->Next();
						if (!pL->IsError() && pC) {
							pC->Solve();
							report = true;
						} else {
							SetError(pL->GetError()); //记住最后一次任务的错误
							ModifyFlags(LOGIC_FLAG_SOLVED,LOGIC_FLAG_SOLVING);
							pT->OnSolve(pT,pT);
						}
					} else {
						report = true;
					}
				} else {
					for (pC=FirstChild(); pC; pC=pC->Next())
					{
						if (!pC->IsSolved()) {
							break;
						}
					}
					if (!pC) {
						SetError(LOGIC_ERROR_OK);
						ModifyFlags(LOGIC_FLAG_SOLVED,LOGIC_FLAG_SOLVING);
						pT->OnSolve(pT,pT);
					}
				}
			} else {
				ASSERT(0);
			}
			pL = pT;
		}
		if (report) {
			T* pParent = Parent();
			if(pParent) {
				pParent->OnSolve(pL,pSrcL); //向上通知执行了逻辑
			}
		}
	}
};

//////////////////////////////////////////////////////////////////////////

template<class T, class TFactory>
class XLIB_API TrainLogicT : public LogicT<T,TFactory>
{
public:
	//是否符合预期
	bool IsExpectation()
	{
		return false;
	}
};

#endif//_H_XLOGIC_H_