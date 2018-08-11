#include "SceneMgr.h"
#include "FmEntityMgr.h"
#include "FmEntity.h"
#include "UI/FmUIMgr.h"
#include "Scene/FmMapLayer.h"
#include "Scene/FmMainScene.h"
#include "GameEntity/FmEntityRender.h"
//#include "GameEntity/FmSceneEntity.h"
#include "Enum/FmComponentEnum.h"
#include "FmGeometry.h"
#include "Script/MyLuaEngine.h"
#include "Script/FmScriptSys.h"
#include "FmXmlWrapper.h"
#include "Sound/AudioEngine.h"
#include "Scene/FmMainScene.h"
#include "NetWork/ClientSinkCallbackMgr.h"
#include "NetWork/GameServer.pb.h"
#include "Player/FmPlayer.h"
#include "Player/FmLocalPlayer.h"
#include "Enum/FmPropertyIndex.h"
#include "GameEntity/FmHero.h"
#include "GameData/DataManager.h"
#include "Numeric/HeroBornData.h"
#include "Util/FmStringMgr.h"
#include "FmFightMgr.h"
#include "Numeric/ItemData.h"
#include "Player/FmBagItem.h"
#include "Numeric/FightSoulData.h"
#include "FightSoul/FmFightSoulBag.h"
#include "Player/FmGodAnimal.h"
#include "Numeric/MonsterData.h"
#include "Numeric/EquipData.h"
#include "Equip/FmEquip.h"
#include "InnatesKill/FmInnatesKill.h"
#include "HeroSoul/FmHeroSoul.h"
#include "InnatesKill/FmHeroFavorite.h"
#include "Numeric/FavoriteData.h"
#include "Common.h"
//#include "SDK/TBTSDK.h"

USING_NS_CC;
NS_FM_BEGIN

PerformenceCounter::PerformenceCounter(const char* strName):m_strName(strName)
{
#ifdef WIN32
	m_dwBeginCounter = GetTickCount();
#endif
}

PerformenceCounter::~PerformenceCounter()
{
#ifdef WIN32
	uint dwCostTime = GetTickCount()-m_dwBeginCounter;
	CCLog("PerformenceCounter[%s] UseTime[%d]", m_strName.c_str(), dwCostTime);
#endif
}


SceneMgr::SceneMgr()
{
	m_Scene = NULL;
	m_PreScene = NULL;
	m_lastState = m_State = EGameState_None;
	m_nextState = EGameState_None;
	m_LoadStep = ESLoadStep_Init;

	m_isServerDataFinish = false;
	m_bGotoMainmenu = false;
	m_isFromMainmenu = false;
	
	m_bHeroCacheDirty = false;

	ClientSinkCallbackMgr::GetInstance().Register(GSProto::CMD_QUERY_HEROFIGHTSOUL, GetHeroFightSoulBag);
	ClientSinkCallbackMgr::GetInstance().Register(GSProto::CMD_FIGHTSOUL_BAG, GetFightSoulBag);
	ClientSinkCallbackMgr::GetInstance().Register(GSProto::CMD_ROLE_FIN, RoleFinish);
	ClientSinkCallbackMgr::GetInstance().Register(GSProto::CMD_GETROLE, GetRole);
	ClientSinkCallbackMgr::GetInstance().Register(GSProto::CMD_HERO_LIST, GetHeroList);
	ClientSinkCallbackMgr::GetInstance().Register(GSProto::CMD_FORMATION_DATA, GetFormationList);
	ClientSinkCallbackMgr::GetInstance().Register(GSProto::CMD_BAG_DATA, GetBagData);
	//背包数据变更
	ClientSinkCallbackMgr::GetInstance().Register(GSProto::CMD_BAG_CHG,UpdateBagData);
	//获取神兽列表
	ClientSinkCallbackMgr::GetInstance().Register(GSProto::CMD_GODANIMAL_LIST,GetGodAnimalList);
	ClientSinkCallbackMgr::GetInstance().Register(GSProto::CMD_EQUIP_BAG, GetEquipBag);
	ClientSinkCallbackMgr::GetInstance().Register(GSProto::CMD_QUERY_HEROEQUIP, GetHeroEquipBag);
}

SceneMgr::~SceneMgr()
{
	FM_SAFE_DELETE(m_Scene);
	FM_SAFE_DELETE(m_PreScene);
}

void SceneMgr::RoleFinish( int iCmd, GSProto::SCMessage& pkg )
{
	SceneMgr::GetInstance().SetIsServerDataFinish(true);
	GetLocalPlayer()->PlayerHeroFavoriteCheck();

	ScriptSys::GetInstance().Execute("RoleLoginFinish"); 
	//ScriptSys::GetInstance().Execute("NoticeFunction_UpdateHeroButton");
#if CC_TARGET_SDK == CC_PLATFORM_SDK_AZUC 
	ScriptSys::GetInstance().Execute("Login_UCSub"); 
#endif
} 

