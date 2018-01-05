#ifndef _H_XUTP_H_
#define _H_XUTP_H_

#include "XLib.h"

#pragma pack(1)

//���裺
//1���������ͣ��ȴ�ȷ�ϻ��߳�ʱ�ط�
//2�����գ��������а����ٷ���ȷ�ϰ�
//3����ʱ�ط������յ���ȷ�Ϻ�������ͺ���İ�
//4�����ͺͽ��շ�����ʱʧ�ܾ�ȷ�϶���
enum
{
	stable_udpbuf_flag_seq		= 0x01,	//���
	stable_udpbuf_flag_ack		= 0x02, //���ȷ��
	stable_udpbuf_flag_retry	= 0x04,	//����
	//stable_udpbuf_flag_seq �������
	//stable_udpbuf_flag_ack ȷ�����
	//stable_udpbuf_flag_seq|stable_udpbuf_flag_retry �ط����
	//stable_udpbuf_flag_ack|stable_udpbuf_flag_retry �����ط����

	stable_udpbuf_header_size	= 8,
};

struct tagUdpBufHeader 
{
	union  
	{
		struct  
		{
			unsigned int nSeq:24; //Ψһ��ţ����������һ��ʱ�����ܱ�֤Ψһ��
			unsigned int nFlags:8; //��־
			unsigned short nNum; //��Ŀ
			unsigned short nNo; //���
		};
		char Reserved[stable_udpbuf_header_size];
	};
};

#pragma pack()

#endif//_H_XUTP_H_