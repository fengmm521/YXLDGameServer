#ifndef __IJZMESSAGELAYER_H__
#define __IJZMESSAGELAYER_H__

#include "Legion.h"
#include "Fight.h"
#include "QQYYBPay.h"
#include "PayReturn.h"
#include "LJSDK.h"

class IJZMessageLayer
{
public:

	virtual ~IJZMessageLayer(){}

	virtual void AsyncGetLegionList(ServerEngine::LegionPrxCallbackPtr cb) = 0;

	// 功能: 创建军团
	virtual void AsyncNewLegion(ServerEngine::LegionPrxCallbackPtr cb, const string& strLegionName, const string& strData) = 0;

	// 功能: 通用全局数据获取
	virtual void AsyncGetLegionData(ServerEngine::LegionPrxCallbackPtr cb, const string& strKey) = 0;

	// 功能: 更新数据
	virtual void AsyncSetLegionData(ServerEngine::LegionPrxCallbackPtr cb, const string& strKey, const string& strValue) = 0;

	// 功能: 删除数据
	virtual void AsyncDelLegionData(ServerEngine::LegionPrxCallbackPtr cb, const string& strKey) = 0;

	virtual IMessageLayer* getMessageLayer() = 0;

	virtual void broadcastMsg(const GSProto::SCMessage& msg) = 0;

	virtual int setLegionData(const string& strKey, const string& strValue) = 0;

	virtual int getLegionData(const string& strKey, string& strValue) = 0;

	virtual int getGlobalData(const string& strKey, string& strValue) = 0;

	virtual int setGlobalData(const string& strKey, const string& strValue) = 0;

	virtual int delGlobalData(const string& strKey) = 0;

	virtual void AsyncGetGlobalData(ServerEngine::LegionPrxCallbackPtr cb, const string& strKey) = 0;

	virtual void AsyncSetGlobalData(ServerEngine::LegionPrxCallbackPtr cb, const string& strKey, const string& strValue) = 0;

	virtual void AsyncDelGlobalData(ServerEngine::LegionPrxCallbackPtr cb, const string& strKey) = 0;

	virtual void AsyncPVEFight(ServerEngine::FightPrxCallbackPtr cb, const ServerEngine::ActorCreateContext & roleInfo, taf::Int32 iMonsterGrpID,const ServerEngine::FightContext & ctx) = 0;

	virtual void AsyncPVPFight(ServerEngine::FightPrxCallbackPtr cb, const ServerEngine::ActorCreateContext & attRoleInfo, const ServerEngine::ActorCreateContext & targetRileInfo, const ServerEngine::FightContext & ctx) = 0;
	virtual void AsyncGetNameDesc(ServerEngine::NamePrxCallbackPtr cb,const string& strKey ) = 0;

	virtual void AsyncMultiPVEFight(ServerEngine::FightPrxCallbackPtr cb, const ServerEngine::ActorCreateContext & roleInfo, vector<taf::Int32> monsterGrpList,const ServerEngine::FightContext & ctx) = 0;

	virtual void AsyncContinuePVEFight(ServerEngine::FightPrxCallbackPtr cb, const ServerEngine::ActorCreateContext & roleInfo, taf::Int32 iMonsterGrpID,const ServerEngine::FightContext & ctx) = 0;

	virtual void AsyncContinueMultiPVEFight(ServerEngine::FightPrxCallbackPtr cb, const ServerEngine::ActorCreateContext & roleInfo, vector<taf::Int32> monsterGrpList,const ServerEngine::FightContext & ctx) = 0;

	virtual void AsyncContinuePVPFight(ServerEngine::FightPrxCallbackPtr cb, const ServerEngine::ActorCreateContext& attRoleInfo, const ServerEngine::ActorCreateContext & targetRileInfo, const ServerEngine::FightContext & ctx) = 0;

	virtual void AsyncQueryQQYYBYuanBao(ServerEngine::QQYYBPayPrxCallbackPtr cb, const ServerEngine::QQYYBPayQueryKey& key) = 0;

	virtual void AsyncPayQQYYBYuanBao(ServerEngine::QQYYBPayPrxCallbackPtr cb, const ServerEngine::QQYYBPayQueryKey& key, int iCostNum) = 0;

	virtual void AsyncPayReturnMoneyAndVip(ServerEngine::PayReturnPrxCallbackPtr cb, const string& account) = 0;

	virtual void AsyncVrifyUserLogin(ServerEngine::LJSDKPrxCallbackPtr cb, const ServerEngine::VerifyKey verifyKey) = 0;
	virtual void AsyncGetPhoneStream(ServerEngine::LJSDKPrxCallbackPtr cb, const string& strUrl) = 0;
	virtual void AsyncGetPrePayInfo(ServerEngine::LJSDKPrxCallbackPtr cb, const string& strPostContext) = 0;
	virtual void AsyncIosPay(		ServerEngine::LJSDKPrxCallbackPtr _cb, 
										int _worldID, 
										const string& _account, 
										int _cPrice, 
										const string& _receipt) = 0;
};

#endif
