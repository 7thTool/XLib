/*
 * Copyright: 7thTool Open Source (i7thTool@qq.com)
 *
 * Version	: 1.1.1
 * Author	: Scott
 * Project	: http://git.oschina.net/7thTool/XSocket
 * Blog		: http://blog.csdn.net/zhangzq86
 *
 * LICENSED UNDER THE GNU LESSER GENERAL PUBLIC LICENSE, VERSION 2.1 (THE "LICENSE");
 * YOU MAY NOT USE THIS FILE EXCEPT IN COMPLIANCE WITH THE LICENSE.
 * UNLESS REQUIRED BY APPLICABLE LAW OR AGREED TO IN WRITING, SOFTWARE
 * DISTRIBUTED UNDER THE LICENSE IS DISTRIBUTED ON AN "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.
 * SEE THE LICENSE FOR THE SPECIFIC LANGUAGE GOVERNING PERMISSIONS AND
 * LIMITATIONS UNDER THE LICENSE.
 */

#ifndef _H_XSOCKETARCHITECTURE_H_
#define _H_XSOCKETARCHITECTURE_H_

#include "XSocketEx.h"

#include <string>
#include <queue>
#include <list>
#include <map>
using namespace std;

/*!
 *	@brief 网络数据包标志 定义.
 *
 *	定义支持的网络数据包标志
 *  兼容send/recv flags 标志
 */
enum  
{
	//MSG_DONTROUTE,
	//MSG_PARTIAL,
	//...
	//
	SOCKET_PACKET_FLAG_NONE			= 0,			//未知
	SOCKET_PACKET_FLAG_PENDING		= 0X00010000,	//未完成
	SOCKET_PACKET_FLAG_COMPLETE		= 0X00020000,	//是完整的包
	SOCKET_PACKET_FLAG_SEND			= 0X00040000,	//是发送包
	SOCKET_PACKET_FLAG_RECEIVE		= 0X00080000,	//是接收包
	SOCKET_PACKET_FLAG_PUSH			= 0X00100000,	//是推送包
	SOCKET_PACKET_FLAG_SENONLY		= 0X00200000,	//只是发送包，不需或者没有回应
	SOCKET_PACKET_FLAG_RESPONSE		= 0X00200000,	//是发送包的回应包
	SOCKET_PACKET_FLAG_CONTINUE		= 0X00400000,	//是还要继续收包
	SOCKET_PACKET_FLAG_HEARTBEAT	= 0X00800000,	//是心跳包
	SOCKET_PACKET_FLAG_TEMPBUF		= 0X80000000,	//临时内存，不能引用buf指针
	SOCKET_PACKET_FLAG_MASK			= 0XFFFF0000
};

/*!
 *	@brief SampleSocketArchitecture 定义.
 *
 *	封装SampleSocketArchitecture，定义简单的数据包网络架构
 */
template<class TBase = SocketEx>
class SampleSocketArchitecture : public TBase
{
	typedef TBase Base;
protected:
	int m_nSendLen;
	const char* m_pSendBuf;
	int m_nSendBufLen;
public:
	SampleSocketArchitecture()
		:Base()
		,m_nSendLen(0)
		,m_pSendBuf(NULL)
		,m_nSendBufLen(0)
	{

	}

	virtual ~SampleSocketArchitecture()
	{

	}

	int Close()
	{
		int rlt = Base::Close();
		m_nSendLen = 0;
		m_pSendBuf = NULL;
		m_nSendBufLen = 0;
		return rlt;
	}

protected:
	//
	//解析接收数据包
	virtual int ParseRecvBuf(const char* lpBuf, int & nBufLen) { return SOCKET_PACKET_FLAG_RECEIVE|SOCKET_PACKET_FLAG_COMPLETE; }
	
	//准备接收缓存
	virtual bool PrepareRecvBuf(char* & lpBuf, int & nBufLen)
	{
		ASSERT(0);
		return false;
	}

	//接收了完整缓存
	virtual void OnRecvBuf(const char* lpBuf, int nBufLen, int nFlags) 
	{

	}

	//解析发送数据包
	virtual int ParseSendBuf(const char* lpBuf, int & nBufLen) { return SOCKET_PACKET_FLAG_SEND|SOCKET_PACKET_FLAG_COMPLETE; }

	//准备发送数据包
	virtual bool PrepareSendBuf(const char* & lpBuf, int & nBufLen)
	{
		ASSERT(0);
		return false;
	}

