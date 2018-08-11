/*********************************************************
*
*	名称: MiniArp.h
*	作者: wuxf
*	时间: 2011-04-17
*	描述: MiniApr总头文件
*********************************************************/

#ifndef __MINI_APR_H__
#define __MINI_APR_H__

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <set>
#include <map>
#include <list>
#include <algorithm>
#include <assert.h>

//#define MINIAPR_ERROR printf
//#define MINIAPR_TRACE	printf

#define MINIAPR_TRACE( _s_, ... ) do{char szLog[1024];  snprintf( szLog, 1024-1, _s_, ##__VA_ARGS__ ); szLog[1024-1] = 0; cout<<szLog<<std::endl;  FDLOG("MiniAprTrace")<<szLog<<std::endl;}while(0)
#define MINIAPR_ERROR( _s_, ... ) do{char szMsg[1024];   snprintf( szMsg, 1024-1, _s_, ##__VA_ARGS__ ); szMsg[1024-1] = 0; FDLOG("MiniAprError")<<szMsg<<"["<<__FILE__<<":"<<__FUNCTION__<<":"<<__LINE__<<"]"<<std::endl;}while(0)
#define MINIAPR_ERRORFLUSH()  do{FDLOG("MiniAprError")<<flush;}while(0);

#include "MiniAprType.h"

// Component Header
#include "Component/IComponent.h"
#include "Component/ComponentBase.h"

// Stream Header
#include "Stream/CsvFile.h"
//#include "Stream/DbcFile.h"
//#include "Stream/InOutBuffer.h"

// EventServer
#include "EventServer/EventServer.h"

#include "TimerManager/ITimerComponent.h"

// MessageServer
//#include "MessageServer/MessageServer.h"

// HandleManager
#include "HandleManager/HandleManager.h"

// ProfileManager
#include "ProfileManager/IProfilerManager.h"
#include "ProfileManager/IProfilerStrategy.h"
#include "ProfileManager/IProfilerTimer.h"

// TimeAxis
//#include "TimeAxis/ITimeAxis.h"
//#include "TimeAxis/ITimerCallback.h"

// Thread
//#include "Thread/IThread.h"
//#include "Thread/ILogicThread.h"

// Log
//#include "Log/FileLog.h"

// ISocketSystem
//#include "SocketSystem/ISocketSystem.h"

// IScriptEngine
#include "ScriptEngine/IScriptEngine.h"

#include "RedisClient/RedisClient.h"

#include "Random/IRandom.h"

// Disassemble
//#include "Disassemble/Mini_Disassembler.h"


// Win32 Hook
//#include "Win32Hook/DynamicSpy.h"
//#include "Win32Hook/ComFunctionSpy.h"

#pragma GCC poison rand


#endif
