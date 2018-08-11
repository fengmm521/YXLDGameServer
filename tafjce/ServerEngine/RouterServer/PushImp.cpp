#include "RouterComm.h"
#include "PushImp.h"
#include "RouterImp.h"

PushImp::PushImp()
{
}

PushImp::~PushImp()
{
}

void PushImp::initialize()
{
	// nothing
}

void PushImp::destroy()
{
}


taf::Int32 PushImp::doPush(taf::Int64 iConnId,const std::string & sMsgPack,taf::JceCurrentPtr current)
{
	ConnectionInfoPtr connPtr = ConnectionManager::getInstance()->get(iConnId);
	if(!connPtr)
	{
		return 0;
	}

	connPtr->current->sendResponse(sMsgPack.c_str(), sMsgPack.size() );

	return 1;
}

taf::Int32 PushImp::doNotifyLoginOff(taf::Int64 iConnId,const std::string & sMsgPack,taf::JceCurrentPtr current)
{
	ConnectionInfoPtr connPtr = ConnectionManager::getInstance()->get(iConnId);
	if(!connPtr)
	{
		return 0;
	}
	connPtr->current->close();

	TRLOG<<"doNotifyLoginOff|"<<connPtr->strAccount<<"|"<<connPtr->ddConnID<<endl;
	
	return 1;
}


