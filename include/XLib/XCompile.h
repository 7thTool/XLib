#ifndef _H_XCOMPILE_H_
#define _H_XCOMPILE_H_

#include "XLogic.h"

class Atom;
class Compile;

#define			COMPILE_MAX_IDENTIFYNAME					32					//最大标识符（变量、参数、函数等名称）长度
#define			COMPILE_MAX_FUNPARAM						10					//函数最大参数数量
#define			COMPILE_MAX_STACK							500					//基本栈大小
#define			COMPILE_MAX_MEMORYCOUNT						1000				//最大存储空间数量
#define			COMPILE_MAX_MEMORYSIZE						16384				//最大存储空间大小
#define			COMPILE_MAX_STRINGSIZE						256					//最大字符串长度
#define			COMPILE_MAX_VARIABLE						200					//最大变量数量
#define			COMPILE_MAX_PARAM							200					//最大参数数量
#define			COMPILE_MAX_REFLINE							16					//最大参考线数量
#define			COMPILE_MAX_CODECOUNT						1000				//生成代码最大数量

enum
{
	COMPILE_DATATYPE_UNKNOW,													//未知类型
	COMPILE_DATATYPE_BOOL,														//Bool类型
	COMPILE_DATATYPE_DOUBLE,													//Double类型
	COMPILE_DATATYPE_STRING,													//字符串类型
	COMPILE_DATATYPE_DRAWDATA,													//画线数据类型
};

enum
{
	DRAWRLT_FLG_OTHERFLAG = 1,	//绘图结果只是一个标记，不用于计算最高最低值
	DRAWRLT_FLG_HIGLLOW = 2,	//绘图结果用于计算最高最低值
};

typedef struct																	//画线数据类型
{
	int										m_lCheckPos;						//用于校验位，计算时需要填入当前计算的K线位置
	double									m_dDataEx[10];
	char									*strData;							//一些指针的值(用于drawtext)等函数
	int										m_iDataFlg[10];						//结果数据是否用于计算最高最低等属性
	int										m_dDataExCount;
}tagCompile_DrawDataType;

enum
{
	COMPILE_ATOM_UNKNOW = 0,													//0、未知

	COMPILE_ATOM_OPERATOR_SYMBOL,												//1、运算符号
	COMPILE_ATOM_OTHER_SYMBOL,													//2、其他符号

	COMPILE_ATOM_CONST,															//3、常数

	COMPILE_ATOM_KEYWORD,														//4、保留字

	COMPILE_ATOM_PARAM,															//5、参数
	COMPILE_ATOM_VARIABLE,														//6、变量
	COMPILE_ATOM_STACK,															//7、堆栈

	COMPILE_ATOM_FUNCTION,														//8、函数
	COMPILE_ATOM_QUOTE,															//9、引用

	COMPILE_ATOM_COMMENT,														//10、注释
};

#define			compile_strncpy(t,s,n)			do { strncpy(t,s,n); t[n-1] = 0; } while(false)

#define			compile_voidptr(n)				((void*)(n))
#define			compile_boolptr(n)				((bool*)(n))
#define			compile_intptr(n)				((int*)(n))
#define			compile_doubleptr(n)			((double*)(n))
#define			compile_stringptr(n)			((char*)(n))
#define			compile_drawdataptr(n)			((tagCompile_DrawDataType *)(n))
#define			compile_doubleequal(a,b)		((((a)-(b))<=0.000005) && (((a)-(b))>=-0.000005))
#define			compile_isnull(ptr)				(*(bool*)((unsigned char*)(ptr)-sizeof(bool)))

typedef struct
{
	char								szFuncName[COMPILE_MAX_IDENTIFYNAME];	//函数名称
	unsigned char						lpParamType[COMPILE_MAX_FUNPARAM];		//函数参数类型
	int									iParamCount;							//参数数量【注意：-1表示不限制数量】
	//unsigned long						iDataType;								//函数所需数据类型【用于计算权限】
	bool								bSpecialFunc;							//是否特殊函数，如特殊函数，则下列值为CreateObject，否则直接填写函数指针
	unsigned char						ucRetDataType;							//函数返回数据类型

	Atom*								pL;										//逻辑
} tagCompile_FuncTableInRecord;

