#ifndef __CLIENT_SOCKET_H__
#define  __CLIENT_SOCKET_H__

#include "IClientSocket.h"

enum
{
	en_ClientOp_Connect = 0,
	en_ClientOp_Close,
	en_ClientOp_Send,
};

class SocketSystem;
class ClientSocket:public IClientSocket
{
public:

	// constructor/destructor
	ClientSocket();
	~ClientSocket();

	// IClientSocket Interface
	void Connect(const std::string& strIpAddress, unsigned short wPort);
	void SetClientSink(IClientSink* pClientSink);
	void SendData(const char* pszData, int nLen);
	int GetSocketId(){return GetFD();}
	void Release();
	
	int GetFD();
	IClientSink* GetClientSink();

	int GetLastAction();
	void SetLastAction(int nAction);

	int _ReadBuffer(char* pszData, int nLen);
	void _SendBuff();
	std::string& GetReadCacheData();
	void parseAddr(const std::string &sAddr, struct in_addr &stSinAddr);
	void SetSocketSystem(SocketSystem* pSocketSystem){m_socketSystem = pSocketSystem;}
private:

	int				m_nFd;
	int				m_nLastAction;
	std::string		m_readCacheData;
	std::string		m_sendCacheData;
	IClientSink*	m_pClientSink;
	SocketSystem*	m_socketSystem;
};

#endif
