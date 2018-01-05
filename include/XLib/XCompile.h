#ifndef _H_XCOMPILE_H_
#define _H_XCOMPILE_H_

#include "XLogic.h"

class Atom;
class Compile;

#define			COMPILE_MAX_IDENTIFYNAME					32					//����ʶ�������������������������ƣ�����
#define			COMPILE_MAX_FUNPARAM						10					//��������������
#define			COMPILE_MAX_STACK							500					//����ջ��С
#define			COMPILE_MAX_MEMORYCOUNT						1000				//���洢�ռ�����
#define			COMPILE_MAX_MEMORYSIZE						16384				//���洢�ռ��С
#define			COMPILE_MAX_STRINGSIZE						256					//����ַ�������
#define			COMPILE_MAX_VARIABLE						200					//����������
#define			COMPILE_MAX_PARAM							200					//����������
#define			COMPILE_MAX_REFLINE							16					//���ο�������
#define			COMPILE_MAX_CODECOUNT						1000				//���ɴ����������

enum
{
	COMPILE_DATATYPE_UNKNOW,													//δ֪����
	COMPILE_DATATYPE_BOOL,														//Bool����
	COMPILE_DATATYPE_DOUBLE,													//Double����
	COMPILE_DATATYPE_STRING,													//�ַ�������
	COMPILE_DATATYPE_DRAWDATA,													//������������
};

enum
{
	DRAWRLT_FLG_OTHERFLAG = 1,	//��ͼ���ֻ��һ����ǣ������ڼ���������ֵ
	DRAWRLT_FLG_HIGLLOW = 2,	//��ͼ������ڼ���������ֵ
};

typedef struct																	//������������
{
	int										m_lCheckPos;						//����У��λ������ʱ��Ҫ���뵱ǰ�����K��λ��
	double									m_dDataEx[10];
	char									*strData;							//һЩָ���ֵ(����drawtext)�Ⱥ���
	int										m_iDataFlg[10];						//��������Ƿ����ڼ��������͵�����
	int										m_dDataExCount;
}tagCompile_DrawDataType;

enum
{
	COMPILE_ATOM_UNKNOW = 0,													//0��δ֪

	COMPILE_ATOM_OPERATOR_SYMBOL,												//1���������
	COMPILE_ATOM_OTHER_SYMBOL,													//2����������

	COMPILE_ATOM_CONST,															//3������

	COMPILE_ATOM_KEYWORD,														//4��������

	COMPILE_ATOM_PARAM,															//5������
	COMPILE_ATOM_VARIABLE,														//6������
	COMPILE_ATOM_STACK,															//7����ջ

	COMPILE_ATOM_FUNCTION,														//8������
	COMPILE_ATOM_QUOTE,															//9������

	COMPILE_ATOM_COMMENT,														//10��ע��
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
	char								szFuncName[COMPILE_MAX_IDENTIFYNAME];	//��������
	unsigned char						lpParamType[COMPILE_MAX_FUNPARAM];		//������������
	int									iParamCount;							//����������ע�⣺-1��ʾ������������
	//unsigned long						iDataType;								//���������������͡����ڼ���Ȩ�ޡ�
	bool								bSpecialFunc;							//�Ƿ����⺯���������⺯����������ֵΪCreateObject������ֱ����д����ָ��
	unsigned char						ucRetDataType;							//����������������

	Atom*								pL;										//�߼�
} tagCompile_FuncTableInRecord;

typedef struct
{
	char								szParamName[COMPILE_MAX_IDENTIFYNAME];//��������
	double								dDefaultVal;						//ȱʡֵ
	double								dMaxVal;							//���ֵ
	double								dMinVal;							//��Сֵ
	double								dStepVal;							//����
} tagCompile_ParamTableInRecord;

typedef struct
{
	char								*	szInBuf;					//ԭ������
	int										iInSize;					//ԭ�ĳ���

	tagCompile_FuncTableInRecord		*	lpFuncRecord;				//������
	int										iFuncCount;					//����������

	tagCompile_ParamTableInRecord		*	lpParamRecord;				//������
	int										iParamCount;				//��������

	double								*	lpRefRecord;				//�ο��߱�
	int										iRefCount;					//�ο�������

	Atom*									pL;							//�߼�

} tagCompile_CompileParamIn;

typedef struct
{
	unsigned char						cID;								//��ʶ��ID��
	char								szName[3];							//��ʶ������
	unsigned char						cPriority;							//���ȼ���
	bool								bIsOperator;						//�Ƿ�Ϊ�����[����Ϊ�ָ����]
	bool								bIsRead;							//�Ƿ�Ϊֱ�Ӷ�ȡ����[����Ϊת�����]
	unsigned char						cParmCount;							//�ò�������������[1��2����1Ԫ��2Ԫ������]
} tagCompile_SymbolTableRecord;

//[���ȼ���]
enum
{
	COMPILE_PRIORITY_ERR = -1,													//����
	COMPILE_PRIORITY_BIG = 1,													//����
	COMPILE_PRIORITY_SMALL = 2,													//С��
	COMPILE_PRIORITY_EQUAL = 3													//����
};

typedef struct
{
	int										iSourceOffset;				//����ʼλ��
	int										iWordLength;				//�ʳ���
} tagCompile_AtomPos;

typedef struct
{
	int										iSourceOffset;				//����ʼλ��
	int										iWordLength;				//�ʳ���
	unsigned char							cType;						//������
} tagCompile_WordInfo;

