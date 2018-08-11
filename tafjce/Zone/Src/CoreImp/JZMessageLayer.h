#ifndef __JZ_MESSAGELAYER_H__
#define __JZ_MESSAGELAYER_H__

#include "IJZMessageLayer.h"
#include "../GameEngine/MessageLayer.h"

class JZMessageLayer:public MessageLayer, public IJZMessageLayer
{
public:

	JZMessageLayer();
	~JZMessageLayer();

	virtual bool initlize(const PropertySet& propSet);
	virtual void* queryInterface(IID interfaceId);
	virtual void AsyncGetLegionList(ServerEngine::LegionPrxCallbackPtr cb);
	virtual void AsyncNewLegion(ServerEngine::LegionPrxCallbackPtr cb, const string& strLegionName, const string& strData);
	virtual void AsyncGetLegionData(ServerEngine::LegionPrxCallbackPtr cb, const string& strKey);
	virtual void AsyncSetLegionData(ServerEngine::LegionPrxCallbackPtr cb, const string& strKey, const string& strValue);
	virtual void AsyncDelLegionData(ServerEngine::LegionPrxCallbackPtr cb, const string& strKey);
	virtual IMessageLayer* getMessageLayer();
	virtual void broadcastMsg(const GSProto::SCMessage& msg);
	virtual int setLegionData(const string& strKey, const string& strValue);
	virtual int getLegionData(const string& strKey, string& strValue);

	virtual int getGlobalData(const string& strKey, string& strValue);
	virtual int setGlobalData(const string& strKey, const string& strValue);
	virtual int delGlobalData(const string& strKey);
	virtual void AsyncGetGlobalData(ServerEngine::LegionPrxCallbackPtr cb, const string& strKey);
	virtual void AsyncSetGlobalData(ServerEngine::LegionPrxCallbackPtr cb, const string& strKey, const string& strValue);
	virtual void AsyncDelGlobalData(ServerEngine::LegionPrxCallbackPtr cb, const string& strKey);
	virtual void AsyncPVEFight(ServerEngine::FightPrxCallbackPtr cb, const ServerEngine::ActorCreateContext & roleInfo, taf::Int32 iMonsterGrpID,const ServerEngine::FightContext & ctx);
	virtual void AsyncPVPFight(ServerEngine::FightPrxCallbackPtr cb, const ServerEngine::ActorCreateContext & attRoleInfo, const ServerEngine::ActorCreateContext & targetRileInfo, const ServerEngine::FightContext & ctx);
	virtual void AsyncGetNameDesc(ServerEngine::NamePrxCallbackPtr cb,const string& strKey );
	virtual void AsyncMultiPVEFight(ServerEngine::FightPrxCallbackPtr cb, const ServerEngine::ActorCreateContext & roleInfo, vector<taf::Int32> monsterGrpList,const ServerEngine::FightContext & ctx);
	virtual void AsyncContinuePVEFight(ServerEngine::FightPrxCallbackPtr cb, const ServerEngine::ActorCreateContext & roleInfo, taf::Int32 iMonsterGrpID,const ServerEngine::FightContext & ctx);
	virtual void AsyncContinueMultiPVEFight(ServerEngine::FightPrxCallbackPtr cb, const ServerEngine::ActorCreateContext & roleInfo, vector<taf::Int32> monsterGrpList,const ServerEngine::FightContext & ctx) ;
	virtual void AsyncContinuePVPFight(ServerEngine::FightPrxCallbackPtr cb, const ServerEngine::ActorCreateContext& attRoleInfo, const ServerEngine::ActorCreateContext & targetRileInfo, const ServerEngine::FightContext & ctx);
	virtual void AsyncQueryQQYYBYuanBao(ServerEngine::QQYYBPayPrxCallbackPtr cb, const ServerEngine::QQYYBPayQueryKey& key);
	virtual void AsyncPayQQYYBYuanBao(ServerEngine::QQYYBPayPrxCallbackPtr cb, const ServerEngine::QQYYBPayQueryKey& key, int iCostNum);
	virtual void AsyncPayReturnMoneyAndVip(ServerEngine::PayReturnPrxCallbackPtr cb, const string& account);
	virtual void AsyncVrifyUserLogin(ServerEngine::LJSDKPrxCallbackPtr cb, const ServerEngine::VerifyKey verifyKey);
	virtual void AsyncGetPhoneStream(ServerEngine::LJSDKPrxCallbackPtr cb,const string& strUrl);
	virtual void AsyncGetPrePayInfo(ServerEngine::LJSDKPrxCallbackPtr cb, const string& strPostContext);

	/**
	 * IOS Pay
	 * author : xionghai
	 * this function is for testing
	 * @param	_cb		[in] : callback
	 * @param	_price	[in] : the price
	 * @param	_receipt[in] : receipt get from app store
	 */
	virtual void AsyncIosPay(	ServerEngine::LJSDKPrxCallbackPtr _cb, 
									int _worldID, 
									const string& _account, 
									int _cPrice, 
									const string& _receipt);
	 
	 
private:

	ServerEngine::LegionPrx m_legionPrx;
	ServerEngine::FightPrx m_fightPrx;
	ServerEngine::NamePrx m_namePrx;
	ServerEngine::QQYYBPayPrx m_qqyybPayPrx;
	ServerEngine::PayReturnPrx m_PayReturnPrx;
	ServerEngine::LJSDKPrx m_LJSDKPrx;
};


#endif
