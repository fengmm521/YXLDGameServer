#include "SnapshotServer.h"
#include "SnapshotImp.h"


SnapshotServer g_app;

SnapshotServer::SnapshotServer()
{
}


SnapshotServer::~SnapshotServer()
{
}


void SnapshotServer::initialize()
{
	signal(SIGPIPE,SIG_IGN);
	string sSnapshotObj = ServerConfig::Application + "." + ServerConfig::ServerName + ".SnapshotObj";
	addServant<SnapshotImp>(sSnapshotObj);
}

	
void SnapshotServer::destroyApp()
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