void SceneMgr::GetGodAnimalList(int iCmd, GSProto::SCMessage& pkg){
	CCLOG("GodAniamlList Enter");
	GSProto::Cmd_Sc_GoldAnimalList lGodAnimalListData;
	if (lGodAnimalListData.ParseFromString(pkg.strmsgbody()))
	{
		size_t size = lGodAnimalListData.szgodanimallist().size();
		Player* player = GetLocalPlayer();
		CCLog("size = %d",size);
		for (size_t i = 0; i < size; i ++ )
		{
			const GoldAnimalRoleBase& lGodAnimalRoleBase = lGodAnimalListData.szgodanimallist().Get(i);
			stGodAnimalData* lGodAnimalData = CDataManager::GetInstance().GetGameData<stGodAnimalData>(DataFileGodAniaml, lGodAnimalRoleBase.ibaseid());
			if (!lGodAnimalData)
			{
				CCLOG("Error lGodAnimalData---------- ItemDataId=%d", lGodAnimalRoleBase.ibaseid());
				continue;
			}
			GodAnimal* lGodAnimal = dynamic_cast<GodAnimal*>( EntityMgr::GetInstance().CreateEntity( EEntityType_GodAnimal, lGodAnimalData->m_animName, NULL ) );
			if ( !lGodAnimal )
			{
				CCLOG("Error create GodAnimal---------- objId= %d", lGodAnimalRoleBase.dwobjectid());
				continue;
			}

			lGodAnimal->setGodAnimalData(lGodAnimalData);
			lGodAnimal->SetInt(EGodAnimal_ObjId, lGodAnimalRoleBase.dwobjectid());
			lGodAnimal->SetInt(EGodAnimal_Level, lGodAnimalRoleBase.ilevel());
			lGodAnimal->SetInt(EGodAnimal_LevelStep, lGodAnimalRoleBase.ilevelstep());
			if (lGodAnimalListData.dwactiveobjectid() ==  lGodAnimalRoleBase.dwobjectid())
			{
				lGodAnimal->SetBool(EGodAnimal_IsActive,true);
				player->GetGodAnimalList().insert(player->GetGodAnimalList().begin(),lGodAnimal);
			}else{
				lGodAnimal->SetBool(EGodAnimal_IsActive,false);
				player->GetGodAnimalList().push_back(lGodAnimal);
			}
			
		}
	}
}
void SceneMgr::UpdateBagData( int iCmd, GSProto::SCMessage& pkg ){
	CCLOG("Update bag Info");
	GSProto::Cmd_Sc_BagChg bagChgData;
	if(bagChgData.ParseFromString(pkg.strmsgbody())){
	CCLOG("bagChgData ======= %s",bagChgData.DebugString().c_str());
		size_t size = bagChgData.szbagchglist().size();
		Player* player = GetLocalPlayer();
		Bag* bag = player->GetPlayerBag();
		if (bagChgData.ibagtype() == GSProto::en_BagType_Materail )
		{
			bag = player->GetPlayerMaterail();
		}
		bool isFavorite = false;
	
		//Add类型的累加数
		int addPlayerItemAddNumber = 0;
		for (size_t i = 0;i < size; i++)
		{
			const BagChgInfo& data = bagChgData.szbagchglist().Get(i);
			const ItemInfo& itemDetail = data.itemdetail();
			switch(data.ichgtype()){
			case en_BagChg_Add:{
				CCLog("AddType");
				stItemData* itemData = CDataManager::GetInstance().GetGameData<stItemData>(DataFileItem, itemDetail.iitemid());
				if (!itemData)
				{
					CCLOG("Error ItemData---------- ItemDataId=%d", itemDetail.iitemcount());
					continue;
				}
				if (itemData->m_type == 4)
				{
					isFavorite = true;
				}
				BagItem* bagItem = dynamic_cast<BagItem*>( EntityMgr::GetInstance().CreateEntity( EEntityType_Item, "", NULL ) );
				if ( !bagItem )
				{
					CCLOG("Error create bagitem---------- objId= %d", itemDetail.dwobjectid());
					continue;
				}
				addPlayerItemAddNumber += itemDetail.iitemcount();
				bagItem->SetItemData(itemData);
				bagItem->SetInt(EBagItem_Count, itemDetail.iitemcount());
				bagItem->SetInt(EBagItem_ObjId, itemDetail.dwobjectid());
				bagItem->SetInt(EBagItem_CombineNeedCount, itemDetail.icombineneedcount());

				bag->GetBagList().push_back(bagItem);
							   }break;
			case en_BagChg_StackCountChg:{
				for (size_t i = 0; i < bag->GetBagList().size(); ++i)
				{
				//	CCLOG("data.delobjectid() === %d",data.itemdetail().dwobjectid());
				//	CCLOG("GetLocalPlayer()->GetPlayerBagList()[i]->GetInt(EBagItem_ObjId)== %d",GetLocalPlayer()->GetPlayerBagList()[i]->GetInt(EBagItem_ObjId));
					CCLog("SatackCountChg");
					if (data.itemdetail().dwobjectid() == bag->GetBagList()[i]->GetInt(EBagItem_ObjId))
					{
						if (bag->GetBagList()[i]->GetInt(EBagItem_Count) < itemDetail.iitemcount())
						{
							addPlayerItemAddNumber += itemDetail.iitemcount() - bag->GetBagList()[i]->GetInt(EBagItem_Count);
						}
						stItemData* itemData = CDataManager::GetInstance().GetGameData<stItemData>(DataFileItem,data.itemdetail().iitemid());
						if (itemData->m_type == 4)
						{
							isFavorite = true;
						}
						bag->GetBagList()[i]->SetItemData(itemData);
						bag->GetBagList()[i]->SetInt(EBagItem_Count, itemDetail.iitemcount());
						bag->GetBagList()[i]->SetInt(EBagItem_ObjId, itemDetail.dwobjectid());
						bag->GetBagList()[i]->SetInt(EBagItem_CombineNeedCount, itemDetail.icombineneedcount());
						break;
					}
				}
			 }break;
			case en_BagChg_Del:{
				for (size_t i = 0; i < bag->GetBagList().size(); ++i)
				{
					if (data.delobjectid() == bag->GetBagList()[i]->GetInt(EBagItem_ObjId))
					{
						stItemData* itemData = bag->GetBagList()[i]->getItemData();
						if (itemData->m_type == 4)
						{
							isFavorite = true;
						}
						EntityMgr::GetInstance().RemoveEntity( bag->GetBagList()[i]->GetEntityId());
						(bag->GetBagList()).erase((bag->GetBagList()).begin() + i);
						break;
					}
				}
			}break;
			default:
				break;
			}
			if (i == size - 1 && addPlayerItemAddNumber != 0)
			{
				std::vector<Var> vars;
				Var var1(itemDetail.iitemid());
				Var var2(addPlayerItemAddNumber);
				vars.push_back(var1);
				vars.push_back(var2);
				ScriptSys::GetInstance().Execute_1("createPromptBoxlayoutbyItem", &vars, "vector<Var>");
			}
		}

		//UIMgr::GetInstance().RefreshBaseWidgetByName("EquipChip");
		UIMgr::GetInstance().RefreshBaseWidgetByName("BagLayer");
		if (isFavorite)
		{
			GetLocalPlayer()->PlayerHeroFavoriteCheck();
		}
	}
}

