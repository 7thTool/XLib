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

#ifndef _H_XPROXYSOCKETEX_H_
#define _H_XPROXYSOCKETEX_H_

#include "XSocketEx.h"

//////////////////////////////////////////////////////////////////////////

/*!
 *	@brief ProxyHelper ģ�嶨��.
 *
 *	���������ProxyHelper
 */
class XLIB_API ProxyHelper
{
public:
	static int Base64Encode(const char *pbSrcData, int nSrcLen, char* szDest,int nDestLen);
};

//////////////////////////////////////////////////////////////////////////

/*!
 *	@brief ��������.
 *
 *	����֧�ֵĴ�������
 */
#define PROXY_TYPE_NONE		0		//!< ��
#define PROXY_TYPE_CONNECT	1		//!< ����
#define PROXY_TYPE_BIND		2		//!< ��

/*!
 *	@brief ����״̬.
 *
 *	�������״̬
 */
#define PROXY_STATE_NONE	0		//!< ��
#define PROXY_STATE_OK		0XFF	//!< ����ɹ�

/*!
 *	@brief ProxyHandler ģ�嶨��.
 *
 *	��װ������̣�ʵ�ִ���Э��
 */
template<class T, byte byProxyType>
class ProxyHandlerT
{
protected:
	byte		m_ProxyState;	//����״̬
	PROXYINFO	m_ProxyInfo;	//����
	char 		m_szHost[256];	//�������ĵ�ַ
	u_short 	m_nPort;		//�������Ķ˿�

public:
	ProxyHandlerT()
	{
		ResetProxyInfo();
	}

	void ResetProxyInfo()
	{
		m_ProxyState = PROXY_STATE_NONE;
		memset(&m_ProxyInfo, 0, sizeof(m_ProxyInfo));
		memset(m_szHost, 0, sizeof(m_szHost));
		m_nPort = 0;
	}

	void SetProxy(const char* lpszHostAddress, unsigned short nHostPort, PPROXYINFO pProxy = NULL)
	{
		strcpy(m_szHost, lpszHostAddress);
		m_nPort = nHostPort;
		if(!pProxy || !pProxy->eType) {
			m_ProxyState = PROXY_STATE_OK;
		} else {
			m_ProxyInfo = *pProxy;
			m_ProxyState = 1;
		}
	}

	PROXYINFO& GetProxy() 
	{ 
		return m_ProxyInfo; 
	}

	const char* GetProxyIp() 
	{ 
		if (IsProxy()) {
			return m_ProxyInfo.szHost;
		}
		ASSERT(0);
		return NULL;
	}
	u_short GetProxyPort()
	{ 
		if (IsProxy()) {
			return m_ProxyInfo.nPort;
		}
		ASSERT(0);
		return 0;
	}

	const char* GetHostIp() 
	{ 
		return m_szHost;
	}
	u_short GetHostPort()
	{ 
		return m_nPort;
	}

	bool IsConnectProxy()
	{
		return byProxyType==PROXY_TYPE_CONNECT;
	}

	bool IsListenProxy()
	{
		return byProxyType==PROXY_TYPE_BIND;
	}

	bool IsProxy()
	{
		return m_ProxyInfo.eType!=PROXYTYPE_NONE;
	}

	bool IsProxyOK()
	{
		return m_ProxyState==PROXY_STATE_OK;
	}

	bool IsInProxy()
	{
		return m_ProxyState!=PROXY_STATE_NONE && m_ProxyState!=PROXY_STATE_OK;
	}

