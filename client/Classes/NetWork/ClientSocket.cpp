#ifdef WIN32
#include <winsock2.h>
#include <Ws2tcpip.h>
#include <Windows.h>
#endif

#include "cocos2d.h"
#include <stdio.h>
#include "RefCountAutoPtr.h"
#include "ClientSocket.h"
#include "SocketSystem.h"


#ifdef WIN32
	#pragma comment(lib,"Ws2_32.lib")
#endif
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS) || (CC_TARGET_PLATFORM == CC_PLATFORM_MAC)
    #include <netdb.h>
#endif
#include "Script/FmScriptSys.h"




using namespace cocos2d;
using namespace cocos2d::Detail;


ClientSocket::ClientSocket(SocketSystem* pSocketSystem):m_iFD(0), m_bConnected(false), m_pClientSink(NULL), 
	m_bConnecting(false), m_pSocketSystem(pSocketSystem)
{
}

ClientSocket::~ClientSocket()
{
	if(m_iFD > 0)
	{
		close();
	}
}

void ClientSocket::_recvData(const char* pBuff, int iLen)
{
	{
		//ClientSocket::Lock guard(*this);
		m_strCacheRcvData.append(pBuff, iLen);
	}

	IPacketParse* pPacketParse = m_pSocketSystem->m_pPacketParse;
	assert(pPacketParse);

	while(m_strCacheRcvData.size() > 0)
	{	
		int nResult = 0;
		std::string strOutData;
		{
			//ClientSocket::Lock guard(*this);
			nResult = pPacketParse->parsePacket(m_strCacheRcvData, strOutData);
		}

		if(PACKET_FULL == nResult)
		{
			m_pSocketSystem->addOperate(new RecvOperate(this, strOutData) );
		}
		else if(PACKET_LESS == nResult)
		{
			break;
		}
		else if(PACKET_ERR == nResult)
		{
			CCLOGERROR("rcv data Fail\n");
			//SvrErrLog("rcv Data Error\n");
		}
	}
}

bool ClientSocket::checkConnect()
{
	assert(m_bConnecting);	
	int so_error = 0;
#ifdef WIN32
	int slen = sizeof(int);
#else
	socklen_t slen = sizeof so_error;
#endif
	if(0 == getsockopt(m_iFD, SOL_SOCKET, SO_ERROR, (char*)&so_error, &slen) )
	{
		if (so_error == 0) 
		{
			m_bConnecting = false;
			m_bConnected = true;
			m_pSocketSystem->addOperate(new ConnectOperate(this, en_Connetc_OK) );
			m_pSocketSystem->updateSocketEvent(this, en_Event_Read);
			CCLOG("connect error|%d", en_Connetc_OK);
			return true;
		}
		else if(so_error != 0)
		{
			m_bConnecting = false;
			m_bConnected = false;
			m_pSocketSystem->addOperate(new ConnectOperate(this, en_Connect_Fail) );
			m_pSocketSystem->updateSocketEvent(this, 0);
			CCLOG("connect error|%d, %d, %d", en_Connect_Fail, so_error, errno);
			return false;
		}
	}

	return false;
}

void ClientSocket::_sendData()
{
	//ClientSocket::Lock guard(*this);
	if(!m_bConnected)
	{
		return;
	}
	
	while(m_strCacheSendData.size() > 0)
	{
		int nWriteLen = send(m_iFD, m_strCacheSendData.c_str(), m_strCacheSendData.length(), 0);
		if(nWriteLen > 0)
		{
			m_strCacheSendData = m_strCacheSendData.substr(nWriteLen);
		}
		else if(0 == nWriteLen)
		{
			break;
		}
		else if (nWriteLen < 0)
		{
			if(wouldBlock() )
			{
				break;
			}
			else
			{
				// socket 关闭了
				//m_pSocketSystem->addOperate(new CloseOperate(this) );
			}
		}
	}

	// 去掉写通知
	if(m_strCacheSendData.size() == 0)
	{
		m_pSocketSystem->updateSocketEvent(this, en_Event_Read);
	}
}

bool ClientSocket::wouldBlock()
{
#ifdef _WIN32
    int error = WSAGetLastError();
    return error == WSAEWOULDBLOCK  || error == ERROR_IO_PENDING;
#else
    return errno == EAGAIN || errno == EWOULDBLOCK;
#endif
}


void ClientSocket::close()
{
	CCLOG("close socket:%d", m_iFD);
	//ClientSocket::Lock guard(*this);
	if(m_iFD > 0)
	{
		m_bConnected = false;
		m_bConnecting = false;
		m_pSocketSystem->updateSocketEvent(this, 0);
		
#ifdef WIN32
		//shutdown(m_iFD, SD_BOTH);
		::closesocket(m_iFD);
#else
		::shutdown(m_iFD, SHUT_RDWR);
		::close(m_iFD);
#endif
		m_iFD = 0;
	}
	m_strCacheRcvData.clear();
	m_strCacheSendData.clear();
}

