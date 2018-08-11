#include "RobotServerPch.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <netdb.h>
#include <errno.h>
#include "SocketSystem.h"
#include "ClientSocket.h"
#include "IClientSink.h"

#define Error	printf

ClientSocket::ClientSocket():m_nFd(0), m_nLastAction(-1), m_pClientSink( NULL), m_socketSystem(NULL)
{
	m_nFd = socket(AF_INET, SOCK_STREAM, 0);
	if(-1 == m_nFd)
	{
		Error("ClientSocket createError:%d\n", errno);
	}
	
	int val = 0;
    if ((val = fcntl(m_nFd, F_GETFL, 0)) == -1)
    {
    	Error("Create Socker Error\n");
        return;
    }
    val |= O_NONBLOCK;
    if (fcntl(m_nFd, F_SETFL, val) == -1)
    {
    	Error("Set Socket NonBlock Fail");
    }
}

ClientSocket::~ClientSocket()
{
	close(m_nFd);
}

void ClientSocket::parseAddr(const std::string &sAddr, struct in_addr &stSinAddr)
{
    int iRet = inet_pton(AF_INET, sAddr.c_str(), &stSinAddr);
    if(iRet < 0)
    {
        Error("[TC_Socket::parseAddr] inet_pton error:%d", errno);
		return;
    }
    else if(iRet == 0)
    {
        struct hostent stHostent;
        struct hostent *pstHostent;
        char buf[2048] = "\0";
        int iError;

        gethostbyname_r(sAddr.c_str(), &stHostent, buf, sizeof(buf), &pstHostent, &iError);

        if (pstHostent == NULL)
        {
            Error("[TC_Socket::parseAddr] gethostbyname_r error! ");
        }
        else
        {
            stSinAddr = *(struct in_addr *) pstHostent->h_addr;
        }
    }
}


void ClientSocket::Connect(const std::string& strIpAddress, unsigned short wPort)
{
	struct sockaddr stServerAddr;
    bzero(&stServerAddr, sizeof(stServerAddr));

    struct sockaddr_in *p = (struct sockaddr_in *)&stServerAddr;

    p->sin_family = AF_INET;
    parseAddr(strIpAddress, p->sin_addr);
    p->sin_port = htons(wPort);

    int nResult = connect(m_nFd, &stServerAddr, sizeof(stServerAddr));
	if(0 == nResult)
	{
		GetClientSink()->OnConnect(en_Connetc_OK);
		m_socketSystem->ctrl(this, EPOLLOUT|EPOLLIN, EPOLL_CTL_ADD);
		return;
	}

	if(errno == EBADF)
	{
		Error("Create Socket Fail\n");
	}

	m_socketSystem->ctrl(this, EPOLLOUT|EPOLLIN, EPOLL_CTL_ADD);
	m_nLastAction = en_ClientOp_Connect;
}

void ClientSocket::SetClientSink(IClientSink* pClientSink)
{
	m_pClientSink = pClientSink;
}

void ClientSocket::Release()
{
	close(m_nFd);
}

int ClientSocket::GetFD()
{
	return m_nFd;
}

IClientSink* ClientSocket::GetClientSink()
{
	return m_pClientSink;
}

int ClientSocket::GetLastAction()
{
	return m_nLastAction;
}

void ClientSocket::SetLastAction(int nAction)
{
	m_nLastAction = nAction;
}

int ClientSocket::_ReadBuffer(char* pszData, int nLen)
{
	int nReadSumLen = 0;
	while(true)
	{
		int nRdLen = read(m_nFd, pszData + nReadSumLen, nLen - nReadSumLen);
		if(0 == nRdLen)
		{
			return 0;
		}
		else if(-1 == nRdLen)
		{
			if(EAGAIN == errno)
			{
				if(nReadSumLen > 0)
				{
					return nReadSumLen;
				}
				else
				{
					return -1;
				}
			}
			else
			{
				return 0;
			}
		}

		nReadSumLen += nRdLen;
		if(nReadSumLen == nLen)
		{
			break;
		}
	}
	
	return nReadSumLen;
}

void ClientSocket::_SendBuff()
{
	if(m_sendCacheData.length() == 0) 
	{
		m_socketSystem->ctrl(this, EPOLLIN, EPOLL_CTL_MOD);
		return;
	}
	
	while(true)
	{
		int nWriteLen = write(m_nFd, m_sendCacheData.c_str(), m_sendCacheData.length() );
		if(nWriteLen > 0)
		{
			m_sendCacheData = m_sendCacheData.substr(nWriteLen);
		}
		else if(0 == nWriteLen)
		{
			return;
		}
		else
		{
			if(errno == EAGAIN)
            {
                return;
            }

			Error("Send Data Fail\n");
		}
	}
}

void ClientSocket::SendData(const char* pszData, int nLen)
{
	m_sendCacheData.append(pszData, nLen);
	m_socketSystem->ctrl(this, EPOLLOUT|EPOLLIN, EPOLL_CTL_MOD);
	m_nLastAction = en_ClientOp_Send;
}

std::string& ClientSocket::GetReadCacheData()
{
	return m_readCacheData;
}