typedef struct
{
	char								szParamName[COMPILE_MAX_IDENTIFYNAME];//参数名称
	double								dDefaultVal;						//缺省值
	double								dMaxVal;							//最大值
	double								dMinVal;							//最小值
	double								dStepVal;							//步长
} tagCompile_ParamTableInRecord;

typedef struct
{
	char								*	szInBuf;					//原文内容
	int										iInSize;					//原文长度

	tagCompile_FuncTableInRecord		*	lpFuncRecord;				//函数表
	int										iFuncCount;					//函数表数量

	tagCompile_ParamTableInRecord		*	lpParamRecord;				//参数表
	int										iParamCount;				//参数数量

	double								*	lpRefRecord;				//参考线表
	int										iRefCount;					//参考线数量

	Atom*									pL;							//逻辑

} tagCompile_CompileParamIn;

typedef struct
{
	unsigned char						cID;								//标识符ID号
	char								szName[3];							//标识符符号
	unsigned char						cPriority;							//优先级别
	bool								bIsOperator;						//是否为运算符[否则为分割符号]
	bool								bIsRead;							//是否为直接读取符号[否则为转译符号]
	unsigned char						cParmCount;							//该操作符参数数量[1或2，即1元和2元操作符]
} tagCompile_SymbolTableRecord;

//[优先级别]
enum
{
	COMPILE_PRIORITY_ERR = -1,													//错误
	COMPILE_PRIORITY_BIG = 1,													//大于
	COMPILE_PRIORITY_SMALL = 2,													//小于
	COMPILE_PRIORITY_EQUAL = 3													//等于
};

typedef struct
{
	int										iSourceOffset;				//词起始位置
	int										iWordLength;				//词长度
} tagCompile_AtomPos;

typedef struct
{
	int										iSourceOffset;				//词起始位置
	int										iWordLength;				//词长度
	unsigned char							cType;						//词类型
} tagCompile_WordInfo;

typedef struct
{
	int										iRow;						//行
	int										iCol;						//列
	char									strError[512];
} tagCompile_ErrorInfo;

enum
{
	COMPILE_EXPRESSION_ORDER = 1,					//顺序语句、后面接	,	;
	COMPILE_EXPRESSION_FUNC,						//函数参数、后接	,	)
	COMPILE_EXPRESSION_IF,							//if条件、后接		then
	COMPILE_EXPRESSION_WHILE,						//while条件、后接	do
	COMPILE_EXPRESSION_FOR1,						//for语句前部分，后接	to	downto
	COMPILE_EXPRESSION_FOR2,						//for语句后部分，后接	do
};

typedef struct
{
	int										iSourceCodeRow;				//源代码行数
	int										iParamCount;				//编译后参数数量
	int										iVariableCount;				//编译后变量数量
	int										iMaxStackCount;				//编译后所需堆栈
	int										iRefCount;					//编译后参考线数量
	unsigned long							iDataType;					//编译后所需数据
	char									cReserved[32];				//保留
} tagCompile_CompileResultInfo;

