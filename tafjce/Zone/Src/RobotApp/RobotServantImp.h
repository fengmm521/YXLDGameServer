#ifndef __ROBOTSERVANTIMP_H__
#define __ROBOTSERVANTIMP_H__

#include "servant/Application.h"
#include "GameInterface.h"
#include "MiniApr.h"


using namespace MINIAPR;


/**
 *
 *
 */
class RobotServantImp : public ServerEngine::Game
{
public:
	/**
	 *
	 */
	virtual ~RobotServantImp() {}

	virtual taf::Int32 doRequest(taf::Int64 iConnId,const std::string & strAccount, int iWorldID, const std::string & sMsgPack,const std::string & sRsObjAddr,const map<std::string, std::string> & mClientParam,taf::JceCurrentPtr current){return -1;}

	virtual taf::Int32 doNotifyLoginOff(const std::string & strAccount,taf::Short nLoginOffCode,const std::string & sRsObjAddr,taf::Int64 iConnId,taf::JceCurrentPtr current){return -1;}

	virtual void getGameStatus(ServerEngine::GameQueryStatus &gameStatus,taf::JceCurrentPtr current){}


	taf::Int32 TbT_AddMoney(const std::string & strAccount,taf::Int32 iWorldID,taf::Int32 iCount,taf::JceCurrentPtr current){return 0;}
	taf::Int32 Comm_Pay_AddMoney(const std::string & strAccount,taf::Int32 iWorldID,taf::Int32 iCount,const std::string & strChannel,taf::JceCurrentPtr current){return 0;}
	taf::Int32 LJSDK_Pay_AddMoney(const std::string & strAccount,taf::Int32 iWorldID,taf::Int32 iCount,const std::string & strChannel,const std::string & strOrderId,taf::JceCurrentPtr current){return 0;}
    taf::Int32 getRoleBaseData(const ServerEngine::PKRole & pkRole,std::string &strJson,taf::JceCurrentPtr current){return 0;}
    taf::Int32 getRoleBagData(const ServerEngine::PKRole & pkRole,std::string &strJson,taf::JceCurrentPtr current){return 0;}
    taf::Int32 getRoleHeroData(const ServerEngine::PKRole & pkRole,std::string &strJson,taf::JceCurrentPtr current){return 0;}
    virtual taf::Int32 sendRoleMail(const ServerEngine::PKRole & pkRole,const map<std::string, std::string> & mailMap,taf::JceCurrentPtr current){return 0;}
       
    virtual taf::Int32 getLegionMember(const std::string & strLegionName,std::string &strJson,taf::JceCurrentPtr current){return 0;}
    virtual taf::Int32 GMOnlneRollMessage(const std::string & strMsg,taf::JceCurrentPtr current){return 0;}


	/**
	 *
	 */
	virtual void initialize();

	/**
	 *
	 */
    virtual void destroy();

	virtual int doResponse(ReqMessagePtr resp){return 1;}

	 virtual void nothing(taf::JceCurrentPtr current){}
   	

	/**
     * @param resp
     * @return int
     */
	//int doResponseException(ReqMessagePtr resp){};
    
    /**
     * 每次handle被唤醒后都会调用，业务可以通过在其他线程中调用handle的notify
     * 实现在主线程中处理自有数据的功能，比如定时器任务或自有网络的异步响应;
     * [一般都需要配合业务自有的队列使用，队列可以封装在ServantImp对象中]
     */
    virtual int doCustomMessage(bool bExpectIdle = false);
};

#endif

