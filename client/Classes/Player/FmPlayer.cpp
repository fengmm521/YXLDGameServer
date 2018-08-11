#include "FmPlayer.h"
#include "FmEntityMgr.h"
#include "Enum/FmComponentEnum.h"

#include "FmLocalPlayer.h"
#include "Script/FmScriptSys.h"
#include "Scene/SceneMgr.h"
//#include "GameEntity/FmSceneEntity.h"
#include "Enum/FmPropertyIndex.h"
#include "Enum/FmCommonEnum.h"
#include "GameEntity/FmHero.h"
#include "NetWork/ClientSinkCallbackMgr.h"
#include "GameData/DataManager.h"
#include "Enum/FmNumricEnum.h"
#include "FightSoul/FmFightSoulBag.h"
#include "CocoStudio/GUI/BaseClasses/UIWidget.h"
#include "UI/FmUIMgr.h"
#include "Equip/FmEquip.h"
#include "Numeric/HeroQualityData.h"
#include "Numeric/FavoriteData.h"

USING_NS_CC_EXT;
using namespace gui;

struct SortAllCanSummonHero
{
	bool operator()(uint id1,uint id2)
	{
		Player *player = GetLocalPlayer();
		stHeroBornData* heroDataOne = CDataManager::GetInstance().GetGameData<stHeroBornData>(DataFileHeroBorn, id1);
		stHeroBornData* heroDataTwo = CDataManager::GetInstance().GetGameData<stHeroBornData>(DataFileHeroBorn, id2);
		if (heroDataOne == NULL || heroDataTwo == NULL)
		{
			if (heroDataOne == NULL && heroDataTwo == NULL)
			{
				return id1 < id2;
			}
			else if (heroDataTwo == NULL)
			{
				return true;
			}
			return false;
		}
		else
		{
			if (heroDataOne->m_ID < heroDataTwo->m_ID)
			{
				return true;
			}
			return false;
		}
		return false;
	}
};

struct SortGodAnimalId
{
	bool operator()(int id1,int id2)
	{	
		Player *Player = GetLocalPlayer();
		/*if (Player->GodAnimalisHaveByAnimalId(id1) == false || Player->GodAnimalisHaveByAnimalId(id2) == false)
		{
		return Player->GodAnimalisHaveByAnimalId(id1) ? true : false;
		}*/
		GodAnimal * a = Player->GetGodAnimalByAnimalId(id1);
		GodAnimal * b = Player->GetGodAnimalByAnimalId(id2);
		if (a == NULL || b == NULL)
		{
			if (a == NULL && b == NULL)
			{
				return id1 < id2 ;
			}
			else if(b == NULL)
			{
				return true;
			}
			return false;
		}
		else 
		{
			if (a->GetBool(EGodAnimal_IsActive) || b->GetBool(EGodAnimal_IsActive))
			{
				return a->GetBool(EGodAnimal_IsActive);
			}
			else
			{
				/*if (a->GetInt(EGodAnimal_LevelStep) == b->GetInt(EGodAnimal_LevelStep))
				{
					if (a->GetInt(EGodAnimal_Level) == b->GetInt(EGodAnimal_Level))
					{
						return id1 > id2;
					}
					return a->GetInt(EGodAnimal_Level) > b->GetInt(EGodAnimal_Level);
				}
				return a->GetInt(EGodAnimal_LevelStep) > b->GetInt(EGodAnimal_LevelStep);*/
				return id1 < id2;
			}
		}
	 }	
};

struct SortHero {
	bool operator ()(uint id1, uint id2)
	{
		Player* player = GetLocalPlayer();
		Hero* a = dynamic_cast<Hero*>(EntityMgr::GetInstance().GetEntityById(id1));
		Hero* b =dynamic_cast<Hero*>(EntityMgr::GetInstance().GetEntityById(id2));

		if (a == NULL || b == NULL)
		{
			return false;
		}

		if (player->IsInFormation(a))
		{
			if (player->IsInFormation(b) == false)
			{
				return true;
			}
			else
			{
				/*if (a->GetInt(EHero_LvlStep) == b->GetInt(EHero_LvlStep))
				{
					if (a->GetInt(EHero_Lvl) == b->GetInt(EHero_Lvl))
					{
						return id1 > id2;
					}
					return a->GetInt(EHero_Lvl) > b->GetInt(EHero_Lvl);
				}
			
				return a->GetInt(EHero_LvlStep) > b->GetInt(EHero_LvlStep);*/

				if (a->GetInt(EHero_Lvl) == b->GetInt(EHero_Lvl))
				{
					if (a->GetInt(EHero_Quality) == b->GetInt(EHero_Quality))
					{
					    if (a->GetInt(EHero_LvlStep) == b->GetInt(EHero_LvlStep))
					    {
							return id1 > id2;
					    }
						return a->GetInt(EHero_LvlStep) > b->GetInt(EHero_LvlStep);
					}
					return a->GetInt(EHero_Quality) > b->GetInt(EHero_Quality);
				}
				return a->GetInt(EHero_Lvl) > b->GetInt(EHero_Lvl);
			}
		}
		else
		{
			/*if (player->IsInFormation(b) == false)
			{
				if (a->GetInt(EHero_LvlStep) == b->GetInt(EHero_LvlStep))
				{
					if (a->GetInt(EHero_Lvl) == b->GetInt(EHero_Lvl))
					{
						return id1 > id2;
					}
					return a->GetInt(EHero_Lvl) > b->GetInt(EHero_Lvl);
				}

				return a->GetInt(EHero_LvlStep) > b->GetInt(EHero_LvlStep);
			}
			else
			{
				return false;
			}
		}

		return false;*/

		if (player->IsInFormation(b) == false)
		{
			if (a->GetInt(EHero_Lvl) == b->GetInt(EHero_Lvl))
			{
				if (a->GetInt(EHero_Quality) == b->GetInt(EHero_Quality))
				{
					if (a->GetInt(EHero_LvlStep) == b->GetInt(EHero_LvlStep))
					{
						return id1 > id2;
					}
					return a->GetInt(EHero_LvlStep) > b->GetInt(EHero_LvlStep);
				}
				return a->GetInt(EHero_Quality) > b->GetInt(EHero_Quality);
			}
			return a->GetInt(EHero_Lvl) > b->GetInt(EHero_Lvl);
		}
		else
		{
			return false;
		}
		}
	    return false;
	}
};


