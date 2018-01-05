#ifndef _H_XSTABLEUDPSOCKETEX_H_
#define _H_XSTABLEUDPSOCKETEX_H_

#include "XUtp.h"
#include "XSocketArchitecture.h"

/*!
 *	@brief StableUdpHandler 定义.
 *
 *	封装StableUdpHandler，实现UDP的稳定可靠传输
 *  
 *  实现类似于TCP协议的超时重传，有序接受，应答确认，滑动窗口流量控制等机制，
 *	使用UDP数据包+序列号，UDP数据包+时间戳，应答确认机制。
 */
template<class SockAddrType = SOCKADDR_IN, u_short uMaxBufSize = 520>
class StableUdpHandler
{
public:
#pragma pack(1)

	enum
	{
		STABLE_UDPBUF_STATE_SEND	= 0X01,	//发送
		STABLE_UDPBUF_STATE_RECEIVE	= 0X02, //接收

		STABLE_UDPBUF_STATE_SENDED	= 0X04,	//确认发送
		STABLE_UDPBUF_STATE_RECEIVED= 0X08,	//确认接收
	};
	typedef struct tagUdpBuf 
	{
		union
		{
			struct  
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
				char data[uMaxBufSize]; //
			};
			char lpBuf[stable_udpbuf_header_size+uMaxBufSize];
		};
		unsigned short nBufLen; //数据长度
		unsigned short nStates; //状态
	}UDPBUF,*PUDPBUF;

	//一开始是这样的内存结构
	//256|{tagUdpBuf}
	//发送接收完成就合并成一块数据
	//256|{char}
	//中间256字节用来存储额外信息
	enum
	{
		stable_udpheader_size = 256,
	};
	typedef struct tagUdpHeader
	{
		union
		{
			struct  
			{
				SockAddrType SockAddr;	//地址
				char* lpBuf;
				int nBufLen;
				unsigned int nFlags;
				unsigned int nSeq;
				unsigned short nNum;
				unsigned long nElapse;
				unsigned char nRetry;
			};
			char Reserved[stable_udpheader_size];
		};
		tagUdpBuf lpUdpBuf[1];
	}UDPHEADER,*PUDPHEADER;

#pragma pack()
protected:
	unsigned int m_nLastSeq;
	virtual unsigned int NewSeq(const SockAddrType & SockAddr) 
	{ 
		return m_nLastSeq++%0x00ffffff;
	}
