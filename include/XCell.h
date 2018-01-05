#ifndef _H_XCELL_H_
#define _H_XCELL_H_

#include "XLib.h"
#include "XLogic.h"

#include <math.h>

class Cell;

class CellFactory //: public LogicFactoryT<Cell>
{
public:
	static Cell* CreateLogicA(const char* pName);
	static Cell* CreateLogicW(const wchar_t* pName);
	static void DestroyLogic(Cell* pL);
};

class Cell : public LogicT<Cell,CellFactory>
{
	typedef LogicT<Cell,CellFactory> Base;
protected:
	//
public:
	//
};

//////////////////////////////////////////////////////////////////////////
//S型神经元

class SCell : public Cell
{
	typedef Cell Base;
protected:
	std::vector<double> m_x; //输入x
	std::vector<double> m_w; //权重w
	double m_b; //偏置b=-threshold
	double m_s;
public:
	//输入
	void SetInput(const std::vector<double>& x, const std::vector<double>& w)
	{
		m_x = x;
		m_w = w;
	}

	//偏置
	void SetBias(const double& b)
	{
		m_b = b;
	}
	double GetBias()
	{
		return m_b;
	}

	//输出
	double GetOutput()
	{
		return m_s;
	}

	//阶跃函数
	bool GetStep()
	{
		if (m_s<-_zero_ctrl_) {
			return false;
		}
		return true;
	}

	//计算
	void Solve()
	{
		//s=1/(1+exp(-z));
		//z=x*w+b;
		//s=1/(1+exp(-((x*w)+b)));
		double z = 0.;
		for (int i=0; i<m_x.size(); i++)
		{
			z += m_x[i]*m_w[i];
		}
		z += m_b;
		m_s = 1/(1+exp(-z));

		Base::Solve();
	}
};

#endif//_H_XCELL_H_