#ifndef __IITEMFACTORY_H__
#define __IITEMFACTORY_H__

#include "RoleData.h"


class IItem;
typedef Handle<IItem*> HItem;

class IItem:public IObject
{
public:

	// 功能: 使用物品,注意:这里不负责消耗自身，需要结合背包和具体逻辑做消耗
	// 参数: [hTarget] 目标
	// 参数: [ctx] 现场
	virtual bool useItem(HEntity hTarget, const PropertySet& ctx) = 0;

	// 功能: 打包成JCE对象
	virtual void packJce(ServerEngine::ItemData& itemSaveData) = 0;

	virtual HItem getHandle() = 0;

	virtual void packScItemInfo(GSProto::ItemInfo& scItemInfo) = 0;

	virtual void packScEquipInfo(GSProto::EquipInfo& scEquipInfo, int iPos) = 0;

	virtual void changeProperty(HEntity hEntity, bool bOn) = 0;
};


class IItemFactory:public IComponent
{
public:

	virtual const PropertySet* getItemPropset(int iItemID) = 0;

	virtual bool createItemFromID(int iItemID, int iCount, vector<HItem>& itemList) = 0;

	virtual IItem* createItemFromDB(const ServerEngine::ItemData& itemSaveData) = 0;

	virtual const map<int, int>* queryFavoriteProp(int iItemID) = 0;

	virtual int getDropIdByConvertGiftId(const string& strConvertGiftId) = 0;

	virtual int getShowLifeAtt(int iPropID) = 0; // 装备动态属性显示LifeAtt

	virtual bool bActorCanRepeatGetFlag(const string& strConvertGiftId) = 0;

	/**
	  * @ breif 获取洗练等级限制
	  * @ return 洗练等级限制
	  */
	virtual int getXilianLvLimit() = 0;
};


IItem* getItemFromHandle(HItem hItem);


#endif
