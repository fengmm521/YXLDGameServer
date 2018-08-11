#ifndef __ISOCKET_SYSTEM_H__
#define __ISOCKET_SYSTEM_H__


#include "IClientSocket.h"
#include "IClientSink.h"

enum
{
	 PACKET_LESS = 0,    //表示收到的包不全
     PACKET_FULL = 1,    //表示已经收到完整的包
     PACKET_ERR  = -1,   //表示协议错误
};

class IPacketParse
{
public:

	// constructor/destructor
	virtual ~IPacketParse(){}

	virtual int ParsePacket(std::string& in, std::string& out) = 0;
};

#define IID_ISocektSystem	9999

class ISocketSystem:public MINIAPR::IComponent
{
public:

	// 功能: 虚析构函数
	virtual ~ISocketSystem(){}

	// 功能: 初始化SocketSystem
	// 参数: [nMaxCon] 最大连接数
	virtual bool Init(int nMaxCon) = 0;

	// 功能: 创建客户端Socket对象
	virtual IClientSocket* CreateClientSocket() = 0;

	// 功能: 设置网络系统包解析函数
	// 参数: [pNewPacketParse] 新的解析函数
	// 返回: 老的解析函数
	virtual IPacketParse* SetPacketParse(IPacketParse* pNewPacketParse) = 0;
	
	// 功能: 运行系统
	virtual void RunSystem() = 0;
};

ISocketSystem* CreateSocketSystem();

#endif
