/********************************************************************
created:	2014-02-28
author:		pelog
summary:	场景内的对象
*********************************************************************/
#pragma once

#include "FmEntity.h"
#include "FmGeometry.h"
#include "Numeric/EquipData.h"

NS_FM_BEGIN
class Equip;

class EquipBag : public Component
{
private:
	vector<Equip*> m_equips;
	int	m_containerType;
public:
	vector<Equip*>& GetEquipList(){return m_equips;}
	EquipBag(Entity* entity, int type);

	Equip* GetEquipByPos(int pos)
	{
		if (pos < 0 || pos >= m_equips.size())
		{
			return NULL;
		}

		return m_equips[pos];
	}

	int GetSellMoneyByQuality(int quality);

	void GetEquipListByWearPos(vector<Equip*>& list, int pos);

	int GetEquipCount( int id, Equip* equip );

	bool EquipIsInBag(Equip* equip);

	void GetEquipList(vector<Equip*>& list);

	//获得符合要求的装备列表
	void getSizeUpEquip(vector<Equip *> &list,const int equipType,const int heroType);
	//hero 使用的 
	bool HeroEquipIsWear(const int pos);
	//是否有可以穿戴的装备
	bool HeroHasCanEquip(const int equipType,const int heroType);
	//是否有可以穿戴的装备 玩家用的
	bool PlayerCanEquip();
    //根据objectID 获取装备
	Equip *getEquipByObjectId(const int objectId);
};

//额外属性
struct AddAttributes
{
	int lifeattId;
	int lifeattValue;
	AddAttributes(){lifeattId = 0;lifeattValue = 0;}
};

class Equip : public Entity
{
protected:
	Equip( uint8 entityType, uint entityId, const string& name );
	stEquipData* m_data;
	AddAttributes addattribute;
public:
	virtual ~Equip();

	// 创建接口
	static Entity* Create( uint entityId, const string& entityName, EntityCreateOpt* createOpt );
	static void InitInterface();
	void SetData(stEquipData* data){m_data = data;}
	stEquipData* GetData(){return m_data;}
	void setAddAttribute(const int lifeattId,const int lifeattValue);
	AddAttributes getAddAttribute()
	{
		return addattribute;
	}
};

NS_FM_END