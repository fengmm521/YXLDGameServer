#ifndef __ICLIENT_SOCKET_H__
#define __ICLIENT_SOCKET_H__

#include <string>

class IClientSink;
class IClientSocket
{
public:

	// 功能: 虚析构类
	virtual ~IClientSocket(){}

	// 功能: 执行Connect操作
	// 参数: [strIpAddress] 远程ID地址
	// 参数: [wPort] 端口
	virtual void Connect(const std::string& strIpAddress, unsigned short wPort) = 0;

	// 功能: 设置客户端Socket处理对象
	// 参数: [pClientSink] 客户端处理对象
	virtual void SetClientSink(IClientSink* pClientSink) = 0;

	// 功能: 发送数据
	// 参数: [pszData] 数据
	// 参数: [nLen] 长度
	virtual void SendData(const char* pszData, int nLen) = 0;

	virtual int GetSocketId() = 0;
	
	// 功能: 释放ClientSocket对象
	virtual void Release() = 0;
};

#endif
