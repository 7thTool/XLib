#ifndef _H_FILE_EX_H_
#define _H_FILE_EX_H_

#include "XFile.h"

#pragma pack(1)
// 文件头部
typedef struct tagFileHeader
{
	ULONG			Ver;				// 文件版本
	ULONG			Type;				// 文件类型
} FILEHEADER,*PFILEHEADER;

typedef struct tagIdxFileHeader// : public tagFileHeader
{
	ULONG			Ver;				// 文件版本
	ULONG			Type;				// 文件类型
	ULONG			IdxBlockSz;			// 索引块大小
	ULONG			MaxUserDataSz;		// 最大用户数据大小
} IDXFILEHEADER,*PIDXFILEHEADER;

typedef struct tagIdxFileExExHeader// : public tagFileHeader
{
	ULONG			Ver;				// 文件版本
	ULONG			Type;				// 文件类型
	ULONG			IdxBlockSz;			// 索引块大小
	ULONG			MaxIdxBlockCount;	// 索引段大小(有多少个索引块组成)
	ULONGLONG		IdxAreaOff;			// 索引段起始偏移
	ULONG			DataBlockSz;		// 数据块大小
	ULONG			MaxDataBlockCount;	// 数据区大小(一个数据区能容纳多少个数据块)
	ULONGLONG		DataAreaOff;		// 数据段起始偏移
	ULONG			MaxUserDataSz;		// 最大用户数据大小
} IDXFILEExHEADER,*PIDXFILEExHEADER;

typedef struct tagIdxAreaHeader
{
	union
	{
		struct
		{
			ULONG			TotalIdxBlockCount;	// 总的索引数量
			ULONGLONG		IdxAreaOff;			// 索引段起始偏移
		};
		struct
		{
			ULONG			UseIdxBlockCount;	// 已经用的索引数量
			ULONGLONG		NextIdxAreaOff;		// 下个索引区的偏移
		};
	};
} IDXAREAHEADER,*PIDXAREAHEADER;

// 索引基本信息
typedef struct tagIdxBaseInfo
{	
	ULONG			DataBlockSz;			// 数据块大小
	ULONG			MaxDataBlockCount;		// 数据区大小(一个数据区能容纳多少个数据块)
	ULONG			DataAreaCount;			// 本商品总的数据区数量
	ULONG			TotalDataBlockCount;	// 本商品总的数据块数量
	ULONGLONG		DataAreaOff;			// 本商品首个数据区偏移
} IDXBASEINFO,*PIDXBASEINFO;

// 数据区头部
typedef struct tagDataAreaHeader
{
	ULONG			MaxDataBlockCount;	// 总共能容纳多少个数据块
	ULONG			UseDataBlockCount;	// 已经使用了多少个数据块
	ULONGLONG		NextDataAreaOff;	// 下个数据区的偏移
} DATAAREAHEADER,*PDATAAREAHEADER;

enum
{
	FIND_FLAG_NONE			= 0,
	FIND_FLAG_NEWIFNOTFIND	= 0X01,
	FIND_FLAG_READIDX		= 0X02,
	FIND_FLAG_WRITEIDX		= 0X04,
};

typedef int (*IdxCompareFunc)(void*, void*);	// 比较函数

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