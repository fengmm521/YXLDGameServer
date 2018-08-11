#ifndef __ILEGION_SYSTEM_H__
#define __ILEGION_SYSTEM_H__

class ILegionSystem:public IEntitySubsystem
{
public:

	virtual string addGift(int iGiftID) = 0;

	virtual string addGift(const ServerEngine::CustomGiftData& customGift) = 0;

	virtual void contributeShopRefreshConfirm() = 0;

	virtual void confirmLeaveLegion()=0;

	virtual void gmAddLegionExp(int iExp) = 0;
	virtual void setLegionGuard(int iCampId, string strLegionName) = 0;

	virtual void openCampBattle(int iReportBegin,int iBattleBegin, int iBattleOver) = 0;
};


#endif
