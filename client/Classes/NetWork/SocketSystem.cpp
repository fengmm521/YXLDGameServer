#include "cocos2d.h"
#include "RefCountAutoPtr.h"
#include "ClientSocket.h"
#include "SocketSystem.h"

#include "stdio.h"
#include "stdlib.h"

#ifndef WIN32
#include <errno.h>
#include <sys/socket.h>
#endif

using namespace std;
using namespace cocos2d;
using namespace cocos2d::Detail;


extern "C" ISocketSystem* createSocketSystem()
{
	return new SocketSystem;
}

SelectThread::SelectThread(SocketSystem* pSocketSystem):m_pSocketSystem(pSocketSystem), m_bTerminate(false), m_iMaxFD(0)
{
	FD_ZERO(&m_rSet);
	FD_ZERO(&m_wSet);
	FD_ZERO(&m_eSet);
}

void SelectThread::updateClientSocketEvent(ClientSocket* pClientSocket, int iEvent)
{
	_updateClientSocketEventNoLock(pClientSocket, iEvent);
}

void SelectThread::_updateClientSocketEventNoLock(ClientSocket* pClientSocket, int iEvent)
{
	assert(pClientSocket);

	int iFD = pClientSocket->m_iFD;
	if(0 == iFD) return;

	// linux下预先判断下FD的合法性
#ifndef WIN32
	if( (iFD >= FD_SETSIZE) || (iFD < 0) )
	{
		stringstream ss;
		ss<<"invalid socket fd|"<<iFD;
		throw GenericException(ss.str() );
		return;
	}
#endif
	CCLOG("update socket event|%d|%d", iFD, iEvent);
	if( ( (iEvent & en_Event_Read) == 0) && ((iEvent & en_Event_Write) == 0) )
	{
		// 如果FD存在在列表中，去掉
		FD_CLR(iFD, &m_rSet);
		FD_CLR(iFD, &m_wSet);
		FD_CLR(iFD, &m_eSet);
		if(m_mapClientSocket.find(iFD) != m_mapClientSocket.end() )
		{
			m_mapClientSocket.erase(iFD);	
		}
		return;
	}

	if(m_mapClientSocket.find(iFD) == m_mapClientSocket.end() )
	{
		m_mapClientSocket[iFD] = pClientSocket;
		if(iFD > m_iMaxFD) m_iMaxFD = iFD;
	}

	FD_CLR(iFD, &m_rSet);
	FD_CLR(iFD, &m_wSet);
	
	
	FD_SET(iFD, &m_eSet);

	if(iEvent & en_Event_Read)
	{
		FD_SET(iFD, &m_rSet);
	}

	if(iEvent & en_Event_Write)
	{
		FD_SET(iFD, &m_wSet);
	}
}

void SelectThread::run()
{
	#define MAX_BUFFER_SIZE  8192


	if(m_mapClientSocket.size() == 0)
	{
		return;
	}

	fd_set tmpRSet = m_rSet;
	fd_set tmpWSet = m_wSet;
	fd_set tmpESet = m_eSet;
		
	struct timeval tv;
	tv.tv_sec = 0;
	tv.tv_usec = 100;
	int iReady = select(m_iMaxFD + 1, &tmpRSet, &tmpWSet, &tmpESet, &tv);
	if(m_bTerminate)
	{
		return;
	}
		
	if(iReady < 0)
	{
		#ifdef WIN32
			assert(WSAGetLastError() != SOCKET_ERROR);
			return;
		#else
			if(iReady == -1)
			{
				assert(errno == EINTR);
				return;
			}
		#endif
		//CCLOGERROR("select fail|%d", iReady);
	}
	else if(iReady == 0)
	{
		return;
	}
	else if(iReady > 0)
	{
		for(std::map<int, ClientSocket*>::iterator it = m_mapClientSocket.begin(); it != m_mapClientSocket.end();)
		{
			ClientSocket* pTmpClientSocket = it->second;
			assert(pTmpClientSocket);
			int iTmpFD = it->first;

			it++;
				
			if(pTmpClientSocket->m_bConnecting)
			{
				pTmpClientSocket->checkConnect();
				continue;
			}

			
			assert(pTmpClientSocket->m_iFD == iTmpFD);
			if(FD_ISSET(iTmpFD, &tmpRSet) || FD_ISSET(iTmpFD, &tmpESet) )
			{
				char szRdBuff[MAX_BUFFER_SIZE];
				int ret = ::recv(iTmpFD, szRdBuff, MAX_BUFFER_SIZE, 0);
				if (ret == 0 || (ret < 0 && errno != EAGAIN))
				{
					updateClientSocketEvent(pTmpClientSocket, 0);
					m_pSocketSystem->addOperate(new CloseOperate(pTmpClientSocket) );
					continue;
				}
				else
				{
					pTmpClientSocket->_recvData(szRdBuff, ret);
				}
			}

			// 发送数据，可以在这个线程里做
			if(FD_ISSET(iTmpFD, &tmpWSet) )
			{
				pTmpClientSocket->_sendData();
			}
		}
	}
	
}

void SelectThread::terminate()
{
	m_bTerminate = true;
}

void SocketSystem::addOperate(ISockerOperate* pOperate)
{
	m_operateList.push_back(pOperate);
}
	
SocketSystem::SocketSystem():m_selectThread(this), m_pPacketParse(NULL), m_bStart(false)
{
}

SocketSystem::~SocketSystem()
{
}

bool SocketSystem::initlize()
{
#ifdef WIN32
	WSADATA wsd;
	WSAStartup(MAKEWORD(2,2), &wsd);
#endif

	// 都放到主线程吧
	m_bStart = true;

	return true;
}
	
IClientSocket* SocketSystem::createClientSocket()
{
	return new ClientSocket(this);
}

IPacketParse* SocketSystem::setPacketParse(IPacketParse* pNewPacketParse)
{
	IPacketParse* pOldPacketParse = m_pPacketParse;
	m_pPacketParse = pNewPacketParse;

	return pOldPacketParse;
}

void SocketSystem::runSystem()
{
	
	for(;;)
	{
		m_selectThread.run();

		if(m_operateList.size() == 0)
		{
			break;
		}

		ISockerOperate* pOperate = m_operateList.front();
		assert(pOperate);

		m_operateList.pop_front();

		pOperate->execute();

		delete pOperate;
	}
}

void SocketSystem::terminnate()
{
	if(m_bStart)
	{
		m_bStart = false;
	}
}

void SocketSystem::updateSocketEvent(ClientSocket* pClientSocket, int iEvent)
{
	if(!m_bStart) return;

	m_selectThread.updateClientSocketEvent(pClientSocket, iEvent);
}




