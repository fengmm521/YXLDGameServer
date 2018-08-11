

#include "FmFightSoulBag.h"
#include "FmEntityMgr.h"
#include "FmEntity.h"
#include "NetWork/GameServer.pb.h"

NS_FM_BEGIN

FightSoulBag::FightSoulBag(Entity* entity, int type)
: Component( entity )
{
	m_containerType = type;
	if (type == GSProto::en_FSConType_Bag)
	{
		m_fightSouls.resize(GSProto::MAX_FIGHTSOUL_BAGSIZE, NULL);
	}
	else
	{
		m_fightSouls.resize(GSProto::MAX_FIGHTSOUL_WEARSIZE, NULL);
	}
}

FightSoul::FightSoul( uint8 entityType, uint entityId, const string& name )
: Entity( entityType, entityId, name )
{	 
	m_data = NULL;
	m_PropertySet.Init("FightSoul");
}

FightSoul::~FightSoul()
{
	
}

Entity* FightSoul::Create( uint entityId, const string& entityName, EntityCreateOpt* createOpt )
{
	FightSoul* entity = new FightSoul( EEntityType_FightSoul, entityId, entityName );
	return entity;
}

void FightSoul::InitInterface()
{
	// 注册对象创建接口
	SEntityAlloc entityAlloc;
	entityAlloc.m_EntityCreator = FightSoul::Create;
	EntityMgr::GetInstance().RegisterEntityAlloc( EEntityType_FightSoul, entityAlloc );
}

NS_FM_END