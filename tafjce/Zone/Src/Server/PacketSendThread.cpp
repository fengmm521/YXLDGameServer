#include "GameServer.h"
#include "GameServantImp.h"
#include "IMessageDispatch.h"
//#include "cs_msg_layer.h"
#include "MessageWrapper.h"
//#include "DirSvrInterface.h"
#include "IUserStateManager.h"
//#include "ICleanUpManager.h"
#include "Role.h"
#include "IEntity.h"
#include "IZoneTime.h"
#include "svr_log.h"
#include "ComponentIdDef.h"
#include "WatchThread.h"
#include "PacketSendThread.h"
#include "FakeRandom.h"
//#include "Proxy.h"
#include "IMessageLayer.h"
#include "Push.h"

#define MAX_SCPACKET_LEN	(64*1024)

extern string g_OssPushObj;

PacketSendThread::PacketSendThread():m_bStopThread(false), m_pMessageQueue(NULL)
{
}

PacketSendThread::~PacketSendThread()
{
}

void PacketSendThread::setMessageQueue(CMsgCircleQueue* pMsgQueue)
{
	m_pMessageQueue = pMsgQueue;
}

void PacketSendThread::run()
{
	char szMaxBuff[MAX_SCPACKET_LEN] = {0};
	WORD wResultSize = MAX_SCPACKET_LEN-1;
	INT64 iConnPos = 0;
	std::string strRsAddr;
	while(!m_bStopThread)
	{
		bool bResult = m_pMessageQueue->PopMsg(szMaxBuff ,wResultSize, iConnPos, strRsAddr);
		if(!bResult)
		{
			usleep(1);
			continue;
		}
		assert(strRsAddr.size() );
		assert(wResultSize < MAX_SCPACKET_LEN-1);
		try
		{
			ServerEngine::PushPrx prx = NULL;
			Application::getCommunicator()->stringToProxy(strRsAddr, prx);
			prx->async_doPush(NULL, iConnPos, string(szMaxBuff, wResultSize) );
		}
		catch(std::exception& e)
		{
			SvrErrLog("%s", e.what() );
		}
	}
}

void PacketSendThread::StopSelf()
{
	m_bStopThread = true;
}




