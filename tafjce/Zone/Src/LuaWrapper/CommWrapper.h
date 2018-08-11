#ifndef __COMM_WRAPPER_H__
#define __COMM_WRAPPER_H__

//tolua_begin

string getWorkDir();

void sendCommDlg(Uint32 dwActor, const string& strMsg, const string& strLeftName, const string& strLeftCmd, const string& strRightName, const string& strRightCmd);
void doFightSoulCombine(Uint32 dwEntityID, int iSrcConType, int iSrcPos, int iDstConType, int iDstPos, Uint32 dwHeroObjectID);
void enableFSAdvance(Uint32 dwEntityID);
void LogMsg(const string& strMsg);
void autoCombineFightSoul(Uint32 dwEntityID);
//void loveMaid(Uint32 dwEntityID, bool bAdvance);
//void playMaid(Uint32 dwEntityID, bool bAdvance, const string& strAccount, int iWorldID);
void addTowerResetCount(Uint32 dwEntityID, bool ensure);
void buyArenaCount(Uint32 dwEntityID);
bool buyPhystrength(Uint32 dwEntityID);
void manorSilverResGoldAddSpeed(Uint32 dwEntityID, int iCost, int iResID);
void manorHeroExpResGoldAddSpeed(Uint32 hEntity,int iCost, int iResID);
void manorGoldAddWuHunDianLevelUpSpeed(Uint32 hEntity,int iCost);
void manorGoldAddTieJiangPuLevelUpSpeed(Uint32 hEntity,int iCost);
void ShopsytemGoldRefreshHonorConvet(Uint32 hEntity);
void ShopsystemYuanbaoRefreshNormalShop(Uint32 hEntity);

void confirmArenaGlodResetCD(Uint32 hEntity);
void manorResHarvestReturn(Uint32 hEntity);
void manorNoCanLootActorReturn(Uint32 hEntity);
void contributeShopRefreshConfirm(Uint32 hEntity);
void leaveLegionCallBack(Uint32 hEntity);

	


//tolua_end

#endif
