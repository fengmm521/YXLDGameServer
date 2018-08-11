#ifndef __IMESSAGE_DISPATCH_H__
#define __IMESSAGE_DISPATCH_H__


#define IID_IMessageListener		MAKE_RID('m','g','l','n')

class IMessageListener:public IComponent
{
public:

	// 功能: 处理消息回调
	// 参数: [dwMsgID] 消息ID
	// 参数: [pMsgBuff] 消息结构地址
	// 参数: [nBuffLen] 消息长度
	virtual void onMessage(Uint32 dwMsgID, const char* pMsgBuff, Int32 nBuffLen) = 0;

	// 功能: 获取支持的消息列表
	virtual std::vector<Uint32> getSupportMsgList() = 0;
};

#define IID_IMessageDispatch	MAKE_RID('m','g','d','c')

class IMessageDispatch:public IComponent
{
public:

	// 功能: 注册一个Listerner
	// 参数: [pListerner] 消息监听者对象
	virtual void registerListener(IMessageListener* pListerner) = 0;

	// 功能: 派发消息
	// 参数: [dwMsgID] 消息ID
	// 参数: [pMsgBuff] 消息地址
	// 参数: [nBuffLen] 消息长度
	// 返回值: 返回true,有做消息处理,返回false,没有做处理
	virtual bool dispatchMessage(Uint32 dwMsgID, const char* pMsgBuff, Int32 nBuffLen) = 0;
};


#endif