void SceneMgr::GetBagData( int iCmd, GSProto::SCMessage& pkg )
{
	GSProto::Cmd_Sc_BagData bagData;
	if(bagData.ParseFromString(pkg.strmsgbody() ) )
	{
		uint size = bagData.szitemdata().size();
		Player* player = GetLocalPlayer();
		for (uint i=0; i<size; i++)
		{
			const ItemInfo& data = bagData.szitemdata().Get(i);
			stItemData* itemData = CDataManager::GetInstance().GetGameData<stItemData>(DataFileItem, data.iitemid());
			if (!itemData)
			{
				CCLOG("Error ItemData---------- ItemDataId=%d", data.iitemcount());
				continue;
			}
			BagItem* bagItem = dynamic_cast<BagItem*>( EntityMgr::GetInstance().CreateEntity( EEntityType_Item, "", NULL ) );
			if ( !bagItem )
			{
				CCLOG("Error create bagitem---------- objId= %d", data.dwobjectid());
				continue;
			}

			bagItem->SetItemData(itemData);
			bagItem->SetInt(EBagItem_Count, data.iitemcount());
			bagItem->SetInt(EBagItem_ObjId, data.dwobjectid());
			bagItem->SetInt(EBagItem_CombineNeedCount, data.icombineneedcount());
			
			player->GetPlayerBag()->GetBagList().push_back(bagItem);
		}

		for (uint i=0; i<bagData.szmaterialdata_size(); i++)
		{
			const ItemInfo& data = bagData.szmaterialdata().Get(i);
			stItemData* itemData = CDataManager::GetInstance().GetGameData<stItemData>(DataFileItem, data.iitemid());
			if (!itemData)
			{
				CCLOG("Error ItemData---------- ItemDataId=%d", data.iitemcount());
				continue;
			}
			BagItem* bagItem = dynamic_cast<BagItem*>( EntityMgr::GetInstance().CreateEntity( EEntityType_Item, "", NULL ) );
			if ( !bagItem )
			{
				CCLOG("Error create bagitem---------- objId= %d", data.dwobjectid());
				continue;
			}

			bagItem->SetItemData(itemData);
			bagItem->SetInt(EBagItem_Count, data.iitemcount());
			bagItem->SetInt(EBagItem_ObjId, data.dwobjectid());
			bagItem->SetInt(EBagItem_CombineNeedCount, data.icombineneedcount());

			player->GetPlayerMaterail()->GetBagList().push_back(bagItem);
		}
	}
}

void SceneMgr::GetRole( int iCmd, GSProto::SCMessage& pkg )
{
	GSProto::Cmd_Sc_GetRole getRoleResult;
	if(getRoleResult.ParseFromString(pkg.strmsgbody() ) )
	{
		CCLOG("getRoleResult ======= %s",getRoleResult.DebugString().c_str());
		//getRoleResult.PrintDebugString();
		uint result = getRoleResult.iresult();

		switch (result)
		{
		case en_GetRole_OK:
			{
				SceneMgr::GetInstance().SwitchToMainUi(true);

				RoleBase base = getRoleResult.rolebasedata();
				Player::PlayerCreateOpt createOpt;
				createOpt.m_EntityId = 0;
				createOpt.m_IsLocalPlayer = true;
				Player* player = dynamic_cast<Player*>( EntityMgr::GetInstance().CreateEntity( EEntityType_Player, base.strname(), &createOpt ) );
				if ( !player )
				{
					CCLOG("Error create player----------------------------------------");
					return;
				}
				player->SetLocalPlayer( true );
				LocalPlayer::GetInstance().SetPlayer( player );
				player->SetUint(EPlayer_ObjId, base.dwobjectid());
				player->SetInt(EPlayer_Lvl, base.ilevel());
				player->SetInt(EPlayer_Exp, base.iexp());
				player->SetInt(EPlayer_Silver, base.isilver());
				player->SetInt(EPlayer_Gold, base.igold());
				player->SetInt(EPlayer_Tili, base.iphystrength());
				player->SetInt(EPlayer_HeroConvertCount, base.iheroconvertcount());
				player->SetInt(EPlayer_MaxExp, base.ilevelupexp());
				player->SetInt(EPlayer_VipLevel,base.iviplevel());
				player->SetInt(EPlayer_VipExp,base.ivipexp());
				player->SetInt(EPlayer_VipLevelUpExp,base.iviplevelupexp());
				player->SetInt(EPlayer_HeadType,base.iheadtype());
				player->SetInt(EPlayer_HeadId,base.iheadid());
				player->SetBool(EPlayer_IsShowFirstPay,base.bshowfirstpaybtn());
				player->SetString(EPlayer_LegionName, base.strlegionname().c_str());
//                if (TBTSDK::GetInstance().isCreateNewRole)
//                {
//#if CC_TARGET_SDK == CC_PLATFORM_SDK_LJAZ 
//                    ScriptSys::GetInstance().Execute("SetExtraDataNewRole");
//#endif
//                    TBTSDK::GetInstance().isCreateNewRole = false;
//                }
//                else
//                {
//#if CC_TARGET_SDK == CC_PLATFORM_SDK_LJAZ 
//                    ScriptSys::GetInstance().Execute("SetExtraDataEnterSever");
//#endif
//                }
			}
			break;
		case en_GetRole_NoRole:
			MainScene::GetInstance().SwitchState(EState_Gaming);
			ScriptSys::GetInstance().Execute("CreateRole_Start");
			break;
		case en_GetRole_Error:
#if CC_TARGET_PLATFORM == CC_PLATFORM_IOS || defined(XH_DEBUG_IOS_LOGIN_UNDER_WIN32)
			ScriptSys::GetInstance().Execute("IOSLogin_BackToMaiMenu");
#else
			ScriptSys::GetInstance().Execute("Login_BackToMaiMenu");
#endif
			break;
		}
	}
}

void SceneMgr::GetHeroList( int iCmd, GSProto::SCMessage& pkg )
{
	GSProto::Cmd_Sc_HeroList heroList;
	if(heroList.ParseFromString(pkg.strmsgbody() ) )
	{
		uint size = heroList.szherolist().size();

		for (uint i=0; i<size; i++)
		{
			const HeroBaseData& data = heroList.szherolist().Get(i);
			stHeroBornData* heroData = CDataManager::GetInstance().GetGameData<stHeroBornData>(DataFileHeroBorn, data.iheroid());
			if (!heroData)
			{
				CCLOG("Error heroData---------- iheroid=%d", data.iheroid());
				continue;
			}

			Hero* hero = dynamic_cast<Hero*>( EntityMgr::GetInstance().CreateEntity( EEntityType_Hero, heroData->m_name, NULL ) );
			if ( !hero )
			{
				CCLOG("Error create hero---------- objId=%d", data.dwobjectid());
				continue;
			}
			hero->SetData(heroData);
			hero->SetUint(EHero_HeroId, data.iheroid());
			hero->SetUint(EHero_ID, data.dwobjectid());
			hero->SetInt(EHero_Lvl, data.ilevel());
			hero->SetInt(EHero_LvlStep, data.ilevelstep());
			hero->SetInt(EHero_FightValue, data.ifightvalue());
			hero->SetInt(EHero_HP, data.imaxhp());
			hero->SetInt(EHero_Att, data.iatt());
			for (size_t j = 0;j < data.szproplist_size(); ++j)
			{
                const PropItem& propItem = data.szproplist().Get(j);
				Var var1(propItem.ilifeattid());
				Var var2(propItem.ivalue());
				ScriptSys::GetInstance().Execute_3("SetLifeAttFromC",hero,"Entity",&var1,"Var",&var2,"Var");
			}
			InnatesKill* innateskill = new InnatesKill(hero);
			hero->AddComponent(EComponentId_InnatesKill, innateskill);
			if(!innateskill)
			{
				CCLOG("Error create heroInnateskill---------- objId=%d", data.dwobjectid());
				continue;
			}
			for(size_t j = 0; j < data.sztallentlist_size();++j)
			{
				const HeroTallent& tallent = data.sztallentlist().Get(j);
				InnatesKillData *innateskillData = new InnatesKillData(tallent.itallentid(),tallent.ilevel(),tallent.bcanupgrade());
				innateskill->GetInnateskillData().push_back(innateskillData);
			}
			//hero->AddComponent(EComponentId_InnatesKill,new InnatesKill(hero));
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
			
			GetLocalPlayer()->GetHeroList().push_back(hero->GetEntityId());		
		}
		ScriptSys::GetInstance().Execute("NoticeFunction_UpdateAfterFormasting");
	}
}

