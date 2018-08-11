#include "RouterServer.h"
#include "RouterComm.h"
#include "PushImp.h"
#include "RouterImp.h"
#include "TimeoutThread.h"
#include "ServerManager.h"

extern int64_t makeConKey(const string& strIp, int iPort);

RouterServer g_app;
void RouterServantHandle::handleClose(const TC_EpollServer::tagRecvData &stRecvData)
{
	int64_t ddConID = makeConKey(stRecvData.ip, stRecvData.port);

	ConnectionInfoPtr conPtr = ConnectionManager::getInstance()->get(ddConID);
	if(!conPtr)
	{
		return;
	}
	
	FDLOG("connection")<<"connection closed|"<<conPtr->strAccount<<"|"<<stRecvData.ip<<"|"<<stRecvData.port<<endl;
	
	ConnectionManager::getInstance()->destroyConn(conPtr);
	ConnectionManager::getInstance()->erase(ddConID);
}

RouterServer::RouterServer()
{
}

RouterServer::~RouterServer()
{
}

void RouterServer::setHandle(TC_EpollServer::BindAdapterPtr& adapter)
{
	adapter->setHandle<RouterServantHandle>();
}

void RouterServer::initialize()
{
	// 启动超时检测线程
	TimeoutThread::getInstance()->start();

	// 初始化GameServer检测Manager
	ServerManager::getInstance()->start();

	// 添加RouterObj
	string strRouterObject = ServerConfig::Application + "." + "RouterServer" + ".RouterObj";
	addServant<RouterImp>(strRouterObject);
	addServantProtocol(strRouterObject, &AppProtocol::parseStream<0, unsigned short, true>);

	RLOG<<"addobject|"<<strRouterObject<<endl;

	// 添加PushObj
	string strPushObject = ServerConfig::Application + "." + "RouterServer" + ".PushObj";
	addServant<PushImp>(strPushObject);
}

void RouterServer::destroyApp()
{
	
}


int main(int argc, char** argv)
{
	try
	{
		g_app.main(argc, argv);
		g_app.waitForShutdown();
	}
	catch (std::exception& e)
	{
		cerr << "std::exception:" << e.what() << std::endl;
	}
	catch (...)
	{
		cerr << "unknown exception." << std::endl;
	}

	return 0;
}



