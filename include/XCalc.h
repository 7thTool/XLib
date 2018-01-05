#ifndef _XCALC_H_
#define _XCALC_H_

#include "XLogic.h"

#include <vector>
#include <string>
using namespace std;

//namespace Calc {

	class Calc;

	class CalcFactory //: public LogicFactoryT<Calc>
	{
	public:
		static Calc* CreateLogicA(const char* pName);
		static Calc* CreateLogicW(const wchar_t* pName);
		static void DestroyLogic(Calc* pL);
	};

	class Calc : public LogicT<Calc,CalcFactory>
	{
		
	};

	class Atom : public Calc
	{
		typedef Calc Base;
	public:
		double fresult;

		void Solve();
	};

	class StringParseParam
	{
	public:
		string strParam;
		int nCursor;

		StringParseParam()
		{
			nCursor = 0;
		}
	};

	class StringCalc : public Calc
	{
		typedef Calc Base;
	public:

		void Solve();
	};

	void Test(string& str);
//}

#endif//_XCALC_H_