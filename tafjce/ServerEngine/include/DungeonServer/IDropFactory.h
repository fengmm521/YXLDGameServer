#ifndef __IDROPFACTORY_H__
#define __IDROPFACTORY_H__

enum
{
	en_DropType_Prop = GSProto::en_ShopType_Prop,
	en_DropType_Item = GSProto::en_ShopType_Item,
	en_DropType_Hero = GSProto::en_ShopType_Hero,
};


class IDropFactory:public IComponent
{
public:

	// 参数: 掉落计算
	virtual bool calcDrop(int iDropID, GSProto::FightAwardResult& awardResult) = 0;

	// 执行掉落到玩家身上
	virtual void excuteDrop(HEntity hActor,  GSProto::FightAwardResult& awardResult, int iReason) = 0;

	// 合并物品奖励到一个Result
	virtual void addItemToResult(GSProto::FightAwardResult& awardResult, int iItemID, int iItemCount) = 0;

	// 合并属性奖励到一个Result
	virtual void addPropToResult(GSProto::FightAwardResult& awardResult, int iLifeAttID, int iValue) = 0;

	virtual void excuteDropWithNotify(HEntity hActor, GSProto::FightAwardResult& awardResult, int iReason) = 0;
};


#endif