	//发送了完整缓存
	virtual void OnSendBuf(const char* lpBuf, int nBufLen, int nFlags) 
	{

	}

protected:
	//
	virtual void OnReceive(int nErrorCode)
	{
		if (nErrorCode) {
			Base::OnReceive(nErrorCode);
			return;
		}
		
		bool bConitnue = false;
		do {
			bConitnue = false;
			char* lpBuf = NULL;
			int nBufLen = 0;
			if(!PrepareRecvBuf(lpBuf,nBufLen)) {
				RemoveEvent(FD_READ);
				return;
			}
			nBufLen = Base::Receive(lpBuf,nBufLen);
			if (nBufLen<=0) {
				nErrorCode = GetLastError();
				if(nBufLen==0 && nErrorCode==0) {
					//
				} else {
					switch(nErrorCode)
					{
					case EWOULDBLOCK:
						break;
#ifdef WIN32
					case WSA_IO_PENDING:
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
				}
			} else {
				bConitnue = true;
			}
		} while (bConitnue);
	}

	virtual void OnReceived(const char* lpBuf, int nBufLen, int nFlags) 
	{
		OnRecvBuf(lpBuf, nBufLen, SOCKET_PACKET_FLAG_RECEIVE);
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
			const char* lpBuf = NULL;
			int nBufLen = 0;
			if (!m_pSendBuf) {
				if(!PrepareSendBuf(lpBuf, nBufLen)) {
					//说明没有可发送数据
					RemoveEvent(FD_WRITE);
					return;
				}
				m_nSendLen = 0;
				m_pSendBuf = lpBuf;
				m_nSendBufLen = nBufLen;
			}
			lpBuf = m_pSendBuf+m_nSendLen;
			nBufLen = (int)(m_nSendBufLen-m_nSendLen);
			ASSERT(lpBuf && nBufLen>0);
			nBufLen = Base::Send(lpBuf, nBufLen);
			if (nBufLen<=0) {
				nErrorCode = GetLastError();
				if(nBufLen==0 && nErrorCode==0) {
					//
				} else {
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
				}
			} else {
				if(m_pSendBuf) {
					ASSERT(m_nSendBufLen>m_nSendLen && m_nSendLen>0);
					bConitnue = true; //继续发送
				}
			}
		} while (bConitnue);
	}

	virtual void OnSended(const char* lpBuf, int nBufLen, int nFlags) 
	{
		m_nSendLen += nBufLen;
		if(m_nSendLen >= m_nSendBufLen) {
			int nSendLen = m_nSendLen;
			int nSendFlags = ParseSendBuf(m_pSendBuf, nSendLen);
			ASSERT(nSendLen==m_nSendLen);
			OnSendBuf(m_pSendBuf, nSendLen, nSendFlags);
			m_nSendLen = 0;
			m_pSendBuf = NULL;
			m_nSendBufLen = 0;
		}
	}
};

/*!
 *	@brief SocketArchitecture 定义.
 *
 *	封装SocketArchitecture，定义对称的发送/接收（写入/读取）网络架构
 */
template<class TBase = SocketEx>
class SocketArchitecture : public SampleSocketArchitecture<TBase>
{
	typedef SampleSocketArchitecture<TBase> Base;
protected:
	//这里只需要第一次PrepareRecvBuf即可，后面会一直重用接收缓存
	int m_nRecvLen;
	char* m_pRecvBuf;
	int m_nRecvBufLen;
public:
	SocketArchitecture()
		:Base()
		,m_nRecvLen(0)
		,m_pRecvBuf(NULL)
		,m_nRecvBufLen(0)
	{
		
	}

	virtual ~SocketArchitecture()
	{

	}

