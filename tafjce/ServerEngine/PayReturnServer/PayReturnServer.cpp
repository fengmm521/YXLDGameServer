#include "PayReturnServer.h"
#include "PayReturnImp.h"

PayReturnServer g_app;

PayReturnServer::PayReturnServer()
{
}

PayReturnServer::~PayReturnServer()
{
}
	
void PayReturnServer::initialize()
{
	signal(SIGPIPE,SIG_IGN);
	string sNameObj = ServerConfig::Application + "." + ServerConfig::ServerName + ".PayReturnObj";
	addServant<PayReturnImp>(sNameObj);
}
	
void PayReturnServer::destroyApp()
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