NS_FM_BEGIN

	Player::Player( uint8 entityType, uint entityId, const string& name, bool isLocalPlayer )
	: Entity( entityType, entityId, name )
{
	m_IsLocalPlayer = isLocalPlayer;

	m_PropertySet.Init( PropertyDescMgr::GetInstance().Get( "Player" ) );

	// 英雄列表
	

	// 本机玩家才会有的模块
	if( m_IsLocalPlayer )
	{
		// 任务模块
		//AddComponent( EComponentId_Quest, new Quest(this) );
		// 背包模块
		//AddComponent( EComponentId_Bags, new FmPlayerBagComponent(this) );
		AddComponent( EComponentId_FightSoulBags, new FightSoulBag(this, GSProto::en_FSConType_Bag) );
		AddComponent( EComponentId_EquipBags, new EquipBag(this, GSProto::en_EquipContainer_ActorBag) );
		AddComponent( EComponentId_Bags, new Bag(this, GSProto::en_BagType_Item) );
		AddComponent( EComponentId_Materail, new Bag(this, GSProto::en_BagType_Materail) );
		AddComponent(EComponentId_HeroSoul,new HeroSoulBag(this));
		

		m_fightValue = 0;
		m_formationLimit = 0;
		InitFormationList();

		ClientSinkCallbackMgr::GetInstance().Register(GSProto::CMD_HERO_UPDATE, HeroUpdate);
		ClientSinkCallbackMgr::GetInstance().Register(GSProto::CMD_HERO_DEL, HeroDel);
		ClientSinkCallbackMgr::GetInstance().Register(GSProto::CMD_DELETE_HEROS, HeroBatchDel);
		ClientSinkCallbackMgr::GetInstance().Register(GSProto::CMD_CHG_FSCONTAINER, FSContainerChg);
		//更新玩家神兽信息
		ClientSinkCallbackMgr::GetInstance().Register(GSProto::CMD_GODANIMAL_UPDATE, GodAnimalUpdate);
		ClientSinkCallbackMgr::GetInstance().Register(GSProto::CMD_EQUIPCONTAINER_CHG, EquipContainerChg);
		ClientSinkCallbackMgr::GetInstance().Register(GSProto::CMD_QUERY_EQUIPDESC_BYID, EquipQuery);

		//更新玩家魂魄
		ClientSinkCallbackMgr::GetInstance().Register(GSProto::CMD_HERO_SOULCHG, HeroSoulChg);
		//更新玩家天赋技能
		ClientSinkCallbackMgr::GetInstance().Register(GSProto::CMD_HERO_TALLENT_UPDATE,HeroTallenSkillChg);
	}

	m_PreDupScene = -1;
}
void Player::GodAnimalUpdate(int iCmd, GSProto::SCMessage& pkg){
	CCLOG("GodAnimalUpdate Enter");
	GSProto::Cmd_Sc_GodAnimalUpdate lGodAnimalListData;
	Player* player = GetLocalPlayer();
	if (lGodAnimalListData.ParseFromString(pkg.strmsgbody()))
	{
		const GoldAnimalRoleBase& lGodAnimalRoleBase = lGodAnimalListData.updateinfo();

		stGodAnimalData* lGodAnimalData = CDataManager::GetInstance().GetGameData<stGodAnimalData>(DataFileGodAniaml, lGodAnimalRoleBase.ibaseid());
		if (!lGodAnimalData)
		{
			CCLOG("Error lGodAnimalData---------- ItemDataId=%d", lGodAnimalRoleBase.ibaseid());
			return;
		}
		GodAnimal* lGodAnimal = player->GetGodAnimalByDwObject( lGodAnimalRoleBase.dwobjectid() );
		if (!lGodAnimal)
		{
			lGodAnimal = dynamic_cast<GodAnimal*>( EntityMgr::GetInstance().CreateEntity( EEntityType_GodAnimal, "", NULL ) );
			if ( !lGodAnimal )
			{
				CCLOG("Error create GodAnimal---------- objId= %d", lGodAnimalRoleBase.dwobjectid());
				return;
			}
			lGodAnimal->setGodAnimalData(lGodAnimalData);
			lGodAnimal->SetInt(EGodAnimal_ObjId, lGodAnimalRoleBase.dwobjectid());
			lGodAnimal->SetInt(EGodAnimal_Level, lGodAnimalRoleBase.ilevel());
			lGodAnimal->SetInt(EGodAnimal_LevelStep, lGodAnimalRoleBase.ilevelstep());
			player->GetGodAnimalList().push_back(lGodAnimal);
		}
		lGodAnimal->setGodAnimalData(lGodAnimalData);
		lGodAnimal->SetInt(EGodAnimal_ObjId, lGodAnimalRoleBase.dwobjectid());
		lGodAnimal->SetInt(EGodAnimal_Level, lGodAnimalRoleBase.ilevel());
		lGodAnimal->SetInt(EGodAnimal_LevelStep, lGodAnimalRoleBase.ilevelstep());
	}
}
void Player::FSContainerChg( int iCmd, GSProto::SCMessage& pkg )
{
	GSProto::Cmd_Sc_ChgFsContainer chgFs;
	if(chgFs.ParseFromString(pkg.strmsgbody() ) )
	{
		FightSoulBag* bag = NULL;
		Player* player = GetLocalPlayer();
		if (player->GetUint(EPlayer_ObjId) == chgFs.dwobjectid())
		{
			bag = player->GetFightSoulBags();
		}

		Hero* hero = player->GetHeroByObjId(chgFs.dwobjectid());
		if ( hero )
		{
			bag = hero->GetFightSoulBags();
		}

		if (bag)
		{
			for (size_t i=0; i<chgFs.szchglist_size(); i++)
			{
				const FightSoulChgItem& chgItem = chgFs.szchglist().Get(i);

				if (chgItem.bdel())
				{
					FM_SAFE_DELETE(bag->GetFightSoulList()[chgItem.ipos()]);
				}
				else
				{
					FightSoul* fightSoul = bag->GetFightSoulByPos(chgItem.ipos());
					const FightSoulItem& data = chgItem.fightsouldata();
					
					if (fightSoul == NULL)
					{
						stFightSoulData* fightSoulData = CDataManager::GetInstance().GetGameData<stFightSoulData>(DataFileFightSoul, data.ibaseid());

						if (!fightSoulData)
						{
							CCLOG("Error fightSoulData---------- ibaseid=%d", data.ibaseid());
							continue;
						}

						fightSoul = dynamic_cast<FightSoul*>( EntityMgr::GetInstance().CreateEntity( EEntityType_FightSoul, fightSoulData->m_name, NULL ) );
						if ( !fightSoul )
						{
							CCLOG("Error create hero---------- objId=%d", data.dwobjectid());
							continue;
						}

						fightSoul->SetData(fightSoulData);
					}

					fightSoul->SetUint(EFightSoul_ObjID, data.dwobjectid());
					fightSoul->SetInt(EFightSoul_Lvl, data.ilevel());
					fightSoul->SetInt(EFightSoul_Exp, data.iexp());
					fightSoul->SetInt(EFightSoul_NextLvExp, data.inextlvexp());
					fightSoul->SetBool(EFightSoul_Lock, data.blocked());
					fightSoul->SetExtraInt("DevourExp", data.ieatexp());
					fightSoul->SetExtraBool("New", true);

					for (size_t j=0; j<data.szproplist_size(); j++)
					{
						const PropItem& propItem =  data.szproplist().Get(j);
						Var var2(propItem.ilifeattid());
						Var var3(propItem.ivalue());
						ScriptSys::GetInstance().Execute_3("SetLifeAttFromC", fightSoul, "Entity", &var2, "Var", &var3, "Var");
					}

					bag->GetFightSoulList()[data.ipos()] = fightSoul;
				}
			}
		}

		UIMgr::GetInstance().RefreshBaseWidgetByName("FightSoul");
		UIMgr::GetInstance().RefreshBaseWidgetByName("FightSoulInfo");
		ScriptSys::GetInstance().Execute("HeroInfo_RefreshFightSoul");
	}
}

void Player::EquipContainerChg( int iCmd, GSProto::SCMessage& pkg )
{
	GSProto::CMD_EQUIPCONTAINER_CHG_SC chgEquip;
	if(chgEquip.ParseFromString(pkg.strmsgbody() ) )
	{
		bool isHero = false;
		EquipBag* bag = NULL;
		Player* player = GetLocalPlayer();
		if (player->GetUint(EPlayer_ObjId) == chgEquip.dwobjectid())
		{
			bag = player->GetEquipBags();
		}

		Hero* hero = player->GetHeroByObjId(chgEquip.dwobjectid());
		if ( hero )
		{
			isHero = true;
			bag = hero->GetEquipBags();
		}

		if (bag)
		{
			for (size_t i=0; i<chgEquip.szchglist_size(); i++)
			{
				const EquipChgItem& chgItem = chgEquip.szchglist().Get(i);

				FM_SAFE_DELETE(bag->GetEquipList()[chgItem.ipos()]);
				if (chgItem.bdel())
				{

				}
				else
				{
					Equip* equip = bag->GetEquipByPos(chgItem.ipos());
					const EquipInfo& data = chgItem.equipdata();

					if (equip == NULL)
					{
						stEquipData* equipData = CDataManager::GetInstance().GetGameData<stEquipData>(DataFileEquip, data.iitemid());

						if (!equipData)
						{
							CCLOG("Error equipData---------- ibaseid=%d", data.iitemid());
							continue;
						}

						equip = dynamic_cast<Equip*>( EntityMgr::GetInstance().CreateEntity( EEntityType_Equip, equipData->m_name, NULL ) );
						if ( !equip )
						{
							CCLOG("Error create equip---------- objId=%d", data.dwobjectid());
							continue;
						}

						equip->SetData(equipData);
					}

					equip->SetInt(EEquip_ObjID, data.dwobjectid());
					equip->SetInt(EEquip_Pos, data.ipos());
					equip->SetInt(EEquip_SuitID, data.isuitid());
					equip->SetInt(EEquip_SellMoney, data.isellmoney());

					for (size_t j=0; j<data.szbaseproplist_size(); j++)
					{
						const PropItem& propItem =  data.szbaseproplist().Get(j);
						Var var2(propItem.ilifeattid());
						Var var3(propItem.ivalue());
						ScriptSys::GetInstance().Execute_3("SetLifeAttFromC", equip, "Entity", &var2, "Var", &var3, "Var");
					}

					for (size_t j=0; j<data.szspecialproplist_size(); j++)
					{
						const PropItem& propItem =  data.szspecialproplist().Get(j);
						Var var2(propItem.ilifeattid());
						Var var3(propItem.ivalue());
						ScriptSys::GetInstance().Execute_3("SetExtraLifeAttFromC", equip, "Entity", &var2, "Var", &var3, "Var");
					}

					if (data.has_equiprandomprop())
					{
						const PropItem& propItem = data.equiprandomprop();
						Var var2(propItem.ilifeattid());
						Var var3(propItem.ivalue());
						ScriptSys::GetInstance().Execute_3("SetLifeAttFromC", equip, "Entity", &var2, "Var", &var3, "Var");
						equip->setAddAttribute(propItem.ilifeattid(),propItem.ivalue());
					}

					bag->GetEquipList()[data.ipos()] = equip;
				}
			}
		}

		//UIMgr::GetInstance().RefreshBaseWidgetByName("Equip");
		UIMgr::GetInstance().RefreshBaseWidgetByName("BagLayer");
		if (isHero)
			ScriptSys::GetInstance().Execute("NewHeroEquip_AfterEquip");
		else
			ScriptSys::GetInstance().Execute("NewHeroEquip_AfterEquip_RefreshBag");
	}
}