void ClientSocket::parseAddr(const std::string &sAddr, struct in_addr &stSinAddr)
{
    int iRet = inet_pton(AF_INET, sAddr.c_str(), &stSinAddr);
    if(iRet < 0)
    {
        CCLOGERROR("[parseAddr] inet_pton error:%d", errno);
		return;
    }
    else if(iRet == 0)
    {
        struct hostent stHostent;
        struct hostent *pstHostent;
        char buf[2048] = "\0";
#ifdef WIN32
		pstHostent = gethostbyname(sAddr.c_str());
		if (pstHostent == NULL)
		{
			throw GenericException("gethostbyname_r error! ");
		}
		stHostent = *pstHostent;
		memcpy ((char *) &stSinAddr.s_addr,
			(char *) stHostent.h_addr,
			stHostent.h_length);
#else
		
        #if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS) || (CC_TARGET_PLATFORM == CC_PLATFORM_MAC)
        pstHostent = gethostbyname(sAddr.c_str());
        #else
        int iError = 0;
        gethostbyname_r(sAddr.c_str(), &stHostent, buf, sizeof(buf), &pstHostent, &iError);
		
        
        #endif
        if (pstHostent == NULL)
		{
			throw GenericException("gethostbyname_r error! ");
		}
		else
		{
			stSinAddr = *(struct in_addr *) pstHostent->h_addr;
		}
      
#endif
        
    }
}



void ClientSocket::connect(const std::string& strIpAddress, unsigned short wPort)
{
	// 每次Connect创建一个新的socket
	
	if(m_iFD != 0)
	{
		close();
	}
	m_iFD = ::socket(AF_INET, SOCK_STREAM, 0);
	int iErrNO = errno;

	if(m_iFD < 0)
	{
		CCLOG("errno:%d", errno);
		assert(m_iFD >= 0);
	}
	else
	{
		CCLOG("create client socket|%d", m_iFD);
	}

	#ifdef WIN32
		unsigned long ul = 1;
		int nRet = ioctlsocket(m_iFD, FIONBIO, (unsigned long *) &ul);
		if (nRet == SOCKET_ERROR)
		{
			throw GenericException("set nonblock fail");
		}
	#else
		if (fcntl(m_iFD, F_SETFL, O_NONBLOCK) == -1)
	    {
	        throw GenericException("set nonblock fail");
	    }
	#endif
	struct sockaddr stServerAddr;
    //bzero(&stServerAddr, sizeof(stServerAddr));
	memset(&stServerAddr, 0, sizeof(stServerAddr));
    struct sockaddr_in *p = (struct sockaddr_in *)&stServerAddr;

    p->sin_family = AF_INET;
    parseAddr(strIpAddress, p->sin_addr);
    p->sin_port = htons(wPort);
    int atmp = wPort;
    int nResult = ::connect(m_iFD, &stServerAddr, sizeof(stServerAddr));
	m_bConnecting = true;
	if(0 == nResult)
	{
		m_bConnecting = false;
		m_bConnected = true;
		m_pClientSink->onConnect(en_Connetc_OK);
		m_pSocketSystem->updateSocketEvent(this, en_Event_Read);
	}
	else if (!connectInProgress() )
	{
		close();
        
        try {
            throw SysCallException(__FILE__, __LINE__, errno);
        } catch (SysCallException &excep) {
            
			//CCScene *pScene = CCDirector::sharedDirector()->getRunningScene();
			//CCLabelTTF* pLable = CCLabelTTF::create();
			//pLable->setString( excep.what());
           // pLable->setPosition(ccp(480, 320));
            //pScene->addChild(pLable,1000000);
        }
		
        
	}

	m_pSocketSystem->updateSocketEvent(this, en_Event_Read|en_Event_Write);
}

bool ClientSocket::connectInProgress()
{
#ifdef _WIN32
	int error = WSAGetLastError();
	return error == WSAEWOULDBLOCK || error == WSA_IO_PENDING || error == ERROR_IO_PENDING;
#else
	return errno == EINPROGRESS;
#endif
}

void ClientSocket::setClientSink(IClientSink* pClientSink)
{
	m_pClientSink = pClientSink;
}

void ClientSocket::sendData(const char* pszData, int nLen)
{
	if(!m_bConnected)
	{
		CCLOGERROR("socket not connected");
		Freeman::ScriptSys::GetInstance().Execute("Socket_Connect");
		return;
	}

	if(m_iFD == 0)
	{
		CCLOG("socket closed");
		return;
	}

	{
		//ClientSocket::Lock guard(*this);
		m_strCacheSendData.append(pszData, nLen);
	}

	m_pSocketSystem->updateSocketEvent(this, en_Event_Read|en_Event_Write);
}