void SceneMgr::GetFightSoulBag( int iCmd, GSProto::SCMessage& pkg )
{
	GSProto::Cmd_Sc_FightSoulBag fightSoulBag;
	if(fightSoulBag.ParseFromString(pkg.strmsgbody() ) )
	{
		GetLocalPlayer()->SetInt(EPlayer_FSChipCount, fightSoulBag.ichipcount());

		uint size = fightSoulBag.szfightsoullist().size();

		for (uint i=0; i<size; i++)
		{
			const FightSoulItem& data = fightSoulBag.szfightsoullist().Get(i);
			stFightSoulData* fightSoulData = CDataManager::GetInstance().GetGameData<stFightSoulData>(DataFileFightSoul, data.ibaseid());
			if (!fightSoulData)
			{
				CCLOG("Error fightSoulData---------- ibaseid=%d", data.ibaseid());
				continue;
			}

			FightSoul* fightSoul = dynamic_cast<FightSoul*>( EntityMgr::GetInstance().CreateEntity( EEntityType_FightSoul, fightSoulData->m_name, NULL ) );
			if ( !fightSoul )
			{
				CCLOG("Error create hero---------- objId=%d", data.dwobjectid());
				continue;
			}
			fightSoul->SetData(fightSoulData);
			fightSoul->SetUint(EFightSoul_ObjID, data.dwobjectid());
			fightSoul->SetInt(EFightSoul_Lvl, data.ilevel());
			fightSoul->SetInt(EFightSoul_Exp, data.iexp());
			fightSoul->SetInt(EFightSoul_NextLvExp, data.inextlvexp());
			fightSoul->SetBool(EFightSoul_Lock, data.blocked());
			fightSoul->SetExtraInt("DevourExp", data.ieatexp());

			for (size_t j=0; j<data.szproplist_size(); j++)
			{
				const PropItem& propItem =  data.szproplist().Get(j);
				Var var2(propItem.ilifeattid());
				Var var3(propItem.ivalue());
				ScriptSys::GetInstance().Execute_3("SetLifeAttFromC", fightSoul, "Entity", &var2, "Var", &var3, "Var");
			}

			GetLocalPlayer()->GetFightSoulBags()->GetFightSoulList()[data.ipos()] = fightSoul;
		}
	}
}

void SceneMgr::GetHeroFightSoulBag( int iCmd, GSProto::SCMessage& pkg )
{
	GSProto::Cmd_Sc_QueryHeroFightSoul fightSoulBag;
	if(fightSoulBag.ParseFromString(pkg.strmsgbody() ) )
	{
		FightSoulBag* bag = NULL;

		Hero* hero = GetLocalPlayer()->GetHeroByObjId(fightSoulBag.dwheroobjectid());
		if ( hero )
		{
			bag = hero->GetFightSoulBags();
		}
		uint size = fightSoulBag.szfightsoullist().size();

		for (uint i=0; i<size; i++)
		{
			const FightSoulItem& data = fightSoulBag.szfightsoullist().Get(i);
			stFightSoulData* fightSoulData = CDataManager::GetInstance().GetGameData<stFightSoulData>(DataFileFightSoul, data.ibaseid());
			if (!fightSoulData)
			{
				CCLOG("Error fightSoulData---------- ibaseid=%d", data.ibaseid());
				continue;
			}

			FightSoul* fightSoul = dynamic_cast<FightSoul*>( EntityMgr::GetInstance().CreateEntity( EEntityType_FightSoul, fightSoulData->m_name, NULL ) );
			if ( !fightSoul )
			{
				CCLOG("Error create hero---------- objId=%d", data.dwobjectid());
				continue;
			}
			fightSoul->SetData(fightSoulData);
			fightSoul->SetUint(EFightSoul_ObjID, data.dwobjectid());
			fightSoul->SetInt(EFightSoul_Lvl, data.ilevel());
			fightSoul->SetInt(EFightSoul_Exp, data.iexp());
			fightSoul->SetInt(EFightSoul_NextLvExp, data.inextlvexp());
			fightSoul->SetBool(EFightSoul_Lock, data.blocked());

			for (size_t j=0; j<data.szproplist_size(); j++)
			{
				const PropItem& propItem =  data.szproplist().Get(j);
				Var var2(propItem.ilifeattid());
				Var var3(propItem.ivalue());
				ScriptSys::GetInstance().Execute_3("SetLifeAttFromC", fightSoul, "Entity", &var2, "Var", &var3, "Var");
			}

			hero->GetFightSoulBags()->GetFightSoulList()[data.ipos()] = fightSoul;
		}
		hero->SetExtraBool("IsQueryFightSoul", true);
	
		UIMgr::GetInstance().RefreshBaseWidgetByName("FightSoul");	
		ScriptSys::GetInstance().Execute("HeroInfo_RefreshFightSoul");
	}
}