void Player::EquipQuery( int iCmd, GSProto::SCMessage& pkg )
{
	GSProto::CMD_QUERY_EQUIPDESC_BYID_SC equipQuery;
	if(equipQuery.ParseFromString(pkg.strmsgbody() ) )
	{
		Equip* equip = NULL;
		const EquipInfo& data = equipQuery.equipdata();

		stEquipData* equipData = CDataManager::GetInstance().GetGameData<stEquipData>(DataFileEquip, data.iitemid());

		if (!equipData)
		{
			CCLOG("Error equipData---------- ibaseid=%d", data.iitemid());
			return;
		}

		equip = dynamic_cast<Equip*>( EntityMgr::GetInstance().CreateEntity( EEntityType_Equip, equipData->m_name, NULL ) );
		if ( !equip )
		{
			CCLOG("Error create equip---------- objId=%d", data.dwobjectid());
			return;
		}

		equip->SetData(equipData);

		equip->SetInt(EEquip_ObjID, data.dwobjectid());
		equip->SetInt(EEquip_Pos, data.ipos());
		equip->SetInt(EEquip_SuitID, data.isuitid());
		equip->SetInt(EEquip_SellMoney, data.isellmoney());

		for (size_t j=0; j<data.szbaseproplist_size(); j++)
		{
			const PropItem& propItem =  data.szbaseproplist().Get(j);
			Var var2(propItem.ilifeattid());
			Var var3(propItem.ivalue());
			ScriptSys::GetInstance().Execute_3("SetLifeAttFromC", equip, "Entity", &var2, "Var", &var3, "Var");
		}

		for (size_t j=0; j<data.szspecialproplist_size(); j++)
		{
			const PropItem& propItem =  data.szspecialproplist().Get(j);
			Var var2(propItem.ilifeattid());
			Var var3(propItem.ivalue());
			ScriptSys::GetInstance().Execute_3("SetExtraLifeAttFromC", equip, "Entity", &var2, "Var", &var3, "Var");
		}

		if (data.has_equiprandomprop())
		{
			const PropItem& propItem = data.equiprandomprop();
			Var var2(propItem.ilifeattid());
			Var var3(propItem.ivalue());
			ScriptSys::GetInstance().Execute_3("SetLifeAttFromC", equip, "Entity", &var2, "Var", &var3, "Var");
			equip->setAddAttribute(propItem.ilifeattid(),propItem.ivalue());
		}

		ScriptSys::GetInstance().Execute_1("Equip_Info", equip, "Equip");
		FM_SAFE_DELETE(equip);
	}
}

void Player::HeroUpdate( int iCmd, GSProto::SCMessage& pkg )
{
	GSProto::Cmd_Sc_HeroUpdate heroUpdate;
	if(heroUpdate.ParseFromString(pkg.strmsgbody() ) )
	{
		const HeroBaseData& data = heroUpdate.herodata();

		Player* player = GetLocalPlayer();

		Hero* hero = player->GetHeroByObjId(data.dwobjectid());
		bool isNewHero = false;
		if ( hero == NULL )
		{
			isNewHero = true;
			stHeroBornData* tmpHeroData = CDataManager::GetInstance().GetGameData<stHeroBornData>(DataFileHeroBorn, data.iheroid());
			if (!tmpHeroData)
			{
				CCLOG("Error heroData---------- iheroid=%d", data.iheroid());
				return;
			}
			
			hero = dynamic_cast<Hero*>( EntityMgr::GetInstance().CreateEntity( EEntityType_Hero, tmpHeroData->m_name, NULL ) );
			if ( !hero )
			{
				CCLOG("Error create hero---------- objId=%d", data.dwobjectid());
				return;
			}
			player->GetHeroList().push_back(hero->GetEntityId());
		}

		stHeroBornData* heroData = CDataManager::GetInstance().GetGameData<stHeroBornData>(DataFileHeroBorn, data.iheroid());
		if (!heroData)
		{
			CCLOG("Error heroData---------- iheroid=%d", data.iheroid());
			return;
		}
		hero->SetData(heroData);

		hero->SetInt(EHero_HeroId, data.iheroid());
		hero->SetInt(EHero_ID, data.dwobjectid());
		hero->SetInt(EHero_Lvl, data.ilevel());
		hero->SetInt(EHero_LvlStep, data.ilevelstep());
		hero->SetInt(EHero_FightValue, data.ifightvalue()); 
		hero->SetInt(EHero_HP, data.imaxhp());
		hero->SetInt(EHero_Att, data.iatt());

		for (size_t j=0; j<data.szproplist_size(); j++)
		{
			const PropItem& propItem =  data.szproplist().Get(j);
			Var var2(propItem.ilifeattid());
			Var var3(propItem.ivalue());
			ScriptSys::GetInstance().Execute_3("SetLifeAttFromC", hero, "Entity", &var2, "Var", &var3, "Var");
		}

		if (data.sztallentlist_size() != 0)
		{
			InnatesKill *innateskill = new InnatesKill(hero);
			hero->AddComponent(EComponentId_InnatesKill,innateskill);

			if(!innateskill)
			{
				CCLOG("Error create heroInnateskill---------- objId=%d", data.dwobjectid());
				return ;
			}
			for (size_t j = 0;j < data.sztallentlist_size();++j)
			{
				const HeroTallent& tallent = data.sztallentlist().Get(j);
				InnatesKillData *innateskillData = new InnatesKillData(tallent.itallentid(),tallent.ilevel(),tallent.bcanupgrade());
				innateskill->GetInnateskillData().push_back(innateskillData);
			} 
		}
		if (isNewHero)
		{
			//喜好品相关
			HeroFavorite *herofavorite = new HeroFavorite(hero);
			hero->AddComponent(EComponentId_HeroFavorite,herofavorite);
			GetLocalPlayer()->GetPlayerFavoriteItemID(herofavorite->GetHeroFavoriteId(),hero);
			for (size_t j = 0; j < data.szfavoritelist_size();++j)
			{
				const HeroFavoriteDesc& favorite = data.szfavoritelist().Get(j);
				const int favoriteId = favorite.iitemid();
				const int pos = favorite.ipos();
				GetLocalPlayer()->EquipHeroFavorite(favoriteId,hero,pos);
			}
			player->PlayerHeroFavoriteCheck();
		}
		UIMgr::GetInstance().RefreshBaseWidgetByName("Formation");
	}
}

void Player::HeroDel( int iCmd, GSProto::SCMessage& pkg )
{
	GSProto::Cmd_Sc_HeroDel heroDel;
	if(heroDel.ParseFromString(pkg.strmsgbody() ) )
	{

		Player* player = GetLocalPlayer();

		Hero* hero = player->GetHeroByObjId(heroDel.dwobjectid());
		if ( hero )
		{
			EntityMgr::GetInstance().RemoveEntity(hero->GetEntityId());
			player->RemoveHeroFromList(hero->GetEntityId());			
		}

		UIMgr::GetInstance().RefreshBaseWidgetByName("Formation");
	}
}

void Player::HeroBatchDel(int iCmd,GSProto::SCMessage& pkg )
{
	GSProto::CMD_DELETE_HEROS_SC heroBatchDel ;
	if (heroBatchDel.ParseFromString(pkg.strmsgbody()))
	{
		Player *player = GetLocalPlayer();
		size_t size = heroBatchDel.szheroobjecetid().size();
		for (size_t i = 0; i < size; ++i)
		{
			Hero *hero = player->GetHeroByObjId(heroBatchDel.szheroobjecetid().Get(i));
			if ( hero )
			{
				EntityMgr::GetInstance().RemoveEntity(hero->GetEntityId());
				player->RemoveHeroFromList(hero->GetEntityId());			
			}
		}
		Widget* HeroConvertwidget = UIMgr::GetInstance().GetBaseWidgetByName("HeroConver");
		if (HeroConvertwidget)
		{
			if (dynamic_cast<UILayout*>(HeroConvertwidget->getChildByTag(18)->getChildByTag(1)->getChildByTag(5))->isVisible())
			{
				//英雄出售
				UIMgr::EndWaiting();
				ScriptSys::GetInstance().Execute_1("HeroSell_AfterSell",HeroConvertwidget,"Widget");
			}
		}

		Widget* HeroBatchSellwidget = UIMgr::GetInstance().GetBaseWidgetByName("HeroBatchSell");
		if (HeroBatchSellwidget)
		{
			ScriptSys::GetInstance().Execute_1("HeroBatchSell_AfterBatchSell",HeroBatchSellwidget,"Widget");
		}
	}
}