	int Close()
	{
		int rlt = Base::Close();
		m_nRecvLen = 0;
		m_pRecvBuf = NULL;
		m_nRecvBufLen = 0;
		return rlt;
	}

protected:
	//

protected:
	//
	virtual void OnReceive(int nErrorCode)
	{
		if (nErrorCode) {
			Base::OnReceive(nErrorCode);
			return;
		}
		bool bConitnue = false;
		do {
			bConitnue = false;
			char* lpBuf = NULL;
			int nBufLen = 0;
			if (!m_pRecvBuf) {
				if(!PrepareRecvBuf(lpBuf, nBufLen)) {
					//说明没有可接收缓存
					RemoveEvent(FD_READ);
					return;
				}
				m_nRecvLen = 0;
				m_pRecvBuf = lpBuf;
				m_nRecvBufLen = nBufLen;
			}
			lpBuf = m_pRecvBuf+m_nRecvLen;
			nBufLen = (int)(m_nRecvBufLen-m_nRecvLen);
			ASSERT(nBufLen>0);
			nBufLen = Base::Receive(lpBuf, nBufLen);
			if (nBufLen<=0) {
				nErrorCode = GetLastError();
				if(nBufLen==0 && nErrorCode==0) {
					//
				} else {
					switch(nErrorCode)
					{
					case EWOULDBLOCK:
						break;
#ifdef WIN32
					case WSA_IO_PENDING:
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
				}
			} else {
				if(m_pRecvBuf) {
					ASSERT(m_nRecvBufLen>m_nRecvLen && m_nRecvLen>0);
					bConitnue = true;
				}
			}
		} while (bConitnue);
	}

	virtual void OnReceived(const char* lpBuf, int nBufLen, int nFlags) 
	{
		ASSERT(!(nFlags&MSG_OOB));
		m_nRecvLen += nBufLen;
		int nRecvLen = m_nRecvLen;
		int nRecvFlags = ParseRecvBuf(m_pRecvBuf, nRecvLen);
		if(SOCKET_PACKET_FLAG_COMPLETE&nRecvFlags) {
			OnRecvBuf(m_pRecvBuf, nRecvLen, nRecvFlags);
			if (nRecvLen<m_nRecvLen) { //说明粘包
				memmove(m_pRecvBuf, m_pRecvBuf+nRecvLen, m_nRecvLen-nRecvLen);
				m_nRecvLen = m_nRecvLen-nRecvBufLen;
				OnReceived(NULL, 0, nFlags);
			} else {
				m_nRecvLen = 0;
				//m_pRecvBuf = NULL;
				//m_nRecvBufLen = 0;
			}
		}
	}
};

/*!
 *	@brief SampleSocketArchitectureImpl 定义.
 *
 *	封装SampleSocketArchitectureImpl，实现简单的流式发送/接收（写入/读取）网络架构
 */
template<class TBase, u_short uCacheBufSize = 1024>
class SampleSocketArchitectureImpl : public TBase
{
	typedef TBase Base;
protected:
	typedef std::string SampleBuffer;
	SampleBuffer m_RecvBuffer;
	SampleBuffer m_PrepareRecvBuffer;
	SampleBuffer m_SendBuffer;
	SampleBuffer m_PrepareSendBuffer;
	CriticalSection m_SendSection;
	CriticalSection m_RecvSection;

public:
	SampleSocketArchitectureImpl()
	{
		m_RecvBuffer.reserve(uCacheBufSize);
		m_PrepareRecvBuffer.resize(uCacheBufSize);
		m_SendBuffer.reserve(uCacheBufSize);
		m_PrepareSendBuffer.reserve(uCacheBufSize);
	}

	virtual ~SampleSocketArchitectureImpl()
	{
		
	}

	int Close()
	{
		int rlt = Base::Close();

		SectionLocker LockSend(&m_SendSection);

		m_PrepareSendBuffer.clear();
		m_SendBuffer.clear();
		LockSend.Detach();

		SectionLocker LockRecv(&m_RecvSection);

		//m_PrepareRecvBuffer
		m_RecvBuffer.clear();
		LockRecv.Detach();

		return rlt;
	}

	int Send(const char* lpBuf, int nBufLen, int nFlags = 0)
	{
		SectionLocker Lock(&m_SendSection);

		m_SendBuffer.insert(m_SendBuffer.end(),lpBuf,lpBuf+nBufLen);

		Trigger(FD_WRITE);

		return nBufLen;
	}

	int Receive(char* lpBuf, int & nBufLen, int & nFlags)
	{
		SectionLocker Lock(&m_RecvSection);
		int nRecvBufLen = m_RecvBuffer.size();
		if (nRecvBufLen>0) {
			if (!nFlags) {
				nFlags = ParseRecvBuf(&m_RecvBuffer[0],nRecvBufLen);
				if (nFlags&SOCKET_PACKET_FLAG_COMPLETE) {
					if (nRecvBufLen<=nBufLen) {
						nBufLen = nRecvBufLen;
						memcpy(lpBuf,&m_RecvBuffer[0],nBufLen);
						m_RecvBuffer.erase(m_RecvBuffer.begin(),m_RecvBuffer.begin()+nBufLen);
						return nBufLen;
					} else {
						return nRecvBufLen;
					}
				}
			} else {
				ASSERT(nBufLen>0);
				if (nBufLen>=nRecvBufLen) {
					nBufLen = nRecvBufLen;
				}
				memcpy(lpBuf,&m_RecvBuffer[0],nBufLen);
				m_RecvBuffer.erase(m_RecvBuffer.begin(),m_RecvBuffer.begin()+nBufLen);
				return nBufLen;
			}
		}
		return 0;
	}

	void Dispatch()
	{
		int nFlags = 0;
		do
		{
			int nBufLen = Receive(NULL,0,&nFlags);
			if (!(nFlags&SOCKET_PACKET_FLAG_COMPLETE)) {
				break;
			}
			string Buf(nBufLen,0);
			char* lpBuf = (char*)Buf.c_str();
			nBufLen = Receive(lpBuf,nBufLen,&nFlags);
			if (nFlags&SOCKET_PACKET_FLAG_RESPONSE) {
				OnResponse(lpBuf,nBufLen,nFlags);
			} else {
				OnPush(lpBuf,nBufLen,nFlags);
			}
		} while(true);
	}

protected:
	//Dispatch 实现接口 
	virtual void OnResponse(const char* lpBuf, int nBufLen, int nFlags)
	{

	}

