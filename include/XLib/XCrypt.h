#ifndef _H_XCRYPT_H_
#define _H_XCRYPT_H_

#include "XLib.h"
//#include "XThread.h"

class XLIB_API XCrypt
{
public:
	static void Init();
	static void Release();
};

typedef void* SSL_HANDLE;

class XLIB_API XSSL
{
protected:
	XSSL(void);
public:
	//��ʼ��OpenSSL����
	static int  InitOpenSSLCondition(void);
	//�ͷ�OpenSSL����
	static void ReleaseOpenSSLCondition(void);

	//��ʼ��SSL������
	static long InitSSL(const char *lpszCACertFile, const char *lpszUserCertFile, const char *lpszUserCertPassword);
	//У��֤��
	static long VerifyCert(const char* lpszUserCertFile, const char* lpszUserCertPassword);
	//����SSL������
	static void CleanSSL();
	//����SSL����
	static SSL_HANDLE New(int fd);
	//�ͷ�SSL����
	static void Free(SSL_HANDLE handle);
};

//////////////////////////////////////////////////////////////////////////

//RSA��һ��Internet�������Ȩϵͳ����ʹ��Ron Rivest, Adi Shamir, ��Leonard Adleman��1977�꿪����һ���㷨��RSA�㷨�����ձ�ʹ�õ�
//�������Ȩ�㷨�������ѱ�������Microsoft��Netscape��Web������С���Ҳ��Lotus��Notes��Intuit��Quicken����������Ʒ��һ���֡�
//�������ϵͳ��RSA Security���С������˾��Ȩ����㷨��ͬʱ�������俪����������������Ѵ��ڻ������Web��Internet�������׼��һ���֡�
//һ���ǹ�Կ���ܣ�˽Կ����
class XLIB_API XSSL_RSA
{
protected:
public:
	XSSL_RSA(void);
	virtual ~XSSL_RSA();
public:
	//�����ܳ׶ԣ����׺�˽�ף�������Ϊ�̶���140�ֽڣ�˽��һ����605--611�ֽ����ң���һ��Ӧ�÷���1024�ֽ����ϡ�
	//ע�⣺�����ܳ׳��ȼ�����ڲ�����Ҳ�ǳ��ڲ�������ʾ������ɵ����볤��
	//����>=0��ʾ�ɹ�
	static int  CreateKey(unsigned char * lpPublicKey,unsigned short * sPublicKeySize,unsigned char * lpPrivateKey,unsigned short * sPrivateKeySize);
	//���ù��׼��ܣ����ܰ���100�ֽ�Ϊһ��Ϊ��λ������128�ֽڵ����Ŀ飬ע�⣺���ܱ�100�ֽ������Ĳ��֣������м��ܣ���258�ֽ����ģ�����128+128+58�ֽڵĽ��������ǰ
	//2��128�ֽ��Ǽ��ܵģ�������ʹ�ù����У�������ĳ���Ϊ128������������ͬ��
	static int  EncryptByPublicKey(unsigned char * lpInBuf,unsigned long lInSize,unsigned char * lpOutBuf,unsigned long lOutSize,unsigned char * lpPublicKey,unsigned short sPublicKeySize);
	//���ù��׽���
	static int  DecryptByPublicKey(unsigned char * lpInBuf,unsigned long lInSize,unsigned char * lpOutBuf,unsigned long lOutSize,unsigned char * lpPublicKey,unsigned short sPublicKeySize);
	//����˽�׼���
	static int  EncryptByPrivateKey(unsigned char * lpInBuf,unsigned long lInSize,unsigned char * lpOutBuf,unsigned long lOutSize,unsigned char * lpPrivateKey,unsigned short sPrivateKeySize);
	//����˽�׽���
	static int  DecryptByPrivateKey(unsigned char * lpInBuf,unsigned long lInSize,unsigned char * lpOutBuf,unsigned long lOutSize,unsigned char * lpPrivateKey,unsigned short sPrivateKeySize);
};