enum
{
	COMPILE_CODE_OPERATOR = 1,
	COMPILE_CODE_MOV,
	COMPILE_CODE_INC,
	COMPILE_CODE_DEC,
	COMPILE_CODE_CALLFUNC,
	COMPILE_CODE_CALLQUOTE,
	COMPILE_CODE_DEFINE,
	COMPILE_CODE_JMP,
	COMPILE_CODE_JZ,
	COMPILE_CODE_JAE,
	COMPILE_CODE_JBE,
};
/*
typedef struct
{
	int									iSourceCodeRow;						//对应源代码行数
	unsigned char						ucCodeType;							//代码类型
	Atom						mParam[COMPILE_MAX_FUNPARAM];		//参数
	Atom						mRet;								//返回值 
	union
	{
		int								iSymbol;							//操作符
		int								iCodeAddr;							//跳转地址
		struct
		{
			int							iParamCount;						//参数数量
			char						szFuncName[COMPILE_MAX_IDENTIFYNAME];//函数名称
		};
	};
} tagCompile_Code;
//..............................................................................................................................
typedef struct
{
	int									iSourceCodeRow;						//对应源代码行数
	tagCompile_Func					*	lpFuncPtr;							//函数指针
	void							*	lpRetValue;							//函数返回值地址
	void							*	lpParam[COMPILE_MAX_FUNPARAM];		//参数地址
	int									iParamCount;						//参数个数
} tagCompile_ExecCode;
//..............................................................................................................................
typedef struct
{
	double							*	lpParamValue;
	int									iParamCount;
} tagCompile_ExecParam;
//..............................................................................................................................
typedef struct
{
	int										iSourceOffset;				//当前源代码执行行数
	int										iTargetOffset;				//当前编译代码执行行数
	char									cReserved[32];				//保留
} tagCompile_ExecStepParam;
//..............................................................................................................................
typedef struct
{
	char								szParamName[COMPILE_MAX_IDENTIFYNAME];//参数名称
	double								dDefaultVal;						//缺省值
	double								dMaxVal;							//最大值
	double								dMinVal;							//最小值
	double								dStepVal;							//步长
	int									iMemoryAddr;						//参数地址
} tagCompile_ParamTableRecord;
//..............................................................................................................................
enum LINE_TYPE
{
	LINE_TYPE_CIRCLEDOT = 0,
	LINE_TYPE_COLOR3D,
	LINE_TYPE_COLORSTICK,
	LINE_TYPE_CROSSDOT,
	LINE_TYPE_DASHLINE,
	LINE_TYPE_DOTLINE,
	LINE_TYPE_LINESTICK,
	LINE_TYPE_POINTDOT,
	LINE_TYPE_STICK,
	LINE_TYPE_STICK3D,
	LINE_TYPE_VOLSTICK,
};
typedef struct
{
	char								szVarName[COMPILE_MAX_IDENTIFYNAME];//变量名称
	int									iMemoryAddr;						//变量存储地址
	unsigned char						cCalType;							//计算类型[0 局部变量	1 全局变量]
	unsigned char						cOutputType;						//输出类型[0 不输出		1 输出]
	unsigned char						cLineType;							//绘制线型
	unsigned char						cAlignType;							//水平对齐
	unsigned char						cVAlignType;						//垂直对齐
	unsigned long						lColor;								//绘制颜色
	unsigned char						cLayer;								//层次
	unsigned char						cLineWidth;							//绘制线宽
	unsigned char						cMove;								//平移控制
	unsigned char						cPrecis;							//精度
} tagCompile_VariableTableRecord;
//..............................................................................................................................
typedef struct __Compile_FuncObjectNode__
{
	MCompile_FunctionBase			*	lpFuncObject;
	__Compile_FuncObjectNode__		*	lpNext;
} tagCompile_FuncObjectNode;
//..............................................................................................................................
typedef struct
{
	char								szFuncName[COMPILE_MAX_IDENTIFYNAME];	//函数名称
	unsigned char						lpParamType[COMPILE_MAX_FUNPARAM];		//函数参数类型
	int									iParamCount;							//参数数量
	unsigned long						iDataType;								//函数所需数据类型【用于计算权限】
	bool								bSpecialFunc;							//是否特殊函数，如特殊函数，则下列值为CreateObject，否则直接填写函数指针
	unsigned char						ucRetDataType;							//函数返回数据类型

	union
	{
		tagCompileFunc_CreateObject	*	lpFuncCreate;							//特殊函数创建函数对象指针
		tagCompile_Func				*	lpFunc;									//一般函数，函数调用指针
	};

	tagCompile_FuncObjectNode		*	lpFuncObjectNode;						//特殊函数对象实例链表
} tagCompile_FuncTableRecord;*/
//..............................................................................................................................
typedef struct
{
	unsigned char						ucDataType;							//数据类型
	bool							*	lpIsNULL;							//是否为空
	void							*	lpData;								//数据地址
} tagCompile_MemoryRecord;

class AtomFactory : public LogicFactoryT<Atom>
{
public:
	AtomFactory();
	virtual ~AtomFactory();

	virtual Atom* CreateLogicA(char* pName);
	virtual Atom* CreateLogicW(wchar_t* pName);
	virtual void DestroyLogic(Atom* pL);
};

class Atom : public LogicT<Atom,AtomFactory>
{
public:
	char m_szName[COMPILE_MAX_IDENTIFYNAME];	//标识符名称
	unsigned char m_ucType;						//标识符类型【参见上面描述】
	int m_iAddress;								//所在地址【各种类型地址表示不同含义】
public:
	Atom(void);
	Atom(const char * szName,unsigned char ucType,int iAddress);
	Atom(const Atom & mIn);
	virtual ~Atom();
public:
	Atom & operator = (const Atom & mIn);
	bool operator == (const char * szIn);
	bool operator == (const Atom & mIn);

public:
	//思考接口，思考生成的执行Logic都存在pL里
	//T* Think(T* pL)
	//执行接口，负责执行pL逻辑,如果pL为不为NULL，表示该Logic是执行pL逻辑主体，否则该Logic就是执行逻辑单元
	//bool Execute(T* pL)
};

