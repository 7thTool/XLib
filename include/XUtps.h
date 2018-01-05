#ifndef _H_XUTPS_H_
#define _H_XUTPS_H_

#include "XLib.h"
#include "XUtp.h"

#pragma pack(1)

//////////////////////////////////////////////////////////////////////////
//�ṹ��Э�飺

enum
{
	utp_protocol_heartbeat	= 0,		//����
};

struct tagUtpHeader
{
	union
	{
		unsigned int id; //Э��Ψһ��ʾ
		struct
		{
			union
			{
				unsigned short ver; //�汾
				struct  
				{
					unsigned char mver; //���汾
					unsigned char cver;	//�Ӱ汾
				};
			};
			union
			{
				unsigned short type; //����
				struct  
				{
					unsigned char mtype; //������
					unsigned char ctype; //������
				};
			};
		};
	};
};

//
//����err��������С��0��ʾ���󣬴���0���������壬
//���ڶ������ʾһ���ж��ٰ���FrameCount���͵�ǰ�ڼ�������FrameNo�������£�
//if (err >= 0) {
//	utpheader.err = (unsigned short)FrameCount * 100 + FrameNo;
//} else {
//	utpheader.err = err;
//}
//

enum
{
	utp_flag_none		= 0,			//��
	utp_flag_response	= 0x01,			//��Ӧ
	utp_flag_encrypt	= 0x02,			//����
	utp_flag_compress	= 0x04,			//ѹ��
};

struct tagUtpFrameHeader
{
	unsigned short reqid;	//�����ţ�ע�⣺0--39999Ϊ�����š�40000--50000Ϊ���ͱ�ţ�����������
	short err;				//�����ţ����������ϸ˵����
	unsigned short crc;		//У���루�����������CRC16У�飩
	unsigned short flags:3;	//���ݰ�״̬���������˵����
	unsigned short len:13;	//������ĳ��ȣ��16K��
};

//////////////////////////////////////////////////////////////////////////
//HTTPЭ��
//ʹ��UDP��HTTPЭ�飬�ͻ���TCP��HTTPЭ����ȫһ��

//////////////////////////////////////////////////////////////////////////
//

#pragma pack()

#endif//_H_XUTPS_H_