#ifndef _H_FILE_EX_H_
#define _H_FILE_EX_H_

#include "XFile.h"

#pragma pack(1)
// �ļ�ͷ��
typedef struct tagFileHeader
{
	ULONG			Ver;				// �ļ��汾
	ULONG			Type;				// �ļ�����
} FILEHEADER,*PFILEHEADER;

typedef struct tagIdxFileHeader// : public tagFileHeader
{
	ULONG			Ver;				// �ļ��汾
	ULONG			Type;				// �ļ�����
	ULONG			IdxBlockSz;			// �������С
	ULONG			MaxUserDataSz;		// ����û����ݴ�С
} IDXFILEHEADER,*PIDXFILEHEADER;

typedef struct tagIdxFileExExHeader// : public tagFileHeader
{
	ULONG			Ver;				// �ļ��汾
	ULONG			Type;				// �ļ�����
	ULONG			IdxBlockSz;			// �������С
	ULONG			MaxIdxBlockCount;	// �����δ�С(�ж��ٸ����������)
	ULONGLONG		IdxAreaOff;			// ��������ʼƫ��
	ULONG			DataBlockSz;		// ���ݿ��С
	ULONG			MaxDataBlockCount;	// ��������С(һ�������������ɶ��ٸ����ݿ�)
	ULONGLONG		DataAreaOff;		// ���ݶ���ʼƫ��
	ULONG			MaxUserDataSz;		// ����û����ݴ�С
} IDXFILEExHEADER,*PIDXFILEExHEADER;

typedef struct tagIdxAreaHeader
{
	union
	{
		struct
		{
			ULONG			TotalIdxBlockCount;	// �ܵ���������
			ULONGLONG		IdxAreaOff;			// ��������ʼƫ��
		};
		struct
		{
			ULONG			UseIdxBlockCount;	// �Ѿ��õ���������
			ULONGLONG		NextIdxAreaOff;		// �¸���������ƫ��
		};
	};
} IDXAREAHEADER,*PIDXAREAHEADER;

// ����������Ϣ
typedef struct tagIdxBaseInfo
{	
	ULONG			DataBlockSz;			// ���ݿ��С
	ULONG			MaxDataBlockCount;		// ��������С(һ�������������ɶ��ٸ����ݿ�)
	ULONG			DataAreaCount;			// ����Ʒ�ܵ�����������
	ULONG			TotalDataBlockCount;	// ����Ʒ�ܵ����ݿ�����
	ULONGLONG		DataAreaOff;			// ����Ʒ�׸�������ƫ��
} IDXBASEINFO,*PIDXBASEINFO;

// ������ͷ��
typedef struct tagDataAreaHeader
{
	ULONG			MaxDataBlockCount;	// �ܹ������ɶ��ٸ����ݿ�
	ULONG			UseDataBlockCount;	// �Ѿ�ʹ���˶��ٸ����ݿ�
	ULONGLONG		NextDataAreaOff;	// �¸���������ƫ��
} DATAAREAHEADER,*PDATAAREAHEADER;

enum
{
	FIND_FLAG_NONE			= 0,
	FIND_FLAG_NEWIFNOTFIND	= 0X01,
	FIND_FLAG_READIDX		= 0X02,
	FIND_FLAG_WRITEIDX		= 0X04,
};

typedef int (*IdxCompareFunc)(void*, void*);	// �ȽϺ���

#pragma pack()

class XLIB_API IdxFile
{
protected:
	File m_File;
	IDXFILEHEADER m_Header;
	IdxCompareFunc m_IdxCompare;

public:
	IdxFile();
	~IdxFile();

	BOOL Open(LPCTSTR lpszFileName
		, ULONG Ver, ULONG Type, LONG IdxBlockSz
		, IdxCompareFunc IdxCompare = NULL, LONG MaxUserDataSz = 0, BOOL bCreateIfFailed = FALSE);
	
	void Close();

	BOOL IsOpen();

	BOOL ReadUserData(void* pUserData);
	BOOL WriteUserData(void* pUserData);

	BOOL ReadIdx(void* pIdx);
	BOOL WriteIdx(void* pIdx);
	BOOL ReadIdxList(void* pIdxList, ULONG* pCount);
	BOOL WriteIdxList(void* pIdxList, ULONG* pCount);

