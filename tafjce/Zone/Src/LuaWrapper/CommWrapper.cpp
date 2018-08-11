#include "LuaWrapperPch.h"
#include "CommWrapper.h"
#include "IFightSoulSystem.h"
#include "IArenaSystem.h"
#include "IManorSystem.h"
#include "IShopSystem.h"
#include "ILegionSystem.h"

string getWorkDir()
{
	return taf::ServerConfig::DataPath;
}


void sendCommDlg(Uint32 dwActor, const string& strMsg, const string& strLeftName, const string& strLeftCmd, const string& strRightName, const string& strRightCmd)
{
	IEntity* pActor = getEntityFromHandle(dwActor);
	if(!pActor) return;

	GSProto::Cmd_Sc_CommDlg scMsg;
	scMsg.set_strmessage(strMsg);
	
	GSProto::ConfirmMsg* pConfirmMsg = scMsg.mutable_confirmdata();
	assert(pConfirmMsg);

	pConfirmMsg->set_strlefttext(strLeftName);
	pConfirmMsg->set_strleftcmd(strLeftCmd);
	pConfirmMsg->set_strrighttext(strRightName);
	pConfirmMsg->set_strrightcmd(strRightCmd);

	GSProto::SCMessage pkg;
	HelpMakeScMsg(pkg, GSProto::CMD_COMM_DLG, scMsg);

	pActor->sendMessage(pkg);
}

void doFightSoulCombine(Uint32 dwEntityID, int iSrcConType, int iSrcPos, int iDstConType, int iDstPos, Uint32 dwHeroObjectID)
{
	IEntity* pActor = getEntityFromHandle(dwEntityID);
	if(!pActor) return;

	IFightSoulSystem* pFightSoulSys = static_cast<IFightSoulSystem*>(pActor->querySubsystem(IID_IFightSoulSystem));
	if(!pFightSoulSys) return;

	pFightSoulSys->combine(iSrcConType, iSrcPos, iDstConType, iDstPos, dwHeroObjectID);
}


void enableFSAdvance(Uint32 dwEntityID)
{
	IEntity* pActor = getEntityFromHandle(dwEntityID);
	if(!pActor) return;

	IFightSoulSystem* pFightSoulSys = static_cast<IFightSoulSystem*>(pActor->querySubsystem(IID_IFightSoulSystem));
	if(!pFightSoulSys) return;

	pFightSoulSys->enableAdvanceMode();
}

void autoCombineFightSoul(Uint32 dwEntityID)
{
	IEntity* pActor = getEntityFromHandle(dwEntityID);
	if(!pActor) return;

	IFightSoulSystem* pFightSoulSys = static_cast<IFightSoulSystem*>(pActor->querySubsystem(IID_IFightSoulSystem));
	if(!pFightSoulSys) return;

	pFightSoulSys->autoCombine();
}

void LogMsg(const string& strMsg)
{
	FDLOG("Script")<<strMsg<<endl;
}


/*void loveMaid(Uint32 dwEntityID, bool bAdvance)
{
	IEntity* pActor = getEntityFromHandle(dwEntityID);
	if(!pActor) return;

	IDomainSystem* pDomainSys = static_cast<IDomainSystem*>(pActor->querySubsystem(IID_IDomainSystem));
	if(!pDomainSys) return;

	pDomainSys->loveMaid(bAdvance);
}


void playMaid(Uint32 dwEntityID, bool bAdvance, const string& strAccount, int iWorldID)
{
	IEntity* pActor = getEntityFromHandle(dwEntityID);
	if(!pActor) return;

	IDomainSystem* pDomainSys = static_cast<IDomainSystem*>(pActor->querySubsystem(IID_IDomainSystem));
	if(!pDomainSys) return;

	ServerEngine::PKRole key;
	key.strAccount = strAccount;
	key.rolePos = 0;
	key.worldID = iWorldID;
	pDomainSys->playMaid(bAdvance, key);
}*/
	
void addTowerResetCount(Uint32 dwEntityID, bool ensure)
{
	IEntity* pActor = getEntityFromHandle(dwEntityID);
	if(!pActor) return;

	IClimbTowerSystem* pClimbTowerSystem = static_cast<IClimbTowerSystem*>(pActor->querySubsystem(IID_IClimbTowerSystem));
	if(!pClimbTowerSystem) return;

	pClimbTowerSystem->addTowerResetCount(ensure);
}

void buyArenaCount(Uint32 dwEntityID)
{
	IEntity* pActor = getEntityFromHandle(dwEntityID);
	if(!pActor) return;

	IArenaSystem* pArenaSys = static_cast<IArenaSystem*>(pActor->querySubsystem(IID_IArenaSystem));
	if(!pArenaSys) return;

	pArenaSys->addArenaCount();
}


