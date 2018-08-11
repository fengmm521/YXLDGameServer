#ifndef __DROP_FACTORY_H__
#define __DROP_FACTORY_H__

#include "IDropFactory.h"

struct DropSingleItem
{
	DropSingleItem():iType(0), iDropTypeID(0), iCount(0), iHeroLevelStep(0), iChance(0){}

	int iType;
	int iDropTypeID;
	int iCount;
	int iHeroLevelStep;
	int iChance;
};

struct DropGroup
{
	map<int, vector<DropSingleItem> > mapDropList;
};


class DropFactory:public ComponentBase<IDropFactory, IID_IDropFactory>
{
public:

	DropFactory();
	~DropFactory();

	// IComponent Interface
	virtual bool initlize(const PropertySet& propSet);

	// IDropFactory Interface
	virtual bool calcDrop(int iDropID, GSProto::FightAwardResult& awardResult);
	virtual void excuteDrop(HEntity hActor,  GSProto::FightAwardResult& awardResult, int iReason);
	virtual void addItemToResult(GSProto::FightAwardResult& awardResult, int iItemID, int iItemCount);
	virtual void addPropToResult(GSProto::FightAwardResult& awardResult, int iLifeAttID, int iValue);
	virtual void excuteDropWithNotify(HEntity hActor,   GSProto::FightAwardResult& awardResult, int iReason);
	
	void addHeroToResult(GSProto::FightAwardResult& awardResult, int iHeroID, int iLevelStep, int iCount);
	void _excuteDrop(HEntity hActor, GSProto::FightAwardResult& awardResult, int iReason);
	void loadDropCfg();

private:

	void _calcDropGroup(const DropGroup& groupData, GSProto::FightAwardResult& awardResult);
	void _calcDropOneTag(const vector<DropSingleItem>& itemList, GSProto::FightAwardResult& awardResult);
	void _awardOneProp(HEntity hActor, int iLifeAttID, int iValue, int iReason);
	void _awardOneItem(HEntity hActor, GSProto::FightAwardItem& pItemAward, int iItemID, int iCount, int iReason);
	void _awardOneHero(HEntity hActor, GSProto::FightAwardHero& awardHero, int iReason);
	void _awardOnePay(HEntity hActor, GSProto::AwardPay& awardPay, int reason);
	
private:

	typedef map<int, DropGroup> MapDropGroup;

	MapDropGroup m_mapDropGroup;
};


#endif



