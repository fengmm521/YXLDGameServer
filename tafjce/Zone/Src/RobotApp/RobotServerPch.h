#ifndef __ROBOT_APP_PCH_H__
#define __ROBOT_APP_PCH_H__

#include <sys/epoll.h>
#include "MiniApr.h"

using namespace MINIAPR;

#include "ISocketSystem.h"
#include "MessageWrapper.h"

#include <sys/resource.h>
#include "RobotComponentConstDef.h"
#include "comm_func.h"
#include "svr_log.h"
#include "FakeRandom.h"
#include "ISocketSystem.h"
#include "IClientSocket.h"
#include "IClientSink.h"
#include "GameServer.pb.h"
#include "GameServerExt.pb.h"


#endif