public:
	StableUdpHandler():m_nLastSeq(0)
	{
		
	}

	virtual tagUdpHeader* SendBuffer(std::string & sBuffer, const char* lpBuf, int nBufLen, const SockAddrType & SockAddr)
	{
		int i;
		char* lpTempBuf = NULL;
		tagUdpHeader* lpTempHeader = NULL;
		tagUdpBuf* lpTempUdpBuf = NULL;
		tagUdpBuf UdpBuf = {0};
		UdpBuf.nSeq = NewSeq(SockAddr); //
		UdpBuf.nFlags = stable_udpbuf_flag_seq;
		UdpBuf.nNum = (nBufLen+(uMaxBufSize-1))/uMaxBufSize;
		UdpBuf.nNo = 0;
		UdpBuf.nBufLen = 0;
		UdpBuf.nStates = 0;
		i = sBuffer.size(); 
		sBuffer.append(stable_udpheader_size+UdpBuf.nNum*sizeof(tagUdpBuf),0);
		lpTempBuf = &sBuffer[i];
		lpTempHeader = (tagUdpHeader*)lpTempBuf;
		lpTempHeader->SockAddr = SockAddr;
		lpTempHeader->lpBuf = (char*)lpBuf;
		lpTempHeader->nBufLen = nBufLen;
		lpTempHeader->nFlags = SOCKET_PACKET_FLAG_SEND;
		lpTempHeader->nSeq = UdpBuf.nSeq;
		lpTempHeader->nNum = UdpBuf.nNum;
		lpTempHeader->nElapse = 0;
		lpTempHeader->nRetry = 0;
		lpTempUdpBuf = (tagUdpBuf*)(lpTempHeader->lpUdpBuf);
		for (;UdpBuf.nNo<UdpBuf.nNum;++UdpBuf.nNo)
		{
			lpTempUdpBuf[UdpBuf.nNo] = UdpBuf;
			int len = min(nBufLen-UdpBuf.nNo*uMaxBufSize,uMaxBufSize);
			memcpy(lpTempUdpBuf[UdpBuf.nNo].data,lpBuf+UdpBuf.nNo*uMaxBufSize,len);
			lpTempUdpBuf[UdpBuf.nNo].nBufLen = stable_udpbuf_header_size + len;
		}
		return lpTempHeader;
	}

	virtual tagUdpHeader* SendBuffer(std::string & sBuffer, unsigned int nFlags, const SockAddrType & SockAddr)
	{
		if (!(nFlags&(stable_udpbuf_flag_ack|stable_udpbuf_flag_retry))) {
			return NULL;
		}
		int i;
		char* lpTempBuf = NULL;
		tagUdpHeader* lpTempHeader = NULL;
		tagUdpBuf* lpTempUdpBuf = NULL;
		tagUdpBuf UdpBuf = {0};
		UdpBuf.nSeq = NewSeq(SockAddr); //
		UdpBuf.nFlags = nFlags;
		UdpBuf.nNum = 1;
		UdpBuf.nNo = 0;
		UdpBuf.nBufLen = stable_udpbuf_header_size;
		UdpBuf.nStates = 0;
		i = sBuffer.size(); 
		sBuffer.append(stable_udpheader_size+UdpBuf.nNum*sizeof(tagUdpBuf),0);
		lpTempBuf = &sBuffer[i];
		lpTempHeader = (tagUdpHeader*)lpTempBuf;
		lpTempHeader->SockAddr = SockAddr;
		lpTempHeader->lpBuf = NULL;
		lpTempHeader->nBufLen = 0;
		lpTempHeader->nFlags = SOCKET_PACKET_FLAG_SEND;
		lpTempHeader->nSeq = UdpBuf.nSeq;
		lpTempHeader->nNum = UdpBuf.nNum;
		lpTempHeader->nElapse = 0;
		lpTempHeader->nRetry = 0;
		lpTempUdpBuf = (tagUdpBuf*)(lpTempHeader->lpUdpBuf);
		lpTempUdpBuf[UdpBuf.nNo] = UdpBuf;
		return lpTempHeader;
	}

	virtual bool PrepareSendBuffer(std::string & sBuffer, char* lpBuf, int & nBufLen, SockAddrType & SockAddr)
	{
		if (sBuffer.empty()) {
			return false;
		}
		bool bPrepare = false;
		int i;
		char* lpTempBuf = NULL;
		char* lpTempEndBuf = NULL;
		tagUdpHeader* lpTempHeader = NULL;
		tagUdpBuf* lpTempUdpBuf = NULL;
		lpTempBuf = &sBuffer[0];
		lpTempEndBuf = lpTempBuf+sBuffer.size();
		do {
			lpTempHeader = (tagUdpHeader*)lpTempBuf;
			lpTempUdpBuf = (tagUdpBuf*)(lpTempHeader->lpUdpBuf);
			if (lpTempHeader->nFlags&SOCKET_PACKET_FLAG_SEND) {
				ASSERT(!(lpTempHeader->nFlags&SOCKET_PACKET_FLAG_COMPLETE));
				for (i=0;i<lpTempHeader->nNum;i++)
				{
					if (lpTempUdpBuf[i].nStates==0) {
						if (!lpTempHeader->nElapse) {
							lpTempHeader->nElapse = GetTickCount();
						}
						memcpy(lpBuf,lpTempUdpBuf[i].lpBuf,lpTempUdpBuf[i].nBufLen);
						nBufLen = lpTempUdpBuf[i].nBufLen;
						SockAddr = lpTempHeader->SockAddr;
						lpTempUdpBuf[i].nStates = STABLE_UDPBUF_STATE_SEND;
						bPrepare = true;
						break;
					}
				}
			} else if (lpTempHeader->nFlags&SOCKET_PACKET_FLAG_RECEIVE) {
				//发送回应包
				if(lpTempHeader->nFlags&SOCKET_PACKET_FLAG_COMPLETE) {
					//一次确认
					tagUdpBuf UdpBuf = {0};
					UdpBuf.nSeq = lpTempHeader->nSeq; //
					UdpBuf.nFlags = stable_udpbuf_flag_ack;
					UdpBuf.nNum = lpTempHeader->nNum;
					UdpBuf.nNo = lpTempHeader->nNum;
					UdpBuf.nBufLen = stable_udpbuf_header_size;
					UdpBuf.nStates = 0;
					memcpy(lpBuf,UdpBuf.lpBuf,UdpBuf.nBufLen);
					nBufLen = UdpBuf.nBufLen;
					SockAddr = lpTempHeader->SockAddr;
					UdpBuf.nStates = STABLE_UDPBUF_STATE_SEND;
					bPrepare = true;
				} 
			} else {
				ASSERT(0);
			}
			if (bPrepare) {
				break;
			}
			lpTempBuf += (stable_udpheader_size+lpTempHeader->nNum*sizeof(tagUdpBuf));
			lpTempHeader = NULL;
			lpTempUdpBuf = NULL;
		}while (lpTempBuf<lpTempEndBuf);
		if (bPrepare) {
			//
		}
		return bPrepare;
	}

	virtual tagUdpHeader* CheckBuffer(std::string & sBuffer, int nMaxElpase = 3*1000)
	{
		if (sBuffer.empty()) {
			return NULL;
		}
		int i;
		char* lpTempBuf = NULL;
		char* lpTempEndBuf = NULL;
		tagUdpHeader* lpTempHeader = NULL;
		tagUdpBuf* lpTempUdpBuf = NULL;
		lpTempBuf = &sBuffer[0];
		lpTempEndBuf = lpTempBuf+sBuffer.size();
		do {
			lpTempHeader = (tagUdpHeader*)lpTempBuf;
			lpTempUdpBuf = (tagUdpBuf*)(lpTempHeader->lpUdpBuf);
			if(!(lpTempHeader->nFlags&(SOCKET_PACKET_FLAG_COMPLETE)) && lpTempHeader->nElapse) {
				unsigned long nElapse = GetTickCount();
				nElapse -= lpTempHeader->nElapse; //这里不用担心1-4294967295的情况，实际上1-4294967295=2
				if (lpTempHeader->nFlags&SOCKET_PACKET_FLAG_SEND) {
					//发送端1/2超时时主动重发一次
					if (nElapse>=nMaxElpase/2) {
						if (!lpTempHeader->nRetry) {
							lpTempHeader->nRetry++;
							for (i=0;i<lpTempHeader->nNum;i++)
							{
								if (lpTempUdpBuf[i].nStates) {
									lpTempUdpBuf[i].nFlags |= stable_udpbuf_flag_retry;
									lpTempUdpBuf[i].nStates = 0;
								}
							}
						}
						lpTempHeader = NULL;
						lpTempUdpBuf = NULL;
						break;
					}
				} else if(lpTempHeader->nFlags&SOCKET_PACKET_FLAG_RECEIVE) {
					//
				} else {
					ASSERT(0);
				}
				if (nElapse>nMaxElpase) {
					break;
				}
			}
			lpTempBuf += (stable_udpheader_size+lpTempHeader->nNum*sizeof(tagUdpBuf));
			lpTempHeader = NULL;
			lpTempUdpBuf = NULL;
		}while (lpTempBuf<lpTempEndBuf);
		return lpTempHeader;
	}

	virtual bool IsUdpBuf(const char* lpBuf, int nBufLen, const SockAddrType & SockAddr)
	{
		if (!lpBuf) {
			return false;
		}
		if (nBufLen<stable_udpbuf_header_size) {
			return false;
		}
		if (nBufLen>(stable_udpbuf_header_size+uMaxBufSize)) {
			return false;
		}
		const tagUdpBuf* lpUdpBuf = (const tagUdpBuf*)lpBuf;
		if (!(lpUdpBuf->nFlags&(stable_udpbuf_flag_seq|stable_udpbuf_flag_ack))) {
			return false;
		}
		if (lpUdpBuf->nNo>lpUdpBuf->nNum) {
			return false;
		}
		if (lpUdpBuf->nNo<(lpUdpBuf->nNum-1)) {
			if ((nBufLen-stable_udpbuf_header_size)!=uMaxBufSize) {
				return false;
			}
		}
		return true;
	}

	virtual tagUdpHeader* OnReceivedBuffer(std::string & sBuffer, const char* lpBuf, int nBufLen, const SockAddrType & SockAddr)
	{
		int i,j;
		char* lpTempBuf = NULL;
		char* lpTempEndBuf = NULL;
		tagUdpBuf* lpUdpBuf = (tagUdpBuf*)lpBuf;
		tagUdpHeader* lpTempHeader = NULL;
		tagUdpBuf* lpTempUdpBuf = NULL;
		if (lpUdpBuf->nFlags&stable_udpbuf_flag_ack) {
			//回应
			if (sBuffer.empty()) {
				return NULL;
			}
			lpTempBuf = &sBuffer[0];
			lpTempEndBuf = lpTempBuf+sBuffer.size();
			do {
				lpTempHeader = (tagUdpHeader*)lpTempBuf;
				if (!(lpTempHeader->nFlags&SOCKET_PACKET_FLAG_COMPLETE) && lpTempHeader->nSeq==lpUdpBuf->nSeq && lpTempHeader->nNum==lpTempHeader->nNum && memcmp(&lpTempHeader->SockAddr,&SockAddr,sizeof(SockAddrType))==0) {
					lpTempUdpBuf = (tagUdpBuf*)(lpTempHeader->lpUdpBuf);
					for (i=0;i<lpTempUdpBuf->nNum;i++)
					{
						if (lpTempHeader->nNum==lpUdpBuf->nNo) {
							//一次确认
							lpTempUdpBuf[i].nStates = STABLE_UDPBUF_STATE_SEND|STABLE_UDPBUF_STATE_SENDED;
						} else {
							if (lpTempUdpBuf[i].nNo==lpUdpBuf->nNo) {
								//if (lpUdpBuf->nFlags&stable_udpbuf_flag_retry) {
								//	if (lpTempUdpBuf[i].nStates&STABLE_UDPBUF_STATE_SEND) {
								//		lpTempUdpBuf[i].nStates = 0;
								//	}
								//	break;
								//} else {
									if (lpTempUdpBuf[i].nStates&STABLE_UDPBUF_STATE_SEND) {
										lpTempUdpBuf[i].nStates = STABLE_UDPBUF_STATE_SENDED;
									}
								//}
							}
						}
					}
					if (lpTempHeader->nFlags&SOCKET_PACKET_FLAG_SEND) {
						for (i=0;i<lpTempUdpBuf->nNum;i++)
						{
							if (!(lpTempUdpBuf[i].nStates&STABLE_UDPBUF_STATE_SENDED)) {
								break;
							}
						}
						if (i>=lpTempUdpBuf->nNum) {
							lpTempHeader->nFlags |= SOCKET_PACKET_FLAG_COMPLETE;
						}
					}/* else if (lpTempHeader->nFlags&SOCKET_PACKET_FLAG_RECEIVE) { //二次确认才有STABLE_UDPBUF_STATE_RECEIVED
					 for (i=0;i<lpTempUdpBuf->nNum;i++)
					 {
					 if (!(lpTempUdpBuf[i].nStates&STABLE_UDPBUF_STATE_RECEIVED)) {
					 break;
					 }
					 }
					 if (i>=lpTempUdpBuf->nNum) {
					 lpTempHeader->lpBuf = (char*)(lpTempHeader->lpUdpBuf);
					 lpTempHeader->nBufLen = 0;
					 for (i=0;i<lpTempHeader->nNum;i++)
					 {
					 j = lpTempUdpBuf[i].nBufLen-stable_udpbuf_header_size;
					 ASSERT(j>=0&&j<uMaxBufSize);
					 memmove(lpTempHeader->lpBuf+lpTempHeader->nBufLen,lpTempUdpBuf[i].data,j);
					 lpTempHeader->nBufLen += j;
					 }
					 lpTempHeader->nFlags |= SOCKET_PACKET_FLAG_COMPLETE;
					 }
					 } */else {
						 ASSERT(0);
					}
					break;
				}
				lpTempBuf += (stable_udpheader_size+lpTempHeader->nNum*sizeof(tagUdpBuf));
				lpTempHeader = NULL;
				lpTempUdpBuf = NULL;
			}while (lpTempBuf<lpTempEndBuf);
		} else if (lpUdpBuf->nFlags&stable_udpbuf_flag_seq) {
			if (!sBuffer.empty()) {
				lpTempBuf = &sBuffer[0];
				lpTempEndBuf = lpTempBuf+sBuffer.size();
				do
				{
					lpTempHeader = (tagUdpHeader*)lpTempBuf;
					if (lpTempHeader->nSeq==lpUdpBuf->nSeq && lpTempHeader->nNum==lpTempHeader->nNum && memcmp(&lpTempHeader->SockAddr,&SockAddr,sizeof(SockAddrType))==0) {
						lpTempUdpBuf = (tagUdpBuf*)(lpTempHeader->lpUdpBuf);
						break;
					}
					lpTempBuf += (stable_udpheader_size+lpTempHeader->nNum*sizeof(tagUdpBuf));
					lpTempHeader = NULL;
					lpTempUdpBuf = NULL;
				}while (lpTempBuf<lpTempEndBuf);
			}
			if(!lpTempHeader && !(lpUdpBuf->nFlags&stable_udpbuf_flag_retry)) {
				tagUdpBuf UdpBuf = {0};
				UdpBuf.nSeq = lpUdpBuf->nSeq;
				UdpBuf.nFlags = 0;
				UdpBuf.nNum = lpUdpBuf->nNum;
				UdpBuf.nNo = 0;
				UdpBuf.nBufLen = 0;
				UdpBuf.nStates = 0;
				i = sBuffer.size(); 
				sBuffer.append(stable_udpheader_size+UdpBuf.nNum*sizeof(tagUdpBuf),0);
				lpTempHeader = (tagUdpHeader*)&sBuffer[i];
				lpTempHeader->SockAddr = SockAddr;
				lpTempHeader->lpBuf = NULL;
				lpTempHeader->nBufLen = 0;
				lpTempHeader->nFlags = SOCKET_PACKET_FLAG_RECEIVE;
				lpTempHeader->nSeq = UdpBuf.nSeq;
				lpTempHeader->nNum = UdpBuf.nNum;
				lpTempHeader->nElapse = GetTickCount();
				lpTempHeader->nRetry = 0;
				lpTempUdpBuf = (tagUdpBuf*)(lpTempHeader->lpUdpBuf);
				for (;UdpBuf.nNo<UdpBuf.nNum;++UdpBuf.nNo)
				{
					lpTempUdpBuf[UdpBuf.nNo] = UdpBuf;
				}
			}
			if (lpTempHeader) {
				memcpy(&lpTempUdpBuf[lpUdpBuf->nNo].lpBuf,lpBuf,nBufLen);
				lpTempUdpBuf[lpUdpBuf->nNo].nBufLen = nBufLen;
				lpTempUdpBuf[lpUdpBuf->nNo].nStates = STABLE_UDPBUF_STATE_RECEIVE;
				ASSERT((lpTempUdpBuf[lpUdpBuf->nNo].nNo==(lpTempHeader->nNum-1)) || (lpTempUdpBuf[lpUdpBuf->nNo].nBufLen==(uMaxBufSize+stable_udpbuf_header_size)));

				if (lpTempHeader->nFlags&SOCKET_PACKET_FLAG_RECEIVE) { //一次确认只有STABLE_UDPBUF_STATE_RECEIVE
					for (i=0;i<lpTempUdpBuf->nNum;i++)
					{
						if (!(lpTempUdpBuf[i].nStates&STABLE_UDPBUF_STATE_RECEIVE)) {
							break;
						}
					}
					if (i>=lpTempUdpBuf->nNum) {
						/*lpTempHeader->lpBuf = (char*)(lpTempHeader->lpUdpBuf);
						lpTempHeader->nBufLen = 0;
						for (i=0;i<lpTempHeader->nNum;i++)
						{
							j = lpTempUdpBuf[i].nBufLen-stable_udpbuf_header_size;
							ASSERT(j>=0&&j<uMaxBufSize);
							memmove(lpTempHeader->lpBuf+lpTempHeader->nBufLen,lpTempUdpBuf[i].data,j);
							lpTempHeader->nBufLen += j;
						}*/
						lpTempHeader->nFlags |= SOCKET_PACKET_FLAG_COMPLETE;
					}
				}
			}
		}
		return lpTempHeader;
	}

	virtual tagUdpHeader* ReceiveBuffer(std::string & sBuffer)
	{
		if (sBuffer.empty()) {
			return NULL;
		}
		int i;
		char* lpTempBuf = NULL;
		char* lpTempEndBuf = NULL;
		tagUdpHeader* lpTempHeader = NULL;
		tagUdpBuf* lpTempUdpBuf = NULL;
		lpTempBuf = &sBuffer[0];
		lpTempEndBuf = lpTempBuf+sBuffer.size();
		do {
			lpTempHeader = (tagUdpHeader*)lpTempBuf;
			lpTempUdpBuf = (tagUdpBuf*)(lpTempHeader->lpUdpBuf);
			if (lpTempHeader->nFlags&SOCKET_PACKET_FLAG_RECEIVE) {
				if(lpTempHeader->nFlags&SOCKET_PACKET_FLAG_COMPLETE) {
					break;
				}
			} else {
				ASSERT(0);
			}
			lpTempBuf += (stable_udpheader_size+lpTempHeader->nNum*sizeof(tagUdpBuf));
			lpTempHeader = NULL;
			lpTempUdpBuf = NULL;
		}while (lpTempBuf<lpTempEndBuf);
		return lpTempHeader;
	}

	virtual tagUdpHeader* ReceiveBuffer(std::string & sBuffer, const SockAddrType & SockAddr)
	{
		if (sBuffer.empty()) {
			return NULL;
		}
		int i;
		char* lpTempBuf = NULL;
		char* lpTempEndBuf = NULL;
		tagUdpHeader* lpTempHeader = NULL;
		tagUdpBuf* lpTempUdpBuf = NULL;
		lpTempBuf = &sBuffer[0];
		lpTempEndBuf = lpTempBuf+sBuffer.size();
		do {
			lpTempHeader = (tagUdpHeader*)lpTempBuf;
			lpTempUdpBuf = (tagUdpBuf*)(lpTempHeader->lpUdpBuf);
			if (lpTempHeader->nFlags&SOCKET_PACKET_FLAG_RECEIVE) {
				if(lpTempHeader->nFlags&SOCKET_PACKET_FLAG_COMPLETE) {
					if (memcmp(&lpTempHeader->SockAddr,&SockAddr,sizeof(SockAddrType))==0) {
						break;
					}
				}
			} else {
				ASSERT(0);
			}
			lpTempBuf += (stable_udpheader_size+lpTempHeader->nNum*sizeof(tagUdpBuf));
			lpTempHeader = NULL;
			lpTempUdpBuf = NULL;
		}while (lpTempBuf<lpTempEndBuf);
		return lpTempHeader;
	}

	virtual void RemoveBuffer(std::string & sBuffer, tagUdpHeader* lpUdpHeader)
	{
		if (!lpUdpHeader) {
			return;
		}
		if (sBuffer.empty()) {
			return;
		}
		char* lpTempBuf = &sBuffer[0];
		char* lpTempEndBuf = lpTempBuf+sBuffer.size();
		char* lpTempUdpBuf = (char*)lpUdpHeader;
		char* lpTempUdpEndBuf = lpTempUdpBuf+(stable_udpheader_size+lpUdpHeader->nNum*sizeof(tagUdpBuf));
		if (lpTempUdpBuf>=lpTempBuf&&lpTempUdpEndBuf<=lpTempEndBuf) {
			sBuffer.erase(sBuffer.begin()+(lpTempUdpBuf-lpTempBuf),sBuffer.begin()+(lpTempUdpEndBuf-lpTempBuf));
		}
	}
};

