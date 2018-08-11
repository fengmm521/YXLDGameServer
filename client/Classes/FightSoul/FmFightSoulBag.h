/********************************************************************
created:	2014-02-28
author:		pelog
summary:	场景内的对象
*********************************************************************/
#pragma once

#include "FmEntity.h"
#include "FmGeometry.h"
#include "Numeric/FightSoulData.h"

NS_FM_BEGIN
class FightSoul;

class FightSoulBag : public Component
{
private:
	vector<FightSoul*> m_fightSouls;
	int	m_containerType;
public:
	vector<FightSoul*>& GetFightSoulList(){return m_fightSouls;}
	FightSoulBag(Entity* entity, int type);

	FightSoul* GetFightSoulByPos(int pos)
	{
		if (pos < 0 || pos >= m_fightSouls.size())
		{
			return NULL;
		}

		return m_fightSouls[pos];
	}
};

class FightSoul : public Entity
{
protected:
	FightSoul( uint8 entityType, uint entityId, const string& name );
	stFightSoulData* m_data;
public:
	virtual ~FightSoul();

	// 创建接口
	static Entity* Create( uint entityId, const string& entityName, EntityCreateOpt* createOpt );
	static void InitInterface();
	void SetData(stFightSoulData* data){m_data = data;}
	stFightSoulData* GetData(){return m_data;}
};

NS_FM_END