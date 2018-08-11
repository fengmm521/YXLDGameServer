#include "QQYYBPayServer.h"
#include "QQYYBPayImp.h"

QQYYBPayServer g_app;
TC_HttpAsync g_httpAsync;


QQYYBPayServer::QQYYBPayServer()
{
}

QQYYBPayServer::~QQYYBPayServer()
{
}
	
void QQYYBPayServer::initialize()
{
	signal(SIGPIPE,SIG_IGN);
	string sPayObj = ServerConfig::Application + "." + ServerConfig::ServerName + ".PayObj";
	addServant<QQYYBPayImp>(sPayObj);
}
	
void QQYYBPayServer::destroyApp()
{
	
}


int main(int argc, char** argv)
{
	try
	{
        g_httpAsync.setTimeout(10000);
        g_httpAsync.start(10);
		
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