/*!
 *	@brief StableUdpSocketArchitecture 定义.
 *
 *	封装StableUdpSocketArchitecture，定义基于UDP的稳定可靠传输的网络架构
 *  
 *  实现类似于TCP协议的超时重传，有序接受，应答确认，滑动窗口流量控制等机制，
 *	使用UDP数据包+序列号，UDP数据包+时间戳，应答确认机制。
 */
template<class TBase = SocketEx, class SockAddrType = SOCKADDR_IN, u_short uMaxBufSize = 520>
class StableUdpSocketArchitecture : public SampleUdpSocketArchitecture<TBase,SockAddrType>
{
	typedef SampleUdpSocketArchitecture<TBase,SockAddrType> Base;
protected:
	//
public:
	StableUdpSocketArchitecture()
		:Base()
	{
		
	}

	virtual ~StableUdpSocketArchitecture()
	{
		
	}

	int Close()
	{
		int rlt = Base::Close();
		return rlt;
	}

protected:
	//
	//解析接收数据包
	virtual int ParseRecvBuf(const char* lpBuf, int & nBufLen, const SockAddrType & SockAddr) { return SOCKET_PACKET_FLAG_RECEIVE|SOCKET_PACKET_FLAG_COMPLETE; }

	//准备接收缓存
	virtual bool PrepareRecvBuf(char* & lpBuf, int & nBufLen, SockAddrType & SockAddr)
	{
		ASSERT(0);
		return false;
	}

