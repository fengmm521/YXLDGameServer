#ifndef __SOCKET_SYSTEM_H__
#define __SOCKET_SYSTEM_H__

#include "ISocketSystem.h"
#include "ClientSocket.h"

enum
{
    ET_LISTEN = 1,
    ET_CLOSE  = 2,
    ET_NOTIFY = 3,
    ET_NET    = 0,
};

class SocketSystem:public MINIAPR::ComponentBase<ISocketSystem, IID_ISocektSystem>
{
public:

	friend class ClientSocket;

	// constructor/destructor
	SocketSystem();
	~SocketSystem();

	// IComponent Interface
	virtual bool initlize(const PropertySet& propSet);

	// ISocketSystem Interface
	bool Init(int nMaxCon);
	IClientSocket* CreateClientSocket();
	IPacketParse* SetPacketParse(IPacketParse* pNewPacketParse);
	void RunSystem();

private:

	void ctrl(ClientSocket* pClientSocket, __uint32_t events, int op);
	void ProcessConnect(ClientSocket* pClientSocket, const epoll_event &ev);
	void ProcessRcv(ClientSocket* pClientSocket, const epoll_event &ev);
	void ProcessSend(ClientSocket* pClientSocket, const epoll_event &ev);
private:

	int	m_nMaxCon;
	int	m_nEpollfd;
	struct epoll_event* m_pEvntArray;
	IPacketParse*	m_pPacketParse;
};

#endif
