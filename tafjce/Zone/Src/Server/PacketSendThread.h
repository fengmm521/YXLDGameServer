#ifndef __PACKET_SEND_THREAD_H__
#define  __PACKET_SEND_THREAD_H__

#include "MiniApr.h"
#include "servant/Application.h"

#include "MsgCircleQueue.h"


class PacketSendThread:public taf::TC_Thread
{
public:

	// Constructor/Destructor
	PacketSendThread();
	~PacketSendThread();

	// 功能: 停止线程
	void StopSelf();
	
	// TC_Thread Interface
	virtual void run();

	void setMessageQueue(CMsgCircleQueue* pMsgQueue);

private:
	bool	m_bStopThread;
	CMsgCircleQueue* m_pMessageQueue;
};

#endif
