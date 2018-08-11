#ifndef __SNAPSHOTIMP_H__
#define __SNAPSHOTIMP_H__

#include "Snapshot.h"
#include "hiredis.h"
#include "MiniApr.h"

using namespace MINIAPR;


class SnapshotImp:public ServerEngine::Snapshot
{
public:

	SnapshotImp();
	~SnapshotImp();

	// Servant Interface
	virtual void initialize();
    virtual void destroy();

	virtual taf::Int32 addWorld(const std::string & strAccount,taf::Int32 iWorldID,taf::JceCurrentPtr current);
    virtual taf::Int32 getRoleWorldDesc(const std::string & strAccount,ServerEngine::RoleSnapshot &snapDesc,taf::JceCurrentPtr current);

private:

	RedisClient m_redisClient;
};


#endif