Player::~Player()
{
	for (size_t i=0; i<m_HeroList.size(); i++)
	{
		EntityMgr::GetInstance().RemoveEntity(m_HeroList[i]);
	}

	for (size_t i = 0; i < m_GodAnimalList.size();++i)
	{
		FM_SAFE_DELETE(m_GodAnimalList[i]);
	}
	m_GodAnimalList.clear();
	m_tempPlayerBagItems.clear();
}

Entity* Player::Create( uint entityId, const string& entityName, EntityCreateOpt* createOpt )
{
	PlayerCreateOpt* opt = (PlayerCreateOpt*)( createOpt );
	Player* player = new Player( EEntityType_Player, entityId, entityName, opt?opt->m_IsLocalPlayer:false );
	return player;
}

void Player::InitInterface()
{
	// 注册对象创建接口
	SEntityAlloc entityAlloc;
	entityAlloc.m_EntityCreator = Player::Create;
	EntityMgr::GetInstance().RegisterEntityAlloc( EEntityType_Player, entityAlloc );
}

void Player::Update( uint delta )
{
	Entity::Update(delta);
	if (this == GetLocalPlayer())
	{
		//ScriptSys::GetInstance().Execute("Guide_Update");

		//ScriptSys::GetInstance().Execute("UpdateLocalPlayer");
	}
}

/*
Quest* Player::GetQuest()
{
	return GetComponent<Quest>( EComponentId_Quest );
}*/
// PlayerBag* Player::GetBags()
// {
// 	return GetComponent<PlayerBag>( EComponentId_Bags );
// }
EquipBag* Player::GetEquipBags()
{
	return GetComponent<EquipBag>( EComponentId_EquipBags );
}

FightSoulBag* Player::GetFightSoulBags()
{
	return GetComponent<FightSoulBag>( EComponentId_FightSoulBags );
}

Hero* Player::GetPlayerHero()
{
	return NULL;
}

uint Player::GetLevel()
{
	return GetUint(EPlayer_Lvl);
}

bool Player::IsInFormation( Hero* hero )
{
	if (GetHeroFormationPos(hero) >= 0 )
	{
		return true;
	}

	return false;
}

int Player::GetHeroFormationPos( Hero* hero )
{
	for (size_t i=0; i<m_FormationList.size(); i++)
	{
		if (m_FormationList[i] == hero->GetUint(EHero_ID))
		{
			return i;
		}
	}

	return -1;
}

Hero* Player::GetHeroByObjId( uint objId )
{
	for (size_t i=0; i<m_HeroList.size(); i++)
	{
		Hero* hero = dynamic_cast<Hero*>(EntityMgr::GetInstance().GetEntityById(m_HeroList[i]));
		if (hero && hero->GetUint(EHero_ID) == objId)
		{
			return hero;
		}
	}

	return NULL;
}

int Player::GetHeroIndexByObjId(uint objId)
{
	for (size_t i=0; i < GetHeroList(false).size(); i++)
	{
		Hero* hero = dynamic_cast<Hero*>(EntityMgr::GetInstance().GetEntityById(GetHeroList(false)[i]));
		if (hero && hero->GetUint(EHero_ID) == objId)
		{
			return i;
		}
	}

	return NULL;
}

int Player::GetHeroInFormationCount()
{
	int count = 0;

	for (size_t i=0; i<m_FormationList.size(); i++)
	{
		if (m_FormationList[i] > 0)
		{
			count++;
		}
	}

	return count;
}

void Player::InitFormationList()
{
	m_FormationList.clear();
	m_FormationList.resize(9, 0);
}

void Player::RemoveHeroFromList( uint entityId )
{
	for (size_t i=0; i<m_HeroList.size(); i++)
	{
		if (m_HeroList[i] == entityId)
		{
			m_HeroList.erase(m_HeroList.begin() + i);
			return;
		}
	}
}

void Player::GetNotExpeditionHeroList(vector<uint>& notExpeditionHeroList)
{
	GetHeroList(true);
	for (vector<uint>::iterator p = m_HeroList.begin(); p != m_HeroList.end(); p++)
	{
		Hero* hero = dynamic_cast<Hero*>(EntityMgr::GetInstance().GetEntityById((*p)));
		if(IsInFormation(hero))
		{
			continue;
		}
		else
		{
			notExpeditionHeroList.push_back((*p));
		}
	} 
}

void Player::GetNotExpeditionHeroListByHeroSell(vector<uint>& notExpeditionHeroList)
{
	GetHeroList(true);
	vector<uint> temcoll;
	for (vector<uint>::iterator p = m_HeroList.begin(); p != m_HeroList.end(); p++)
	{
		Hero* hero = dynamic_cast<Hero*>(EntityMgr::GetInstance().GetEntityById((*p)));
		if(IsInFormation(hero))
		{
			continue;
		}
		else
		{
			temcoll.push_back((*p));
		}
	} 
	for (vector<uint>::reverse_iterator p = temcoll.rbegin();p != temcoll.rend();++p)
	{
		notExpeditionHeroList.push_back((*p));
	}
}

void Player::GetFormationHeroList(vector<uint>& FormationHeroList)
{
	for (vector<uint>::iterator p = m_HeroList.begin(); p != m_HeroList.end(); p++)
	{
		Hero* hero = dynamic_cast<Hero*>(EntityMgr::GetInstance().GetEntityById((*p)));
		if(IsInFormation(hero))
		{
			FormationHeroList.push_back((*p));
		}
	} 
}

void Player::EraseHerolistInHeroBatchSell(vector<uint>& heroBatchSellList,uint ObjectId)
{
	for (vector<uint>::iterator p = heroBatchSellList.begin(); p != heroBatchSellList.end(); p++)
	{
		Hero * hero = dynamic_cast<Hero*>(EntityMgr::GetInstance().GetEntityById((*p)));
		if (hero && hero->GetUint(EHero_ID) == ObjectId)
		{
			heroBatchSellList.erase(p);
			break;
		}
	}
}

void Player::AddHerolistInHeroBatchSell(vector<uint>& heroBatchSellList,uint ObjectId)
{
	Hero *hero = GetHeroByObjId(ObjectId);
	//heroBatchSellList.push_back(hero->GetEntityId());
	heroBatchSellList.insert(heroBatchSellList.begin(),hero->GetEntityId());
}

bool Player::IsSameAsInforMationHero(const uint HeroId)
{
	for (vector<uint>::iterator p = m_HeroList.begin(); p != m_HeroList.end(); p++)
	{
		Hero* hero = dynamic_cast<Hero*>(EntityMgr::GetInstance().GetEntityById((*p)));
		if(IsInFormation(hero))
		{
			uint formastionHeroId = hero->GetInt(EHero_HeroId);
			if (formastionHeroId == HeroId)
			{
				return true;
			}
		}
	}
	return false;
}

void Player::SetGodAnimalActive(int dwObjctId){
	for (size_t i = 0; i < m_GodAnimalList.size(); ++i)
	{
		if (m_GodAnimalList[i]->GetBool(EGodAnimal_IsActive))
		{
			m_GodAnimalList[i]->SetBool(EGodAnimal_IsActive,false);
		}
	}
	GetGodAnimalByDwObject(dwObjctId)->SetBool(EGodAnimal_IsActive,true);
}
vector<GodAnimal*> Player::GetCanInheritGodAnimal(GodAnimal* animal){
	vector<GodAnimal*> vec;
	vec.clear();
	for (size_t i = 0; i < m_GodAnimalList.size(); ++i)
	{
		if ((animal->GetInt(EGodAnimal_Level)< m_GodAnimalList[i]->GetInt(EGodAnimal_Level)))
		{
			CCLOG("m_GodAnimalList.dwobjectId = %s |level = %d,| levelStep = %d, " ,
				(m_GodAnimalList[i]->getGodAnimalData()->m_name).c_str(),
				m_GodAnimalList[i]->GetInt(EGodAnimal_Level)
				,m_GodAnimalList[i]->GetInt(EGodAnimal_LevelStep));
			vec.push_back( m_GodAnimalList[i]);
		}
	}
	return vec;
}
GodAnimal* Player::GetGodAnimalByDwObject(int dwobject){
	for (size_t i = 0; i < m_GodAnimalList.size(); ++i)
	{
		if (dwobject == m_GodAnimalList[i]->GetInt(EGodAnimal_ObjId))
		{
			return m_GodAnimalList[i];
		}
	}
	return NULL;
}
void Player::GetHeroAscendingOrderList(vector<uint>& HeroAscendingOrderList,uint entityId,uint objId)
{
	//出战的和主体都不能被选择当做材料，这里要进行判断
	for (vector<uint>::iterator p = m_HeroList.begin();p != m_HeroList.end();p++)
	{
		Hero* hero = dynamic_cast<Hero*>(EntityMgr::GetInstance().GetEntityById((*p)));
		if(IsInFormation(hero) || hero->GetUint(EHero_ID) == objId)
		{
			continue;
		}
		else
		{
			if(hero->GetUint(EHero_HeroId) == entityId && hero->GetUint(EHero_HasFightSoul) != 1 && hero->GetUint(EHero_HasEquip) != 1)
			{
				HeroAscendingOrderList.push_back((*p));
			}
		}
	}
}

