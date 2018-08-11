#ifndef __NET_WORK_H__
#define __NET_WORK_H__

//#include "cocos2d.h"
#include "Thread.h"
#include "Lock.h"
#include <vector>
#include <string>

#include "RefCountAutoPtr.h"

#ifndef WIN32
	#include <errno.h>
	#include <sys/socket.h>
	#include <arpa/inet.h>
	#include <netdb.h>
	#include <fcntl.h>
	#include <sys/types.h>
#endif


namespace cocos2d
{

enum
{
	en_Connetc_OK = 0,
	en_Connect_Fail,
};


class IClientSink
{
public:

	virtual ~IClientSink(){}

	virtual void onConnect(int nErrorCode) = 0;

	// 功能: 收到数据处理
	// 参数: [pszData] 数据开始地址
	// 参数: [nLen] 数据长度
	virtual void onRecv(const char* pszData, int nLen) = 0;

	// 功能: Socket被关闭通知
	virtual void onClosed() = 0; 
};

class IClientSocket:public EventHandle
{
public:
	virtual ~IClientSocket(){}
	
	virtual void connect(const std::string& strIpAddress, unsigned short wPort) = 0;

	virtual void setClientSink(IClientSink* pClientSink) = 0;

	virtual void sendData(const char* pszData, int nLen) = 0;

	virtual void close() = 0;

	virtual bool isConnect() = 0;
};


namespace Detail
{
	class SocketSystem;
	class ClientSocket:public IClientSocket
	{
	public:
		
		friend class SelectThread;
		friend class SocketSystem;
		
		ClientSocket(SocketSystem*);
		~ClientSocket();

		virtual void connect(const std::string& strIpAddress, unsigned short wPort);
		virtual void setClientSink(IClientSink* pClientSink);
		virtual void sendData(const char* pszData, int nLen);
		virtual void close();
		virtual bool isConnect(){return m_bConnected;}

		bool checkConnect();
		IClientSink* getSink(){return m_pClientSink;}
		void _recvData(const char* pBuff, int iLen);
		void _sendData();
		void parseAddr(const std::string &sAddr, struct in_addr &stSinAddr);
		bool wouldBlock();
		bool connectInProgress();
		void markValidUUID();
		bool checkOperateValid();
		
	private:
		int m_iFD;
		bool m_bConnected;
		IClientSink* m_pClientSink;
		bool m_bConnecting;
		SocketSystem* m_pSocketSystem;
		std::string m_strCacheSendData;
		std::string m_strCacheRcvData;
	};
};


}

#endif

