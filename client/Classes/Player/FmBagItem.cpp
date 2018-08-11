#include "FmBagItem.h"
#include "FmEntityMgr.h"
#include "FmLocalPlayer.h"
#include "Enum/FmPropertyIndex.h"

Bag::Bag(Entity* entity, int type)
	: Component( entity )
{
	m_containerType = type;
}

int Bag::GetItemCountByItemId(int itemId)
{
	int itemNumber = 0;
	for(vector<BagItem*>::iterator p = m_bagItems.begin(); p != m_bagItems.end(); p++)
	{
		if((*p)->m_itemData->m_ID == itemId)
		{
			itemNumber += (*p)->GetInt(EBagItem_Count);
		}
	}
	return itemNumber;
}


BagItem::~BagItem(){
	
}
BagItem::BagItem( uint8 entityType, uint entityId, const string& name )
	:Entity(entityType,entityId,name)
	,m_itemData(NULL)
{
	m_PropertySet.Init( PropertyDescMgr::GetInstance().Get( "BagItem" ) );
}
Entity* BagItem::Create( uint entityId, const string& entityName, EntityCreateOpt* createOpt )
{
	BagItem* entity = new BagItem( EEntityType_Item,entityId, entityName );
	return entity;
}

void BagItem::InitInterface()
{
	// 注册对象创建接口
	SEntityAlloc entityAlloc;
	entityAlloc.m_EntityCreator = BagItem::Create;
	EntityMgr::GetInstance().RegisterEntityAlloc( EEntityType_Item, entityAlloc );
}
