#ifndef __CLIENTPLAYER_STATE_H__
#define __CLIENTPLAYER_STATE_H__

class ClientPlayerState_Base: public Detail::EventHandle
{
public:

	virtual ~ClientPlayerState_Base(){}

	// 功能: 进入状态
	virtual void enterState() = 0;

	// 功能: 离开状态
	virtual void leaveState() = 0;

	// 功能: 消息处理
	virtual void onMessage(const GSProto::SCMessage& message) = 0;

	virtual void onUpdate() = 0;

	//virtual int getStateID() = 0;
};

template <class T> void parseAndShowMsg(const GSProto::SCMessage& message)
{
	//return;
	T scTmpMsg;
	if(!scTmpMsg.ParseFromString(message.strmsgbody() ) )
	{
		return;
	}

	FDLOG("Robot")<<"message|"<<message.icmd()<<endl;
	FDLOG("Robot")<<scTmpMsg.DebugString()<<endl;
}


#endif
