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
 *	@brief �������ݰ���־ ����.
 *
 *	����֧�ֵ��������ݰ���־
 *  ����send/recv flags ��־
 */
enum  
{
	//MSG_DONTROUTE,
	//MSG_PARTIAL,
	//...
	//
	SOCKET_PACKET_FLAG_NONE			= 0,			//δ֪
	SOCKET_PACKET_FLAG_PENDING		= 0X00010000,	//δ���
	SOCKET_PACKET_FLAG_COMPLETE		= 0X00020000,	//�������İ�
	SOCKET_PACKET_FLAG_SEND			= 0X00040000,	//�Ƿ��Ͱ�
	SOCKET_PACKET_FLAG_RECEIVE		= 0X00080000,	//�ǽ��հ�
	SOCKET_PACKET_FLAG_PUSH			= 0X00100000,	//�����Ͱ�
	SOCKET_PACKET_FLAG_SENONLY		= 0X00200000,	//ֻ�Ƿ��Ͱ����������û�л�Ӧ
	SOCKET_PACKET_FLAG_RESPONSE		= 0X00200000,	//�Ƿ��Ͱ��Ļ�Ӧ��
	SOCKET_PACKET_FLAG_CONTINUE		= 0X00400000,	//�ǻ�Ҫ�����հ�
	SOCKET_PACKET_FLAG_HEARTBEAT	= 0X00800000,	//��������
	SOCKET_PACKET_FLAG_TEMPBUF		= 0X80000000,	//��ʱ�ڴ棬��������bufָ��
	SOCKET_PACKET_FLAG_MASK			= 0XFFFF0000
};

/*!
 *	@brief SampleSocketArchitecture ����.
 *
 *	��װSampleSocketArchitecture������򵥵����ݰ�����ܹ�
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
	//�����������ݰ�
	virtual int ParseRecvBuf(const char* lpBuf, int & nBufLen) { return SOCKET_PACKET_FLAG_RECEIVE|SOCKET_PACKET_FLAG_COMPLETE; }
	
	//׼�����ջ���
	virtual bool PrepareRecvBuf(char* & lpBuf, int & nBufLen)
	{
		ASSERT(0);
		return false;
	}

	//��������������
	virtual void OnRecvBuf(const char* lpBuf, int nBufLen, int nFlags) 
	{

	}

	//�����������ݰ�
	virtual int ParseSendBuf(const char* lpBuf, int & nBufLen) { return SOCKET_PACKET_FLAG_SEND|SOCKET_PACKET_FLAG_COMPLETE; }

	//׼���������ݰ�
	virtual bool PrepareSendBuf(const char* & lpBuf, int & nBufLen)
	{
		ASSERT(0);
		return false;
	}

	//��������������
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
					//˵��û�пɷ�������
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
					bConitnue = true; //��������
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
 *	@brief SocketArchitecture ����.
 *
 *	��װSocketArchitecture������ԳƵķ���/���գ�д��/��ȡ������ܹ�
 */
