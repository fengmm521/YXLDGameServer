

#include "FmEquip.h"
#include "FmEntityMgr.h"
#include "FmEntity.h"
#include "NetWork/GameServer.pb.h"
#include "Enum/FmPropertyIndex.h"
#include "Player/FmPlayer.h"
#include "Player/FmLocalPlayer.h"
#include "GameEntity/FmHero.h"
#include "Numeric/HeroBornData.h"
#include "GameData/DataManager.h"
#include "Enum/FmNumricEnum.h"


NS_FM_BEGIN

struct SortEquip {
	bool operator ()(Equip* equip1,Equip* equip2)
	{
		if (equip1 == NULL)
		{
			return true;
		}
		if (equip2 == NULL)
		{
			return false;
		}

		if (equip1->GetData()->m_quality < equip2->GetData()->m_quality)
		{
			return false;
		}
		else if (equip1->GetData()->m_quality > equip2->GetData()->m_quality)
		{
			return true;
		}
		else
		{
			if (equip1->GetData()->m_step < equip2->GetData()->m_step)
			{
				return false;
			}
			else if (equip1->GetData()->m_step > equip2->GetData()->m_step)
			{
				return true;
			}
			else
			{
				return equip1->GetEntityId() < equip2->GetEntityId();
			}
		}

		return false;
	}
};

EquipBag::EquipBag(Entity* entity, int type)
: Component( entity )
{
	m_containerType = type;
	if (type == GSProto::en_EquipContainer_HeroWear)
	{
		m_equips.resize(6, NULL);
	}
	else
	{
		m_equips.resize(GSProto::MAX_EQUIP_BAGSIZE, NULL);
	}
}

int EquipBag::GetSellMoneyByQuality( int quality )
{
	int money = 0;

	for (size_t i=0; i<m_equips.size(); i++)
	{
		Equip* equip = m_equips[i];
		if (equip && quality >= equip->GetData()->m_quality)
		{
			money += equip->GetInt(EEquip_SellMoney);
		}
	}
	return money;
}

void EquipBag::GetEquipListByWearPos( vector<Equip*>& list, int pos)
{
	if (pos == -1)
	{
		for (size_t i=0; i<m_equips.size(); i++)
		{
			Equip* equip = m_equips[i];
			if (equip)
			{
				list.push_back(equip);
			}
		}
	}
	else
	{
		for (size_t i=0; i<m_equips.size(); i++)
		{
			Equip* equip = m_equips[i];
			if (equip && pos == equip->GetData()->m_equipPos)
			{
				list.push_back(equip);
			}
		}
	}

	sort(list.begin(), list.end(), SortEquip());
}

bool EquipBag::EquipIsInBag( Equip* equip )
{
	if (equip == NULL)
		return false;

	for (size_t i=0; i<m_equips.size(); i++)
	{
		if (equip == m_equips[i])
		{
			return true;
		}
	}

	return false;
}

int EquipBag::GetEquipCount( int id, Equip* equip )
{
	int count = 0;

	for (size_t i=0; i<m_equips.size(); i++)
	{
		if ( m_equips[i] &&  m_equips[i] != equip && m_equips[i]->GetData()->m_ID == id)
		{
			count++;
		}
	}

	return count;
}

void EquipBag::GetEquipList( vector<Equip*>& list )
{
	for (size_t i=0; i<m_equips.size(); i++)
	{
		Equip* equip = m_equips[i];
		if (equip)
		{
			list.push_back(equip);
		}
	}

	sort(list.begin(), list.end(), SortEquip());
}

void EquipBag::getSizeUpEquip(vector<Equip *> &list,const int equipType,const int heroType)
{
	for (size_t i = 0; i < m_equips.size(); ++i)
	{
		Equip *equip = m_equips[i];
		if (equip == NULL)
		{
			continue;
		}
		if ((((1 << (heroType - 1)) & equip->GetData()->m_zhiyeLimit) != 0) && (equipType == equip->GetData()->m_equipPos))
		{
			list.push_back(equip);
		}
	}
	sort(list.begin(), list.end(), SortEquip());
}

bool EquipBag::HeroEquipIsWear(const int pos)
{
	//这里只是给英雄使用
	if (m_equips[pos] == NULL)
	{
		return false;
	}
	return true;
}

bool EquipBag::HeroHasCanEquip(const int equipType,const int heroType)
{
	vector<Equip *> list;
	getSizeUpEquip(list,equipType,heroType);
	bool isCanEquip = false;
	Player *player = GetLocalPlayer();
	for (size_t i = 0 ; i < list.size(); ++i)
	{
		if (player->GetInt(EPlayer_Lvl) >= list[i]->GetData()->m_lvlLimit)
		{
			isCanEquip = true;
			break;
		}
	}
	return isCanEquip;
}

Equip * EquipBag::getEquipByObjectId(const int objectId)
{
	for (size_t i = 0; i < m_equips.size(); ++i)
	{
		if (m_equips[i] != NULL && m_equips[i]->GetInt(EEquip_ObjID) == objectId)
		{
			return m_equips[i];
		}
	}
	return NULL;
}

bool EquipBag::PlayerCanEquip()
{
	for (size_t i = 0; i < GetLocalPlayer()->GetHeroList(false).size();++i)
	{
		Hero *hero = dynamic_cast<Hero*>(EntityMgr::GetInstance().GetEntityById(GetLocalPlayer()->GetHeroList(false)[i]));
		stHeroBornData* heroData = CDataManager::GetInstance().GetGameData<stHeroBornData>(DataFileHeroBorn, hero->GetInt(EHero_HeroId));
		int a[6] = {1,2,3,4,5,6};
		for (size_t j = 1; j < 6;++j)
		{
			if (HeroHasCanEquip(a[j],heroData->m_heroType))
			{
				return true;
			}
		}
	}	
	return false;
}


Equip::Equip( uint8 entityType, uint entityId, const string& name )
: Entity( entityType, entityId, name )
{	 
	m_data = NULL;
	m_PropertySet.Init("Equip");
}

Equip::~Equip()
{

}

Entity* Equip::Create( uint entityId, const string& entityName, EntityCreateOpt* createOpt )
{
	Equip* entity = new Equip( EEntityType_Equip, entityId, entityName );
	return entity;
}

void Equip::InitInterface()
{
	// 注册对象创建接口
	SEntityAlloc entityAlloc;
	entityAlloc.m_EntityCreator = Equip::Create;
	EntityMgr::GetInstance().RegisterEntityAlloc( EEntityType_Equip, entityAlloc );
}

void Equip::setAddAttribute(const int lifeattId,const int lifeattValue)
{
	addattribute.lifeattId = lifeattId;
	addattribute.lifeattValue = lifeattValue;
}

NS_FM_END