vector<uint>& Player::GetHeroList(bool isfresh /*= true*/)
{
	if (isfresh == true)
	{
	    sort(m_HeroList.begin(), m_HeroList.end(), SortHero());
	}
	return m_HeroList;
}


void Player::GetCanSummonHeroList(vector<uint>& cansummomHeroList)
{
	GetPlayerCanSummonHeroList(cansummomHeroList);
	if (cansummomHeroList.size() != 0 && cansummomHeroList.size() != 1)
	{
		sort(cansummomHeroList.begin(),cansummomHeroList.end(),SortAllCanSummonHero());
	}
}

void Player::GetNotSummonHeroList(vector<uint>& notcansummomHeroList)
{
	const vector<int > *allHerolist = CDataManager::GetInstance().GetGameDataKeyList(DataFileHeroBorn);
	for (size_t i = 0; i < allHerolist->size(); ++i)
	{
		int heroID = (*allHerolist)[i];
		if (PlayerHeroCannotSummon(heroID) && heroID <= 1033)
		{
			notcansummomHeroList.push_back(heroID);
		}
	}
}

void Player::GetCanSummonHeroAndHeroList(vector<uint>& summondHeroList)
{
	GetCanSummonHeroList(summondHeroList);
	GetHeroList(false);
	for (size_t i = 0; i < m_HeroList.size(); ++i)
	{
		summondHeroList.push_back(m_HeroList[i]);
	}
}

void Player::HerolistSort()
{
	sort(m_HeroList.begin(), m_HeroList.end(), SortHero());
}

void Player::GetPlayerNotHave(vector<int>& nothavegodanimal)
{
	const vector<int > *allgodanimalId;
	allgodanimalId = CDataManager::GetInstance().GetGameDataKeyList(DataFileGodAniaml);
	std::copy(allgodanimalId->begin(),allgodanimalId->end(),std::back_inserter(nothavegodanimal));
	for (vector<GodAnimal *>::iterator p = m_GodAnimalList.begin(); p != m_GodAnimalList.end(); ++p)
	{
		vector<int >::iterator q = std::find(nothavegodanimal.begin(),nothavegodanimal.end(),((*p)->getGodAnimalData()->m_ID));
		if (q != nothavegodanimal.end())
		{
			nothavegodanimal.erase(q);
		}
	}
}

void Player::sortGodAnimalWithId(vector<int>& AllgodanimalId)
{
	const vector<int > *allgodanimalId;
	allgodanimalId = CDataManager::GetInstance().GetGameDataKeyList(DataFileGodAniaml);
	AllgodanimalId.clear();
	std::copy(allgodanimalId->begin(),allgodanimalId->end(),std::back_inserter(AllgodanimalId));
	sort(AllgodanimalId.begin(), AllgodanimalId.end(), SortGodAnimalId());	
	CCLOG("%d",allgodanimalId->size());
}

bool Player::GodAnimalisHaveByAnimalId(const int godanimalId)
{
    for (vector<GodAnimal *>::iterator p = m_GodAnimalList.begin(); p != m_GodAnimalList.end();++p)
	{
		if ((*p)->getGodAnimalData()->m_ID == godanimalId)
		{
			return true;
		}
	}
	return false;
}

const uint Player::GetGodAnimalWithId(const int godanimalId)
{
	for (vector<GodAnimal *>::iterator p = m_GodAnimalList.begin(); p != m_GodAnimalList.end();++p)
	{
		if ((*p)->getGodAnimalData()->m_ID == godanimalId)
		{
			return (*p)->GetUint(EGodAnimal_ObjId);
		}
	}
	return NULL;
}

GodAnimal* Player::GetGodAnimalByAnimalId(const int godanimalId)
{
	for (size_t i=0; i<m_GodAnimalList.size(); i++)
	{
		if (m_GodAnimalList[i]->getGodAnimalData()->m_ID == godanimalId)
		{
			return m_GodAnimalList[i];
		}
	}

	return NULL;
}

void Player::GetHaveSoulGodAnimal(vector<int>& havesoulgodanimal)
{
	const vector<int > *allgodanimalId;
	allgodanimalId = CDataManager::GetInstance().GetGameDataKeyList(DataFileGodAniaml);
	for (int i = 0;i < allgodanimalId->size(); ++i)
	{
		stGodAnimalData* data = CDataManager::GetInstance().GetGameData<stGodAnimalData>(DataFileGodAniaml, (*allgodanimalId)[i]);
		if (data)
		{
			if (data->m_changeItemId != 0)
			{
				havesoulgodanimal.push_back(data->m_ID);
			}
		}
	}
}

Bag* Player::GetPlayerBag()
{
	return GetComponent<Bag>( EComponentId_Bags );
}

Bag* Player::GetPlayerMaterail()
{
	return GetComponent<Bag>( EComponentId_Materail );
}

HeroSoulBag* Player::GetPlayerHeroSoul()
{
	return GetComponent<HeroSoulBag>(EComponentId_HeroSoul);
}

void Player::HeroSoulChg(int iCmd,GSProto::SCMessage& pkg)
{
   GSProto::CMD_HERO_SOULCHG_SC heroSoulChg;
   Player* player = GetLocalPlayer();
   if(heroSoulChg.ParseFromString(pkg.strmsgbody() ) )
   {
	   uint size = heroSoulChg.szchglist().size();
	   HeroSoulBag *heroSoulBag = player->GetPlayerHeroSoul();
	   for (uint i = 0; i < size; ++i)
	   {
		   //首先判断玩家是否有该
		   int heroID = heroSoulChg.szchglist().Get(i).iheroid();
		   int count = heroSoulChg.szchglist().Get(i).icurcount();
		   player->OperatorHeroSoulByShoulID(heroID,count);
	   }
   }
}

void Player::HeroTallenSkillChg(int iCmd,GSProto::SCMessage& pkg)
{
	GSProto::CMD_HERO_TALLENT_UPDATE_SC herotallenskillChg;
	Player *player = GetLocalPlayer();
	if (herotallenskillChg.ParseFromString(pkg.strmsgbody() ))
	{
		uint32 HeroObjId = herotallenskillChg.dwobjectid();
		int32 tallenSkillId = herotallenskillChg.tallentdata().itallentid();
		int32 tallenSkillLevel = herotallenskillChg.tallentdata().ilevel();
		bool  tallenSkillCanup = herotallenskillChg.tallentdata().bcanupgrade();
		player->UpdateHeroTallentSkillData(HeroObjId,tallenSkillId,tallenSkillLevel,tallenSkillCanup);
	}
	Hero *hero = player->GetHeroByObjId(herotallenskillChg.dwobjectid());
	Widget* heroSystem = UIMgr::GetInstance().GetBaseWidgetByName("HeroSystemSecond");
	if (heroSystem)
	{
		//更新英雄界面
		vector<Var> vars;
		Var var1(herotallenskillChg.tallentdata().itallentid());
		Var var2(herotallenskillChg.tallentdata().ilevel());
		vars.push_back(var1);
		vars.push_back(var2);
		ScriptSys::GetInstance().Execute_3("HeroSystem_AfterTallentSkillChange",heroSystem,"Widget",hero,"Hero",&vars,"vector<Var>");
	    ScriptSys::GetInstance().Execute_2("HeroCurInfoSecond_HeroTallentUpdate",heroSystem,"Widget",hero,"Hero");
		//if (dynamic_cast<UILayout*>(heroSystem->getChildByTag(18)->getChildByTag(1)->getChildByTag(5))->isVisible())
		//{
		//	//英雄出售
		//	UIMgr::EndWaiting();
		//	ScriptSys::GetInstance().Execute_1("HeroSell_AfterSell",heroSystem,"Widget");
		//}
	}

	//弱引导
	if (GetLocalPlayer()->IsInFormation(hero))
	{
		ScriptSys::GetInstance().Execute("NoticeFunction_UpdateAfterFormasting");
	}
	
}