//////////////////////////////////////////////////////////////////////////
//[存储表]

class MemoryTable
{
protected:
	tagCompile_MemoryRecord						m_sRecord[COMPILE_MAX_MEMORYCOUNT];
	int											m_iRecordCount;
	char										m_lpData[COMPILE_MAX_MEMORYSIZE];
	int											m_iDataSize;

	Compile										*	m_lpParent;
protected:
	__inline int  inner_allocmemory(unsigned char ucDataType);
public:
	MemoryTable(void);
	virtual ~MemoryTable();
public:
	int	 Instance(Compile * lpParent);
	void Release(void);
public:
	int  AllocConst(bool bIn);
	int  AllocConst(double dIn);
	int  AllocConst(const char * szIn);
	int  AllocMemory(unsigned char ucDataType);
	int  AssignType(int iSerial,unsigned char ucDataType);
public:
	int  GetMemoryRecord(int iSerial,tagCompile_MemoryRecord * lpOut);
};

//////////////////////////////////////////////////////////////////////////

class Compile : public Atom
{
	friend MemoryTable;
protected:
	const char							*	m_szInBuf;					//代码
	int										m_iOffset;					//当前解析偏移
	int										m_iInSize;					//代码内容长度
	int										m_iRow;						//当前行数
	int										m_iRowOffset;				//当前行的起始位置
protected:
	vector <tagCompile_WordInfo>			m_VecWordInfo;
	vector <tagCompile_ErrorInfo>			m_VecErrorInfo;
protected:
	__inline int  inner_compileinitialize(Atom* pL);
	__inline int  inner_compilesentences();
	__inline int  inner_readdescribe(char cCh,char cChNext);
	__inline int  inner_readstring(char cCh,Atom * mOut);
	__inline int  inner_issymbol(char cCh,char cChNext,Atom * mOut);
	__inline int  inner_isconst(const char * szInString,Atom * mOut,bool bLook);
	__inline int  inner_iskeyword(const char * szInString);
	__inline void inner_updateatom(char * szInString,Atom * mOut,bool bLook);
	__inline int  inner_readatom(Atom * mOut,bool bLook);
	__inline int  inner_getatom(Atom * mOut, tagCompile_AtomPos * mPosOut);
	__inline int  inner_isotherkeyword(Atom * mIn);

public:
	int  ReadAtom(Atom * mOut);
	int  LookAtom(Atom * mOut);
	int  CheckAtom(const char * szInString);
	int  CheckAtom2(const char * szInString);
public:
	int	 GetWord();
	int	 GetWordList(tagCompile_WordInfo * mOut, int iInSize);
	int	 GetErrorListCount();
	int	 GetErrorList(tagCompile_ErrorInfo * mOut, int iInSize);

public:
	void WriteReport(const char * szFormat,...);
	//yhm add
public:
	int SearchDrawFunc(char * szOutString);//返回找到的画图函数名称

protected:
	MemoryTable m_mMemoryTable;
public:
	//Compile(void);
	//virtual ~Compile();
	int Instance(const tagCompile_CompileParamIn * mParam);
	void Release();

	Atom* Think(Atom* pL);
};

//////////////////////////////////////////////////////////////////////////
//
//class Expression : public Atom
//{
//protected:
//	Atom						m_mLastCalAddr;
//	MCompile_Stack<int>					m_mSymbolStack;
//	MCompile_Stack<Atom>		m_mVariableStack;
//	int									m_iKH;
//protected:
//	__inline  int inner_compile(Atom mAtom);
//	__inline  int inner_inputsymbol(int iSymbolID);
//	__inline  int inner_inputvariable(Atom mVariable);
//	__inline  unsigned char inner_operatorretdatatype(int iSymbol,Atom m1,Atom m2);
//public:
//	MCompile_Expression(MMyCompile * lpParent);
//	virtual ~MCompile_Expression();
//public:
//	int  Compile(unsigned char cType,Atom * mRet);
//};

#endif//_H_XCOMPILE_H_