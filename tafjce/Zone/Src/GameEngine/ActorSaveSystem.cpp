#include "GameEnginePch.h"
#include "ActorSaveSystem.h"

extern "C" IObject* createActorSaveSystem()
{
	return new ActorSaveSystem;
}


ActorSaveSystem::ActorSaveSystem()
{
}

ActorSaveSystem::~ActorSaveSystem()
{
	ITimerComponent* pTimeAxis = getComponent<ITimerComponent>(COMPNAME_TimeAxis, IID_ITimerComponent);
	if(pTimeAxis)
	{
		pTimeAxis->killTimer(m_hSaveHandle);
	}
}

Uint32 ActorSaveSystem::getSubsystemID() const
{
	return IID_IActorSaveSystem;
}

Uint32 ActorSaveSystem::getMasterHandle()
{
	return m_hHandle;
}

bool ActorSaveSystem::create(IEntity* pEntity, const std::string& strData)
{
	m_hHandle = pEntity->getHandle();

	IGlobalCfg* pGlobalCfg = getComponent<IGlobalCfg>(COMPNAME_GlobalCfg, IID_IGlobalCfg);
	assert(pGlobalCfg);

	ITimerComponent* pTimeAxis = getComponent<ITimerComponent>(COMPNAME_TimeAxis, IID_ITimerComponent);
	assert(pTimeAxis);

	int iSaveInterval = pGlobalCfg->getInt("角色保存间隔", 300);
	m_hSaveHandle = pTimeAxis->setTimer(this, 1, iSaveInterval*1000, "ActorSave");
	
	return true;
}

const std::vector<Uint32>& ActorSaveSystem::getSupportMessage()
{
	static std::vector<Uint32> s_emptyMsgList;

	return s_emptyMsgList;
}

class ActorSaveCb:public ServerEngine::RolePrxCallback
{
public:

	ActorSaveCb(HEntity hEntity):m_hEntity(hEntity)
	{
		IEntity* pEntity = getEntityFromHandle(hEntity);
		assert(pEntity);
		
		m_strAccoount = pEntity->getProperty(PROP_ACTOR_ACCOUNT, "");
		m_iRolsPos = pEntity->getProperty(PROP_ACTOR_ROLEPOS, 0);
	}

	virtual void callback_updateRole(taf::Int32 ret)
	{
		FDLOG("UpdateRole")<<"UpdateRole|Ret|"<<ret<<"|"<<m_strAccoount<<"|"<<m_iRolsPos<<endl;
	}

	virtual void callback_updateRole_exception(taf::Int32 ret)
	{
		FDLOG("UpdateRole")<<"UpdateRole|Excetion|"<<ret<<"|"<<m_strAccoount<<"|"<<m_iRolsPos<<endl;
	}
	 
private:

	string m_strAccoount;
	int m_iRolsPos;
	HEntity m_hEntity;
};

void ActorSaveSystem::onTimer(int nEventId)
{
	doSave(false);
}

void ActorSaveSystem::doSave(bool bSync)
{
	IEntity* pEntity = getEntityFromHandle(m_hHandle);
	if(!pEntity) return;

	IMessageLayer* pMsgLayer = getComponent<IMessageLayer>("MessageLayer", IID_IMessageLayer);
	assert(pMsgLayer);

	IGlobalCfg* pGlobalCfg = getComponent<IGlobalCfg>("GlobalCfg", IID_IGlobalCfg);
	assert(pGlobalCfg);

	ServerEngine::RoleSaveData tmpSaveData;
	pEntity->packSaveData(tmpSaveData);

	ServerEngine::PKRole key;
	key.strAccount = pEntity->getProperty(PROP_ACTOR_ACCOUNT, "");
	key.rolePos = pEntity->getProperty(PROP_ACTOR_ROLEPOS, 0);
	key.worldID = pEntity->getProperty(PROP_ACTOR_WORLD, 0);

	ServerEngine::PIRole piRole;
	piRole.strAccount = key.strAccount;
	piRole.rolePos = key.rolePos;
	piRole.worldID = key.worldID;
	piRole.roleData = ServerEngine::JceToStr(tmpSaveData);

	if(bSync)
	{
		pMsgLayer->SaveRoleData(key, piRole);
	}
	else
	{
		ServerEngine::RolePrxCallbackPtr tmpCb = new ActorSaveCb(m_hHandle);
		pMsgLayer->AsyncUpdateRole(key,  piRole, tmpCb);
	}
}


