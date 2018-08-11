#ifndef _GAMESERVANT_IMP_H__
#define _GAMESERVANT_IMP_H__

#include "servant/Application.h"
#include "GameInterface.h"


class GameServantImp:public ServerEngine::Game, public ITimerCallback
{
public:

	GameServantImp();
	~GameServantImp();

	// Servant Interface
	virtual void initialize();
    virtual void destroy();
	virtual int doCustomMessage(bool bExpectIdle = false);

	// Game Interface
	virtual taf::Int32 doRequest(taf::Int64 iConnId,const std::string & strAccount, int iWorldID, const std::string & sMsgPack,const std::string & sRsObjAddr,const map<std::string, std::string> & mClientParam,taf::JceCurrentPtr current);
	virtual taf::Int32 doNotifyLoginOff(const std::string & strAccount,taf::Short nLoginOffCode,const std::string & sRsObjAddr,taf::Int64 iConnId,taf::JceCurrentPtr current);
	virtual void getGameStatus(ServerEngine::GameQueryStatus &gameStatus,taf::JceCurrentPtr current);

	virtual taf::Int32 getRoleBaseData(const ServerEngine::PKRole & pkRole,std::string &strJson,taf::JceCurrentPtr current);
	virtual taf::Int32 getRoleBagData(const ServerEngine::PKRole & pkRole,std::string &strJson,taf::JceCurrentPtr current);
   	virtual taf::Int32 getRoleHeroData(const ServerEngine::PKRole & pkRole,std::string &strJson,taf::JceCurrentPtr current);
    virtual taf::Int32 sendRoleMail(const ServerEngine::PKRole & pkRole,const map<std::string, std::string> & mailMap,taf::JceCurrentPtr current);
	virtual taf::Int32 getLegionMember(const std::string & strLegionName,std::string &strJson,taf::JceCurrentPtr current);	
	virtual taf::Int32 GMOnlneRollMessage(const std::string & strMsg,taf::JceCurrentPtr current);
	//同步推充值
	 virtual taf::Int32 TbT_AddMoney(const std::string & strAccount,taf::Int32 iWorldID,taf::Int32 iCount,taf::JceCurrentPtr current) ;
	 virtual taf::Int32 Comm_Pay_AddMoney(const std::string & strAccount,taf::Int32 iWorldID,taf::Int32 iCount,const std::string & strChannel,taf::JceCurrentPtr current) ;
	 virtual taf::Int32 LJSDK_Pay_AddMoney(const std::string & strAccount,taf::Int32 iWorldID,taf::Int32 iCount,const std::string & strChannel,const std::string & strOrderId,taf::JceCurrentPtr current) ;
	// Servant Interface
	virtual int doResponse(ReqMessagePtr resp);
	virtual int doResponseException(ReqMessagePtr resp);

	// ITimerCallback Interface
	virtual void onTimer(int nEventId);
};

#endif