void SceneMgr::GetEquipBag( int iCmd, GSProto::SCMessage& pkg )
{
	GSProto::CMD_EQUIP_BAG_SC equipBag;
	if(equipBag.ParseFromString(pkg.strmsgbody() ) )
	{
		uint size = equipBag.szequiplist().size();

		for (uint i=0; i<size; i++)
		{
			const EquipInfo& data = equipBag.szequiplist().Get(i);
			stEquipData* equipData = CDataManager::GetInstance().GetGameData<stEquipData>(DataFileEquip, data.iitemid());
			if (!equipData)
			{
				CCLOG("Error equipData---------- ibaseid=%d", data.iitemid());
				continue;
			}

			Equip* equip = dynamic_cast<Equip*>( EntityMgr::GetInstance().CreateEntity( EEntityType_Equip, equipData->m_name, NULL ) );
			if ( !equip )
			{
				CCLOG("Error create hero---------- objId=%d", data.dwobjectid());
				continue;
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

			GetLocalPlayer()->GetEquipBags()->GetEquipList()[data.ipos()] = equip;
		}
	}
}

void SceneMgr::GetHeroEquipBag( int iCmd, GSProto::SCMessage& pkg )
{
	GSProto::CMD_QUERY_HEROEQUIP_SC equipBag;
	if(equipBag.ParseFromString(pkg.strmsgbody() ) )
	{
		EquipBag* bag = NULL;

		Hero* hero = GetLocalPlayer()->GetHeroByObjId(equipBag.dwheroobjectid());
		if ( hero )
		{
			bag = hero->GetEquipBags();
		}
		uint size = equipBag.szequiplist().size();

		for (uint i=0; i<size; i++)
		{
			const EquipInfo& data = equipBag.szequiplist().Get(i);
			stEquipData* equipData = CDataManager::GetInstance().GetGameData<stEquipData>(DataFileEquip, data.iitemid());
			if (!equipData)
			{
				CCLOG("Error equipData---------- ibaseid=%d", data.iitemid());
				continue;
			}

			Equip* equip = dynamic_cast<Equip*>( EntityMgr::GetInstance().CreateEntity( EEntityType_Equip, equipData->m_name, NULL ) );
			if ( !equip )
			{
				CCLOG("Error create equip---------- objId=%d", data.dwobjectid());
				continue;
			}
			equip->SetData(equipData);
			equip->SetUint(EFightSoul_ObjID, data.dwobjectid());
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
				//equip->SetExtraInt(TypeConvert::ToString(propItem.ilifeattid()).c_str(), propItem.ivalue());
			}
			if (data.has_equiprandomprop())
			{
				const PropItem& propItem = data.equiprandomprop();
				Var var2(propItem.ilifeattid());
				Var var3(propItem.ivalue());
				ScriptSys::GetInstance().Execute_3("SetLifeAttFromC", equip, "Entity", &var2, "Var", &var3, "Var");
				equip->setAddAttribute(propItem.ilifeattid(),propItem.ivalue());
			}

			hero->GetEquipBags()->GetEquipList()[data.ipos()] = equip;
		}

		for (size_t i=0; i<equipBag.szsuitstatelist_size(); i++)
		{
			const EquipSuitState suitState = equipBag.szsuitstatelist().Get(i);
			ScriptSys::GetInstance().Execute_number("Equip_SetSuitState", hero->GetInt(EHero_ID), suitState.isuitid(), suitState.iactivecount());
			//equip->SetExtraInt(TypeConvert::ToString(propItem.ilifeattid()).c_str(), propItem.ivalue());
		}

		hero->SetExtraBool("IsQueryEquip", true);

		ScriptSys::GetInstance().Execute("NewHeroEquip_ShowLayout");
	}
}

void SceneMgr::updateHeroCache()
{
	if(!m_bHeroCacheDirty)
	{
		return;
	}

	set<int> heroIDList;
	const vector<uint>& list = GetLocalPlayer()->GetFormationList();
	for(size_t i = 0; i < list.size(); i++)
	{
		uint dwHeroObjectID = list[i];
		Freeman::Hero* pHero = GetLocalPlayer()->GetHeroByObjId(dwHeroObjectID);
		if(!pHero) continue;

		int iHeroID = pHero->GetInt(EHero_HeroId);
		stHeroBornData* heroData = CDataManager::GetInstance().GetGameData<stHeroBornData>(DataFileHeroBorn, iHeroID);
		if (!heroData)
		{
			continue;
		}

		heroIDList.insert(iHeroID);
		if(m_cacheFormationHeroList.find(iHeroID) != m_cacheFormationHeroList.end() )
		{
			continue;
		}
	
		string strAnimationName = heroData->m_animName;
		string strAnimationPath = StringMgr::GetInstance().Format("AnimExport/%s/%s.ExportJson", strAnimationName.c_str(), strAnimationName.c_str() );
		CCArmatureDataManager::sharedArmatureDataManager()->addArmatureFileInfo(strAnimationPath.c_str() );
		CCArmatureDataManager::sharedArmatureDataManager()->rebuildArmatureSpriteFrame(strAnimationPath.c_str() );
		CCArmature* pCache = CCArmature::create(strAnimationName.c_str());
		pCache->retain();
		m_cacheFormationHeroList[iHeroID] = pCache;
	}
	
	for(std::map<int, CCArmature*>::iterator it = m_cacheFormationHeroList.begin(); it != m_cacheFormationHeroList.end();)
	{
		CCArmature* pCacheArmature = it->second;
		
		if(heroIDList.find(it->first) == heroIDList.end() )
		{
			pCacheArmature->release();
			m_cacheFormationHeroList.erase(it++);
			continue;
		}
		it++;
	}

	m_bHeroCacheDirty = false;
}

CCArmature* SceneMgr::queryCacheArmature(int iHeroID)
{
	std::map<int, CCArmature*>::iterator it = m_cacheFormationHeroList.find(iHeroID);
	if(it != m_cacheFormationHeroList.end() )
	{
		return it->second;
	}

	return NULL;
}

void SceneMgr::GetFormationList( int iCmd, GSProto::SCMessage& pkg )
{
	GSProto::Cmd_Sc_FormationData fList;
	if(fList.ParseFromString(pkg.strmsgbody() ) )
	{
		uint size = fList.szformationlist_size();
		GetLocalPlayer()->InitFormationList();
		vector<uint>& list = GetLocalPlayer()->GetFormationList();

		for (uint i=0; i<size; i++)
		{
			const FormationItemData& data = fList.szformationlist().Get(i);
			
			if (list.size() < 9)
			{
				CCLOG("error GetFormationList``````````````````");
				return;
			}
			list[data.ipos()] = data.dwobjectid();
		}
		
		SceneMgr::GetInstance().m_bHeroCacheDirty = true;
		
		GetLocalPlayer()->SetInt(EPlayer_FightValue, fList.ifightvalue());
		GetLocalPlayer()->SetInt(EPlayer_FormationLimit, fList.imemberlimit());

		if (UIMgr::GetInstance().HasBaseWidget("FormationSecond"))
		{
			ScriptSys::GetInstance().Execute_1("FormationSecond_Refresh", UIMgr::GetInstance().GetBaseWidgetByName("FormationSecond"), "Widget");
		}
		
		//改变阵形之后更新弱引导
		ScriptSys::GetInstance().Execute("NoticeFunction_UpdateAfterFormasting");
		//刷新掠夺时己方队形
		ScriptSys::GetInstance().Execute("TerritoryOthersTeam1_Refresh");
		//刷新竞技场的己方数据
		ScriptSys::GetInstance().Execute("Arena_RefreshData");
	}
}