void Player::UpdateHeroTallentSkillData(const uint32 heroObjId,const int32 tallenSillID,const int tallSkilllevel,const bool tallSkillCanup)
{
	Hero *hero = GetHeroByObjId(heroObjId);
    hero->GetHeroInnateskill()->GetInnateskillData();
	for (vector<InnatesKillData *>::iterator p = hero->GetHeroInnateskill()->GetInnateskillData().begin();p != hero->GetHeroInnateskill()->GetInnateskillData().end(); ++p)
	{
		if ((*p)->m_TallentID == tallenSillID)
		{
			(*p)->m_iLevel = tallSkilllevel;
			(*p)->m_bCanUpgrade = tallSkillCanup;
		}
	}
}

int Player::GetPlayerHeroSoulCountById(const int herosoulID)
{
    HeroSoulBag *heroSoulBag = this->GetPlayerHeroSoul();
	for (int i = 0; i < heroSoulBag->GetHeroSoulData().size();++i)
	{
	    if (heroSoulBag->GetHeroSoulData()[i]->m_iHeroID == herosoulID)
	    {
			return heroSoulBag->GetHeroSoulData()[i]->m_iSoulCount;
	    }
	}
	return 0;
}

bool Player::PlayerHeroSoulIsAdd(const int herosoulID,const int count)
{
	int addcount = count - GetPlayerHeroSoulCountById(herosoulID);
	if (addcount > 0)
	{
		return true;
	}
	else 
	{
		return false;
	}
}

bool Player::PlayerHeroSoulIsHave(const int herosoulId)
{
	HeroSoulBag *heroSoulBag = this->GetPlayerHeroSoul();
	for (int i = 0; i < heroSoulBag->GetHeroSoulData().size();++i)
	{
		if (heroSoulBag->GetHeroSoulData()[i]->m_iHeroID == herosoulId)
		{
			return true;
		}
	}
	return false;
}

void Player::OperatorHeroSoulByShoulID(const int herosoulID,const int count)
{
	HeroSoulBag *heroSoulBag = this->GetPlayerHeroSoul();
	for(vector<HeroSoulData*>::iterator p = heroSoulBag->GetHeroSoulData().begin();p != heroSoulBag->GetHeroSoulData().end();++p)
	{
		if ((*p)->m_iHeroID == herosoulID)
		{
			if (count == 0)
			{
				//移除
				heroSoulBag->GetHeroSoulData().erase(p);
				return ;
			}
			else 
			{
				(*p)->m_iSoulCount = count;
				return;
			}
		}
	}

	//没有返回会执行到这里 新添加
	HeroSoulData *herosouldata = new HeroSoulData(herosoulID,count);
	heroSoulBag->GetHeroSoulData().push_back(herosouldata);
}

bool Player::CheckHeroTallentCanUpOrAct()
{
	for (size_t i = 0; i < m_HeroList.size(); ++i)
	{
		if (m_HeroList[i])
		{
			Hero *hero = dynamic_cast<Hero*>(EntityMgr::GetInstance().GetEntityById(m_HeroList[i]));
			vector<HeroFavoriteState> favoriteState = hero->GetHeroFavorite()->GetHeroFavoriteState();
			for (size_t i = 0; i < favoriteState.size(); i++)
			{
				if ( favoriteState[i] == CanEquipFavorite)
				{
					return true;
				}
			}
		}
	}
	return false;
}

bool Player::PlayerHeroCanSummon(int heroId)
{
	for (size_t i = 0; i < m_HeroList.size() ; ++i)
	{
       Hero* hero = dynamic_cast<Hero*>(EntityMgr::GetInstance().GetEntityById(m_HeroList[i]));
	   if (hero->GetInt(EHero_HeroId) == heroId)
	   {
		   return false;
	   }
	}
	stHeroBornData* heroData = CDataManager::GetInstance().GetGameData<stHeroBornData>(DataFileHeroBorn, heroId);
	int playerhaveSoulNumber = this->GetPlayerBag()->GetItemCountByItemId(heroData->m_heroSoulItemId);
	if (heroData->m_heroNeedSoulNumber <= playerhaveSoulNumber)
	{
		return true;
	}
	return false;
}

void Player::GetPlayerCanSummonHeroList(vector<uint> &canSummonHeroList)
{
	const vector<int > *allHeroList;
	allHeroList = CDataManager::GetInstance().GetGameDataKeyList(DataFileHeroBorn);
	for (size_t i = 0; i < allHeroList->size(); ++i)
	{
		int heroId = (*allHeroList)[i];
		if (PlayerHeroCanSummon(heroId))
		{
			canSummonHeroList.push_back(heroId);
		}
	}
}

bool Player::PlayerHeroCannotSummon(int heroId)
{
	for (size_t i = 0; i < m_HeroList.size(); ++i)
	{
		Hero* hero = dynamic_cast<Hero*>(EntityMgr::GetInstance().GetEntityById(m_HeroList[i]));
		if (hero->GetInt(EHero_HeroId) == heroId)
		{
			return false;
		}
	}
	stHeroBornData* heroData = CDataManager::GetInstance().GetGameData<stHeroBornData>(DataFileHeroBorn, heroId);
	int playerhaveSoulNumber = this->GetPlayerBag()->GetItemCountByItemId(heroData->m_heroSoulItemId);
	if (heroData->m_heroNeedSoulNumber >= playerhaveSoulNumber)
	{
		return true;
	}
	return false;
}

void Player::GetHeroQualityFavoriteItemId(std::string src,vector<int >& itemId)
{
	vector<string> vInfos = StringUtil::Split(src, "#");

	for (size_t i=0; i<vInfos.size();i++)
	{
		itemId.push_back(TypeConvert::ToInt(vInfos[i].c_str()));
	}
}

void Player::GetHeroFavoriteNatureByItemId(vector<string>& favoriteNature,const int itemId)
{
	stItemData *itemdate = CDataManager::GetInstance().GetGameData<stItemData>(DataFileItem, itemId);
	string src = itemdate->m_attinfo;
	favoriteNature = StringUtil::Split(src,"\n");
} 

void Player::GetPlayerFavoriteItemID(vector<int >& itemID,Hero *hero)
{
	stHeroQualityData *heroqualityData = CDataManager::GetInstance().GetGameData<stHeroQualityData>(DataFileHeroQuality, hero->GetInt(EHero_HeroId));
	string src = GetPlayerHeroFavorite(hero->GetInt(EHero_Quality),hero->GetInt(EHero_HeroId));
	if (src != "")
	{
		GetHeroQualityFavoriteItemId(src,itemID);
	}
	else 
	{
		CCLOG("src not null");
	}

}

bool Player::PlayerHaveFavorite(const int heroObjectId,const int itemID)
{
	 Hero *hero = GetHeroByObjId(heroObjectId);
	 //先到玩家背包寻找
	 int itemNumber = GetPlayerBag()->GetItemCountByItemId(itemID);
	 if (itemNumber >= 0)
	 {
		 return true;
	 }
	 //没有的话就迭代查找是否可以合成 

}

bool Player::PlayerHaveEquipFavorite(const int heroObjectId,const int itemId)
{
	Hero* hero = GetHeroByObjId(heroObjectId);
	vector<HeroFavoriteData *> heroFavoriteData = hero->GetHeroFavorite()->GetHeroFavoriteData();
    for (size_t i = 0; i < heroFavoriteData.size(); ++i)
    {
		if (heroFavoriteData[i]->m_HeroFavoriteID == itemId)
		{
			return true;
		}
    }
	return false;
}

string Player::GetPlayerHeroFavorite(const int quality,const int heroId)
{
	stHeroQualityData *heroqualityData = CDataManager::GetInstance().GetGameData<stHeroQualityData>(DataFileHeroQuality, heroId);
	if (quality == 1)
	{
		return heroqualityData->m_quality1;
	}
	if (quality == 2)
	{
		return heroqualityData->m_quality2;
	}
	if (quality == 3)
	{
		return heroqualityData->m_quality3;
	}
	if (quality == 4)
	{
		return heroqualityData->m_quality4;
	}
	if (quality == 5)
	{
		return heroqualityData->m_quality5;
	}
	if (quality == 6)
	{
		return heroqualityData->m_quality6;
	}
	if (quality == 7)
	{
		return heroqualityData->m_quality7;
	}
	if (quality == 8)
	{
		return heroqualityData->m_quality8;
	}
	if (quality == 9)
	{
		return heroqualityData->m_quality9;
	}
	if (quality == 10)
	{
		return heroqualityData->m_quality10;
	}
	if (quality == 11)
	{
		return heroqualityData->m_quality11;
	}
	if (quality == 12)
	{
		return heroqualityData->m_quality12;
	}
	if (quality == 13)
	{
		return heroqualityData->m_quality13;
	}
	return "";
}

int Player::GetPlayerHeroFavoritePosition(const int herofavoriteId , const int heroObjectId)
{
	vector<int > itemId;
	Hero *hero = GetHeroByObjId(heroObjectId);
	GetPlayerFavoriteItemID(itemId,hero);
	for (size_t i = 0; i < itemId.size(); ++i)
	{
        if (herofavoriteId == itemId[i])
        {
			return i;
        }
	}
	return 10;
}

