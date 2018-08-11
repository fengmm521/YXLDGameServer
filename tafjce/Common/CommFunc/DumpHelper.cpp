#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <execinfo.h>
#include <time.h>
#include <assert.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/resource.h>
#include "DumpHelper.h"
#include <iostream>
#include <sstream>
#include <string>

using namespace std;
static int s_logCount = 0;

DumpHelper::DumpHelper(bool bEnableCore)
{
}

DumpHelper::~DumpHelper()
{
}

void DumpHelper::OnCrash(int nSigno)
{
}

int DumpHelper::GetThreadCount()
{
	char szCommand[1024] = {0};
	snprintf(szCommand, sizeof(szCommand), "cat /proc/%d/status|grep Threads >thread.log", getpid() );
	system(szCommand);
	
	FILE* pFile = fopen("./thread.log", "rb");
	if(!pFile) return 1;
	
	char szThreadData[1024] = {0};
	fseek(pFile, 0, SEEK_END);
	long dataSize = ftell(pFile);
	if(dataSize > (long)sizeof(szThreadData))
	{
		dataSize = sizeof(szThreadData);
	}
	fseek(pFile, 0, SEEK_SET);
	fread(szThreadData, dataSize, 1, pFile);
	fclose(pFile);

	int nThreadCount = 1;
	sscanf(szThreadData, "Threads:\t%d", &nThreadCount);
	system("rm -f thread.log");

	return nThreadCount;
}

void DumpHelper::DumpBackTrace()
{
	#define NAX_FRAME_SIZE		100

	void* szStackFrame[NAX_FRAME_SIZE];
    int nFrameCount = backtrace(szStackFrame, NAX_FRAME_SIZE);
    char** strFrameInfo = backtrace_symbols(szStackFrame, nFrameCount);
    char szDumpFileName[1024] = {0};
    snprintf(szDumpFileName, sizeof(szDumpFileName), "dump_%u.log", (unsigned int)time(NULL) );
    FILE* pFile = fopen(szDumpFileName, "wb");
    if(!pFile) return;
    for(int i = 0; i < nFrameCount; i++)
    {
    	fprintf(pFile, "%s\n", strFrameInfo[i]);
    }

	fclose(pFile);
    free(strFrameInfo);
}

void DumpHelper::DumpGdbLog()
{
	stringstream ossSqlHelper;
	// first Get Thread Count
	int nThreadCount = GetThreadCount();
	
	if(nThreadCount > 1)
	{
		ossSqlHelper<<"echo \"info thread\n";
		for(int i = 0; i < nThreadCount; i++)
		{
			ossSqlHelper<<"thread "<<(i+1)<<"\nbt full\n";
		}
		ossSqlHelper<<"\">gdbcmd";
	}
	else
	{
		ossSqlHelper<<"echo \"bt\n\">gdbcmd";
	}
	string strCmd = ossSqlHelper.str();

	system(strCmd.c_str() );
    char dbx[160]={0};
    sprintf(dbx, "gdb -p %d < gdbcmd >gdbdump_%d_%d.log", getpid(), getpid(), s_logCount);
    system(dbx);
	system("rm -f gdbcmd");
	s_logCount++;
}


