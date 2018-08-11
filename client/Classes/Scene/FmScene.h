/********************************************************************
created:	2012-05-20
author:		Fish (于国平)
summary:	游戏场景
*********************************************************************/
#pragma once

#include "FmSafeTraverseSet.h"
#include "FmGeometry.h"
#include "cocoa/CCGeometry.h"
#include "FmMapLayer.h"
#include "GameEntity/FmHero.h"


NS_FM_BEGIN

class Scene
{
protected:
	uint					m_SceneId;			// 场景id,注意:不是对象id
	SafeTraverseSet<uint>	m_SceneEntitys;		// 场景内的对象
	MapLayer*				m_mapLayer;
	CCLayer*				m_objLayer;
	CCLayer*				m_effectLayer;
	vector<CCPoint>			m_heroPosList;
	bool					m_pause;
public:
	static Scene* Create(uint sceneId);
	Scene();
	virtual ~Scene();
	
	void init(uint sceneId);

	void LoadTileMap();
	
	// 场景定义id
	uint GetSceneId() { return m_SceneId; }

	// 场景内的子对象id列表
	set<uint>& GetEntityIds();
	// 场景内的子对象id列表(方便导出到tolua++)
	void GetObjectList( vector<uint>& objectIds );

	// 添加一个子对象
	void AddEntity( Hero* sceneEntity );
	// 移除一个子对象
	void RemoveEntity( uint entityId );

	//释放所有对象
	void ReleaseAllEntity();

	// 更新
	virtual void Update( uint delta );
	void AddEntityRender( EntityRender* entityRender);

	CCLayer* GetObjectLayer(){return m_objLayer;}
	CCLayer* GetEffectLayer(){return m_effectLayer;}
	vector<CCPoint>& GetHeroPosList(){return m_heroPosList;}
	void Pause();
	bool IsPause(){return m_pause;}
	void Resume(int iSkillID);
	void GetLeftHeroList(vector<Hero*>& heroList);
	Hero* GetLeftGodAnimal();
	void ChangeMap(int index);
};

NS_FM_END