void SceneMgr::SwitchToMainUi(bool isFromMainmenu)
{
	CCTextureCache::sharedTextureCache()->setAutoAsyncLoadEnable(true);
	if (isFromMainmenu)
	{
		MainScene::GetInstance().SwitchState(EState_Gaming);
	}
	
	if( m_PreScene )
	{
		m_PreScene->ReleaseAllEntity();
		FM_SAFE_DELETE(m_PreScene);
	}
	m_PreScene = m_Scene;
	m_Scene = NULL;
	m_bGotoMainmenu = false;
	m_isFromMainmenu = isFromMainmenu;
	SwitchState(EGameState_LoadScene, EGameState_MainUi);
}

void SceneMgr::SwitchToTerritory()
{
	if( m_PreScene )
	{
		m_PreScene->ReleaseAllEntity();
		FM_SAFE_DELETE(m_PreScene);
	}
	m_PreScene = m_Scene;
	m_Scene = NULL;
	m_bGotoMainmenu = false;
	SwitchState(EGameState_LoadScene, EGameState_Territory);
}

void SceneMgr::SwitchToMyTerritory()
{
	if( m_PreScene )
	{
		m_PreScene->ReleaseAllEntity();
		FM_SAFE_DELETE(m_PreScene);
	}
	m_PreScene = m_Scene;
	m_Scene = NULL;
	m_bGotoMainmenu = false;
	SwitchState(EGameState_LoadScene, EGameState_MyTerritory);
}


void SceneMgr::SwitchFightingScene(uint sceneId )
{
	if( m_PreScene )
	{
		m_PreScene->ReleaseAllEntity();
		FM_SAFE_DELETE(m_PreScene);
	}
	
	m_PreScene = m_Scene;
	m_Scene = Scene::Create(sceneId);
	m_bGotoMainmenu = false;
	SwitchState(EGameState_LoadScene, EGameState_Fighting);
}

void SceneMgr::SwitchToMainMenu()
{
	if( m_PreScene )
	{
		m_PreScene->ReleaseAllEntity();
		FM_SAFE_DELETE(m_PreScene);
	}
	m_PreScene = m_Scene;
	m_Scene = NULL;
	m_bGotoMainmenu = true;
	ScriptSys::GetInstance().Execute("Login_CleanDataWhenBackToMainMenu");
	SwitchState(EGameState_LoadScene, EGameState_None);
}


int SceneMgr::Update(uint delta)
{
	if (GetLocalPlayer())
	{
		GetLocalPlayer()->Update(delta);
	}

	ScriptSys::GetInstance().Execute("Guide_Update");

	switch(m_State)
	{
	case EGameState_Territory:
		
		break;
	case EGameState_MyTerritory:

		break;
	case EGameState_Fighting:
		if(m_Scene == NULL)
			return 0;
		if (m_Scene->IsPause() == false)
		{
			m_Scene->Update(delta);
			FightMgr::GetInstance().OnUpdate(delta);		
		}	
		break;
	case EGameState_LoadScene:
		UpdateLoading(delta);
		break;
	case EGameState_MainUi:
		Widget* widget = UIMgr::GetInstance().GetBaseWidgetByName("HeroList");
		if (widget)
		{
			/*ListView* listView = static_cast<ListView*>(widget->getChildByTag(72));
			CCPoint pos = listView->getInnerContainer()->getPosition();
			//CCLOG("pos x = %f", pos.x);
			ccArray* arrayItems = listView->getItems()->data;
			int length = arrayItems->num;
			if (length == 0)
			{
				return 0;
			}

			float posY = 0;
			
			for (int i=0; i<length; i++)
			{	
				Widget* item = static_cast<Widget*>(arrayItems->arr[i]);
				//CCLOG("item->getPositionX() = %f", item->getPositionX());
				float itemPosX = item->getPositionX() + pos.x + 96;
				if ( itemPosX > -96 && itemPosX < 1052)
				{
					if (itemPosX <= 480)
					{
						float tmp = powf(itemPosX + 96, 1.0f/3.0f);
						item->setPosition(ccp(item->getPositionX(), posY + tmp*12.0f));
						item->setScale(1.1f - tmp/30);
					}
					else
					{
						float tmp = powf(1052.0f - itemPosX, 1.0f/3.0f);
						item->setPosition(ccp(item->getPositionX(), posY + tmp*12.0f));
						item->setScale(1.1f - tmp/30);
					}
				}
				else
				{
					item->setPosition(ccp(item->getPositionX(), posY));
				}
			}*/
		}
		break;
	}

	updateHeroCache();

	return 0;
}

void SceneMgr::SwitchState( EGameState newState, EGameState nextState )
{
	if(newState == EGameState_LoadScene)
	{
		m_LoadStep = ESLoadStep_Init;
	}
	if (m_State != EGameState_Fighting && m_State != EGameState_LoadScene)
	{
		m_lastState = m_State;
	}
	
	m_State = newState;
	m_nextState = nextState;

	ScriptSys::GetInstance().Execute("Guide_CheckIsHave");
}

void SceneMgr::freeUnusedMemory()
{
	CCSpriteFrameCache::sharedSpriteFrameCache()->removeUnusedSpriteFrames();
	CCDirector::sharedDirector()->purgeCachedData();
}

