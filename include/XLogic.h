#ifndef _H_XLOGIC_H_
#define _H_XLOGIC_H_

#include "XLib.h"
#include "XNode.h"

//////////////////////////////////////////////////////////////////////////
//�߼���
//�߼���һ��˼��-����-ִ�У�ִ��Ҳ�����ǽ�һ��˼�����Ĺ��̣�˼�����ǹ���
//�ݹ۴�ǧ���磬˵���׾������ʺ���ʶ������̬����ʶ����������֮��
//������Ļ���ʵ�ʾ����߼����ɵĻ
//�����������ﶼ�����߼����ɵ���������ת
//��������ʵ����������߼����
//�͹۹������ʺ�����������߼�������������
//��������������ѧϰ���洢�������߼�����ʱ�����ƻ�����������������ʧ��
//��������Ĺ������߼���Ĺ��̣����̻�߼�����������Ľ�������ʧ��
//��������ķ����߼����Ը�����ʽ��������籣�棬�����������������߼������ʽ������
//
//������
//���һ�������߼������崦��һ���ֲ�ʽ�����У��������������������������У�ʹ��DHT����ʵ�֣���ӵ��һ�ݼ�ֵ�˱���ӵ������������Ϣ�ȵȣ�����������ʵ�֣�
//�����������߼������忪ʼ˼����������������ȥ����ʲô��Ϊʲô����Ϊʲô���ң��ң����Һ�˭����������ʲô�����壿�����ֻ��������㣿�����ǣ�......�ȵ�
//XSocket+XDHT+XChain+XLogic+...->���ܻ�ʵ����Щ������
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

	//˼���ӿڣ�˼�����ɵ�ִ��Logic������pL��
	virtual ILogic* Think(ILogic* pL) = 0;
	//ִ�нӿڣ�����ִ��pL�߼�,���pLΪ��ΪNULL����ʾ��Logic��ִ��pL�߼����壬�����Logic����ִ���߼���Ԫ
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
#define	LOGIC_FLAG_NONE			0			//�򵥵��߼���Ԫ
#define LOGIC_FLAG_NODE			0X80000000	//�߼��ڵ㣬���������߼�֮��Ĺ�ϵ
#define LOGIC_FLAG_SEQUENCE		0X40000000	//���߼��ڵ����˳���ϵ
#define LOGIC_FLAG_DEPENDENT	0X20000000	//���߼��ڵ����������ϵ
#define LOGIC_FLAG_INACTIVE		0X10000000	//���״̬
//
//
#define LOGIC_FLAG_SOLVING		0X02000000	//ִ����
#define LOGIC_FLAG_SOLVED		0X01000000	//ִ�й�

//nError < 0 ��ʾ����
#define LOGIC_ERROR_UNKNOWN		-1	//����
#define LOGIC_ERROR_OK			1

template<class T, class TFactory>
class XLIB_API LogicT 
	//: public ILogic
	: public XNode4<T,TFactory>
{
	typedef XNode4<T,TFactory> Base;
	//��Logic�����и��������ɺ��ͷţ�ʹ��ILogicFactoryʵ�����ɺ��ͷ���Logic
	//ÿ��Logic�඼Ӧ��֪���Լ��������߼�������Ҫ��ЩLogic��ʵ�֣������Լ�������С���߼���Ԫ
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

	//�߼�����ӿڣ�˼������߼�����

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

	//�������ִ�н���,���糬ʱ�ȣ������߼�ִ�н���0-10000
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
	//ִ�з����ӿڣ�ÿ����Logic��ִ���궼�ᷴ������Logic
	//pL:ִ���߼�����
	//pSrcL:��ԭʼ��ִ���߼�����
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
			//�߼��Լ����������ϱ���仯
			ModifyFlags(LOGIC_FLAG_SOLVED,LOGIC_FLAG_SOLVING);
			report = true;
		} else {
			//�����Լ����������ϱ���仯
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
							SetError(pL->GetError()); //��ס���һ������Ĵ���
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
				pParent->OnSolve(pL,pSrcL); //����ִ֪ͨ�����߼�
			}
		}
	}
};

//////////////////////////////////////////////////////////////////////////

template<class T, class TFactory>
class XLIB_API TrainLogicT : public LogicT<T,TFactory>
{
public:
	//�Ƿ����Ԥ��
	bool IsExpectation()
	{
		return false;
	}
};

#endif//_H_XLOGIC_H_