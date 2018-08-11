#include "GameEnginePch.h"
#include "ICleanupManager.h"
#include "CleanupManager.h"
#include "servant/Application.h"
#include "nodeF/taf_nodeF.h"
#include "DumpHelper.h"
#include "ILegionFactory.h"
#include "IArenaFactory.h"
#include "IBossFactory.h"

extern Uint32 g_lastUpdateTime;


extern "C" IComponent* createCleanupManager(Int32)
{
	return new CleanUpManager;
}

extern bool isGameServer();
extern bool isInitlizeFinish();

CleanUpManager::CleanUpManager()
{
}

CleanUpManager::~CleanUpManager()
{
}

bool CleanUpManager::initlize(const PropertySet& propSet)
{
	return true;
}

void CleanUpManager::heatBeat()
{
	/*TC_Config& tmpCfg = Application::getConfig();
	vector<string> adapterName;
	tmpCfg.getDomainVector("/taf/application/server", adapterName);

	for(size_t i = 0; i < adapterName.size(); i++)
	{
		TAF_KEEPALIVE(adapterName[i]);
		MINIAPR_TRACE("Report HeatBeat:%s", adapterName[i].c_str() );
		FDLOG("CleanUp")<<"Report HeatBeat:"<<adapterName[i]<<endl;
	}*/

	// here we need to do nothing
}

void CleanUpManager::doCleanUp(bool boIsKickOffRole/* = true*/)
{
	/*if(!isGameServer() )
	{
		return;
	}*/

	if(!isInitlizeFinish() )
	{
		FDLOG("CleanUp")<<"Server Not InitlizeFinish Do not Save"<<endl;
		return;
	}

	DumpHelper tmpHelp(false);
	tmpHelp.DumpBackTrace();
	tmpHelp.DumpGdbLog();

	IUserStateManager* pUserStateManager = getComponent<IUserStateManager>("LoginManager", IID_IUserStateManager);
	assert(pUserStateManager);

	IMessageLayer* pMsgLayer = getComponent<IMessageLayer>("MessageLayer", IID_IMessageLayer);
	assert(pMsgLayer);

	IGlobalCfg* pGlobalCfg = getComponent<IGlobalCfg>("GlobalCfg", IID_IGlobalCfg);
	assert(pGlobalCfg);

    if(boIsKickOffRole) //目前仅用于传false避免把停服操作人员踢下线
    {
    	std::vector<string> accountList;
    	pUserStateManager->getAllUserQQ(accountList);

    	time_t begin = time(0);
    	for(size_t i = 0; i < accountList.size(); i++)
    	{
    		g_lastUpdateTime = time(NULL);
    		heatBeat();
    		string strAccount = accountList[i];
    		const UserInfo* pUserInfo = pUserStateManager->getUserInfo(strAccount);
    		if(!pUserInfo) continue;

    		IEntity* pEntity = getEntityFromHandle(pUserInfo->hEntity);
    		if(!pEntity) continue;

    		bool bCreateFinish = pEntity->getProperty(PROP_ACTOR_CREATEFINISH, 0) == 1;
    		if(!bCreateFinish)
    		{
    			MINIAPR_TRACE("CleanUp Not Save[%s]Data, Entity Not Finish", strAccount.c_str() );
    			FDLOG("CleanUp")<<"CleanUp Not Save["<<strAccount<<"]Data, Entity Not Finish"<<endl;
    			continue;
    		}

    		ServerEngine::PKRole key;
    		key.strAccount = pEntity->getProperty(PROP_ACTOR_ACCOUNT, "");
    		key.rolePos = pEntity->getProperty(PROP_ACTOR_ROLEPOS, (Int32)0);
    		key.worldID = pEntity->getProperty(PROP_ACTOR_WORLD, 0);
    		ServerEngine::PIRole data;
    		data.strAccount = key.strAccount;
    		data.rolePos = key.rolePos;
    		data.worldID = key.worldID;
    		ServerEngine::RoleSaveData saveData;
    		pEntity->packSaveData(saveData);
    		data.roleData = ServerEngine::JceToStr(saveData);
    		pMsgLayer->SaveRoleData(key, data);	
    		
    		MINIAPR_TRACE("CleanUp Save[%s]Data Success", strAccount.c_str() );
    		FDLOG("CleanUp")<<"CleanUp Save["<<strAccount<<"]Data Success"<<endl;
    	}
        Uint32 dwCostSecond = (Uint32)(time(0) - begin);
    	MINIAPR_TRACE("CleanUp Finish count%d costtime:[%d]second", (int)accountList.size(), dwCostSecond);
    	FDLOG("CleanUp")<<"CleanUp Finish count:"<<accountList.size()<<" costtime:"<<"["<<dwCostSecond<<"]"<<endl;
    }


	// 保存军团
	ILegionFactory* pLegionFactory = getComponent<ILegionFactory>(COMPNAME_LegionFactory, IID_ILegionFactory);
	assert(pLegionFactory);

	if(pLegionFactory->isReady())
	{
		pLegionFactory->cleanupSave();
	}
	


	// 保存竞技场
	IArenaFactory* pArenaFactory = getComponent<IArenaFactory>(COMPNAME_ArenaFactory, IID_IArenaFactory);
	assert(pArenaFactory);

	if(pArenaFactory->isInitFinish())
	{
	      pArenaFactory->saveArenaData(true);
	}

	IBossFactory* pBossFactory = getComponent<IBossFactory>(COMPNAME_BossFactory, IID_IBossFactory);
	assert(pBossFactory);

	if(pBossFactory->isInitFinish())
	{
		pBossFactory->saveWorldBossData(true);
	}
	sleep(3);
	
}

