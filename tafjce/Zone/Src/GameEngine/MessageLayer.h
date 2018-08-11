#ifndef __MESSAGE_LAYER_H__
#define __MESSAGE_LAYER_H__

#include "IMessageLayer.h"
#include "Name.h"

class MessageLayer:public ComponentBase<IMessageLayer, IID_IMessageLayer>
{
public:

	// Constructor/Destructor
	MessageLayer();
	~MessageLayer();

	// IComponent Interface
	virtual bool initlize(const PropertySet& propSet);

	// IMessageLayer Interface
	virtual void sendPlayerMessage( const UserInfo *pUserInfo, const GSProto::SCMessage& msgPack);
	virtual void sendMessage2Connection(const std::string& strRsAddress, Int64 ddCon, const GSProto::SCMessage& msgPack);
	virtual void sendPlayerErrorMessage(const UserInfo *pUserInfo,	int iErrorCode);
    virtual void sendErrorMessage2Connection( const std::string& strRsAddress, Int64 ddCon, int iErrorCode);

	virtual int NewRole(const std::string& strAccount, int iWorldID,  int nRolePos, const std::string& strName,  const ServerEngine::PIRole& data);
	
	virtual int SaveRoleData(const ServerEngine::PKRole& key, const ServerEngine::PIRole& data);

	// 功能: 删除角色数据
	virtual int DelRoleData(const ServerEngine::PKRole& key);

	virtual int GetRoleData(const ServerEngine::PKRole& key, ServerEngine::PIRole& data);

	virtual void AsyncNewRole(const std::string& strAccount, int iWorldID, int nRolePos, const std::string& strName, const ServerEngine::PIRole& data, ServerEngine::RolePrxCallbackPtr callback);

	virtual void AsyncUpdateRole(const ServerEngine::PKRole& key, const ServerEngine::PIRole& data, ServerEngine::RolePrxCallbackPtr callback);

	virtual void AsyncDelRole(const ServerEngine::PKRole& key,const string &sRoleName, ServerEngine::RolePrxCallbackPtr callback);

	virtual void AsyncGetRole(const ServerEngine::PKRole& key, ServerEngine::RolePrxCallbackPtr callback);
  
	virtual void AsyncGetRoleList(const std::string& strAccount, ServerEngine::RolePrxCallbackPtr callback);

	virtual CMsgCircleQueue* getCsMessageQueue();

	virtual void AsyncGetNameDesc(const string& strName, ServerEngine::NamePrxCallbackPtr callback);

	virtual void AsyncSaveFightRecord(const ServerEngine::BattleData& fightData, ServerEngine::FightDataPrxCallbackPtr callback);

	virtual void AsyncGetFightRecord(const ServerEngine::PKFight& fightKey, ServerEngine::FightDataPrxCallbackPtr callback);

	virtual void AsyncDelFightRecord(const ServerEngine::PKFight& fightKey, ServerEngine::FightDataPrxCallbackPtr callback);

	ServerEngine::RolePrx getRolePrx(int iWorldID);
	ServerEngine::FightDataPrx getFightPrx(int iWorldID);

	virtual void AsyncGetConvertCode(const std::string& strName, ServerEngine::ConvertCodePrxCallbackPtr callback);
	virtual void AsyncUpdateConvertCode(const std::string& strName, int state ,ServerEngine::ConvertCodePrxCallbackPtr callback);
	
	
private:
	void initProxy();

private:
	
	map<int, ServerEngine::RolePrx> m_rolePrxMap;
	map<int, ServerEngine::FightDataPrx> m_fightPrxMap;
	
    //int             m_iWorldID;
	map<HEntity, map<int, int> > m_selfPropMap;
	map<HEntity, map<int, int> > m_broadcastPropMap;
	CMsgCircleQueue m_csMsgQueue;
	
	ServerEngine::NamePrx	m_namePrx;

	ServerEngine::ConvertCodePrx m_ConvertCodePrx;
};

#endif