int Player::GetPlayerHeroFavoritePositionByHero(const int herofavoriteId,Hero*hero)
{
	vector<int > itemId;
	GetPlayerFavoriteItemID(itemId,hero);
	for (size_t i = 0; i < itemId.size(); ++i)
	{
		if (herofavoriteId == itemId[i])
		{
			return i;
		}
	}
	return 10;
}

bool Player::PlayerHeroFavoriteCanAdmix(const int herofavoriteId)
{
	//bool iscanAdmix = false;
	stFavoriteData *favoritedata = CDataManager::GetInstance().GetGameData<stFavoriteData>(DataFileFavorite, herofavoriteId);
	if (favoritedata == NULL)
	{
		//该物品不能合成 
		//如果有该物品就返回true ，没有就返回false
		if (GetPlayerBag()->GetItemCountByItemId(herofavoriteId) > 0)
		{
			return true;
		}
		else
		{
			return false;
		}
	}
	//如果有该物品就返回true
	if (CheckTempPlayerBagItemCount(herofavoriteId) > 0)
	{
		return true;
	}
	int count = favoritedata->m_favoriteCount;
	for (size_t i = 1; i <= count ; ++i)
	{
		int favoriteId = 0;
		int favoriteCount = 0;
		if (i == 1)
		{
			favoriteId = favoritedata->m_favoriteOne;
			favoriteCount = favoritedata->m_favoriteOneNeed;
			stFavoriteData *tempfavoritedata = CDataManager::GetInstance().GetGameData<stFavoriteData>(DataFileFavorite, favoriteId);
			int bagItemNumber = CheckTempPlayerBagItemCount(favoriteId);//GetPlayerBag()->GetItemCountByItemId(favoriteId);
			if (favoriteCount <= bagItemNumber)
			{
				//iscanAdmix = true;
				ChangeTempPlayerBagItem(favoriteId,favoriteCount);
			}
			else
			{
				if (tempfavoritedata == NULL)
				{
					return false;
				}
				else
				{
					if (PlayerHeroFavoriteCanAdmix(favoriteId))
					{
						//iscanAdmix = true;
					}
					else
					{
						return false;
					}
				}
			}		
		}

		//2
		if (i == 2)
		{
			favoriteId = favoritedata->m_favoriteTwo;
			favoriteCount = favoritedata->m_favoriteTwoNeed;
			stFavoriteData *tempfavoritedata = CDataManager::GetInstance().GetGameData<stFavoriteData>(DataFileFavorite, favoriteId);
			int bagItemNumber = CheckTempPlayerBagItemCount(favoriteId);//GetPlayerBag()->GetItemCountByItemId(favoriteId);
			if (favoriteCount <= bagItemNumber)
			{
				//iscanAdmix = true;
				ChangeTempPlayerBagItem(favoriteId,favoriteCount);
			}
			else
			{
				if (tempfavoritedata == NULL)
				{
					return false;
				}
				else
				{
					if (PlayerHeroFavoriteCanAdmix(favoriteId))
					{
						//iscanAdmix = true;
					}
					else
					{
						return false;
					}
				}
			}		
		}

		//3
		if (i == 3)
		{
			favoriteId = favoritedata->m_favoriteThree;
			favoriteCount = favoritedata->m_favoriteThreeNeed;
			stFavoriteData *tempfavoritedata = CDataManager::GetInstance().GetGameData<stFavoriteData>(DataFileFavorite, favoriteId);
			int bagItemNumber = CheckTempPlayerBagItemCount(favoriteId);//GetPlayerBag()->GetItemCountByItemId(favoriteId);
			if (favoriteCount <= bagItemNumber)
			{
				//iscanAdmix = true;
				ChangeTempPlayerBagItem(favoriteId,favoriteCount);
			}
			else
			{
				if (tempfavoritedata == NULL)
				{
					return false;
				}
				else
				{
					if (PlayerHeroFavoriteCanAdmix(favoriteId))
					{
						//iscanAdmix = true;
					}
					else
					{
						return false;
					}
				}
			}		
		}

		//4
		if (i == 4)
		{
			favoriteId = favoritedata->m_favoriteFour;
			favoriteCount = favoritedata->m_favoriteFourNeed;
			stFavoriteData *tempfavoritedata = CDataManager::GetInstance().GetGameData<stFavoriteData>(DataFileFavorite, favoriteId);
			int bagItemNumber = CheckTempPlayerBagItemCount(favoriteId);//GetPlayerBag()->GetItemCountByItemId(favoriteId);
			if (favoriteCount <= bagItemNumber)
			{
				//iscanAdmix = true;
				ChangeTempPlayerBagItem(favoriteId,favoriteCount);
			}
			else
			{
				if (tempfavoritedata == NULL)
				{
					return false;
				}
				else
				{
					if (PlayerHeroFavoriteCanAdmix(favoriteId))
					{
						//iscanAdmix = true;
					}
					else
					{
						return false;
					}
				}
			}		
		}
	}
	return true;
}

void Player::InitTempPlayerBagItemWithFavorite()
{
	m_tempPlayerBagItems.clear();
	for (size_t i = 0; i < GetPlayerBag()->GetBagList().size(); ++i)
	{
		m_tempPlayerBagItems.insert(pair<uint,uint>(GetPlayerBag()->GetBagList()[i]->getItemData()->m_ID,GetPlayerBag()->GetBagList()[i]->GetInt(EBagItem_Count)));
	}
}

int Player::CheckTempPlayerBagItemCount(const int itemId)
{
	int itemcount = 0;
	for (map<uint,uint>::iterator p = m_tempPlayerBagItems.begin(); p != m_tempPlayerBagItems.end(); ++p)
	{
		if (itemId == p->first)
		{
			itemcount += p->second;
		}
	}
	return itemcount;
}

void Player::ChangeTempPlayerBagItem(const int itemId,const int number)
{
	int emptyItemId = 0;
	for (map<uint,uint>::iterator p = m_tempPlayerBagItems.begin(); p != m_tempPlayerBagItems.end(); ++p)
	{
		if(itemId == p->first)
		{
			int itemcount = p->second - number;
			if (itemcount == 0)
			{
				emptyItemId = p->first;
			}
			else
			{
				p->second = itemcount;
			}
		}
	}
	map<uint,uint>::iterator p ;
	if (emptyItemId != 0)
	{
	   p = m_tempPlayerBagItems.find(emptyItemId);
	   if (p != m_tempPlayerBagItems.end())
	   {
		   m_tempPlayerBagItems.erase(p);
	   }
	}
}

void Player::PlayerHeroFavoriteCheck()
{
	for (size_t j = 0; j < m_HeroList.size(); ++j)
	{
		Hero *hero = dynamic_cast<Hero*>(EntityMgr::GetInstance().GetEntityById(GetLocalPlayer()->GetHeroList(false)[j]));
		HeroFavorite *herofavorite = hero->GetHeroFavorite();
		//继续检测
		for (size_t i = 0; i < herofavorite->GetHeroFavoriteState().size(); ++i)
		{
			if (herofavorite->GetHeroFavoriteState()[i] != HaveEquipFavorite)
			{	
				InitTempPlayerBagItemWithFavorite();
				bool ishaveFavorite = PlayerHeroFavoriteCanAdmix(herofavorite->GetHeroFavoriteId()[i]);
				if (ishaveFavorite == false)
				{
					herofavorite->GetHeroFavoriteState()[i] = NoHaveHeroFavorite;
				}
				else
				{
					stItemData *itemData = CDataManager::GetInstance().GetGameData<stItemData>(DataFileItem, herofavorite->GetHeroFavoriteId()[i]);
					if (itemData->m_lvLimits > GetLocalPlayer()->GetUint(EPlayer_Lvl))
					{
						herofavorite->GetHeroFavoriteState()[i] = CannotEquipFavorite;
					}
					else
					{
						herofavorite->GetHeroFavoriteState()[i] = CanEquipFavorite;
					}
				}
			}
		}
	}
	ScriptSys::GetInstance().Execute("NoticeFunction_UpdateHeroButton");
}

void Player::PlayerHeroFavoriteCheckByObjectId(const int heroObjectId)
{
    Hero *hero = GetHeroByObjId(heroObjectId);
	HeroFavorite *herofavorite = hero->GetHeroFavorite();
	//继续检测
	for (size_t i = 0; i < herofavorite->GetHeroFavoriteState().size(); ++i)
	{
		if (herofavorite->GetHeroFavoriteState()[i] != HaveEquipFavorite)
		{			
			bool ishaveFavorite = PlayerHeroFavoriteCanAdmix(herofavorite->GetHeroFavoriteId()[i]);
			if (ishaveFavorite == false)
			{
				herofavorite->GetHeroFavoriteState()[i] = NoHaveHeroFavorite;
			}
			else
			{
				stItemData *itemData = CDataManager::GetInstance().GetGameData<stItemData>(DataFileItem, herofavorite->GetHeroFavoriteId()[i]);
				if (itemData->m_lvLimits > GetLocalPlayer()->GetUint(EPlayer_Lvl))
				{
					herofavorite->GetHeroFavoriteState()[i] = CannotEquipFavorite;
				}
				else
				{
					herofavorite->GetHeroFavoriteState()[i] = CanEquipFavorite;
				}
			}
		}
	}
}