int SceneMgr::UpdateLoading(uint delta)
{
	switch(m_LoadStep)
	{
	case ESLoadStep_Init:
		{
			CCDirector::sharedDirector()->setScaleTime(1.0);
			//m_curWeatherId = EWeatherFlag_Normal;
			
			//CCTextureCache::sharedTextureCache()->removeAllTextures();
			SimpleAudioEngine::sharedEngine()->stopBackgroundMusic();

			MainScene::GetInstance().removeChildByTag(EMSTag_DungeonEffect);
			
			//ScriptSys::GetInstance().Execute( "SceneMgr_OnLoadingInit" );
			UpdateLoadBar(StringMgr::GetInstance().FormatString("LauncherResInit").c_str(),0);

			if (m_isFromMainmenu || m_bGotoMainmenu)
			{
				UIMgr::GetInstance().CloseAllBaseWidget(true);
				UIMgr::EndWaiting();

				CCTextureCache::sharedTextureCache()->setAutoAsyncLoadEnable(false);
				UIWidget *widget = UIMgr::GetInstance().CreateBaseWidgetByFileName("UIExport/DemoLogin/Logo.json");
				ScriptSys::GetInstance().Execute_1("LogoTip",widget,"Widget");
				if (m_isFromMainmenu)
				{
					m_LoadStep = ESLoadStep_ServerData;
				}	
				else
				{
					m_LoadStep = ESLoadStep_FreeLastScene;
				}

				CCArmatureDataManager::sharedArmatureDataManager()->removeArmatureFileInfo("UIEffect/Loding/Loding.ExportJson");
				CCArmatureDataManager::sharedArmatureDataManager()->addArmatureFileInfo("UIEffect/Loding/Loding.ExportJson");
				CCArmature* pAnimature = CCArmature::create("Loding");
				
				pAnimature->getAnimation()->playWithIndex(1);
				widget->getChildByName("BG")->addNode(pAnimature);
				pAnimature->setPosition(ccp(0, 0));

				CCTextureCache::sharedTextureCache()->setAutoAsyncLoadEnable(true);
			}
			else
			{
				UIMgr::ShowWaiting(15);
				m_LoadStep = ESLoadStep_FreeLastScene;
			}			

			m_isFromMainmenu = false;
		}
		break;

	case ESLoadStep_ServerData:
		{
			//Sleep(1000);
			UpdateLoadBar(StringMgr::GetInstance().FormatString("LauncherResInit").c_str(),10);
			if (m_isServerDataFinish)
			{
				m_isServerDataFinish = false;
				m_LoadStep = ESLoadStep_FreeLastScene;
			}
		}
		break;
            
	case ESLoadStep_FreeLastScene://释放上个场景数据
		{
			//Sleep(1000);
			//ScriptSys::GetInstance().Execute( "SceneMgr_OnFreeLastScene" );
			if(m_PreScene != NULL)
			{			
				m_PreScene->ReleaseAllEntity();
				FM_SAFE_DELETE(m_PreScene);
			}
            
            m_LoadStep = ESLoadStep_FreeTexTure;
			UpdateLoadBar(StringMgr::GetInstance().FormatString("LauncherResInit").c_str(),30);
		}
		break;
            
    case ESLoadStep_FreeTexTure:
        {
			//Sleep(1000);
           // CCSpriteFrameCache::sharedSpriteFrameCache()->removeSpriteFrames();
			//CCTextureCache::sharedTextureCache()->removeAllTextures();

            if(m_bGotoMainmenu)
			{
				//ScriptSys::GetInstance().ExecuteString( "SceneMgr_ClearGameStatus()" );
				EntityMgr::GetInstance().RemoveEntity(GetLocalPlayer());
				LocalPlayer::GetInstance().SetPlayer( NULL );
				m_LoadStep = ESLoadStep_SwitchMainMenu;
			}
			else
			{
				m_LoadStep = ESLoadStep_LoadTileMap;
			}
			UpdateLoadBar(StringMgr::GetInstance().FormatString("LauncherResInit").c_str(),40);
        }
        break;
	case ESLoadStep_LoadTileMap: //加载新地图
		{
			//Sleep(1000);
			if (m_Scene)
			{
				PerformenceCounter tmpCounter("LoadTileMap");
				m_Scene->LoadTileMap();
			}
					
			m_LoadStep = ESLoadStep_LoadEnity;
			UpdateLoadBar(StringMgr::GetInstance().FormatString("LauncherResInit").c_str(),60);
		}
		break;
	case ESLoadStep_LoadEnity: //加载物件
		{
			//if (m_nextState == EGameState_Fighting && CCTextureCache::sharedTextureCache()->isAsyncLoadFinished() && !m_bAsyncLoading)
			if (m_nextState == EGameState_Fighting)
			{
				GSProto::Cmd_Sc_FightMember& fightMember = FightMgr::GetInstance().m_fightMember;
				
				uint size = fightMember.szfightobject_size();
				PerformenceCounter tmpCounter("ESLoadStep_LoadEnity");
				for (uint i=0; i<size; i++)
				{
					const FightObj& data = fightMember.szfightobject().Get(i);
					
					AddEntityInToScene(data);
				}

				// 先隐藏
				FightMgr::GetInstance().setAllFightObjVisible(false);
			}
			else if (m_nextState == EGameState_MainUi)
			{
				//ScriptSys::GetInstance().Execute("SceneMgr_CacheUI");
			}

			//if(CCTextureCache::sharedTextureCache()->isAsyncLoadFinished())
			{
				m_LoadStep = ESLoadStep_Finished;
				UpdateLoadBar(StringMgr::GetInstance().FormatString("LauncherResInit").c_str(),80);
			}
		}
		break;
	
	case ESLoadStep_Finished:
		{
			UIMgr::EndWaiting();
			UIMgr::GetInstance().CloseAllBaseWidget(true);
			
			SwitchToNextState();
			UpdateLoadBar(StringMgr::GetInstance().FormatString("LauncherResInit").c_str(),90);
			//ScriptSys::GetInstance().ExecuteString( "SceneMgr_OnSwitchInScene()" );
		}
		break;
	case ESLoadStep_SwitchMainMenu:
		{
			UpdateLoadBar(StringMgr::GetInstance().FormatString("LauncherResInit").c_str(),90);
			UIMgr::GetInstance().CloseAllBaseWidget(true);
			SwitchToNextState();
			
			ShowMainMenuSceneUi();
		}
		break;
	}
	return 0;
}


void SceneMgr::AddEntity(Hero* entity)
{
	if(entity == NULL)
		return;

	if(m_Scene == NULL)
		return;
	m_Scene->AddEntity( entity); 
}

void SceneMgr::RemoveEntity( uint id )
{
	if(m_Scene != NULL)
		m_Scene->RemoveEntity( id );
}

void SceneMgr::SwitchToNextState()
{
	m_LoadStep = ESLoadStep_Init;

	if (m_State == m_nextState)
	{
		m_nextState = EGameState_None;
		return;
	}

	m_State = m_nextState;
	freeUnusedMemory();
	switch (m_nextState)
	{
	case EGameState_MainUi:
		ScriptSys::GetInstance().Execute("SceneMgr_OpenMainUi");
		
		break;
	case EGameState_Territory:
		ScriptSys::GetInstance().Execute("Territory_Create");
		break;
	case EGameState_MyTerritory:
		ScriptSys::GetInstance().Execute("MyTerritory_Create");
		break;
	case EGameState_Fighting:
		if (FightMgr::GetInstance().m_isCg)
		{
			FightMgr::GetInstance().GetMainState().ResetState(EFightState_Pause, 1);
			ScriptSys::GetInstance().Execute("CreateRole_First");
		}
		//ScriptSys::GetInstance().Execute("Fight_AddBlackScreen");
		break;
	}
	
	m_nextState = EGameState_None;
}

void SceneMgr::ShowMainMenuSceneUi()
{
	MainScene::GetInstance().SwitchState(EState_MainMenu);
#if CC_TARGET_PLATFORM == CC_PLATFORM_IOS || defined(XH_DEBUG_IOS_LOGIN_UNDER_WIN32)
	ScriptSys::GetInstance().Execute("IOSLogin_BackToMaiMenu");
#else
	ScriptSys::GetInstance().Execute("Login_BackToMaiMenu");
#endif
}

