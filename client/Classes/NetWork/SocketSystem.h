#ifndef __SOCKET_SYSTEM_H__
#define __SOCKET_SYSTEM_H__

#include "ClientSocket.h"
#include <set>
#include <map>

namespace cocos2d
{

enum
{
	PACKET_LESS = 0,    //表示收到的包不全
	PACKET_FULL = 1,    //表示已经收到完整的包
	PACKET_ERR  = -1,   //表示协议错误
};

enum
{
	en_Event_Read = 1,
	en_Event_Write = 2,
};

class IPacketParse
{
public:

	// constructor/destructor
	virtual ~IPacketParse(){}

	virtual int parsePacket(std::string& in, std::string& out) = 0;
};

class ISocketSystem
{
public:

	virtual ~ISocketSystem(){}

	virtual bool initlize() = 0;

	virtual IClientSocket* createClientSocket() = 0;

	virtual IPacketParse* setPacketParse(IPacketParse* pNewPacketParse) = 0;

	virtual void runSystem() = 0;

	virtual void terminnate() = 0;
};

extern "C" ISocketSystem* createSocketSystem();

namespace Detail
{

class SocketSystem;

struct UpdateOpInfo
{
	ClientSocket* pClientSocket;
	int iEvent;
};

class SelectThread
{
public:

	SelectThread(SocketSystem* pSocketSystem);

	void updateClientSocketEvent(ClientSocket* pClientSocket, int iEvent);
	void _updateClientSocketEventNoLock(ClientSocket* pClientSocket, int iEvent);
	virtual void run();
	void terminate();

private:

	SocketSystem* m_pSocketSystem;
	bool m_bTerminate;
	int m_iMaxFD;
	fd_set m_rSet;
	fd_set m_wSet;
	fd_set m_eSet;
	std::map<int, ClientSocket*> m_mapClientSocket;

	//std::vector<UpdateOpInfo> m_changeList;
};

class ISockerOperate
{
public:

	ISockerOperate(ClientSocket* pClientSocket):m_socketHandle(pClientSocket->getEventHandle()){}

	virtual ~ISockerOperate(){}
	virtual void execute() = 0;
	virtual ClientSocket* getClientSocket()
	{
		if(!m_socketHandle.get() )
		{
			return NULL;
		}

		return static_cast<ClientSocket*>(m_socketHandle.get() );
	}

private:

	EventHandle::Proxy m_socketHandle;
};

class CloseOperate:public ISockerOperate
{
public:

	CloseOperate(ClientSocket* pClientSocket):ISockerOperate(pClientSocket){}

	virtual void execute()
	{
		if(!getClientSocket() ) return;
	
		getClientSocket()->close();
		getClientSocket()->getSink()->onClosed();
	}
	
private:

	
};

class RecvOperate:public ISockerOperate
{
public:

	RecvOperate(ClientSocket* pClientSocket, const std::string& strData):ISockerOperate(pClientSocket), m_strRcvData(strData){}

	
	virtual void execute()
	{
		if(!getClientSocket() ) return;
		
		getClientSocket()->getSink()->onRecv(m_strRcvData.c_str(), (int)m_strRcvData.size() );
	}

private:

	std::string m_strRcvData;
};

class ConnectOperate:public ISockerOperate
{
public:

	ConnectOperate(ClientSocket* pClientSocket, int iResult):ISockerOperate(pClientSocket), m_iResult(iResult){}

	virtual void execute()
	{
		if(!getClientSocket() ) return;
		getClientSocket()->getSink()->onConnect(m_iResult);
	}

private:

	int m_iResult;
};

class SocketSystem:public ISocketSystem
{
public:

	friend class ClientSocket;

	SocketSystem();
	~SocketSystem();

	virtual bool initlize();
	virtual IClientSocket* createClientSocket();
	virtual IPacketParse* setPacketParse(IPacketParse* pNewPacketParse);
	virtual void runSystem();
	virtual void terminnate();
	void updateSocketEvent(ClientSocket* pClientSocket, int iEvent);
	void addOperate(ISockerOperate* pOperate);

private:

	SelectThread m_selectThread;
	std::list<ISockerOperate*> m_operateList;
	IPacketParse* m_pPacketParse;
	bool m_bStart;
};
}

};

#endif