	virtual void OnPush(const char* lpBuf, int nBufLen, int nFlags)
	{

	}

protected:
	//SocketArchitecture 实现接口
	virtual bool PrepareRecvBuf(char* & lpBuf, int & nBufLen)
	{
		lpBuf = (char*)m_PrepareRecvBuffer.c_str();
		nBufLen = m_PrepareRecvBuffer.size();
		return true;
	}

	virtual void OnRecvBuf(const char* lpBuf, int nBufLen, int nFlags) 
	{
		Base::OnRecvBuf(lpBuf, nBufLen, nFlags);

		SectionLocker Lock(&m_RecvSection);

		m_RecvBuffer.insert(m_RecvBuffer.end(),lpBuf,lpBuf+nBufLen);
	}

	virtual bool PrepareSendBuf(const char* & lpBuf, int & nBufLen)
	{
		SectionLocker Lock(&m_SendSection);

		int nSendBufLen = m_SendBuffer.size();
		if (nSendBufLen>0) {
			m_PrepareSendBuffer.swap(m_SendBuffer);
			m_SendBuffer.clear();
			lpBuf = m_PrepareSendBuffer.c_str();
			nBufLen = m_PrepareSendBuffer.size();
			Lock.Detach();
			return true;
		}
		Lock.Detach();
		return false;
	}

	virtual void OnSendBuf(const char* lpBuf, int nBufLen, int nFlags) 
	{
		Base::OnSendBuf(lpBuf, nBufLen, nFlags);
	}
};

/*!
 *	@brief SampleSocketArchitectureImpl 定义.
 *
 *	封装SampleSocketArchitectureImpl，实现块状的数据包式的发送/接收（写入/读取）网络架构
 */
template<class TBase, u_short uMaxBufSize = 1024>
class SocketArchitectureImpl : public TBase
{
	typedef TBase Base;
protected:
	struct tagSASendBuf
	{
		int nSendBufLen;
		const char* pSendBuf;
		int nSendFlags;
		void* pUserData;
	};
	struct tagSARecvBuf : public tagSASendBuf
	{
		int nRecvLen;
		char RecvBuf[uMaxBufSize];
		int nRecvFlags;
	};
	typedef std::queue<tagSASendBuf> SendBufList;
	typedef std::vector<tagSARecvBuf> RecvBufList;
	SendBufList m_SendBufList;
	RecvBufList m_RecvBufList;
	typedef std::string SampleBuffer;
	SampleBuffer m_PrepareRecvBuffer;
	int m_nRecvBuf; //Parse计算存放收到的BUF的通道索引
	CriticalSection m_SendSection;
	CriticalSection m_RecvSection;

public:
	SocketArchitectureImpl()
	{
		m_RecvBufList.resize(8); //8
		m_PrepareRecvBuffer.resize(uMaxBufSize);
		m_nRecvBuf = -1;
	}

	virtual ~SocketArchitectureImpl()
	{
		
	}

	int Close()
	{
		int i;
		int rlt = Base::Close();

		SectionLocker LockSend(&m_SendSection);
		while (!m_SendBufList.empty())
		{
			tagSASendBuf & saBuf = m_SendBufList.front();
			if (saBuf.nSendFlags&SOCKET_PACKET_FLAG_TEMPBUF) {
				delete []saBuf.pSendBuf;
			}
			m_SendBufList.pop();
		}
		LockSend.Detach();

		SectionLocker LockRecv(&m_RecvSection);
		for(i=0;i<m_RecvBufList.size();i++)
		{
			tagSARecvBuf & saBuf = m_RecvBufList[i];
			if (saBuf.nSendFlags&SOCKET_PACKET_FLAG_TEMPBUF) {
				delete []saBuf.pSendBuf;
			}
			memset(&saBuf,0,sizeof(tagSARecvBuf));
		}
		//m_RecvBufList.clear();
		//m_PrepareRecvBuffer.clear();
		m_nRecvBuf = -1;
		LockRecv.Detach();

		return rlt;
	}

	int Send(const char* lpBuf, int nBufLen, int nFlags = 0)
	{
		SectionLocker Lock(&m_SendSection);

		tagSASendBuf saZero = {0};
		m_SendBufList.push(saZero);
		tagSASendBuf & saBuf = m_SendBufList.back();
		if (nFlags&SOCKET_PACKET_FLAG_TEMPBUF) {
			char* pTempBuf = new char[nBufLen+1];
			memcpy(pTempBuf,lpBuf,nBufLen);
			pTempBuf[nBufLen] = 0;
			saBuf.pSendBuf = pTempBuf;
		} else {
			saBuf.pSendBuf = lpBuf;
		}
		saBuf.nSendBufLen = nBufLen;
		saBuf.nSendFlags = nFlags;

		Trigger(FD_WRITE);

		return nBufLen;
	}

