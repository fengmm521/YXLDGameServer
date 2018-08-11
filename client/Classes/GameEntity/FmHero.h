/********************************************************************
created:	2012-06-02
author:		pelog
summary:	场景内的对象
*********************************************************************/
#pragma once

#include "FmEntity.h"
#include "FmGeometry.h"
#include "cocos2d.h"
#include "Numeric/HeroBornData.h"
#include "InnatesKill/FmInnatesKill.h"
#include "InnatesKill/FmHeroFavorite.h"

USING_NS_CC;

NS_FM_BEGIN

class Scene;
class EntityRender;
class FightSoulBag;
class EquipBag;

class Hero : public Entity
{
protected:
	Hero( uint8 entityType, uint entityId, const string& name );

	Scene*	m_scene;		// 所在场景对象id
	CCPoint m_Position;
	EntityRender* m_render;
	stHeroBornData* m_data;

public:
	virtual ~Hero();

	// 创建接口
	static Entity* Create( uint entityId, const string& entityName, EntityCreateOpt* createOpt );
	static void InitInterface();

	void SetData(stHeroBornData* data){m_data = data;}
	// 更新接口
	virtual void Update( uint delta );

	// 当前所在的场景对象id
	Scene* GetScene() { return m_scene; }

	// 进入场景
	virtual void OnEntryScene( Scene* scene );

	// 离开场景
	virtual void OnLeaveScene( Scene* scene );

	// 获取渲染组件
	EntityRender* GetRender();

	// 坐标
	CCPoint& GetPosition()  { return m_Position; }
	void SetPosition( CCPoint& position ) { m_Position=position; }

	stHeroBornData* GetHeroData();

	void AddRender(EntityRender* render){m_render = render;}

	// 背包
	FightSoulBag* GetFightSoulBags();
	EquipBag* GetEquipBags();

	//天赋技能
	InnatesKill* GetHeroInnateskill();

	//喜好品
	HeroFavorite* GetHeroFavorite();
};

NS_FM_END