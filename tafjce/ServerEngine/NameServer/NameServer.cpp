#include "NameServer.h"
#include "NameImp.h"

NameServer g_app;

NameServer::NameServer()
{
}

NameServer::~NameServer()
{
}
	
void NameServer::initialize()
{
	signal(SIGPIPE,SIG_IGN);
	string sNameObj = ServerConfig::Application + "." + ServerConfig::ServerName + ".NameObj";
	addServant<NameImp>(sNameObj);
}
	
void NameServer::destroyApp()
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


