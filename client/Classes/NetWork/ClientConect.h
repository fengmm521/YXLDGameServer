#pragma once

#include "FmConfig.h"
#include "ClientSocket.h"
#include "SocketSystem.h"
#include "RouterServer.pb.h"

using namespace cocos2d;

NS_FM_BEGIN

ISocketSystem* GetSocketSystem();
IClientSocket* GetClientSocket();
void InitSocket();

class StreamParse:public cocos2d::IPacketParse
{
public:

	virtual int parsePacket(std::string& in, std::string& out)
	{
		out = in;
		in.clear();

		return cocos2d::PACKET_FULL;
	}
};



class StreamParse2:public IPacketParse
{
public:

	virtual int parsePacket(std::string& in, std::string& out)
	{
		if(in.size() < 2)
		{
			return PACKET_LESS;
		}

		unsigned short wLen = *(unsigned short*)in.c_str();
		wLen = ntohs(wLen);
		if(wLen < 2)
		{
			return PACKET_ERR;
		}

		if(in.size() < (size_t)wLen)
		{
			return PACKET_LESS;
		}

		out = in.substr(0, wLen);
		in = in.substr(wLen);

		return PACKET_FULL;
	}
};

class ClientSink:public IClientSink
{
public:
	SINGLETON_MODE(ClientSink);

	ClientSink();

	virtual void onConnect(int nErrorCode);
	

	virtual void onRecv(const char* pszData, int nLen);
	

	virtual void onClosed();

	void sendStringPacket(const char* pStr, int nLen );

	static void OnAuth( int iCmd, ServerEngine::SCMessage& pkg);

	static void Connect(string address, int port);

	void SetIsConnectGameServer(bool isConnectGameServer){m_isConnectGameServer = isConnectGameServer;}
	bool IsConnect();
	bool m_isConnectGameServer;

	void SendPkg(ServerEngine::CSMessage& outPkg);
};

static void sendPkg(ServerEngine::CSMessage& outPkg);

NS_FM_END