	//接收包超时
	virtual void OnReceiveBufTimeOut(const char* lpBuf, int nBufLen, const SockAddrType & SockAddr, int nFlags)
	{

	}

	//接收完整一个包
	virtual void OnRecvBuf(const char* lpBuf, int nBufLen, const SockAddrType & SockAddr, int nFlags)
	{

	}

	//解析发送数据包
	virtual int ParseSendBuf(const char* lpBuf, int & nBufLen, const SockAddrType & SockAddr) { return SOCKET_PACKET_FLAG_SEND|SOCKET_PACKET_FLAG_COMPLETE; }

	//准备发送数据包
	virtual bool PrepareSendBuf(char* lpBuf, int & nBufLen, SockAddrType & SockAddr)
	{
		ASSERT(0);
		return false;
	}

	//发送包超时
	virtual void OnSendBufTimeOut(const char* lpBuf, int nBufLen, const SockAddrType & SockAddr, int nFlags)
	{

	}

	//发送完整一个包
	virtual void OnSendBuf(const char* lpBuf, int nBufLen, const SockAddrType & SockAddr, int nFlags)
	{

	}

protected:
	//
	virtual void OnIdle(int nErrorCode)
	{
		Base::OnIdle(nErrorCode);
	}

	virtual void OnReceive(int nErrorCode)
	{
		if (nErrorCode) {
			Base::OnReceive(nErrorCode);
			return;
		}

		bool bConitnue = false;
		do {
			bConitnue = false;
			//UDP 保证一次接收一个完整UDP包
			char lpBuf[stable_udpbuf_header_size+uMaxBufSize+1] = {0};
			int nBufLen = stable_udpbuf_header_size+uMaxBufSize+1;
			SockAddrType SockAddr;
			int nSockAddrLen = sizeof(SockAddrType);
			nBufLen = Base::ReceiveFrom(lpBuf,nBufLen,(SOCKADDR*)&SockAddr,&nSockAddrLen);
			if (nBufLen<=0) {
				nErrorCode = GetLastError();
				switch(nErrorCode)
				{
				case EWOULDBLOCK:
					break;
#ifdef WIN32
				case WSA_IO_PENDING:
					break;
				case WSAEMSGSIZE:
					//非法大包
					break;
#else
				case EINTR:
					bConitnue = true;
					break;
#endif//
				default:
					OnClose(nErrorCode);
					break;
				}
			} else {
				//bConitnue = true;
			}
		} while(bConitnue);
	}