template<class TBase = SocketEx>
class SocketArchitecture : public SampleSocketArchitecture<TBase>
{
	typedef SampleSocketArchitecture<TBase> Base;
protected:
	//����ֻ��Ҫ��һ��PrepareRecvBuf���ɣ������һֱ���ý��ջ���
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
					//˵��û�пɽ��ջ���
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
			if (nRecvLen<m_nRecvLen) { //˵��ճ��
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
 *	@brief SampleSocketArchitectureImpl ����.
 *
 *	��װSampleSocketArchitectureImpl��ʵ�ּ򵥵���ʽ����/���գ�д��/��ȡ������ܹ�
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
	//Dispatch ʵ�ֽӿ� 
	virtual void OnResponse(const char* lpBuf, int nBufLen, int nFlags)
	{

	}

	virtual void OnPush(const char* lpBuf, int nBufLen, int nFlags)
	{

	}

protected:
	//SocketArchitecture ʵ�ֽӿ�
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
 *	@brief SampleSocketArchitectureImpl ����.
 *
 *	��װSampleSocketArchitectureImpl��ʵ�ֿ�״�����ݰ�ʽ�ķ���/���գ�д��/��ȡ������ܹ�
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
	int m_nRecvBuf; //Parse�������յ���BUF��ͨ������
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
	//Dispatch ʵ�ֽӿ� 
	virtual void OnResponse(const char* lpBuf, int nBufLen, int nFlags
		, const char* lpSendBuf, int nSendBufLen, int nSendFlags, void* pUserData
		)
	{

	}

	virtual void OnPush(const char* lpBuf, int nBufLen, int nFlags)
	{

	}

protected:
	//SocketArchitecture ʵ�ֽӿ�
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
 *	@brief SampleUdpSocketArchitecture ����.
 *
 *	��װSampleUdpSocketArchitecture������򵥵�Udp���ݰ�����ܹ�
 *
 *	1.�ڽ���UDP��̵�ʱ��,�����������뵽���������,һ�η��Ͷ���bytes��?     
 *	��Ȼ,���û��Ψһ�𰸣�����ڲ�ͬ��ϵͳ,��ͬ��Ҫ��,��õ��Ĵ��ǲ�һ����,���������     
 *	��ICQһ��ķ���������Ϣ�������������������������������Ҳ�ܵõ�һ�����:     
 *	����,����֪��,TCP/IPͨ������Ϊ��һ���Ĳ�Э��ϵͳ,������·��,�����,�����,Ӧ�ò�.     
 *	UDP���������,����������������һ��һ������:     
 *	��̫��(Ethernet)����֡�ĳ��ȱ�����46-1500�ֽ�֮��,��������̫�����������Ծ�����.     
 *	���1500�ֽڱ���Ϊ��·���MTU(����䵥Ԫ).     
 *	���Ⲣ����ָ��·��ĳ��ȱ�������1500�ֽ�,��ʵ�����MTUָ������·���������.     
 *	����������·����ײ���β����18���ֽ�.     
 *	����,��ʵ��,���1500�ֽھ��������IP���ݱ��ĳ�������.     
 *	��ΪIP���ݱ����ײ�Ϊ20�ֽ�,����IP���ݱ����������������Ϊ1480�ֽ�.     
 *	�����1480�ֽھ���������TCP������TCP���Ķλ�UDP������UDP���ݱ���.     
 *	����ΪUDP���ݱ����ײ�8�ֽ�,����UDP���ݱ�����������󳤶�Ϊ1472�ֽ�.     
 *	���1472�ֽھ������ǿ���ʹ�õ��ֽ�����:)     
 *	
 *	�����Ƿ��͵�UDP���ݴ���1472��ʱ��������أ�     
 *	��Ҳ����˵IP���ݱ�����1500�ֽ�,����MTU.���ʱ���ͷ�IP�����Ҫ��Ƭ(fragmentation).     
 *	�����ݱ��ֳ�����Ƭ,ʹÿһƬ��С��MTU.�����շ�IP������Ҫ�������ݱ�������.     
 *	�����ͻ�����������,�������ص���,����UDP������,��ĳһƬ���ݴ����ж�ʧʱ,���շ���     
 *	�޷��������ݱ�.�����¶�������UDP���ݱ���     
 *	
 *	���,����ͨ�ľ����������£��ҽ��齫UDP�����ݿ�����1472�ֽ�����Ϊ��.     
 *	
 *	����Internet���ʱ��ͬ,��ΪInternet�ϵ�·�������ܻὫMTU��Ϊ��ͬ��ֵ.     
 *	������Ǽٶ�MTUΪ1500���������ݵ�,��;����ĳ�������MTUֵС��1500�ֽ�,��ôϵͳ����ʹ��һϵ�еĻ�     
 *	��������MTUֵ,ʹ���ݱ��ܹ�˳������Ŀ�ĵ�,�����ͻ�����಻��Ҫ�Ĳ���.     
 *	
 *	����Internet�ϵı�׼MTUֵΪ576�ֽ�,�����ҽ����ڽ���Internet��UDP���ʱ.     
 *	��ý�UDP�����ݳ��ȿؼ���548�ֽ�(576-8-20)����.  
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
	//�����������ݰ�
	virtual int ParseRecvBuf(const char* lpBuf, int & nBufLen, const SockAddrType & SockAddr) { return SOCKET_PACKET_FLAG_RECEIVE|SOCKET_PACKET_FLAG_COMPLETE; }

	//׼�����ջ���
	virtual bool PrepareRecvBuf(char* & lpBuf, int & nBufLen, SockAddrType & SockAddr)
	{
		ASSERT(0);
		return false;
	}

	//��������һ����
	virtual void OnRecvBuf(const char* lpBuf, int nBufLen, const SockAddrType & SockAddr, int nFlags)
	{

	}

	//�����������ݰ�
	virtual int ParseSendBuf(const char* lpBuf, int & nBufLen, const SockAddrType & SockAddr) { return SOCKET_PACKET_FLAG_SEND|SOCKET_PACKET_FLAG_COMPLETE; }

	//׼���������ݰ�
	virtual bool PrepareSendBuf(const char* & lpBuf, int & nBufLen, SockAddrType & SockAddr)
	{
		ASSERT(0);
		return false;
	}

	//��������һ����
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
			//UDP ��֤һ�ν���һ������UDP��
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
					//˵��û�пɷ�������
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
				//bConitnue = true; //��������
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
			ASSERT(0); //UDP ��Ӧ�÷���̫��İ����·����̫꣬��İ������紫���п��ܻ��Ƭ�������Ƭ����ʧ�ܾͻᶪ��������520-540�ֽڰ�
		}
	}
};

/*!
 *	@brief SampleUdpSocketArchitectureImpl ����.
 *
 *	��װSampleUdpSocketArchitectureImpl��ʵ�ּ򵥵�Udp���ݰ�����ܹ�
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
	//Dispatch ʵ�ֽӿ� 
	virtual void OnResponse(const char* lpBuf, int nBufLen, const SockAddrType & SockAddr, int nFlags)
	{

	}

	virtual void OnPush(const char* lpBuf, int nBufLen, const SockAddrType & SockAddr, int nFlags)
	{

	}*/

protected:
	//SocketArchitecture ʵ�ֽӿ�
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