void SceneMgr::UpdateLoadBar( const char* tips,int val )
{
	UIWidget* widget = UIMgr::GetInstance().GetBaseWidgetByName("Logo");
	if (widget)
	{
		UILoadingBar* bar = dynamic_cast<UILoadingBar*>(widget->getChildByTag(1));
		if (bar)
		{
			bar->setPercent(val);
		}
		/*UILabel* label = dynamic_cast<UILabel*>(widget->getChildByTag(2));
		if (label)
		{
			label->setText(tips);
		}*/
	}

	/*CCNode* bar = getChildByTag(E_LauncherTag_LoadBar);
	if(bar == NULL)
		return;
	CCControlSlider* slider = dynamic_cast<CCControlSlider*>(bar);
	if(slider)
	{
		slider->setValue(val);
	}

	bar->removeChildByTag(11);
	if(tips != NULL)
	{
		//CCLog("Loading....  %f  ",val);
		CCSize cs = bar->getContentSize();
		vector<Var> args;
		args.push_back( Var( tips ) );
		std::string txt = StringMgr::GetInstance().FormatString( "ProgressPatten", args );
		CCNode* lab = CompLabel::GetDefaultCompLabel(txt.c_str(),0);
		lab->setPosition(ccp((cs.width-lab->getContentSize().width)/2,(cs.height+lab->getContentSize().height)/2));
		bar->addChild(lab,1,11);
	}*/
}

void SceneMgr::AddEntityInToScene( const GSProto::FightObj& data, bool isFromSky)
{
	int heroId = -1;

	if (data.iobjecttype() == en_class_Hero)
	{
		heroId = data.hero().ibaseid();
	}
	else if (data.iobjecttype() == en_class_Monster)
	{
		heroId = data.monster().ibaseid();
	}
	else if (data.iobjecttype() == en_class_GodAnimal)
	{
		heroId = data.godanimal().ibaseid();
	}

	if (heroId == -1)
	{
		return;
	}

	Hero* hero = dynamic_cast<Hero*>( EntityMgr::GetInstance().CreateEntity( EEntityType_Hero, "", NULL ) );
	if ( !hero )
	{
		//CCLOG("Error create hero---------- objId=%d", data.dwobjectid());
		return;
	}

	hero->SetUint(EHero_HeroId, heroId);
	hero->SetUint(EHero_ObjectType, data.iobjecttype());

	if (data.iobjecttype() == en_class_Hero)
	{
		stHeroBornData* heroData = CDataManager::GetInstance().GetGameData<stHeroBornData>(DataFileHeroBorn, heroId);
		if (!heroData)
		{
			CCLOG("Error heroData---------- heroId=%d", heroId);
			EntityMgr::GetInstance().RemoveEntity(hero);
			return;
		}
		hero->SetExtraString("Name", heroData->m_name.c_str());
		hero->SetExtraString("AnimName", heroData->m_animName.c_str());
		hero->SetExtraInt("AnimScale", heroData->m_animScale);
		hero->SetInt(EHero_Lvl, data.hero().ilevel());
		hero->SetInt(EHero_LvlStep, data.hero().ilevelstep());
		hero->SetInt(EHero_MaxHP, data.hero().imaxhp());
		hero->SetInt(EHero_HP, data.hero().ihp());
		hero->SetInt(EHero_InitAnger, data.hero().iinitanger());
		hero->SetInt(EHero_Quality, data.hero().iquality());
		hero->SetData(heroData);
	}
	else if (data.iobjecttype() == en_class_Monster)
	{
		stMonsterData* mosterData = CDataManager::GetInstance().GetGameData<stMonsterData>(DataFileMonster, heroId);
		if (!mosterData)
		{
			CCLOG("Error mosterData---------- heroId=%d", heroId);
			EntityMgr::GetInstance().RemoveEntity(hero);
			return;
		}
		hero->SetExtraString("Name", mosterData->m_name.c_str());
		hero->SetExtraString("AnimName", mosterData->m_animName.c_str());
		hero->SetExtraInt("AnimScale", mosterData->m_animScale);
		hero->SetExtraInt("MonterType", mosterData->m_monterType);
		hero->SetInt(EHero_Lvl, data.monster().ilevel());
		hero->SetInt(EHero_LvlStep, data.monster().ilevelstep());
		hero->SetInt(EHero_MaxHP, data.monster().imaxhp());
		hero->SetUint(EHero_HP, data.monster().ihp());
		hero->SetUint(EHero_InitAnger, data.monster().iinitanger());
	}
	else if (data.iobjecttype() == en_class_GodAnimal)
	{
		stGodAnimalData* animalData = CDataManager::GetInstance().GetGameData<stGodAnimalData>(DataFileGodAniaml, heroId);
		if (!animalData)
		{
			CCLOG("Error mosterData---------- heroId=%d", heroId);
			EntityMgr::GetInstance().RemoveEntity(hero);
			return;
		}
		hero->SetExtraString("Name", animalData->m_name.c_str());
		hero->SetExtraString("AnimName", animalData->m_animName.c_str());
		hero->SetExtraInt("AnimScale", animalData->m_animScale);
		hero->SetUint(EHero_Lvl, data.godanimal().ilevel());
		hero->SetUint(EHero_LvlStep, data.godanimal().ilevelstep());
		hero->SetUint(EHero_MaxHP, data.godanimal().imaxhp());
		hero->SetUint(EHero_HP, data.godanimal().imaxhp());
		hero->SetUint(EHero_InitAnger, data.godanimal().iinitanger());
	}
	//hero->SetUint(EHero_ID, data.dwobjectid());
	hero->SetUint(EHero_FormationPos, data.ipos());
	//hero->SetExtraBool("IsSky", isFromSky);

	if (hero->GetInt(EHero_HP) == 0)
	{
		EntityMgr::GetInstance().RemoveEntity(hero);
	}
	else
	{
		AddEntity(hero);		
	}
}

void SceneMgr::CleanEntitys( bool isAttack )
{
	int posMin = 9;
	int posMax = 17;
	int godPos = 19;
	if (isAttack)
	{
		posMin = 0;
		posMax = 8;
		godPos = 18;
	}
	
	for (size_t i=posMin; i<=posMax; i++)
	{
		Hero* hero = FightMgr::GetInstance().GetHeroByPos(i);
		if (hero)
		{
			m_Scene->RemoveEntity(hero->GetEntityId());
		}
	}

	Hero* hero = FightMgr::GetInstance().GetHeroByPos(godPos);
	if (hero)
	{
		m_Scene->RemoveEntity(hero->GetEntityId());
	}
}

NS_FM_END