	virtual void OnReceivedFrom(const char* lpBuf, int nBufLen, const SOCKADDR* lpSockAddr, int nSockAddrLen, int nFlags) 
	{
		
	}

	virtual void OnSend(int nErrorCode)
	{
		if (nErrorCode) {
			Base::OnSend(nErrorCode);
			return;
		}
		bool bConitnue = false;
		do {
			bConitnue = false;
			char Buf[stable_udpbuf_header_size+uMaxBufSize+1] = {0};
			char* lpBuf = Buf;
			int nBufLen = stable_udpbuf_header_size+uMaxBufSize+1;
			SockAddrType SockAddr;
			ASSERT(!m_pSendBuf);
			if(!PrepareSendBuf(lpBuf,nBufLen,SockAddr)) {
				//说明没有可发送数据
				RemoveEvent(FD_WRITE);
				return;
			}
			m_nSendLen = 0;
			m_pSendBuf = lpBuf;
			m_nSendBufLen = nBufLen;
			m_SendAddr = SockAddr;
			ASSERT(lpBuf && nBufLen>0);
#if 0
			PRINTF("echo:(%s:%d)\n",N2Ip(SockAddr.sin_addr.s_addr),N2H(lpSockAddr.sin_port));
#endif//
			nBufLen = Base::SendTo(lpBuf,nBufLen,(const SOCKADDR*)&SockAddr,sizeof(SockAddrType));
			if (nBufLen<=0) {
				nErrorCode = GetLastError();
				switch(nErrorCode)
				{
				case EWOULDBLOCK:
					break;
#ifdef WIN32
				case WSA_IO_PENDING:
					break;
#else
				case EINTR:
					break;
#endif//
				default:
					OnClose(nErrorCode);
					break;
				}
			} else {
				//bConitnue = true; //继续发送
			}
		} while(bConitnue);
	}

	virtual void OnSendedTo(const char* lpBuf, int nBufLen, const SOCKADDR* lpSockAddr, int nSockAddrLen, int nFlags) 
	{
		ASSERT(nSockAddrLen==sizeof(SockAddrType));
		m_nSendLen += nBufLen;
		ASSERT(m_nSendLen>=m_nSendBufLen);
		m_nSendLen = 0;
		m_pSendBuf = NULL;
		m_nSendBufLen = 0;
		memset(&m_SendAddr,0,sizeof(SockAddrType));
	}
};

/*!
 *	@brief StableUdpSocketArchitectureImpl 定义.
 *
 *	封装StableUdpSocketArchitectureImpl，实现稳定的Udp数据包网络架构
 */
template<class TBase = SocketEx, class SockAddrType = SOCKADDR_IN, u_short uMaxBufSize = 520>
class StableUdpSocketArchitectureImpl : public TBase, StableUdpHandler<SockAddrType,uMaxBufSize>
{
	typedef TBase Base;
	//typedef StableUdpHandler<SockAddrType,uMaxBufSize> StableHandler;
protected:
	std::string m_SendBuffer;
	std::string m_RecvBuffer;
	CriticalSection m_SendSection;
	CriticalSection m_RecvSection;
public:
	StableUdpSocketArchitectureImpl()
		:Base()
	{
		m_SendBuffer.reserve(1024*1024);
		m_RecvBuffer.reserve(1024*1024);
	}

	virtual ~StableUdpSocketArchitectureImpl()
	{
		
	}

	int Close()
	{
		int rlt = Base::Close();

		SectionLocker LockSend(&m_SendSection);

		m_SendBuffer.clear();
		LockSend.Detach();

		SectionLocker LockRecv(&m_RecvSection);

		m_RecvBuffer.clear();
		LockRecv.Detach();

		return rlt;
	}