	int Receive(char* lpBuf, int & nBufLen, int & nFlags
		, const char* & lpSendBuf, int & nSendBufLen, int & nSendFlags, void* & pUserData
		)
	{
		SectionLocker Lock(&m_RecvSection);
		int i;
		ASSERT(lpBuf&&(nBufLen>=uMaxBufSize));
		for (i=0;i<m_RecvBufList.size();i++)
		{
			tagSARecvBuf & saBuf = m_RecvBufList[0];
			if (saBuf.nRecvLen>0) {
				ASSERT(saBuf.nRecvFlags&SOCKET_PACKET_FLAG_COMPLETE);
				if (nFlags) {
					*nFlags = saBuf.nRecvFlags;
				}
				nBufLen = saBuf.nRecvLen;
				memcpy(lpBuf,saBuf.RecvBuf,nBufLen);
				lpSendBuf = saBuf.pSendBuf;
				nSendBufLen = saBuf.nSendBufLen;
				nSendFlags = saBuf.nSendFlags;
				pUserData = saBuf.pUserData;
				memset(&saBuf,0,sizeof(saBuf));
				return nBufLen;
			}
		}
		return 0;
	}

	void Dispatch()
	{
		char lpBuf[uMaxBufSize];
		int nBufLen = uMaxBufSize;
		int nFlags = 0;
		const char* lpSendBuf;
		int nSendBufLen;
		int nSendFlags;
		void* pUserData;
		while ((nBufLen=Receive(lpBuf,nBufLen,nFlags,lpSendBuf,nSendBufLen,nSendFlags,pUserData))>0)
		{
			if (nFlags&SOCKET_PACKET_FLAG_RESPONSE) {
				OnResponse(lpBuf,nBufLen,nFlags);
			} else {
				OnPush(lpBuf,nBufLen,nFlags);
			}
		}
	}

protected:
	//Dispatch 实现接口 
	virtual void OnResponse(const char* lpBuf, int nBufLen, int nFlags
		, const char* lpSendBuf, int nSendBufLen, int nSendFlags, void* pUserData
		)
	{

	}

	virtual void OnPush(const char* lpBuf, int nBufLen, int nFlags)
	{

	}

protected:
	//SocketArchitecture 实现接口
	virtual bool PrepareRecvBuf(char* & lpBuf, int & nBufLen)
	{
		lpBuf = (char*)m_PrepareRecvBuffer.c_str();
		nBufLen = m_PrepareRecvBuffer.size();
		return true;
	}

	virtual void OnRecvBuf(const char* lpBuf, int nBufLen, int nFlags) 
	{
		Base::OnRecvBuf(lpBuf, nBufLen, nFlags);

		SectionLocker Lock(&m_RecvSection);

		int nRecvPos = m_nRecvBuf;
		if (nRecvPos<0 || nRecvPos>=m_RecvBufList.size()) {
			ASSERT(0);
			nRecvPos = m_RecvBufList.size();
			tagSARecvBuf saZero;
			m_RecvBufList.resize(nRecvPos*2,saZero);
		}
		tagSARecvBuf & saBuf = m_RecvBufList[nRecvPos];
		memcpy(saBuf.RecvBuf, lpBuf, nBufLen);
		saBuf.nRecvLen = nBufLen;
		saBuf.nRecvFlags = nFlags;
	}

	virtual bool PrepareSendBuf(const char* & lpBuf, int & nBufLen)
	{
		SectionLocker Lock(&m_SendSection);
		
		if (!m_SendBufList.empty()) {
			tagSASendBuf & saBuf = m_SendBufList.front();
			lpBuf = saBuf.pSendBuf;
			nBufLen = saBuf.nSendBufLen;
			return true;
		}
		Lock.Detach();
		return false;
	}