	ULONGLONG FindIdx(void* pIdx, LONG Flag = FIND_FLAG_NONE);

protected:
	BYTE* m_pIdxBuffer;
	BYTE* AllocIdxBuffer();
	void FreeIdxBuffer();
};

class IdxFileEx : public IdxFile
{
public:
	BOOL Open(LPCTSTR lpszFileName
		, ULONG Ver, ULONG Type, LONG IdxBlockSz
		, IdxCompareFunc IdxCompare = NULL, LONG MaxUserDataSz = 0, BOOL bCreateIfFailed = FALSE);

	BOOL ReadIdx(void* pIdx, IDXBASEINFO* pIdxBase = NULL);
	BOOL WriteIdx(void* pIdx, IDXBASEINFO* pIdxBase = NULL);
	BOOL ReadIdxList(void* pIdxList, ULONG* pCount, BOOL bFullIdx = FALSE);
	BOOL WriteIdxList(void* pIdxList, ULONG* pCount, BOOL bFullIdx = FALSE);

	ULONGLONG FindIdx(void* pIdx, IDXBASEINFO* pIdxBase = NULL, LONG Flag = FIND_FLAG_NONE);

public:
	static BOOL Read(File& DataFile, IDXBASEINFO* pIdx, void* pData, ULONG* pCount);
	static BOOL Write(File& DataFile, IDXBASEINFO* pIdx, void* pData, ULONG* pCount);
};

class XLIB_API DataIdxFile : public IdxFileEx
{
protected:
	LONG m_IdxBlockSz;
	LONG m_DataBlockSz;
	LONG m_MaxDataBlockCount;

public:
	DataIdxFile();
	~DataIdxFile();

	BOOL Open(LPCTSTR lpszFileName, ULONG Ver, ULONG Type
		, LONG IdxBlockSz, LONG DataBlockSz, LONG MaxDataBlockCount
		, IdxCompareFunc IdxCompare = NULL, LONG MaxUserDataSz = 0, BOOL bCreateIfFailed = FALSE);
	BOOL Save();
	void Close();

	BOOL ReadIdx(void* pIdx);
	BOOL WriteIdx(void* pIdx);
	BOOL ReadIdxList(void* pIdxList, ULONG* pCount);
	BOOL WriteIdxList(void* pIdxList, ULONG* pCount);

	BOOL Read(void* pIdx, void* pData, ULONG* pCount, BOOL bReadIdx = TRUE);
	BOOL Write(void* pIdx, void* pData, ULONG* pCount, BOOL bWriteIdx = TRUE);

	ULONGLONG FindIdx(void* pIdx, IDXBASEINFO* pIdxBase = NULL, LONG Flag = FIND_FLAG_NONE);

protected:
	ULONG m_nIdxBlockCount;
	ULONG m_nMaxIdxBlockCount;
	BYTE* AllocIdxBuffer();
	void FreeIdxBuffer();
};

class XLIB_API Idx2DataFile
{
protected:
	IdxFileEx m_IdxFile;
	LONG m_DataBlockSz;
	LONG m_MaxDataBlockCount;
public:
	Idx2DataFile();
	~Idx2DataFile();

	BOOL Open(LPCTSTR lpszIdxFileName, ULONG Ver, ULONG Type
		, LONG IdxBlockSz, LONG DataBlockSz, LONG MaxDataBlockCount
		, IdxCompareFunc IdxCompare = NULL, LONG MaxUserDataSz = 0, BOOL bCreateIfFailed = FALSE);
	void Close();

	BOOL IsOpen();

	BOOL ReadUserData(void* pUserData);
	BOOL WriteUserData(void* pUserData);

	BOOL ReadIdx(void* pIdx);
	BOOL WriteIdx(void* pIdx);
	BOOL ReadIdxList(void* pIdxList, ULONG* pCount);
	BOOL WriteIdxList(void* pIdxList, ULONG* pCount);

	BOOL Read(LPCTSTR lpszDataFile, void* pIdx, void* pData, ULONG* pCount, BOOL bReadIdx = TRUE);
	BOOL Write(LPCTSTR lpszDataFile, void* pIdx, void* pData, ULONG* pCount, BOOL bWriteIdx = TRUE);
};

#endif//_H_FILE_EX_H_