typedef struct
{
	int										iRow;						//��
	int										iCol;						//��
	char									strError[512];
} tagCompile_ErrorInfo;

enum
{
	COMPILE_EXPRESSION_ORDER = 1,					//˳����䡢�����	,	;
	COMPILE_EXPRESSION_FUNC,						//�������������	,	)
	COMPILE_EXPRESSION_IF,							//if���������		then
	COMPILE_EXPRESSION_WHILE,						//while���������	do
	COMPILE_EXPRESSION_FOR1,						//for���ǰ���֣����	to	downto
	COMPILE_EXPRESSION_FOR2,						//for���󲿷֣����	do
};

typedef struct
{
	int										iSourceCodeRow;				//Դ��������
	int										iParamCount;				//������������
	int										iVariableCount;				//������������
	int										iMaxStackCount;				//����������ջ
	int										iRefCount;					//�����ο�������
	unsigned long							iDataType;					//�������������
	char									cReserved[32];				//����
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
	int									iSourceCodeRow;						//��ӦԴ��������
	unsigned char						ucCodeType;							//��������
	Atom						mParam[COMPILE_MAX_FUNPARAM];		//����
	Atom						mRet;								//����ֵ 
	union
	{
		int								iSymbol;							//������
		int								iCodeAddr;							//��ת��ַ
		struct
		{
			int							iParamCount;						//��������
			char						szFuncName[COMPILE_MAX_IDENTIFYNAME];//��������
		};
	};
} tagCompile_Code;
//..............................................................................................................................
typedef struct
{
	int									iSourceCodeRow;						//��ӦԴ��������
	tagCompile_Func					*	lpFuncPtr;							//����ָ��
	void							*	lpRetValue;							//��������ֵ��ַ
	void							*	lpParam[COMPILE_MAX_FUNPARAM];		//������ַ
	int									iParamCount;						//��������
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
	int										iSourceOffset;				//��ǰԴ����ִ������
	int										iTargetOffset;				//��ǰ�������ִ������
	char									cReserved[32];				//����
} tagCompile_ExecStepParam;
//..............................................................................................................................
typedef struct
{
	char								szParamName[COMPILE_MAX_IDENTIFYNAME];//��������
	double								dDefaultVal;						//ȱʡֵ
	double								dMaxVal;							//���ֵ
	double								dMinVal;							//��Сֵ
	double								dStepVal;							//����
	int									iMemoryAddr;						//������ַ
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
	char								szVarName[COMPILE_MAX_IDENTIFYNAME];//��������
	int									iMemoryAddr;						//�����洢��ַ
	unsigned char						cCalType;							//��������[0 �ֲ�����	1 ȫ�ֱ���]
	unsigned char						cOutputType;						//�������[0 �����		1 ���]
	unsigned char						cLineType;							//��������
	unsigned char						cAlignType;							//ˮƽ����
	unsigned char						cVAlignType;						//��ֱ����
	unsigned long						lColor;								//������ɫ
	unsigned char						cLayer;								//���
	unsigned char						cLineWidth;							//�����߿�
	unsigned char						cMove;								//ƽ�ƿ���
	unsigned char						cPrecis;							//����
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
	char								szFuncName[COMPILE_MAX_IDENTIFYNAME];	//��������
	unsigned char						lpParamType[COMPILE_MAX_FUNPARAM];		//������������
	int									iParamCount;							//��������
	unsigned long						iDataType;								//���������������͡����ڼ���Ȩ�ޡ�
	bool								bSpecialFunc;							//�Ƿ����⺯���������⺯����������ֵΪCreateObject������ֱ����д����ָ��
	unsigned char						ucRetDataType;							//����������������

	union
	{
		tagCompileFunc_CreateObject	*	lpFuncCreate;							//���⺯��������������ָ��
		tagCompile_Func				*	lpFunc;									//һ�㺯������������ָ��
	};

	tagCompile_FuncObjectNode		*	lpFuncObjectNode;						//���⺯������ʵ������
} tagCompile_FuncTableRecord;*/
//..............................................................................................................................
typedef struct
{
	unsigned char						ucDataType;							//��������
	bool							*	lpIsNULL;							//�Ƿ�Ϊ��
	void							*	lpData;								//���ݵ�ַ
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
	char m_szName[COMPILE_MAX_IDENTIFYNAME];	//��ʶ������
	unsigned char m_ucType;						//��ʶ�����͡��μ�����������
	int m_iAddress;								//���ڵ�ַ���������͵�ַ��ʾ��ͬ���塿
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
	//˼���ӿڣ�˼�����ɵ�ִ��Logic������pL��
	//T* Think(T* pL)
	//ִ�нӿڣ�����ִ��pL�߼�,���pLΪ��ΪNULL����ʾ��Logic��ִ��pL�߼����壬�����Logic����ִ���߼���Ԫ
	//bool Execute(T* pL)
};

//////////////////////////////////////////////////////////////////////////
//[�洢��]

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
	const char							*	m_szInBuf;					//����
	int										m_iOffset;					//��ǰ����ƫ��
	int										m_iInSize;					//�������ݳ���
	int										m_iRow;						//��ǰ����
	int										m_iRowOffset;				//��ǰ�е���ʼλ��
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
	int SearchDrawFunc(char * szOutString);//�����ҵ��Ļ�ͼ��������

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