	int ReceiveProxy()
	{
		T* pT = static_cast<T*>(this);
		ASSERT(IsProxy() && IsInProxy());
		if(!IsProxy() || !IsInProxy()) {
			return 0;
		}

		char Buf[1024] = {0};
		int nBufLen = 0;

		byte byProxyState = m_ProxyState;
		if (byProxyState%2==1) {
			return 0;
		}

		int ret = 0;
		//��֯�����¼Э��
		switch(m_ProxyInfo.eType)
		{
		case PROXYTYPE_SOCKS4:
		case PROXYTYPE_SOCKS4A:
			{
				byProxyState = PROXY_STATE_NONE;
				nBufLen = pT->Receive(Buf, sizeof(Buf));
				if (nBufLen >= 8 && Buf[0] == 0x00 && Buf[1] == 0x5A) {
					//�������ӳɹ�
					byProxyState = PROXY_STATE_OK;
				}
			}
			break;
		case PROXYTYPE_SOCKS5:
			{
				switch (byProxyState) 
				{
				case 2:
					{
						nBufLen = pT->Receive(Buf, sizeof(Buf));
						if(nBufLen>0) {
							byProxyState = 5;
							//if (Buf[0] == 0x05) {
							//	//
							//}
							if(nBufLen>1) {
								if (Buf[1] == 0x02) {
									byProxyState = 3;
								}
							}
						}
						if (byProxyState == 2) {
							byProxyState = PROXY_STATE_NONE;
						}
					}
					break;
				case 4:
					{
						nBufLen = pT->Receive(Buf, sizeof(Buf));
						if(nBufLen>1) {
							if (/*Buf[0] == 0x05 &&*/ Buf[1] == 0x00) {
								byProxyState = 5;
							}
						}
						if (byProxyState == 4) {
							byProxyState = PROXY_STATE_NONE;
						}
					}
					break;
				case 6:
					{
						nBufLen = pT->Receive(Buf, sizeof(Buf));
						if(nBufLen>1) {
							if (/*Buf[0] == 0x05 &&*/ Buf[1] == 0x00) {
								byProxyState = PROXY_STATE_OK;
							}
						}
						if (byProxyState != PROXY_STATE_OK) {
							byProxyState = PROXY_STATE_NONE;
						}
					}
					break;
				default:
					break;
				}
			}
			break;
		case PROXYTYPE_HTTP10:
		case PROXYTYPE_HTTP11:
			{
				byProxyState = PROXY_STATE_NONE;
				nBufLen = pT->Receive(Buf, sizeof(Buf));
				PRINTF("[%d]\r\n%*s", nBufLen, nBufLen, Buf);
				if (strnicmp(Buf,"HTTP/",5)==0 && strnicmp(Buf+nBufLen-4,("\r\n\r\n"),4)==0) {
					char* pEnd = strstr(Buf, "\r\n");
					if(pEnd) {
						char* pStart = strstr(Buf, " ");
						if (pStart) {
							pStart++;
							if(pStart<pEnd) {
								if (*pStart=='2') {
									byProxyState = PROXY_STATE_OK;
								}
							}
						}
					}
				}
			}
			break;
		}
		if (byProxyState == PROXY_STATE_NONE) {
			if (nBufLen<0) {
				ret = nBufLen;
			} else {
				ret = SOCKET_ERROR;
				//pT->SetLastError(ECONNREFUSED);
			}
		}
		m_ProxyState = byProxyState;
		return ret;
	}

