#include "GameServer.h"
#include "GameServantImp.h"
#include "MiniApr.h"
#include "DumpHelper.h"
//#include "ICleanUpManager.h"
#include "ComponentConstDef.h"
#include "WatchThread.h"
#include <sys/ptrace.h>
#include <sys/wait.h>


using namespace MINIAPR;
using namespace std;

Uint32 g_lastUpdateTime;



WatchThread::WatchThread():m_bStopThread(false)
{
}

WatchThread::~WatchThread()
{
}

void WatchThread::run()
{
	TC_Common::ignorePipe();
	while(!m_bStopThread)
	{
		TC_Config conf;
	    conf.parseFile(ServerConfig::ServerName + ".conf");
		int iDeadLoopWatch = TC_Common::strto<int>(conf.get("/Zone<DeadLoopWatch>","0"));
		
		if(0 == iDeadLoopWatch)
		{
			sleep(1);
			continue;
		}

		Uint32 dwCurTime = time(NULL);
		if(dwCurTime > (g_lastUpdateTime + 35) )
		{
			/*ICleanUpManager* pCleanMgr = getComponent<ICleanUpManager>(COMPNAME_CleanUpManager, IID_ICleanUpManager);
			assert(pCleanMgr);

			pCleanMgr->doCleanUp();*/
			
		
			DumpHelper tmpHelper(true);
			tmpHelper.DumpGdbLog();
			FDLOG("DeadLock")<<"find Dead Lock, please check gdbdumpLog"<<endl;
			g_lastUpdateTime = dwCurTime;
			sleep(5);
			break;
		}
		usleep(100);
	}
}

void WatchThread::StopSelf()
{
	m_bStopThread = true;
}




