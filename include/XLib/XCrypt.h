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
	//初始化OpenSSL环境
	static int  InitOpenSSLCondition(void);
	//释放OpenSSL环境
	static void ReleaseOpenSSLCondition(void);

	//初始化SSL上下文
	static long InitSSL(const char *lpszCACertFile, const char *lpszUserCertFile, const char *lpszUserCertPassword);
	//校验证书
	static long VerifyCert(const char* lpszUserCertFile, const char* lpszUserCertPassword);
	//清理SSL上下文
	static void CleanSSL();
	//创建SSL对象
	static SSL_HANDLE New(int fd);
	//释放SSL对象
	static void Free(SSL_HANDLE handle);
};

//////////////////////////////////////////////////////////////////////////

//RSA是一个Internet加密与鉴权系统，它使用Ron Rivest, Adi Shamir, 及Leonard Adleman在1977年开发的一个算法。RSA算法是最普遍使用的
//加密与鉴权算法，并且已被包含在Microsoft和Netscape的Web浏览器中。它也是Lotus的Notes、Intuit的Quicken及其他许多产品的一部分。
//这个加密系统归RSA Security所有。这个公司授权这个算法，同时还销售其开发包。这个技术是已存在或被提议的Web、Internet及计算标准的一部分。
//一般是公钥加密，私钥解密
class XLIB_API XSSL_RSA
{
protected:
public:
	XSSL_RSA(void);
	virtual ~XSSL_RSA();
public:
	//创建密匙对（公匙和私匙），公匙为固定的140字节，私匙一般在605--611字节左右，但一般应该分配1024字节以上。
	//注意：两个密匙长度即是入口参数，也是出口参数，表示最后生成的密码长度
	//返回>=0表示成功
	static int  CreateKey(unsigned char * lpPublicKey,unsigned short * sPublicKeySize,unsigned char * lpPrivateKey,unsigned short * sPrivateKeySize);
	//利用公匙加密，加密按照100字节为一块为单位，生成128字节的密文块，注意：不能被100字节整除的部分，不进行加密，如258字节明文，生成128+128+58字节的结果，其中前
	//2个128字节是加密的，所以在使用过程中，最好明文长度为128整数倍数，下同。
	static int  EncryptByPublicKey(unsigned char * lpInBuf,unsigned long lInSize,unsigned char * lpOutBuf,unsigned long lOutSize,unsigned char * lpPublicKey,unsigned short sPublicKeySize);
	//利用公匙解密
	static int  DecryptByPublicKey(unsigned char * lpInBuf,unsigned long lInSize,unsigned char * lpOutBuf,unsigned long lOutSize,unsigned char * lpPublicKey,unsigned short sPublicKeySize);
	//利用私匙加密
	static int  EncryptByPrivateKey(unsigned char * lpInBuf,unsigned long lInSize,unsigned char * lpOutBuf,unsigned long lOutSize,unsigned char * lpPrivateKey,unsigned short sPrivateKeySize);
	//利用私匙解密
	static int  DecryptByPrivateKey(unsigned char * lpInBuf,unsigned long lInSize,unsigned char * lpOutBuf,unsigned long lOutSize,unsigned char * lpPrivateKey,unsigned short sPrivateKeySize);
};

//DES 加密算法是分组算法。 DES 的基本操作是把 64 比特明文在 56 比特密钥指引下加密成 64 比特密文。在实际使用中把密钥看作 64 比特可以更方便。
//所以，加密、加密是以8字节（64bit）为单位，不能被8整除的部分，不进行加密。
//注意：对于分组加密有多种方式，如ECB、CBC、CFB、OFB,这里仅仅提供ECB方式。
class XLIB_API XSSL_DES
{
protected:
public:
	XSSL_DES(void);
	virtual ~XSSL_DES();
public:
	//DES ECB方式加密，密码8字节
	static int  ECB_Encrypt(unsigned char * lpInBuf,unsigned long lInSize,unsigned char * lpOutBuf,unsigned long lOutSize,unsigned char * lpKey);
	//DES ECB方式解密，密码8字节
	static int  ECB_Decrypt(unsigned char * lpInBuf,unsigned long lInSize,unsigned char * lpOutBuf,unsigned long lOutSize,unsigned char * lpKey);
	//3DES ECB方式加密，密码24字节
	static int  ECB_Encrypt3(unsigned char * lpInBuf,unsigned long lInSize,unsigned char * lpOutBuf,unsigned long lOutSize,unsigned char * lpKey);
	//3DES ECB方式解密，密码24字节
	static int  ECB_Decrypt3(unsigned char * lpInBuf,unsigned long lInSize,unsigned char * lpOutBuf,unsigned long lOutSize,unsigned char * lpKey);
};

//IDEA（国际数据加密算法）也是分组加密算法，主要是用128bit密码以64bit分组为单位加密数据
class XLIB_API XSSL_IDEA
{
protected:
public:
	XSSL_IDEA(void);
	virtual ~XSSL_IDEA();
public:
	//IDEA ECB方式加密，密码16字节
	static int  ECB_Encrypt(unsigned char * lpInBuf,unsigned long lInSize,unsigned char * lpOutBuf,unsigned long lOutSize,unsigned char * lpKey);
	//IDEA ECB方式解密，密码16字节
	static int  ECB_Decrypt(unsigned char * lpInBuf,unsigned long lInSize,unsigned char * lpOutBuf,unsigned long lOutSize,unsigned char * lpKey);
};