	void OnReceivedProxy(const char* lpBuf, int nBufLen, int nFlags)
	{
		T* pT = static_cast<T*>(this);
		ASSERT(IsProxy() && IsInProxy());

		byte byProxyState = m_ProxyState;
		ASSERT (byProxyState%2==0);

		//��֯�����¼Э��
		switch(m_ProxyInfo.eType)
		{
		case PROXYTYPE_SOCKS4:
		case PROXYTYPE_SOCKS4A:
			{
				byProxyState = PROXY_STATE_NONE;
				if (nBufLen >= 8 && lpBuf[0] == 0x00 && lpBuf[1] == 0x5A) {
					//�������ӳɹ�
					byProxyState = PROXY_STATE_OK;
				}
			}
			break;
		case PROXYTYPE_SOCKS5:
			{
				switch (byProxyState) 
				{
				case 2:
					{
						if(nBufLen>0) {
							byProxyState = 5;
							//if (lpBuf[0] == 0x05) {
							//	//
							//}
							if(nBufLen>1) {
								if (lpBuf[1] == 0x02) {
									byProxyState = 3;
								}
							}
						}
						if (byProxyState == 2) {
							byProxyState = PROXY_STATE_NONE;
						}
					}
					break;
				case 4:
					{
						if(nBufLen>1) {
							if (/*lpBuf[0] == 0x05 &&*/ lpBuf[1] == 0x00) {
								byProxyState = 5;
							}
						}
						if (byProxyState == 4) {
							byProxyState = PROXY_STATE_NONE;
						}
					}
					break;
				case 6:
					{
						if(nBufLen>1) {
							if (/*lpBuf[0] == 0x05 &&*/ lpBuf[1] == 0x00) {
								byProxyState = PROXY_STATE_OK;
							}
						}
						if (byProxyState != PROXY_STATE_OK) {
							byProxyState = PROXY_STATE_NONE;
						}
					}
					break;
				default:
					break;
				}
			}
			break;
		case PROXYTYPE_HTTP10:
		case PROXYTYPE_HTTP11:
			{
				byProxyState = PROXY_STATE_NONE;
				PRINTF("[%d]\r\n%*s", nBufLen, nBufLen, lpBuf);
				if (strnicmp(lpBuf,"HTTP/",5)==0 && strnicmp(lpBuf+nBufLen-4,("\r\n\r\n"),4)==0) {
					char* pEnd = strstr((char*)lpBuf, "\r\n");
					if(pEnd) {
						char* pStart = strstr((char*)lpBuf, " ");
						if (pStart) {
							pStart++;
							if(pStart<pEnd) {
								if (*pStart=='2') {
									byProxyState = PROXY_STATE_OK;
								}
							}
						}
					}
				}
			}
			break;
		}
		if (byProxyState == PROXY_STATE_NONE) {
			//pT->SetLastError(ECONNREFUSED);
		}
		m_ProxyState = byProxyState;
	}