	virtual void OnSendBuf(const char* lpBuf, int nBufLen, int nFlags) 
	{
		Base::OnSendBuf(lpBuf, nBufLen, nFlags);

		SectionLocker Lock(&m_SendSection);
		int i;
		ASSERT (lpBuf && nBufLen);
		tagSASendBuf & saBuf = m_SendBufList.front();
		ASSERT(saBuf.pSendBuf==lpBuf && saBuf.nSendBufLen==nBufLen);
		if (saBuf.nSendFlags&SOCKET_PACKET_FLAG_TEMPBUF) {
			delete []saBuf.pSendBuf;
		}
		if (!(saBuf.nSendFlags&SOCKET_PACKET_FLAG_SENONLY)) {
			SectionLocker LockRecv(&m_RecvSection);
			for (i=0;i<m_RecvBufList.size();i++)
			{
				tagSARecvBuf & saRecvBuf = m_RecvBufList[i];
				if (saRecvBuf.nSendBufLen==0 && saRecvBuf.nRecvLen==0) {
					break;
				}
			}
			if (i>=m_RecvBufList.size()) {
				ASSERT(0);
				tagSARecvBuf saZero;
				m_RecvBufList.resize(i*2,saZero);
			}
			if (i<m_RecvBufList.size()) {
				tagSARecvBuf & saRecvBuf = m_RecvBufList[i];
				saRecvBuf.nSendBufLen = saBuf.nSendBufLen;
				saRecvBuf.pSendBuf = saBuf.pSendBuf;
				saRecvBuf.nSendFlags = saBuf.nSendFlags;
				saRecvBuf.pUserData = saBuf.pUserData;
			} else {
				ASSERT(0);
			}
			LockRecv.Detach();
		}
		m_SendBufList.pop();
	}
};

//////////////////////////////////////////////////////////////////////////

/*!
 *	@brief SampleUdpSocketArchitecture 定义.
 *
 *	封装SampleUdpSocketArchitecture，定义简单的Udp数据包网络架构
 *
 *	1.在进行UDP编程的时候,我们最容易想到的问题就是,一次发送多少bytes好?     
 *	当然,这个没有唯一答案，相对于不同的系统,不同的要求,其得到的答案是不一样的,我这里仅对     
 *	像ICQ一类的发送聊天消息的情况作分析，对于其他情况，你或许也能得到一点帮助:     
 *	首先,我们知道,TCP/IP通常被认为是一个四层协议系统,包括链路层,网络层,运输层,应用层.     
 *	UDP属于运输层,下面我们由下至上一步一步来看:     
 *	以太网(Ethernet)数据帧的长度必须在46-1500字节之间,这是由以太网的物理特性决定的.     
 *	这个1500字节被称为链路层的MTU(最大传输单元).     
 *	但这并不是指链路层的长度被限制在1500字节,其实这这个MTU指的是链路层的数据区.     
 *	并不包括链路层的首部和尾部的18个字节.     
 *	所以,事实上,这个1500字节就是网络层IP数据报的长度限制.     
 *	因为IP数据报的首部为20字节,所以IP数据报的数据区长度最大为1480字节.     
 *	而这个1480字节就是用来放TCP传来的TCP报文段或UDP传来的UDP数据报的.     
 *	又因为UDP数据报的首部8字节,所以UDP数据报的数据区最大长度为1472字节.     
 *	这个1472字节就是我们可以使用的字节数。:)     
 *	
 *	当我们发送的UDP数据大于1472的时候会怎样呢？     
 *	这也就是说IP数据报大于1500字节,大于MTU.这个时候发送方IP层就需要分片(fragmentation).     
 *	把数据报分成若干片,使每一片都小于MTU.而接收方IP层则需要进行数据报的重组.     
 *	这样就会多做许多事情,而更严重的是,由于UDP的特性,当某一片数据传送中丢失时,接收方便     
 *	无法重组数据报.将导致丢弃整个UDP数据报。     
 *	
 *	因此,在普通的局域网环境下，我建议将UDP的数据控制在1472字节以下为好.     
 *	
 *	进行Internet编程时则不同,因为Internet上的路由器可能会将MTU设为不同的值.     
 *	如果我们假定MTU为1500来发送数据的,而途经的某个网络的MTU值小于1500字节,那么系统将会使用一系列的机     
 *	制来调整MTU值,使数据报能够顺利到达目的地,这样就会做许多不必要的操作.     
 *	
 *	鉴于Internet上的标准MTU值为576字节,所以我建议在进行Internet的UDP编程时.     
 *	最好将UDP的数据长度控件在548字节(576-8-20)以内.  
 */
template<class TBase = SocketEx, class SockAddrType = SOCKADDR_IN>
class SampleUdpSocketArchitecture : public TBase
{
	typedef TBase Base;
protected:
	int m_nSendLen;
	const char* m_pSendBuf;
	int m_nSendBufLen;
	SockAddrType m_SendAddr;
public:
	SampleUdpSocketArchitecture()
		:Base()
		,m_nSendLen(0)
		,m_pSendBuf(NULL)
		,m_nSendBufLen(0)
		//,m_SendAddr(0)
	{
		memset(&m_SendAddr,0,sizeof(SockAddrType));
	}

	virtual ~SampleUdpSocketArchitecture()
	{

	}

	int Close()
	{
		int rlt = Base::Close();
		m_nSendLen = 0;
		m_pSendBuf = NULL;
		m_nSendBufLen = 0;
		memset(&m_SendAddr,0,sizeof(SockAddrType));
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

	//接收完整一个包
	virtual void OnRecvBuf(const char* lpBuf, int nBufLen, const SockAddrType & SockAddr, int nFlags)
	{

	}

	//解析发送数据包
	virtual int ParseSendBuf(const char* lpBuf, int & nBufLen, const SockAddrType & SockAddr) { return SOCKET_PACKET_FLAG_SEND|SOCKET_PACKET_FLAG_COMPLETE; }

	//准备发送数据包
	virtual bool PrepareSendBuf(const char* & lpBuf, int & nBufLen, SockAddrType & SockAddr)
	{
		ASSERT(0);
		return false;
	}

	//发送完整一个包
	virtual void OnSendBuf(const char* lpBuf, int nBufLen, const SockAddrType & SockAddr, int nFlags)
	{

	}

protected:
	//
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
			char lpBuf[1025] = {0};
			int nBufLen = 1024;
			SockAddrType SockAddr;
			int nSockAddrLen = sizeof(SockAddrType);
			nBufLen = Base::ReceiveFrom(lpBuf,nBufLen,(SOCKADDR*)&SockAddr,&nSockAddrLen);
			if (nBufLen<=0) {
				nErrorCode = GetLastError();
				if(nBufLen==0 && nErrorCode==0) {
					//
				} else {
					switch(nErrorCode)
					{
					case EWOULDBLOCK:
						break;
	#ifdef WIN32
					case WSA_IO_PENDING:
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
				}
			} else {
				//bConitnue = true;
			}
		} while(bConitnue);
	}