//AES（高级加密标准）也是分组算法。密文长度支持128bit,192bit,256bit。主要是以128bit为单位进行加密。
class XLIB_API XSSL_AES
{
protected:
public:
	XSSL_AES(void);
	virtual ~XSSL_AES();
public:
	//AES ECB方式加密，密码16字节
	static int  ECB_Encrypt128(unsigned char * lpInBuf,unsigned long lInSize,unsigned char * lpOutBuf,unsigned long lOutSize,unsigned char * lpKey);
	//AES ECB方式解密，密码16字节
	static int  ECB_Decrypt128(unsigned char * lpInBuf,unsigned long lInSize,unsigned char * lpOutBuf,unsigned long lOutSize,unsigned char * lpKey);
	//AES ECB方式加密，密码24字节
	static int  ECB_Encrypt192(unsigned char * lpInBuf,unsigned long lInSize,unsigned char * lpOutBuf,unsigned long lOutSize,unsigned char * lpKey);
	//AES ECB方式解密，密码24字节
	static int  ECB_Decrypt192(unsigned char * lpInBuf,unsigned long lInSize,unsigned char * lpOutBuf,unsigned long lOutSize,unsigned char * lpKey);
	//AES ECB方式加密，密码32字节
	static int  ECB_Encrypt256(unsigned char * lpInBuf,unsigned long lInSize,unsigned char * lpOutBuf,unsigned long lOutSize,unsigned char * lpKey);
	//AES ECB方式解密，密码32字节
	static int  ECB_Decrypt256(unsigned char * lpInBuf,unsigned long lInSize,unsigned char * lpOutBuf,unsigned long lOutSize,unsigned char * lpKey);
};

//BlowFish算法由著名的密码学专家部鲁斯·施耐尔所开发，是一个基于64位分组及可变密钥长度[32-448位]的分组密码算法。
class XLIB_API XSSL_BLOWFISH
{
protected:
public:
	XSSL_BLOWFISH(void);
	virtual ~XSSL_BLOWFISH();
public:
	//BLOWFISH ECB方式加密，密码长度可变（4字节--56字节）
	static int  ECB_Encrypt(unsigned char * lpInBuf,unsigned long lInSize,unsigned char * lpOutBuf,unsigned long lOutSize,unsigned char * lpKey,int iKeySize);
	//BLOWFISH ECB方式解密，密码长度可变（4字节--56字节）
	static int  ECB_Decrypt(unsigned char * lpInBuf,unsigned long lInSize,unsigned char * lpOutBuf,unsigned long lOutSize,unsigned char * lpKey,int iKeySize);
};

//MD5全称"message-digest algorithm 5"(信息-摘要算法)。 90年代初由MIT计算机科学实验室和RSA Data Security Inc联合开发。 
//MD5算法将任意长度的数据生成128bit的摘要。
class XLIB_API XSSL_MD5
{
protected:
public:
	XSSL_MD5(void);
	virtual ~XSSL_MD5();
public:
	//输出128bit=16字节的摘要
	static int  MD5(unsigned char * lpInBuf,unsigned long lInSize,unsigned char * lpOutBuf);
};

//SHA（即Secure Hash Algorithm，安全散列算法），由美国国家标准与技术局于1993年做为联邦信息处理标准公布，先版本SHA-1，SHA-2。 
//它能产生160bit的信息摘要，具有比MD5更强的安全性。 
class XLIB_API XSSL_SHA1
{
protected:
public:
	XSSL_SHA1(void);
	virtual ~XSSL_SHA1();
public:
	//输出160bit=20字节的摘要
	static int  SHA1(unsigned char * lpInBuf,unsigned long lInSize,unsigned char * lpOutBuf);
};

//文件形式保存

//带有私钥的证书：
//由Public Key Cryptography Standards #12，PKCS#12标准定义，包含了公钥和私钥的二进制格式的证书形式，以pfx作为证书文件后缀名。
//公钥加密技术12号标准（Public Key Cryptography Standards #12，PKCS#12）为存储和传输用户或服务器私钥、公钥和证书指定了一个可移植的格式。
//它是一种二进制格式，这些文件也称为PFX文件或P12文件。

//二进制编码的证书：证书中没有私钥，DER 编码二进制格式的证书文件，以cer作为证书文件后缀名。
//分布式编码规则（Distinguished Encoding Rules，DER）格式也可以包含私钥、公钥和证书。它是大多数浏览器的默认格式，
//并且按照ASN1 DER格式进行存储。它没有头部——PEM是文本头部包装的DER。 

//Base64编码的证书：证书中没有私钥，BASE64 编码格式的证书文件，以cer或pem作为证书文件后缀名。
//增强型私人邮件（Privacy Enhanced Mail，PEM）格式现在更多地用作密钥格式，并且可以包含私钥（RSA和DSA）、公钥（RSA和DSA）和x509证书。
//它存储ASCII头部包装的Base64编码DER格式的数据，所以它适用于系统之间的文本模式传输。
/*class XLIB_API XSSL_Certificate
{
protected:
	void				*	m_X509Handle;
public:
	XSSL_Certificate(void);
	virtual ~XSSL_Certificate();
public:
	//打开Cer证书文件
	int  OpenCerFile(MString strFileName);
	//关闭证书文件
	void CloseFile(void);
public:
	//读取证书的版本号
	int  GetVersion(void);
	//获取证书的序列号
	MString GetSerial(void);
	//获取证书的签名算法
	MString GetSignatureAlgorithm(void);
	//获取证书的发布者
	MString GetIssuer(void);
	//获取证书的主题
	MString GetSubject(void);
	//获取证书的有效期
	int  GetValidateDate(MDateTime * mStartDate,MDateTime * mEndDate);
	//获取证书的公钥
	int  GetPublicKey(unsigned char * lpOutBuf,unsigned short sInSize);
};*/

#endif//_H_XCRYPT_H_