	int Send(const char* lpBuf, int nBufLen, const SockAddrType & SockAddr, int nFlags = 0)
	{
		SectionLocker Lock(&m_SendSection);

		if(SendBuffer(m_SendBuffer, lpBuf, nBufLen, SockAddr)) {
			Trigger(FD_WRITE);
			return nBufLen;
		}
		return 0;
	}

	int Receive(char* lpBuf, int nBufLen, SockAddrType* lpSockAddr, int* nFlags = NULL)
	{
		SectionLocker Lock(&m_RecvSection);
		ASSERT(nFlags);
		int i,j;
		tagUdpHeader* lpTempHeader = ReceiveBuffer(m_RecvBuffer);
		if (lpTempHeader) {
			*nFlags = lpTempHeader->nFlags;
			if (lpSockAddr) {
				*lpSockAddr = lpTempHeader->SockAddr;
			}
			int nPackLen = (lpTempHeader->nNum-1)*uMaxBufSize+(lpTempHeader->lpUdpBuf[lpTempHeader->nNum-1].nBufLen-stable_udpbuf_header_size);
			if (nBufLen>=nPackLen) {
				nBufLen = 0;
				for (i=0;i<lpTempHeader->nNum;i++)
				{
					if (i==(lpTempHeader->nNum-1)) {
						j = (lpTempHeader->lpUdpBuf[i].nBufLen-stable_udpbuf_header_size);
					} else {
						j = uMaxBufSize;
					}
					memcpy(lpBuf+nBufLen,lpTempHeader->lpUdpBuf[i].data,j);
					nBufLen += j;
				}
				RemoveBuffer(m_RecvBuffer,lpTempHeader);
			}
			return nPackLen;
		}
		return 0;
	}

protected:
	//
	
	//准备接收缓存
	virtual bool PrepareRecvBuf(char* & lpBuf, int & nBufLen, SockAddrType & SockAddr)
	{
		ASSERT(0);
		return false;
	}

	//接收包超时，相当于和SockAddr的通讯中断，可以当做OnClose(SockAddr)处理
	virtual void OnReceiveBufTimeOut(const char* lpBuf, int nBufLen, const SockAddrType & SockAddr, int nFlags)
	{
		PRINTF("recv timeout\n");
	}

	//接收完整一个包，lpBuf/nBufLen不一定有效
	virtual void OnRecvBuf(const char* lpBuf, int nBufLen, const SockAddrType & SockAddr, int nFlags)
	{
		
	}

	//准备发送数据包
	virtual bool PrepareSendBuf(char* lpBuf, int & nBufLen, SockAddrType & SockAddr)
	{
		bool bPrepare = false;

		SectionLocker LockRecv(&m_RecvSection);

		bPrepare = PrepareSendBuffer(m_RecvBuffer,lpBuf,nBufLen,SockAddr);
		LockRecv.Detach();
		if (bPrepare) {
			return bPrepare;
		}

		SectionLocker LockSend(&m_SendSection);

		bPrepare = PrepareSendBuffer(m_SendBuffer,lpBuf,nBufLen,SockAddr);
		LockSend.Detach();

		return bPrepare;
		//return false;
	}

	//发送包超时，相当于和SockAddr的通讯中断，可以当做OnClose(SockAddr)处理
	virtual void OnSendBufTimeOut(const char* lpBuf, int nBufLen, const SockAddrType & SockAddr, int nFlags)
	{
		PRINTF("send timeout\n");
	}

	//发送完整一个包
	virtual void OnSendBuf(const char* lpBuf, int nBufLen, const SockAddrType & SockAddr, int nFlags)
	{
		
	}

protected:
	//
	virtual void OnIdle(int nErrorCode)
	{
		Base::OnIdle(nErrorCode);

		tagUdpHeader* lpUdpHeader = NULL;

		SectionLocker LockSend(&m_SendSection);

		lpUdpHeader = CheckBuffer(m_SendBuffer);
		if (lpUdpHeader) {
			OnSendBufTimeOut(lpUdpHeader->lpBuf,lpUdpHeader->nBufLen,lpUdpHeader->SockAddr,lpUdpHeader->nFlags);
			RemoveBuffer(m_SendBuffer,lpUdpHeader);
		}
		LockSend.Detach();
		
		SectionLocker LockRecv(&m_RecvSection);

		lpUdpHeader = CheckBuffer(m_RecvBuffer);
		if (lpUdpHeader) {
			OnReceiveBufTimeOut(lpUdpHeader->lpBuf,lpUdpHeader->nBufLen,lpUdpHeader->SockAddr,lpUdpHeader->nFlags);
			RemoveBuffer(m_RecvBuffer,lpUdpHeader);
		}
		LockRecv.Detach();
	}

	virtual void OnReceivedFrom(const char* lpBuf, int nBufLen, const SOCKADDR* lpSockAddr, int nSockAddrLen, int nFlags)
	{
		Base::OnReceivedFrom(lpBuf, nBufLen, lpSockAddr, nSockAddrLen, nFlags);

		SockAddrType SockAddr = *(SockAddrType*)lpSockAddr;

		//int i;
		if (!IsUdpBuf(lpBuf,nBufLen,SockAddr)) {
			return;
		}
		tagUdpBuf* lpUdpBuf = (tagUdpBuf*)lpBuf;
		tagUdpHeader* lpUdpHeader = NULL;
		if (lpUdpBuf->nFlags&stable_udpbuf_flag_ack) {
			SectionLocker LockSend(&m_SendSection);

			lpUdpHeader = OnReceivedBuffer(m_SendBuffer,lpBuf,nBufLen,SockAddr);
			if (lpUdpHeader) {
				if (lpUdpHeader->nFlags&SOCKET_PACKET_FLAG_COMPLETE) {
					OnSendBuf(lpUdpHeader->lpBuf,lpUdpHeader->nBufLen,lpUdpHeader->SockAddr,lpUdpHeader->nFlags);
					RemoveBuffer(m_SendBuffer,lpUdpHeader);
				}
			}
			LockSend.Detach();
		} else {
			SectionLocker LockRecv(&m_RecvSection);

			lpUdpHeader = OnReceivedBuffer(m_RecvBuffer,lpBuf,nBufLen,SockAddr);
			if (lpUdpHeader) {
				if (lpUdpBuf->nFlags&stable_udpbuf_flag_ack) {
					ASSERT(0);
				} else {
					if (lpUdpHeader->nFlags&SOCKET_PACKET_FLAG_COMPLETE) {
						//SendBuffer(m_SendBuffer,stable_udpbuf_flag_ack,SockAddr);
						OnRecvBuf(lpUdpHeader->lpBuf,lpUdpHeader->nBufLen,lpUdpHeader->SockAddr,lpUdpHeader->nFlags);
						//RemoveBuffer(m_RecvBuffer,lpUdpHeader);
					}
				}
			}
			LockRecv.Detach();
		}
	}