void Player::EquipHeroFavorite(const int herofavoriteId,Hero *hero,const int pos)
{
    HeroFavorite *herofavorite = hero->GetHeroFavorite();
    herofavorite->GetHeroFavoriteState()[pos] = HaveEquipFavorite;
}

void Player::EquipHeroFavoriteByHeroObject(const int herofavoriteId,const int heroObjectId,const int pos)
{
	Hero *hero = GetHeroByObjId(heroObjectId);
	HeroFavorite *herofavorite = hero->GetHeroFavorite();
    herofavorite->GetHeroFavoriteState()[pos] = HaveEquipFavorite;
}

bool Player::HeroHaveAllEquipFavorite(const int heroObjectId)
{
	Hero *hero = GetHeroByObjId(heroObjectId);
	vector<HeroFavoriteState> herofavoritestate = hero->GetHeroFavorite()->GetHeroFavoriteState();
	for (size_t i = 0; i < herofavoritestate.size(); ++i)
	{
		if (herofavoritestate[i] != HaveEquipFavorite)
		{
			return false;
		}
	}
	return true;
}

void Player::UpdateHeroFavoriteByHeroObjectId(const int heroObjectId)
{
	Hero *hero = GetHeroByObjId(heroObjectId);
	HeroFavorite *herofavorite = hero->GetHeroFavorite();
	herofavorite->GetHeroFavoriteId().clear();
	GetLocalPlayer()->GetPlayerFavoriteItemID(herofavorite->GetHeroFavoriteId(),hero);
	for (size_t i = 0 ; i < herofavorite->GetHeroFavoriteState().size(); ++i)
	{
		herofavorite->GetHeroFavoriteState()[i] = NoHaveHeroFavorite;
	}
	PlayerHeroFavoriteCheckByObjectId(heroObjectId);
}

void Player::GetHeroFavoriteSummondItemId(const int herofavoriteId,vector<int> &favoriteId,vector<int > &favoriteMaterialNumber)
{
	stFavoriteData *favoritedata = CDataManager::GetInstance().GetGameData<stFavoriteData>(DataFileFavorite, herofavoriteId);
	if (favoritedata == NULL)
	{
		CCLOG("favoritedata has error");
		return ;
	}
	int needMaterialKind = favoritedata->m_favoriteCount;
	if (needMaterialKind == 1)
	{
		favoriteId.push_back(favoritedata->m_favoriteOne);
		favoriteMaterialNumber.push_back(favoritedata->m_favoriteOneNeed);
	}
	else if (needMaterialKind == 2)
	{
		favoriteId.push_back(favoritedata->m_favoriteOne);
		favoriteMaterialNumber.push_back(favoritedata->m_favoriteOneNeed);
		favoriteId.push_back(favoritedata->m_favoriteTwo);
		favoriteMaterialNumber.push_back(favoritedata->m_favoriteTwoNeed);
	}
	else if (needMaterialKind == 3)
	{
		favoriteId.push_back(favoritedata->m_favoriteOne);
		favoriteMaterialNumber.push_back(favoritedata->m_favoriteOneNeed);
		favoriteId.push_back(favoritedata->m_favoriteTwo);
		favoriteMaterialNumber.push_back(favoritedata->m_favoriteTwoNeed);
		favoriteId.push_back(favoritedata->m_favoriteThree);
		favoriteMaterialNumber.push_back(favoritedata->m_favoriteThreeNeed);
	}
	else if (needMaterialKind == 4)
	{
		favoriteId.push_back(favoritedata->m_favoriteOne);
		favoriteMaterialNumber.push_back(favoritedata->m_favoriteOneNeed);
		favoriteId.push_back(favoritedata->m_favoriteTwo);
		favoriteMaterialNumber.push_back(favoritedata->m_favoriteTwoNeed);
		favoriteId.push_back(favoritedata->m_favoriteThree);
		favoriteMaterialNumber.push_back(favoritedata->m_favoriteThreeNeed);
		favoriteId.push_back(favoritedata->m_favoriteFour);
		favoriteMaterialNumber.push_back(favoritedata->m_favoriteFourNeed);
	}
}

bool Player::HeroFavoriteCanSummondByFavoriteId(const int heroFavoriteId)
{
	// 这里的合成不迭代
	stFavoriteData *favoritedata = CDataManager::GetInstance().GetGameData<stFavoriteData>(DataFileFavorite, heroFavoriteId);
	if (favoritedata != NULL)
	{
		vector<int > herofavoriteMateriallist;
		vector<int > herofavoriteMaterialNeedlist;
		GetHeroFavoriteSummondItemId(heroFavoriteId,herofavoriteMateriallist,herofavoriteMaterialNeedlist);
		for (size_t i = 0; i < herofavoriteMateriallist.size(); ++i)
		{
			int playerhavenumber = GetPlayerBag()->GetItemCountByItemId(herofavoriteMateriallist[i]);
			if (playerhavenumber < herofavoriteMaterialNeedlist[i])
			{
				return false;
			}
		}
		return true;
	}
	else
	{
		return false;
	}
}

int Player::HeroFavoriteNotHave_ByCanSummondByFavoriteId(const int heroFavoriteId)
{
	stFavoriteData *favoritedata = CDataManager::GetInstance().GetGameData<stFavoriteData>(DataFileFavorite, heroFavoriteId);
	if (favoritedata != NULL)
	{
		vector<int > herofavoriteMateriallist;
		vector<int > herofavoriteMaterialNeedlist;
		GetHeroFavoriteSummondItemId(heroFavoriteId,herofavoriteMateriallist,herofavoriteMaterialNeedlist);
		for (size_t i = 0; i < herofavoriteMateriallist.size(); ++i)
		{
			if (!HeroFavoriteIsBaseFavorite(herofavoriteMateriallist[i]))
			{
				int playerhavenumber = GetPlayerBag()->GetItemCountByItemId(herofavoriteMateriallist[i]);
				if (playerhavenumber < herofavoriteMaterialNeedlist[i])
				{
					return (i + 1);
				}
			}
		}
		return 0;
	}
	else 
	{
		CCLOG("error:----------------------favoritelist is error");
	}
}

bool Player::HeroFavoriteIsBaseFavorite(const int heroFavoriteId)
{
	stFavoriteData *favoritedata = CDataManager::GetInstance().GetGameData<stFavoriteData>(DataFileFavorite, heroFavoriteId);
	if (favoritedata)
	{
		return false;
	}
	else 
	{
		return true;
	}
}

long Player::getCurrentTime()
{
	struct   tm  * tm ;  
	time_t  timep;  
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)   
	time(&timep);  
#else   
	struct  cc_timeval now;   
	CCTime::gettimeofdayCocos2d(&now, NULL);   
	timep = now.tv_sec;  
#endif 
	 tm  = localtime(&timep);  
	 long timeSecond = tm->tm_sec + tm->tm_min * 60 + tm->tm_hour * 3600;
	 return timeSecond;
}

int Player::getCurrentMinTime()
{
	struct   tm  * tm ;  
	time_t  timep;  
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)   
	time(&timep);  
#else   
	struct  cc_timeval now;   
	CCTime::gettimeofdayCocos2d(&now, NULL);   
	timep = now.tv_sec;  
#endif 
	tm  = localtime(&timep);  
	long timeMin = tm->tm_min * 60 + tm->tm_sec ;
	return timeMin;
}

bool Player::IsInCurTime(long timeBegin,long timeEnd)
{
	long secondTime = getCurrentTime();
	if (secondTime <= timeEnd && secondTime >= timeBegin)
	{
		return true;
	}
	return false;
}

int Player::getMoth()
{
	struct   tm  * tm ;  
	time_t  timep;  
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)   
	time(&timep);  
#else   
	struct  cc_timeval now;   
	CCTime::gettimeofdayCocos2d(&now, NULL);   
	timep = now.tv_sec;  
#endif 
	tm  = localtime(&timep);  
	return tm->tm_mon;
}

int Freeman::Player::getDelayTime(const int hour, const int min)
{
	int curSecond = getCurrentTime();
	int pushtime = hour * 3600 + min * 60;
	int delaytime = pushtime - curSecond;
	return delaytime;
}

/*
void Player::SetName( const char* name )
{
	m_EntityName = name;
}
*/
NS_FM_END
