#include "RoleServer.h"
#include "RoleImp.h"
#include "SyncSettingThread.h"

RoleServer g_app;

SyncSettingThread g_syncSettingThread;



RoleServer::RoleServer()
{
}

RoleServer::~RoleServer()
{
}
	
void RoleServer::initialize()
{
	signal(SIGPIPE,SIG_IGN);
	string sNameObj = ServerConfig::Application + "." + ServerConfig::ServerName + ".RoleObj";
	addServant<RoleImp>(sNameObj);
}
	
void RoleServer::destroyApp()
{
	
}


int main(int argc, char** argv)
{
	try
	{
		g_app.main(argc, argv);
		g_app.waitForShutdown();
		g_syncSettingThread.terminate();
		g_syncSettingThread.getThreadControl().join();
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