//DES �����㷨�Ƿ����㷨�� DES �Ļ��������ǰ� 64 ���������� 56 ������Կָ���¼��ܳ� 64 �������ġ���ʵ��ʹ���а���Կ���� 64 ���ؿ��Ը����㡣
//���ԣ����ܡ���������8�ֽڣ�64bit��Ϊ��λ�����ܱ�8�����Ĳ��֣������м��ܡ�
//ע�⣺���ڷ�������ж��ַ�ʽ����ECB��CBC��CFB��OFB,��������ṩECB��ʽ��
class XLIB_API XSSL_DES
{
protected:
public:
	XSSL_DES(void);
	virtual ~XSSL_DES();
public:
	//DES ECB��ʽ���ܣ�����8�ֽ�
	static int  ECB_Encrypt(unsigned char * lpInBuf,unsigned long lInSize,unsigned char * lpOutBuf,unsigned long lOutSize,unsigned char * lpKey);
	//DES ECB��ʽ���ܣ�����8�ֽ�
	static int  ECB_Decrypt(unsigned char * lpInBuf,unsigned long lInSize,unsigned char * lpOutBuf,unsigned long lOutSize,unsigned char * lpKey);
	//3DES ECB��ʽ���ܣ�����24�ֽ�
	static int  ECB_Encrypt3(unsigned char * lpInBuf,unsigned long lInSize,unsigned char * lpOutBuf,unsigned long lOutSize,unsigned char * lpKey);
	//3DES ECB��ʽ���ܣ�����24�ֽ�
	static int  ECB_Decrypt3(unsigned char * lpInBuf,unsigned long lInSize,unsigned char * lpOutBuf,unsigned long lOutSize,unsigned char * lpKey);
};

//IDEA���������ݼ����㷨��Ҳ�Ƿ�������㷨����Ҫ����128bit������64bit����Ϊ��λ��������
class XLIB_API XSSL_IDEA
{
protected:
public:
	XSSL_IDEA(void);
	virtual ~XSSL_IDEA();
public:
	//IDEA ECB��ʽ���ܣ�����16�ֽ�
	static int  ECB_Encrypt(unsigned char * lpInBuf,unsigned long lInSize,unsigned char * lpOutBuf,unsigned long lOutSize,unsigned char * lpKey);
	//IDEA ECB��ʽ���ܣ�����16�ֽ�
	static int  ECB_Decrypt(unsigned char * lpInBuf,unsigned long lInSize,unsigned char * lpOutBuf,unsigned long lOutSize,unsigned char * lpKey);
};

//AES���߼����ܱ�׼��Ҳ�Ƿ����㷨�����ĳ���֧��128bit,192bit,256bit����Ҫ����128bitΪ��λ���м��ܡ�
class XLIB_API XSSL_AES
{
protected:
public:
	XSSL_AES(void);
	virtual ~XSSL_AES();
public:
	//AES ECB��ʽ���ܣ�����16�ֽ�
	static int  ECB_Encrypt128(unsigned char * lpInBuf,unsigned long lInSize,unsigned char * lpOutBuf,unsigned long lOutSize,unsigned char * lpKey);
	//AES ECB��ʽ���ܣ�����16�ֽ�
	static int  ECB_Decrypt128(unsigned char * lpInBuf,unsigned long lInSize,unsigned char * lpOutBuf,unsigned long lOutSize,unsigned char * lpKey);
	//AES ECB��ʽ���ܣ�����24�ֽ�
	static int  ECB_Encrypt192(unsigned char * lpInBuf,unsigned long lInSize,unsigned char * lpOutBuf,unsigned long lOutSize,unsigned char * lpKey);
	//AES ECB��ʽ���ܣ�����24�ֽ�
	static int  ECB_Decrypt192(unsigned char * lpInBuf,unsigned long lInSize,unsigned char * lpOutBuf,unsigned long lOutSize,unsigned char * lpKey);
	//AES ECB��ʽ���ܣ�����32�ֽ�
	static int  ECB_Encrypt256(unsigned char * lpInBuf,unsigned long lInSize,unsigned char * lpOutBuf,unsigned long lOutSize,unsigned char * lpKey);
	//AES ECB��ʽ���ܣ�����32�ֽ�
	static int  ECB_Decrypt256(unsigned char * lpInBuf,unsigned long lInSize,unsigned char * lpOutBuf,unsigned long lOutSize,unsigned char * lpKey);
};

//BlowFish�㷨������������ѧר�Ҳ�³˹��ʩ�Ͷ�����������һ������64λ���鼰�ɱ���Կ����[32-448λ]�ķ��������㷨��
class XLIB_API XSSL_BLOWFISH
{
protected:
public:
	XSSL_BLOWFISH(void);
	virtual ~XSSL_BLOWFISH();
public:
	//BLOWFISH ECB��ʽ���ܣ����볤�ȿɱ䣨4�ֽ�--56�ֽڣ�
	static int  ECB_Encrypt(unsigned char * lpInBuf,unsigned long lInSize,unsigned char * lpOutBuf,unsigned long lOutSize,unsigned char * lpKey,int iKeySize);
	//BLOWFISH ECB��ʽ���ܣ����볤�ȿɱ䣨4�ֽ�--56�ֽڣ�
	static int  ECB_Decrypt(unsigned char * lpInBuf,unsigned long lInSize,unsigned char * lpOutBuf,unsigned long lOutSize,unsigned char * lpKey,int iKeySize);
};

