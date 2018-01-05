#ifndef _H_XUTPS_H_
#define _H_XUTPS_H_

#include "XLib.h"
#include "XUtp.h"

#pragma pack(1)

//////////////////////////////////////////////////////////////////////////
//结构化协议：

enum
{
	utp_protocol_heartbeat	= 0,		//心跳
};

struct tagUtpHeader
{
	union
	{
		unsigned int id; //协议唯一标示
		struct
		{
			union
			{
				unsigned short ver; //版本
				struct  
				{
					unsigned char mver; //主版本
					unsigned char cver;	//子版本
				};
			};
			union
			{
				unsigned short type; //类型
				struct  
				{
					unsigned char mtype; //主类型
					unsigned char ctype; //子类型
				};
			};
		};
	};
};

//
//对于err的描述，小于0标示错误，大于0有特殊意义，
//对于多包，表示一共有多少包（FrameCount）和当前第几个包（FrameNo），如下：
//if (err >= 0) {
//	utpheader.err = (unsigned short)FrameCount * 100 + FrameNo;
//} else {
//	utpheader.err = err;
//}
//

enum
{
	utp_flag_none		= 0,			//无
	utp_flag_response	= 0x01,			//回应
	utp_flag_encrypt	= 0x02,			//加密
	utp_flag_compress	= 0x04,			//压缩
};

struct tagUtpFrameHeader
{
	unsigned short reqid;	//请求编号（注意：0--39999为请求编号、40000--50000为推送编号，其他保留）
	short err;				//错误编号（请见上面详细说明）
	unsigned short crc;		//校验码（对数据体进行CRC16校验）
	unsigned short flags:3;	//数据包状态（定义见上说明）
	unsigned short len:13;	//数据体的长度（最长16K）
};

//////////////////////////////////////////////////////////////////////////
//HTTP协议
//使用UDP的HTTP协议，和基于TCP的HTTP协议完全一致

//////////////////////////////////////////////////////////////////////////
//

#pragma pack()

#endif//_H_XUTPS_H_