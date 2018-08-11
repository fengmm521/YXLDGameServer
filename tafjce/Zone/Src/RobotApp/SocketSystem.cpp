#include "RobotServerPch.h"
#include "SocketSystem.h"

extern "C" ISocketSystem* CreateSocketSystem(Int32)
{
	return new SocketSystem;
}


SocketSystem::SocketSystem():m_nMaxCon(100), m_nEpollfd(0), m_pEvntArray(NULL), m_pPacketParse(NULL)
{
}

SocketSystem::~SocketSystem()
{
}

bool SocketSystem::initlize(const PropertySet& propSet)
{
	Init(15000);
	return true;
}


bool SocketSystem::Init(int nMaxCon)
{
	if(m_nEpollfd)
	{
		SvrErrLog("SocketSystem Have Inited\n");
		return false;
	}

	struct rlimit tmpLimit;
	tmpLimit.rlim_cur = 20000;
	tmpLimit.rlim_max = 20000;
	if(0 != setrlimit(RLIMIT_NOFILE, &tmpLimit) )
	{
		SvrErrLog("set rlimit Fail|%d", errno);
		assert(false);
		return false;
	}

	m_nMaxCon = nMaxCon;
	m_nEpollfd = epoll_create(nMaxCon + 1);

	m_pEvntArray = new epoll_event[m_nMaxCon + 1];
	
	return true;
}

IClientSocket* SocketSystem::CreateClientSocket()
{
	ClientSocket* pClientSocket = new ClientSocket;
	pClientSocket->SetSocketSystem(this);
	
	return pClientSocket;
}

void SocketSystem::ctrl(ClientSocket* pClientSocket, __uint32_t events, int op)
{
	struct epoll_event ev;
	ev.data.ptr = pClientSocket;
    ev.events   = events;
	epoll_ctl(m_nEpollfd, op, pClientSocket->GetFD(), &ev);

	if(EPOLL_CTL_DEL == op)
	{
		SvrRunLog("ctrl fd[%d] op[%d] events:[%d]", pClientSocket->GetFD(), op, events);
	}
}


IPacketParse* SocketSystem::SetPacketParse(IPacketParse* pNewPacketParse)
{
	IPacketParse* pPrePacketParse = pNewPacketParse;
	m_pPacketParse = pNewPacketParse;
	
	return pPrePacketParse;
}

void SocketSystem::ProcessConnect(ClientSocket* pClientSocket, const epoll_event &ev)
{
	if(!pClientSocket || !(pClientSocket->GetClientSink()) )
	{
		return;
	}

	if(ev.events & EPOLLOUT)
	{
		pClientSocket->GetClientSink()->OnConnect(en_Connetc_OK);
	}
	else if( (ev.events & EPOLLERR) || (ev.events & EPOLLHUP) )
	{
		ctrl(pClientSocket, 0, EPOLL_CTL_DEL);
		pClientSocket->GetClientSink()->OnConnect(en_Connect_Fail);	
	}
}

void SocketSystem::ProcessRcv(ClientSocket* pClientSocket, const epoll_event &ev)
{
	if(ev.events & EPOLLIN)
	{
		char szMaxBuff[64*1024] = {0};
		int nRdLen = pClientSocket->_ReadBuffer(szMaxBuff, sizeof(szMaxBuff) );
		if(0 == nRdLen)
		{
			ctrl(pClientSocket, 0, EPOLL_CTL_DEL);
			pClientSocket->GetClientSink()->OnClosed();
			return;
		}
		else if(nRdLen > 0)
		{
			std::string& strCacheData = pClientSocket->GetReadCacheData();
			strCacheData.append(szMaxBuff, nRdLen);
			while(true)
			{	
				std::string strOutData;
				int nResult = m_pPacketParse->ParsePacket(strCacheData, strOutData);
				if(PACKET_FULL == nResult)
				{
					pClientSocket->GetClientSink()->OnRecv(strOutData.c_str(), strOutData.length() );
				}
				else if(PACKET_LESS == nResult)
				{
					break;
				}
				else if(PACKET_ERR == nResult)
				{
					SvrErrLog("rcv Data Error\n");
				}
			}
		}
	}

	if(ev.events & EPOLLOUT)
	{
		ProcessSend(pClientSocket, ev);
	}
}

void SocketSystem::ProcessSend(ClientSocket* pClientSocket, const epoll_event &ev)
{
	if(ev.events & EPOLLOUT)
	{
		pClientSocket->_SendBuff();
	}
}

void SocketSystem::RunSystem()
{
	ITimerComponent* pTimerComponent = getComponent<ITimerComponent>(COMPNAME_TimeAxis, IID_ITimerComponent);
	assert(pTimerComponent);
	
	//while(1)
	{
		int nEventNum = epoll_wait(m_nEpollfd, m_pEvntArray, m_nMaxCon + 1, 200);
		for(int i = 0; i < nEventNum; i++)
		{
			const epoll_event &ev = m_pEvntArray[i];
			ClientSocket* pClientSocket = static_cast<ClientSocket*>(ev.data.ptr);
			if(!pClientSocket)
			{
			    SvrErrLog("Client Socket is NULL\n");
				continue;
			}

			int nLastOp = pClientSocket->GetLastAction();
			
			switch(nLastOp)
			{
				case en_ClientOp_Connect:
					ProcessConnect(pClientSocket, ev);
					break;
					
				case en_ClientOp_Close:
					break;
				case en_ClientOp_Send:
					//ProcessSend(pClientSocket, ev);
					ProcessRcv(pClientSocket, ev);
					break;
				default:
					ProcessRcv(pClientSocket, ev);
					break;
			}
			pClientSocket->SetLastAction(-1);
			pTimerComponent->runTimer();
		}
		pTimerComponent->runTimer();
	}
}