//MD5ȫ��"message-digest algorithm 5"(��Ϣ-ժҪ�㷨)�� 90�������MIT�������ѧʵ���Һ�RSA Data Security Inc���Ͽ����� 
//MD5�㷨�����ⳤ�ȵ���������128bit��ժҪ��
class XLIB_API XSSL_MD5
{
protected:
public:
	XSSL_MD5(void);
	virtual ~XSSL_MD5();
public:
	//���128bit=16�ֽڵ�ժҪ
	static int  MD5(unsigned char * lpInBuf,unsigned long lInSize,unsigned char * lpOutBuf);
};

//SHA����Secure Hash Algorithm����ȫɢ���㷨�������������ұ�׼�뼼������1993����Ϊ������Ϣ�����׼�������Ȱ汾SHA-1��SHA-2�� 
//���ܲ���160bit����ϢժҪ�����б�MD5��ǿ�İ�ȫ�ԡ� 
class XLIB_API XSSL_SHA1
{
protected:
public:
	XSSL_SHA1(void);
	virtual ~XSSL_SHA1();
public:
	//���160bit=20�ֽڵ�ժҪ
	static int  SHA1(unsigned char * lpInBuf,unsigned long lInSize,unsigned char * lpOutBuf);
};

//�ļ���ʽ����

//����˽Կ��֤�飺
//��Public Key Cryptography Standards #12��PKCS#12��׼���壬�����˹�Կ��˽Կ�Ķ����Ƹ�ʽ��֤����ʽ����pfx��Ϊ֤���ļ���׺����
//��Կ���ܼ���12�ű�׼��Public Key Cryptography Standards #12��PKCS#12��Ϊ�洢�ʹ����û��������˽Կ����Կ��֤��ָ����һ������ֲ�ĸ�ʽ��
//����һ�ֶ����Ƹ�ʽ����Щ�ļ�Ҳ��ΪPFX�ļ���P12�ļ���

//�����Ʊ����֤�飺֤����û��˽Կ��DER ��������Ƹ�ʽ��֤���ļ�����cer��Ϊ֤���ļ���׺����
//�ֲ�ʽ�������Distinguished Encoding Rules��DER����ʽҲ���԰���˽Կ����Կ��֤�顣���Ǵ�����������Ĭ�ϸ�ʽ��
//���Ұ���ASN1 DER��ʽ���д洢����û��ͷ������PEM���ı�ͷ����װ��DER�� 

//Base64�����֤�飺֤����û��˽Կ��BASE64 �����ʽ��֤���ļ�����cer��pem��Ϊ֤���ļ���׺����
//��ǿ��˽���ʼ���Privacy Enhanced Mail��PEM����ʽ���ڸ����������Կ��ʽ�����ҿ��԰���˽Կ��RSA��DSA������Կ��RSA��DSA����x509֤�顣
//���洢ASCIIͷ����װ��Base64����DER��ʽ�����ݣ�������������ϵͳ֮����ı�ģʽ���䡣
/*class XLIB_API XSSL_Certificate
{
protected:
	void				*	m_X509Handle;
public:
	XSSL_Certificate(void);
	virtual ~XSSL_Certificate();
public:
	//��Cer֤���ļ�
	int  OpenCerFile(MString strFileName);
	//�ر�֤���ļ�
	void CloseFile(void);
public:
	//��ȡ֤��İ汾��
	int  GetVersion(void);
	//��ȡ֤������к�
	MString GetSerial(void);
	//��ȡ֤���ǩ���㷨
	MString GetSignatureAlgorithm(void);
	//��ȡ֤��ķ�����
	MString GetIssuer(void);
	//��ȡ֤�������
	MString GetSubject(void);
	//��ȡ֤�����Ч��
	int  GetValidateDate(MDateTime * mStartDate,MDateTime * mEndDate);
	//��ȡ֤��Ĺ�Կ
	int  GetPublicKey(unsigned char * lpOutBuf,unsigned short sInSize);
};*/

#endif//_H_XCRYPT_H_
