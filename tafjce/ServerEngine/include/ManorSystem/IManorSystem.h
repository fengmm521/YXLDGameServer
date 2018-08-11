#ifndef __IMANOR_SYSTEM_H__
#define __IMANOR_SYSTEM_H__

class IManorSystem : public IEntitySubsystem
{
public:
	virtual void manorSilverResGoldAddSpeed(int iCost,int iResID) = 0;
	virtual void manorHeroExpResGoldAddSpeed(int iCost, int iResID) = 0;
	virtual void manorGoldAddWuHunDianLevelUpSpeed(int iCost) = 0;
	virtual void manorGoldAddTieJiangPuLevelUpSpeed(int iCost) = 0;
	virtual void processLootFight(const ServerEngine::PKRole& targetKey, const ServerEngine::BattleData& data) = 0;
	virtual void getActorSilverRes(vector<ServerEngine::ManorResData>& silverRes) = 0;
	virtual void getActorHeroExpRes(vector<ServerEngine::ManorResData>& heroExpRes) = 0;

	virtual void initMachineData(string strData) = 0;
	virtual void resetMachineData() = 0;
	virtual void manorResHarvestReturn() = 0;
	virtual void resetHarvest() = 0;
	virtual void manorNoCanLootActorReturn() = 0;

	virtual int getTieJiangPuLevel() = 0 ;
	virtual int getWuHunDianLevel() = 0;

	virtual void GMLootFight(ServerEngine::PKRole pkRole) = 0;
};


#endif
