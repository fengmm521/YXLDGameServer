#ifndef __ICLIENT_SINK_H__
#define __ICLIENT_SINK_H__


enum
{
	en_Connetc_OK = 0,
	en_Connect_Fail,
};

class IClientSink
{
public:

	// 功能: 虚析构类
	virtual ~IClientSink(){}

	// 功能: 连接结果回调
	// 参数: [nErrorCode] 错误码
	virtual void OnConnect(int nErrorCode) = 0;

	// 功能: 收到数据处理
	// 参数: [pszData] 数据开始地址
	// 参数: [nLen] 数据长度
	virtual void OnRecv(const char* pszData, int nLen) = 0;

	// 功能: Socket被关闭通知
	virtual void OnClosed() = 0;
};

#endif
