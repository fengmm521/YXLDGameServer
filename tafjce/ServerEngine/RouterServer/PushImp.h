#ifndef __PUSH_IMP_H__
#define __PUSH_IMP_H__

#include "Push.h"

class PushImp:public ServerEngine::Push
{
public:

	PushImp();
	~PushImp();
	
	virtual void initialize();

	virtual void destroy();

	virtual taf::Int32 doPush(taf::Int64 iConnId,const std::string & sMsgPack,taf::JceCurrentPtr current);

	virtual taf::Int32 doNotifyLoginOff(taf::Int64 iConnId,const std::string & sMsgPack,taf::JceCurrentPtr current);
};


#endif

