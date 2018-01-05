#ifndef _H_XDHT_H_
#define _H_XDHT_H_

#include "XLib.h"
#include "XNode.h"

//////////////////////////////////////////////////////////////////////////
//DHT 分布式HASH技术 去中心化P2P的基石
//1、节点HASH
//2、资源HASH
//3、距离计算，通过异或运算，比如1101&~1001=0100，所以距离就是0100
//4、每个节点都保存部分邻近节点，例如：节点保存范围为2的i次方及2的i+1次方的节点
//5、每个节点都保存部分邻近资源
//6、查找节点，通过邻近最接近该节点的节点查找
//7、查找资源，通过邻近最有可能的节点查找该资源
//8、新接的的发布，新接的可以通过节点存储数据、广播，NATServer，可信的中心服务器等各种方式声明自己的存在，并获取邻近节点
//
//备注：
//1、实现了分布式节点和资源的合理分布存储
//2、实现了节点的查找算法
//3、实现了资源的查找算法
//4、节点的发布和邻近节点获取
//

//在ed2k网络里面，我们的id是通过ip进行如下的算法计算得出的
//设我们的IP = A.B.C.D
//那么我们的ID number= A + 256*B + 256*256*C + 256*256*256*D
//low ID的产生是由于我们的ID计算结果小于16777216.
//即 ID number= A + 256*B + 256*256*C + 256*256*256*D < 16777216
//Kad里面计算方法ID number=256*256*256*A+256*256*B+256*C+D

struct PeerInfo
{
	//ver
	//type
	char id[32]; //节点HASH
	char ip[32]; //ip
	int port; //port
};

struct ResourceInfo
{
	//ver
	//type
	char id[32];	//资源HASH
	char uri[260];	//资源唯一标示
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

//每个节点下有多种资源
class DHTPeer : public Peer<DHTPeer>
{
public:
	//关于资源可以分类为各种资源,比如：
	//文件，消息，交易等等
	Resource m_resx; //节点下的资源
	Resource m_res1;
	Resource m_res2;
};

//每个节点下都要保存邻近节点信息
class DHTManager
{
public:
	DHTPeer m_peer;
};


#endif//_H_XDHT_H_