	virtual void OnSendedTo(const char* lpBuf, int nBufLen, const SOCKADDR* lpSockAddr, int nSockAddrLen, int nFlags)
	{
		Base::OnSendedTo(lpBuf, nBufLen, lpSockAddr, nSockAddrLen, nFlags);

		/*SockAddrType SockAddr = *(SockAddrType*)lpSockAddr;

		int i;
		tagUdpBuf* lpUdpBuf = lpBuf;
		if (lpUdpBuf->nFlags&stable_udpbuf_flag_ack) {

		} else {

		}*/
	}
};

/*!
 *	@brief UdpPeer 定义.
 *
 *	封装UdpPeer，实现基于SockAddrType的Peer节点
 *	Peer节点处于单线程过程
 */
 class UdpPeer
 {
 public:
	 UdpPeer() {}
	 virtual ~UdpPeer() {}

	 virtual int Send(const char* lpBuf, int nBufLen, int nFlags = 0)
	 {
		return 0;
	 }

	 enum Event { UDP_PEER_EVT_NONE = 0, UDP_PEER_EVT_CLOSE,UDP_PEER_EVT_RECEIVE,UDP_PEER_EVT_SEND,};
	 virtual void Post(Event evt, int nErrorCode = 0)
	 {

	 }

 protected:
	 //通讯线程

	 //连接上
	 void OnConnect(const SOCKADDR* lpSockAddr)
	 {

	 }

	 //连接关闭
	 void OnClose(int nErrorCode = 0)
	 {

	 }

	 //接收完整一个包
	 void OnReceive(const char* lpBuf, int nBufLen)
	 {

	 }

	 //发送完整一个包
	 void OnSend(const char* lpBuf, int nBufLen)
	 {

	 }
 };

/*!
 *	@brief StableUdpSocketArchitectureExImpl 定义.
 *
 *	封装StableUdpSocketArchitectureExImpl，实现稳定有序的类似TCP的Udp数据包网络架构
 */
template<class TBase = SocketEx, class TPeer = UdpPeer, class SockAddrType = SOCKADDR_IN, u_short uMaxBufSize = 520>
class StableUdpSocketArchitectureExImpl : public StableUdpSocketArchitectureImpl<TBase,SockAddrType,uMaxBufSize>
{
	typedef StableUdpSocketArchitectureExImpl<TBase,TPeer,SockAddrType,uMaxBufSize> This;
	typedef StableUdpSocketArchitectureImpl<TBase,SockAddrType,uMaxBufSize> Base;
	class WorkPeer : public TPeer
	{
		friend class This;
		typedef TPeer Base;
	protected:
		This* m_pSocket;
		SockAddrType m_SockAddr;
		unsigned long m_nLastActiveTime;
		unsigned int m_nLastSeq;
		int m_evtPost;
		int m_nErrorCode;
	public:
		WorkPeer(This* pSocket):m_pSocket(pSocket),m_nLastActiveTime(0),m_nLastSeq(0),m_evtPost(UDP_PEER_EVT_NONE),m_nErrorCode(0) { /*memset(m_SockAddr,0,sizeof(SockAddrType));*/ }
		virtual ~WorkPeer() { }

		bool IsPeer(const SockAddrType & SockAddr) 
		{
			return memcmp(&m_SockAddr,&SockAddr,sizeof(SockAddrType))==0;
		}

		bool IsPeer()
		{
			return m_nLastActiveTime!=0;
		}

		bool IsIdle(unsigned long nIdleTime = GetTickCount(), unsigned long nMaxIdleTime = 10*1000)
		{
			if (!m_nLastActiveTime) {
				return true;
			}
			if ((nIdleTime-m_nLastActiveTime)>=nMaxIdleTime) {
				return true;
			}
			return false;
		}

		virtual int Send(const char* lpBuf, int nBufLen, int nFlags = 0)
		{
			return m_pSocket->Send(lpBuf,nBufLen,m_SockAddr,nFlags);
		}

		int Receive(char* lpBuf, int nBufLen, int* nFlags = NULL)
		{
			return m_pSocket->Receive(lpBuf,nBufLen,&m_SockAddr,nFlags);
		}

		virtual void Post(int evt, int nErrorCode = 0)
		{
			m_evtPost = evt;
			m_nErrorCode = nErrorCode;
		}

		void Work()
		{
			if (m_evtPost!=UDP_PEER_EVT_NONE) {
				switch(m_evtPost)
				{
				case UDP_PEER_EVT_CLOSE:
					{
						OnClose();
					}
					break;
				case UDP_PEER_EVT_RECEIVE:
					{
						int nBufLen = Receive(NULL,0,&nFlags);
						if (!(nFlags&SOCKET_PACKET_FLAG_COMPLETE)) {
							break;
						}
						string Buf(nBufLen,0);
						char* lpBuf = (char*)Buf.c_str();
						nBufLen = Receive(lpBuf,nBufLen,&nFlags);
						OnReceive(lpBuf,nBufLen);
					}
					break;
				case UDP_PEER_EVT_RECEIVE:
					{
						OnSend(NULL,0);
					}
					break;
				}
				m_evtPost = UDP_PEER_EVT_NONE;
				m_nErrorCode = 0;
			}
		}

	protected:
		//
		void OnConnect(const SockAddrType & SockAddr)
		{
			Base::OnConnect((const SOCKADDR*)&SockAddr);
			m_SockAddr = SockAddr;
			m_nLastActiveTime = GetTickCount();
			m_nLastSeq = 0;
			m_evtPost = UDP_PEER_EVT_NONE;
			m_nErrorCode = 0;
		}

		void OnClose(int nErrorCode = 0)
		{
			Base::OnClose(m_nErrorCode);
			m_nLastActiveTime = 0;
			memset(m_SockAddr,0,sizeof(SockAddrType));
		}

