#ifndef _H_XUTP_H_
#define _H_XUTP_H_

#include "XLib.h"

#pragma pack(1)

//步骤：
//1、连续发送，等待确认或者超时重发
//2、接收，收完所有包，再发送确认包
//3、超时重发或者收到到确认后继续发送后面的包
//4、发送和接收方都超时失败就确认丢包
enum
{
	stable_udpbuf_flag_seq		= 0x01,	//序号
	stable_udpbuf_flag_ack		= 0x02, //序号确认
	stable_udpbuf_flag_retry	= 0x04,	//重试
	//stable_udpbuf_flag_seq 发送序号
	//stable_udpbuf_flag_ack 确认序号
	//stable_udpbuf_flag_seq|stable_udpbuf_flag_retry 重发序号
	//stable_udpbuf_flag_ack|stable_udpbuf_flag_retry 请求重发序号

	stable_udpbuf_header_size	= 8,
};

struct tagUdpBufHeader 
{
	union  
	{
		struct  
		{
			unsigned int nSeq:24; //唯一序号（至少在相对一段时间内能保证唯一）
			unsigned int nFlags:8; //标志
			unsigned short nNum; //数目
			unsigned short nNo; //编号
		};
		char Reserved[stable_udpbuf_header_size];
	};
};

#pragma pack()

#endif//_H_XUTP_H_