	int SendProxy()
	{
		T* pT = static_cast<T*>(this);
		ASSERT(IsProxy() && IsInProxy());
		if(!IsProxy() || !IsInProxy()) {
			return 0;
		}

		char Buf[1024] = {0};
		int nBufLen = 0;
#ifdef SOCKET_PACKET_FLAG_TEMPBUF
		int nFlags = SOCKET_PACKET_FLAG_TEMPBUF;
#else
		int nFlags = 0;
#endif//

		byte byProxyState = m_ProxyState;
		if (byProxyState%2==0) {
			return 0;
		}

		int ret = 0;
		//��֯�����¼Э��
		switch(m_ProxyInfo.eType)
		{
		case PROXYTYPE_SOCKS4:
		case PROXYTYPE_SOCKS4A:
			{
				byProxyState++;

				const char* lpszHostAddress = m_szHost;
				int nHostAddress = strlen(lpszHostAddress);

				// SOCKS 4
				// ---------------------------------------------------------------------------
				//            +----+----+----+----+----+----+----+----+----+----+....+----+
				//            | VN | CD | DSTPORT |      DSTIP        | USERID       |NULL|
				//            +----+----+----+----+----+----+----+----+----+----+....+----+
				//# of bytes:   1    1      2              4           variable       1
				Buf[0] = 0x04;													// VN: 4
				Buf[1] = (IsConnectProxy()) ? 0x01 : 0x02;						// CD: 1=CONNECT, 2=BIND;
				*(u_short*)&Buf[2] = pT->H2N(m_nPort);							// DSTPORT

				nBufLen = 4 + 4 + 1;

				SOCKADDR_IN sockAddr = {0};
				sockAddr.sin_addr.s_addr = inet_addr(lpszHostAddress);
				if (sockAddr.sin_addr.s_addr == INADDR_NONE && m_ProxyInfo.eType == PROXYTYPE_SOCKS4) {
					LPHOSTENT lpHostent = gethostbyname(lpszHostAddress);
					if (lpHostent) {
						sockAddr.sin_addr.s_addr = ((LPIN_ADDR)lpHostent->h_addr)->s_addr;
					}
				}

				if (sockAddr.sin_addr.s_addr == INADDR_NONE) {
					if (m_ProxyInfo.eType == PROXYTYPE_SOCKS4) {
						ret = SOCKET_ERROR;
						pT->SetLastError(ECONNABORTED);
						break;
					} else {
						// For version 4A, if the client cannot resolve the destination host's
						// domain name to find its IP address, it should set the first three bytes
						// of DSTIP to NULL and the last byte to a non-zero value. (This corresponds
						// to IP address 0.0.0.x, with x nonzero.)

						// DSTIP: Set the IP to 0.0.0.x (x is nonzero)
						Buf[4] = 0;
						Buf[5] = 0;
						Buf[6] = 0;
						Buf[7] = 1;

						Buf[8] = 0;	// Terminating NUL-byte for USERID

						// Following the NULL byte terminating USERID, the client must send the 
						// destination domain name and termiantes it with another NULL byte. 

						// Add hostname (including terminating NUL-byte)
						memcpy(&Buf[9], lpszHostAddress, nHostAddress);
						nBufLen += nHostAddress;
					}
				} else {
					*(u_long*)&Buf[4] = sockAddr.sin_addr.s_addr;				// DSTIP
				}
				Buf[nBufLen-1] = 0;

				ret = pT->Send(Buf, nBufLen, nFlags);
			}
			break;
		case PROXYTYPE_SOCKS5:
			{
				// SOCKS 5
				// -------------------------------------------------------------------------------------------
				// The client connects to the server, and sends a version identifier/method selection message:
				//                +----+----------+----------+
				//                |VER | NMETHODS | METHODS  |
				//                +----+----------+----------+
				//                | 1  |    1     | 1 to 255 |
				//                +----+----------+----------+
				//
				// The values currently defined for METHOD are:
				//
				//       o  X'00' NO AUTHENTICATION REQUIRED
				//       o  X'01' GSSAPI
				//       o  X'02' USERNAME/PASSWORD
				//       o  X'03' to X'7F' IANA ASSIGNED
				//       o  X'80' to X'FE' RESERVED FOR PRIVATE METHODS
				//       o  X'FF' NO ACCEPTABLE METHODS
				switch (byProxyState) 
				{
				case 1:
					{
						byProxyState++;

						Buf[0] = 0x05;
						Buf[1] = (m_ProxyInfo.bAuth ? 0x02 : 0x01);
						Buf[2] = (m_ProxyInfo.bAuth ? 0x02 : 0x00);
						nBufLen = (m_ProxyInfo.bAuth ? 4 : 3);
						ret = pT->Send(Buf, nBufLen, nFlags);
					}
					break;
				case 3:
					{
						byProxyState++;

						char nUserLen = (char)strlen(m_ProxyInfo.szUser);
						char nPwdLen = (char)strlen(m_ProxyInfo.szPwd);
						Buf[0] = 0x05;
						Buf[1] = nUserLen;
						nBufLen = 2;
						memcpy(Buf + nBufLen, m_ProxyInfo.szUser, nUserLen);
						nBufLen += nUserLen;
						Buf[nBufLen] = nPwdLen;
						nBufLen += 1;
						memcpy(Buf + nBufLen, m_ProxyInfo.szPwd, nPwdLen);
						nBufLen += nPwdLen;
						ret = pT->Send(Buf, nBufLen, nFlags);
					}
					break;
				case 5:
					{
						byProxyState++;

						const char* lpszHostAddress = m_szHost;
						int nHostAddress = strlen(lpszHostAddress);
						SOCKADDR_IN sockAddr = {0};
						sockAddr.sin_addr.s_addr = inet_addr(lpszHostAddress);
						if (sockAddr.sin_addr.s_addr == INADDR_NONE) {
							//����DNS��������������������
							/*LPHOSTENT lpHostent = gethostbyname(lpszHostAddress);
							if (lpHostent) {
							sockAddr.sin_addr.s_addr = ((LPIN_ADDR)lpHostent->h_addr)->s_addr;
							}*/
						}
						sockAddr.sin_port = pT->H2N(m_nPort);

						Buf[0] = 0x05;
						Buf[1] = 0x01;
						Buf[2] = 0x00;
						Buf[3] = (sockAddr.sin_addr.s_addr!=INADDR_NONE ? 0x01 : 0x03);
						nBufLen = 4;
						if (sockAddr.sin_addr.s_addr != INADDR_NONE) {
							memcpy(Buf + nBufLen, &sockAddr.sin_addr.s_addr, sizeof(sockAddr.sin_addr.s_addr));
							nBufLen += sizeof(sockAddr.sin_addr.s_addr);
						} else {
							Buf[nBufLen] = (char)strlen(lpszHostAddress);
							nBufLen += 1;
							memcpy(Buf + nBufLen, lpszHostAddress, nHostAddress);
							nBufLen += nHostAddress;
						}
						memcpy(Buf + nBufLen, &sockAddr.sin_port, sizeof(sockAddr.sin_port));
						nBufLen += sizeof(sockAddr.sin_port);

						ret = pT->Send(Buf, nBufLen, nFlags);
					}
					break;
				default:
					break;
				}
			}
			break;
		case PROXYTYPE_HTTP10:
		case PROXYTYPE_HTTP11:
			{
				byProxyState++;

				const char* lpszHostAddress = m_szHost;
				int nHostAddress = strlen(lpszHostAddress);

				sprintf(Buf, "CONNECT %s:%d HTTP/1.%d\r\nHost: %s:%d\r\n",
					lpszHostAddress, m_nPort, /*m_ProxyInfo.nType == PROXYTYPE_HTTP10 ? 0 : */1, lpszHostAddress, m_nPort);

				if (m_ProxyInfo.bAuth)
				{
					char szAuth[1024] = {0};
					char szBase64Encode[1024] = {0};
					//char nNameLen = (char)strlen(m_ProxyInfo.szUser);
					//char nPwdLen = (char)strlen(m_ProxyInfo.szPwd);
					//if (m_ProxyInfo.szPwd[0]) {
						_snprintf(szAuth, 1024, "%s:%s", m_ProxyInfo.szUser, m_ProxyInfo.szPwd);
					//} else {
					//	_snprintf(szAuth, 1024, "%s", m_ProxyInfo.szUser);
					//}
					ProxyHelper::Base64Encode(szAuth, strlen(szAuth), szBase64Encode, 1024);

					strcat(Buf, "Authorization: Basic ");
					strcat(Buf, szBase64Encode);
					strcat(Buf, "\r\n");
					strcat(Buf, "Proxy-Authorization: Basic ");
					strcat(Buf, szBase64Encode);
					strcat(Buf, "\r\n");
				}

				strcat(Buf, "\r\n");

				nBufLen = strlen(Buf);
				ret = pT->Send(Buf, nBufLen, nFlags);
			}
			break;
		}
		if (ret<0) {
			m_ProxyState = PROXY_STATE_NONE;
		} else {
			m_ProxyState = byProxyState;
		}
		return ret;
	}

