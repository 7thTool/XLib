#ifndef _H_XDHT_H_
#define _H_XDHT_H_

#include "XLib.h"
#include "XNode.h"

//////////////////////////////////////////////////////////////////////////
//DHT �ֲ�ʽHASH���� ȥ���Ļ�P2P�Ļ�ʯ
//1���ڵ�HASH
//2����ԴHASH
//3��������㣬ͨ��������㣬����1101&~1001=0100�����Ծ������0100
//4��ÿ���ڵ㶼���沿���ڽ��ڵ㣬���磺�ڵ㱣�淶ΧΪ2��i�η���2��i+1�η��Ľڵ�
//5��ÿ���ڵ㶼���沿���ڽ���Դ
//6�����ҽڵ㣬ͨ���ڽ���ӽ��ýڵ�Ľڵ����
//7��������Դ��ͨ���ڽ����п��ܵĽڵ���Ҹ���Դ
//8���½ӵĵķ������½ӵĿ���ͨ���ڵ�洢���ݡ��㲥��NATServer�����ŵ����ķ������ȸ��ַ�ʽ�����Լ��Ĵ��ڣ�����ȡ�ڽ��ڵ�
//
//��ע��
//1��ʵ���˷ֲ�ʽ�ڵ����Դ�ĺ���ֲ��洢
//2��ʵ���˽ڵ�Ĳ����㷨
//3��ʵ������Դ�Ĳ����㷨
//4���ڵ�ķ������ڽ��ڵ��ȡ
//

//��ed2k�������棬���ǵ�id��ͨ��ip�������µ��㷨����ó���
//�����ǵ�IP = A.B.C.D
//��ô���ǵ�ID number= A + 256*B + 256*256*C + 256*256*256*D
//low ID�Ĳ������������ǵ�ID������С��16777216.
//�� ID number= A + 256*B + 256*256*C + 256*256*256*D < 16777216
//Kad������㷽��ID number=256*256*256*A+256*256*B+256*C+D

struct PeerInfo
{
	//ver
	//type
	char id[32]; //�ڵ�HASH
	char ip[32]; //ip
	int port; //port
};

struct ResourceInfo
{
	//ver
	//type
	char id[32];	//��ԴHASH
	char uri[260];	//��ԴΨһ��ʾ
};

template<class T,class TBase = XNode>
class Peer : public XNode2<T,TBase>
{
public:
};

class Resource : public XNode2<Resource>
{
public:
};

//ÿ���ڵ����ж�����Դ
class DHTPeer : public Peer<DHTPeer>
{
public:
	//������Դ���Է���Ϊ������Դ,���磺
	//�ļ�����Ϣ�����׵ȵ�
	Resource m_resx; //�ڵ��µ���Դ
	Resource m_res1;
	Resource m_res2;
};

//ÿ���ڵ��¶�Ҫ�����ڽ��ڵ���Ϣ
class DHTManager
{
public:
	DHTPeer m_peer;
};


#endif//_H_XDHT_H_