	virtual void OnReceivedFrom(const char* lpBuf, int nBufLen, const SOCKADDR* lpSockAddr, int nSockAddrLen, int nFlags) 
	{
		ASSERT(nSockAddrLen==sizeof(SockAddrType));
#if 0
		PRINTF("(%s:%d):%s\n", N2Ip(lpSockAddr->sin_addr.s_addr), N2H(lpSockAddr->sin_port), lpBuf);
#endif//
		OnRecvBuf(lpBuf, nBufLen, *(SockAddrType*)lpSockAddr, SOCKET_PACKET_FLAG_RECEIVE);
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
			const char* lpBuf = NULL;
			int nBufLen = 0;
			SockAddrType SockAddr;
			if (!m_pSendBuf) {
				if(!PrepareSendBuf(lpBuf,nBufLen,SockAddr)) {
					//说明没有可发送数据
					RemoveEvent(FD_WRITE);
					return;
				}
				m_nSendLen = 0;
				m_pSendBuf = lpBuf;
				m_nSendBufLen = nBufLen;
				m_SendAddr = SockAddr;
			}
			lpBuf = m_pSendBuf+m_nSendLen;
			nBufLen = (int)(m_nSendBufLen-m_nSendLen);
			SockAddr = m_SendAddr;
			ASSERT(lpBuf && nBufLen>0);
	#if 0
			PRINTF("echo:(%s:%d)\n",N2Ip(SockAddr.sin_addr.s_addr),N2H(SockAddr.sin_port));
	#endif//
			nBufLen = Base::SendTo(lpBuf,nBufLen,(const SOCKADDR*)&SockAddr,sizeof(SockAddrType));
			if (nBufLen<=0) {
				nErrorCode = GetLastError();
				if(nBufLen==0 && nErrorCode==0) {
					//
				} else {
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
		if(m_nSendLen>=m_nSendBufLen) {
			int nSendLen = m_nSendLen;
			int nSendFlags = ParseSendBuf(m_pSendBuf, nSendLen, m_SendAddr);
			ASSERT(nSendLen==m_nSendLen);
			OnSendBuf(m_pSendBuf, nSendLen, m_SendAddr, nSendFlags);
			m_nSendLen = 0;
			m_pSendBuf = NULL;
			m_nSendBufLen = 0;
			memset(&m_SendAddr,0,sizeof(SockAddrType));
		} else {
			ASSERT(0); //UDP 不应该发送太大的包导致发不完，太大的包在网络传输中可能会分片，如果分片重组失败就会丢包，建议520-540字节包
		}
	}
};

/*!
 *	@brief SampleUdpSocketArchitectureImpl 定义.
 *
 *	封装SampleUdpSocketArchitectureImpl，实现简单的Udp数据包网络架构
 */
template<class TBase, class SockAddrType = SOCKADDR_IN>
class SampleUdpSocketArchitectureImpl : public TBase
{
	typedef TBase Base;
protected:
	typedef std::string SampleBuffer;
	SampleBuffer m_RecvBuffer;
	SampleBuffer m_SendBuffer;
	SampleBuffer m_PrepareSendBuffer;
	CriticalSection m_SendSection;
	CriticalSection m_RecvSection;

public:
	SampleUdpSocketArchitectureImpl()
	{
		m_RecvBuffer.reserve(8*1024); //8k
		m_SendBuffer.reserve(8*1024); //8k
		m_PrepareSendBuffer.reserve(1024); //1k
	}

	virtual ~SampleUdpSocketArchitectureImpl()
	{
		
	}

	int Close()
	{
		int rlt = Base::Close();

		SectionLocker LockSend(&m_SendSection);

		m_PrepareSendBuffer.clear();
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

		const char* pPack = NULL;
		int nPackLen = sizeof(int) + sizeof(SockAddrType) + nBufLen;
		pPack = (const char*)&nPackLen;
		m_SendBuffer.insert(m_SendBuffer.end(),pPack,pPack+sizeof(int));
		pPack = (const char*)&SockAddr;
		m_SendBuffer.insert(m_SendBuffer.end(),pPack,pPack+sizeof(SockAddrType));
		m_SendBuffer.insert(m_SendBuffer.end(),lpBuf,lpBuf+nBufLen);

		Trigger(FD_WRITE);

		return nBufLen;
	}

	int Receive(char* lpBuf, int & nBufLen, SockAddrType & SockAddr, int & nFlags)
	{
		SectionLocker Lock(&m_RecvSection);
		int nRecvBufLen = m_RecvBuffer.size();
		if (nRecvBufLen>0) {
			ASSERT(nRecvBufLen>(sizeof(int)+sizeof(SockAddrType)));
			int* pPackLen = (int*)&m_RecvBuffer[0];
			SockAddrType* pPackAddr = (SockAddrType*)(pPackLen+1);
			const char* pPackBuf = (const char*)(pPackAddr+1);
			ASSERT(*pPackLen<=nRecvBufLen);
			int nPackBufLen = (*pPackLen-(sizeof(int)+sizeof(SockAddrType)));
			SockAddr = *pPackAddr;
			if (nBufLen>=nPackBufLen) {
				nBufLen = nPackBufLen;
				memcpy(lpBuf,pPackBuf,nBufLen);
				m_RecvBuffer.erase(m_RecvBuffer.begin(),m_RecvBuffer.begin()+*pPackLen);
				return nBufLen;
			} else {
				return nPackBufLen;
			}
		}
		return 0;
	}

	/*void Dispatch()
	{
		int nFlags = 0;
		do
		{
			int nBufLen = Receive(NULL,0);
			if (!(nFlags&SOCKET_PACKET_FLAG_COMPLETE)) {
				break;
			}
			string Buf(nBufLen,0);
			char* lpBuf = (char*)Buf.c_str();
			nBufLen = Receive(lpBuf,nBufLen);
			if (nFlags&SOCKET_PACKET_FLAG_RESPONSE) {
				OnResponse(lpBuf,nBufLen,nFlags);
			} else {
				OnPush(lpBuf,nBufLen,nFlags);
			}
		} while(true);
	}

protected:
	//Dispatch 实现接口 
	virtual void OnResponse(const char* lpBuf, int nBufLen, const SockAddrType & SockAddr, int nFlags)
	{

	}

	virtual void OnPush(const char* lpBuf, int nBufLen, const SockAddrType & SockAddr, int nFlags)
	{

	}*/

protected:
	//SocketArchitecture 实现接口
	virtual bool PrepareRecvBuf(char* & lpBuf, int & nBufLen, SockAddrType* & lpSockAddr)
	{
		ASSERT(0);
		return false;
	}

	virtual void OnRecvBuf(const char* lpBuf, int nBufLen, const SockAddrType & SockAddr, int nFlags) 
	{
		Base::OnRecvBuf(lpBuf, nBufLen, SockAddr, nFlags);

		SectionLocker Lock(&m_RecvSection);

		const char* pTempBuf = NULL;
		int nPackLen = sizeof(int) + sizeof(SockAddrType) + nBufLen;
		pTempBuf = (const char*)&nPackLen;
		m_RecvBuffer.insert(m_RecvBuffer.end(),pTempBuf,pTempBuf+sizeof(int));
		pTempBuf = (const char*)&SockAddr;
		m_RecvBuffer.insert(m_RecvBuffer.end(),pTempBuf,pTempBuf+sizeof(SockAddrType));
		m_RecvBuffer.insert(m_RecvBuffer.end(),lpBuf,lpBuf+nBufLen);
	}

	virtual bool PrepareSendBuf(const char* & lpBuf, int & nBufLen, SockAddrType & SockAddr)
	{
		SectionLocker Lock(&m_SendSection);

		int nSendBufLen = m_SendBuffer.size();
		if (nSendBufLen>0) {
			ASSERT(nSendBufLen>sizeof(int)+sizeof(SockAddrType));
			int nPackLen = 0;
			int nOffset = 0;
			do
			{
				nPackLen = *(int*)&m_SendBuffer[nOffset];
				nOffset += nPackLen;
			} while(nOffset<nSendBufLen);
			ASSERT(nOffset==nSendBufLen);
			m_PrepareSendBuffer.assign(m_SendBuffer.end()-nPackLen,m_SendBuffer.end());
			m_SendBuffer.erase(m_SendBuffer.end()-nPackLen,m_SendBuffer.end());
			int* pPackLen = (int*)&m_PrepareSendBuffer[0];
			const SockAddrType* pPackAddr = (const SockAddrType*)(pPackLen+1);
			const char* pPackBuf = (const char*)(pPackAddr+1);
			lpBuf = pPackBuf;
			nBufLen = *pPackLen-(sizeof(int)+sizeof(SockAddrType));
			SockAddr = *pPackAddr;
			Lock.Detach();
			return true;
		}
		Lock.Detach();

		return false;
	}

	virtual void OnSendBuf(const char* lpBuf, int nBufLen, const SockAddrType & SockAddr, int nFlags) 
	{
		Base::OnSendBuf(lpBuf, nBufLen, SockAddr, nFlags);
	}
};

#endif//_H_XSOCKETARCHITECTURE_H_