	/*!
	 *	@brief ����ͬ���������.
	 *
	 *	���������Ҫʹ���߱�֤����IO.
	 */
	int ConnectProxy()
	{
		T* pT = static_cast<T*>(this);

		int ret = 0;
		if (!IsInProxy()) {
			return ret;
		}

		do {
			ret = SendProxy();
			if (ret<0) {
				break;
			}
			ret = ReceiveProxy();
			if (ret<0) {
				break;
			}
		} while (IsInProxy());

		return ret;
	}
};

/*!
 *	@brief ProxyConnectHandler ģ�嶨��.
 *
 *	��װProxyConnectHandler��ʵ�����Ӵ���
 */
template<class TBase = SocketEx>
class ProxyConnectHandler 
	: public TBase
	, public ProxyHandlerT<ProxyConnectHandler<TBase>,PROXY_TYPE_CONNECT>
{
	typedef ProxyConnectHandler<TBase> This;
	typedef TBase Base;
	typedef ProxyHandlerT<ProxyConnectHandler<TBase>,PROXY_TYPE_CONNECT> ProxyHandler;
public:
	ProxyConnectHandler():Base()
	{

	}

	int Close()
	{
		int rlt = Base::Close();
		ProxyHandler::ResetProxyInfo();
		return rlt;
	}

	int Connect(const char* lpszHostAddress, unsigned short nHostPort, PPROXYINFO pProxy = NULL)
	{
		ProxyHandler::SetProxy(lpszHostAddress, nHostPort, pProxy);
		if (ProxyHandler::IsProxy())
		{
			return Base::Connect(ProxyHandler::GetProxyIp(), ProxyHandler::GetProxyPort());
		}
		else
		{
			return Base::Connect(ProxyHandler::GetHostIp(), ProxyHandler::GetHostPort());
		}
	}

protected:
	void OnReceive(int nErrorCode)
	{
		if (ProxyHandler::IsInProxy()) {
			if (nErrorCode) {
				Base::OnConnect(nErrorCode);
			} else {
#if 0
				ProxyHandler::ReceiveProxy();
				//
				if (!ProxyHandler::IsInProxy()) {
					Base::OnConnect(IsProxyOK()?0:ECONNABORTED);
				}
#else
				bool bConitnue = false;
				do {
					bConitnue = false;
					char lpBuf[1025] = {0};
					int nBufLen = 1024;
					int nFlags = 0;
					nBufLen = Base::Receive(lpBuf, nBufLen, nFlags);
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
							bConitnue = true;
							break;
#endif//
						default:
							OnConnect(nErrorCode);
							break;
						}
					} else {
						bConitnue = true;
					}
				} while (bConitnue);
#endif//
			}
		} else {
			Base::OnReceive(ProxyHandler::IsProxyOK()?nErrorCode:ECONNABORTED);
		}
	}

	virtual void OnReceived(const char* lpBuf, int nBufLen, int nFlags) 
	{
		if (ProxyHandler::IsInProxy()) {
			ProxyHandler::OnReceivedProxy(lpBuf, nBufLen, nFlags);
			//
			if (!ProxyHandler::IsInProxy()) {
				Base::OnConnect(IsProxyOK()?0:ECONNABORTED);
			}
		} else {
			Base::OnReceived(lpBuf, nBufLen, nFlags);
		}
	}
	
	void OnSend(int nErrorCode)
	{
		if (ProxyHandler::IsInProxy()) {
			if (nErrorCode) {
				Base::OnConnect(nErrorCode);
			} else {
				int err = ProxyHandler::SendProxy();
				if (err < 0) {
					Base::OnConnect(ECONNABORTED);
				}
			}
		} else {
			Base::OnSend(ProxyHandler::IsProxyOK()?nErrorCode:ECONNABORTED);
		}
	}

	virtual void OnSended(const char* lpBuf, int nBufLen, int nFlags) 
	{
		if (ProxyHandler::IsInProxy()) {
			//
		} else {
			Base::OnSended(lpBuf, nBufLen, nFlags);
		}
	}
	
	void OnConnect(int nErrorCode)
	{
		if (nErrorCode) {
			Base::OnConnect(nErrorCode);
			return;
		}

		if (ProxyHandler::IsInProxy()) {
			//ProxyHandler::SendProxy();
		} else {
			Base::OnConnect(ProxyHandler::IsProxyOK()?0:ECONNABORTED);
		}
	}
};

#endif//_H_XPROXYSOCKETEX_H_