		//接收完整一个包
		void OnReceive(const char* lpBuf, int nBufLen)
		{
			Base::OnReceive(lpBuf,nBufLen);
			m_nLastActiveTime = GetTickCount();
		}

		//发送完整一个包
		void OnSend(const char* lpBuf, int nBufLen)
		{
			m_nLastActiveTime = GetTickCount();
		}
	};
protected:
	virtual unsigned int NewSeq(const SockAddrType & SockAddr) 
	{ 
		WorkPeer* pPeer = FindPeer(SockAddr);
		if(pPeer) {
			return pPeer->m_nLastSeq++%0x00ffffff;
		}
		return Base::NewSeq();
	}
protected:
	std::vector<WorkPeer> m_Peers;
	unsigned long m_nWorkIndex;
	CriticalSection m_WorkSection;
public:
	StableUdpSocketArchitectureExImpl(int nMaxPeerCount)
		:m_Peers(((nMaxPeerCount+(FD_SETSIZE-1))/FD_SETSIZE)*FD_SETSIZE),m_nWorkIndex(0),Base()
	{
		//m_Peers.resize(nMaxPeerCount);
	}

	virtual ~StableUdpSocketArchitectureExImpl()
	{
		SectionLocker Lock(&m_Section);
		m_nWorkIndex = 0;
		m_Peers.clear();
		Lock.Detach();
	}

	int Close()
	{
		int i;

		int rlt = Base::Close();

		for (i=0;i<m_Peers.size();i++)
		{
			const WorkPeer& TempPeer = m_Peers[i];
			TempPeer.OnClose();
		}
		//m_Peers.clear();

		return rlt;
	}

	//Send(const char* lpBuf, int nBufLen, const SOCKADDR_IN & SockAddr, int nFlags SOCKADDR_IN 0)

	int Receive(char* lpBuf, int nBufLen, SockAddrType* lpSockAddr, int* nFlags = NULL)
	{
		SectionLocker Lock(&m_RecvSection);
		ASSERT(nFlags && lpSockAddr);
		int i,j;
		tagUdpHeader* lpTempHeader = ReceiveBuffer(m_RecvBuffer,*lpSockAddr);
		if (lpTempHeader) {
			*nFlags = lpTempHeader->nFlags;
			int nPackLen = (lpTempHeader->nNum-1)*uMaxBufSize+(lpTempHeader->lpUdpBuf[lpTempHeader->nNum-1].nBufLen-stable_udpbuf_header_size);
			if (nBufLen>=nPackLen) {
				nBufLen = 0;
				for (i=0;i<lpTempHeader->nNum;i++)
				{
					if (i==(lpTempHeader->nNum-1)) {
						j = (lpTempHeader->lpUdpBuf[i].nBufLen-stable_udpbuf_header_size);
					} else {
						j = uMaxBufSize;
					}
					memcpy(lpBuf+nBufLen,lpTempHeader->lpUdpBuf[i].data,j);
					nBufLen += j;
				}
				RemoveBuffer(m_RecvBuffer,lpTempHeader);
			}
			return nPackLen;
		}
		return 0;
	}

	void Dispatch()
	{
		int i,j;
		SectionLocker Lock(&m_WorkSection);
		i = m_nWorkIndex;
		m_nWorkIndex += FD_SETSIZE;
		m_nWorkIndex %= m_Peers.size();
		Lock.Detach();
		j = i + FD_SETSIZE;
		if (j>m_Peers.size()) {
			j = m_Peers.size();
		}
		for (; i<j; i++)
		{
			WorkPeer & TempPeer = m_Peers[i];
			if (TempPeer.IsPeer()) {
				TempPeer.Work();
			}
		}
	}

protected:
	enum { FIND_PEER_FLAG_ADDIFNOTFIND = 1, FIND_PEER_FLAG_REMOVE = 2, };
	WorkPeer* FindPeer(const SockAddrType & SockAddr, UINT nFlags = 0)
	{
		WorkPeer* pPeer = NULL;
		int i;
		for (i=0;i<m_Peers.size();i++)
		{
			WorkPeer & TempPeer = m_Peers[i];
			if (TempPeer->IsPeer(SockAddr)) {
				pPeer = &TempPeer;
				break;
			}
		}
		if (!pPeer) {
			if (nFlags&FIND_PEER_FLAG_ADDIFNOTFIND) {
				for (i=0;i<m_Peers.size();i++)
				{
					WorkPeer & TempPeer = m_Peers[i];
					if (!TempPeer->IsPeer()) {
						TempPeer->OnConnect(SockAddr);
						pPeer = &TempPeer;
						break;
					}
				}
			} 
		} else {
			if (nFlags&FIND_PEER_FLAG_REMOVE) {
				pPeer->OnClose();
			}
		}
		return pPeer;
	}

protected:
	//
	//接收包超时，相当于和SockAddr的通讯中断，可以当做OnClose(SockAddr)处理
	virtual void OnReceiveBufTimeOut(const char* lpBuf, int nBufLen, const SockAddrType & SockAddr, int nFlags)
	{
		WorkPeer* pPeer = FindPeer(SockAddr);
		if(pPeer) {
			pPeer->Post(WorkPeer::UDP_PEER_EVT_RECEIVE,ETIMEDOUT);
		}
	}

	//接收完整一个包，lpBuf/nBufLen不一定有效
	virtual void OnRecvBuf(const char* lpBuf, int nBufLen, const SockAddrType & SockAddr, int nFlags)
	{
		WorkPeer* pPeer = FindPeer(SockAddr);
		if(pPeer) {
			pPeer->Post(WorkPeer::UDP_PEER_EVT_RECEIVE);
		}
	}

	//发送包超时，相当于和SockAddr的通讯中断，可以当做OnClose(SockAddr)处理
	virtual void OnSendBufTimeOut(const char* lpBuf, int nBufLen, const SockAddrType & SockAddr, int nFlags)
	{
		WorkPeer* pPeer = FindPeer(SockAddr);
		if(pPeer) {
			pPeer->Post(WorkPeer::UDP_PEER_EVT_SEND,ETIMEDOUT);
		}
	}

	//发送完整一个包
	virtual void OnSended(const char* lpBuf, int nBufLen, const SockAddrType & SockAddr, int nFlags)
	{
		WorkPeer* pPeer = FindPeer(SockAddr);
		if(pPeer) {
			pPeer->Post(WorkPeer::UDP_PEER_EVT_SEND);
		}
	}
};

#endif//_H_XSTABLEUDPSOCKETEX_H_