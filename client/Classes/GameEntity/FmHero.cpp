#include "FmHero.h"
#include "FmEntityMgr.h"
#include "Event/FmUpdateEvent.h"
#include "FmEntity.h"
#include "Enum/FmComponentEnum.h"
#include "GameData/DataManager.h"
#include "Enum/FmPropertyIndex.h"
#include "FmEntityRender.h"
#include "FightSoul/FmFightSoulBag.h"
#include "NetWork/GameServer.pb.h"
#include "Equip/FmEquip.h"


NS_FM_BEGIN

	Hero::Hero( uint8 entityType, uint entityId, const string& name )
	: Entity( entityType, entityId, name )
{	 
	m_PropertySet.Init("Hero");
	m_render = NULL;
	m_scene = NULL;
	m_data = NULL;
	AddComponent(EComponentId_FightSoulBags, new FightSoulBag(this, GSProto::en_FSConType_Wear));
	AddComponent(EComponentId_EquipBags, new EquipBag(this, GSProto::en_EquipContainer_HeroWear));
}

Hero::~Hero()
{
	FM_SAFE_DELETE(m_render);
}

Entity* Hero::Create( uint entityId, const string& entityName, EntityCreateOpt* createOpt )
{
	Hero* entity = new Hero( EEntityType_Hero, entityId, entityName );
	return entity;
}

void Hero::InitInterface()
{
	// 注册对象创建接口
	SEntityAlloc entityAlloc;
	entityAlloc.m_EntityCreator = Hero::Create;
	EntityMgr::GetInstance().RegisterEntityAlloc( EEntityType_Hero, entityAlloc );
}


//bool g_optizSetVisibleNeed = false;
void Hero::Update( uint delta )
{
	Entity::Update( delta );
	EventUpdate evt( GetEntityId(), delta );
	GetEventDispatch()->DispatchEvent( &evt );

	m_render->Update(delta);
}


void Hero::OnEntryScene( Scene* scene )
{
	if( scene == 0 )
		return;
	m_scene = scene;

	/*if (this == GetLocalPlayer())
	{
		const SceneData* sceneData = SceneMgr::GetInstance().GetSceneData( scene->GetSceneId() );
		if (sceneData)
		{
			this->GetRender()->SetFlipX(Math::HasFlag( sceneData->GetEntity()->GetUint( EScene_Flag ), ESceneFlag_PlayerFlip ));	
		}		
	}	*/
}

void Hero::OnLeaveScene( Scene* scene )
{
	m_scene = NULL;
}


EntityRender* Hero::GetRender()
{
	return m_render;
}

stHeroBornData* Hero::GetHeroData()
{
	return m_data;
}

FightSoulBag* Hero::GetFightSoulBags()
{
	return GetComponent<FightSoulBag>( EComponentId_FightSoulBags );
}

EquipBag* Hero::GetEquipBags()
{
	return GetComponent<EquipBag>( EComponentId_EquipBags );
}

InnatesKill* Hero::GetHeroInnateskill()
{
	return GetComponent<InnatesKill>(EComponentId_InnatesKill);
}

HeroFavorite* Hero::GetHeroFavorite()
{
	return GetComponent<HeroFavorite>(EComponentId_HeroFavorite);
}

NS_FM_END