bool buyPhystrength(Uint32 dwEntityID)
{
	IEntity* pActor = getEntityFromHandle(dwEntityID);
	if(!pActor) return false;

	IItemSystem* pItemSys = static_cast<IItemSystem*>(pActor->querySubsystem(IID_IItemSystem) );
	assert(pItemSys);

	return pItemSys->buyPhyStength();
}

void manorSilverResGoldAddSpeed(Uint32 dwEntityID, int iCost, int iResID)
{
	IEntity* pActor = getEntityFromHandle(dwEntityID);
	if(!pActor) return ;

	IManorSystem *pManorSys = static_cast<IManorSystem*>(pActor->querySubsystem(IID_IManorSystem));
	assert(pManorSys);
 	pManorSys->manorSilverResGoldAddSpeed(iCost,iResID);
}

void manorHeroExpResGoldAddSpeed(Uint32 dwEntityID, int iCost, int iResID)
{
	IEntity* pActor = getEntityFromHandle(dwEntityID);
	if(!pActor) return ;

	IManorSystem *pManorSys = static_cast<IManorSystem*>(pActor->querySubsystem(IID_IManorSystem));
	assert(pManorSys);
 	pManorSys->manorHeroExpResGoldAddSpeed(iCost,iResID);
}

void manorGoldAddWuHunDianLevelUpSpeed(Uint32 hEntity,int iCost)
{
	IEntity* pActor = getEntityFromHandle(hEntity);
	if(!pActor) return ;

	IManorSystem *pManorSys = static_cast<IManorSystem*>(pActor->querySubsystem(IID_IManorSystem));
	assert(pManorSys);
 	pManorSys->manorGoldAddWuHunDianLevelUpSpeed(iCost);
}
void manorGoldAddTieJiangPuLevelUpSpeed(Uint32 hEntity,int iCost)
{
	IEntity* pActor = getEntityFromHandle(hEntity);
	if(!pActor) return ;

	IManorSystem *pManorSys = static_cast<IManorSystem*>(pActor->querySubsystem(IID_IManorSystem));
	assert(pManorSys);
 	pManorSys->manorGoldAddTieJiangPuLevelUpSpeed(iCost);
}

void ShopsytemGoldRefreshHonorConvet(Uint32 hEntity)
{
	IEntity* pActor = getEntityFromHandle(hEntity);
	if(!pActor) return ;

	IShopSystem *pShopSys = static_cast<IShopSystem*>(pActor->querySubsystem(IID_IShopSystem));
	assert(pShopSys);
 	pShopSys->refreshActorHonorConvertShop();
}

void ShopsystemYuanbaoRefreshNormalShop(Uint32 hEntity)
{
	IEntity* playerE =  getEntityFromHandle(hEntity);
	if(!playerE)
	{
		return;
	}
	IShopSystem* shopSystem = static_cast<IShopSystem*>(playerE->querySubsystem(IID_IShopSystem));
	assert(shopSystem);
	shopSystem->normalShopRefreshCallback();
}

void confirmArenaGlodResetCD(Uint32 hEntity)
{
	IEntity* pActor = getEntityFromHandle(hEntity);
	if(!pActor) return ;

	IArenaSystem *pArenaSys = static_cast<IArenaSystem*>(pActor->querySubsystem( IID_IArenaSystem));
	assert(pArenaSys);

	pArenaSys->confirmArenaGlodResetCD();
}

void manorResHarvestReturn(Uint32 hEntity)
{
	IEntity* pActor = getEntityFromHandle(hEntity);
	if(!pActor) return ;

	IManorSystem *pManorSys = static_cast<IManorSystem*>(pActor->querySubsystem(IID_IManorSystem));
	assert(pManorSys);
 	pManorSys-> manorResHarvestReturn();
}

void manorNoCanLootActorReturn(Uint32 hEntity)
{
	IEntity* pActor = getEntityFromHandle(hEntity);
	if(!pActor) return ;

	IManorSystem *pManorSys = static_cast<IManorSystem*>(pActor->querySubsystem(IID_IManorSystem));
	assert(pManorSys);
 	pManorSys-> manorNoCanLootActorReturn();
}

void contributeShopRefreshConfirm(Uint32 hEntity)
{
	IEntity* pActor = getEntityFromHandle(hEntity);
	if(!pActor) return ;

	ILegionSystem *pLegionSys = static_cast<ILegionSystem*>(pActor->querySubsystem(IID_ILegionSystem));
	assert(pLegionSys);
	pLegionSys->contributeShopRefreshConfirm();
}

void leaveLegionCallBack(Uint32 hEntity)
{
	IEntity* pActor = getEntityFromHandle(hEntity);
	if(!pActor) return ;

	ILegionSystem *pLegionSys = static_cast<ILegionSystem*>(pActor->querySubsystem(IID_ILegionSystem));
	assert(pLegionSys);
	pLegionSys->confirmLeaveLegion();
}



