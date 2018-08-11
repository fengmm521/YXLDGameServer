#include "CoreImpPch.h"
#include "PropObserveSystem.h"


extern "C" IObject* createPropObserveSystem()
{
	return new PropObserveSystem;
}

static int s_selfNotifyProp[PROP_ENTITY_MAX] = {0};
static bool s_bSelfPropInited = false;

static map<int, int> s_prop2LifeAtttMap;
static map<int, int> s_lifeAtt2PropMap;


static void makePropLifeAttPair(int iPropID, int iLifeAttID)
{
	s_prop2LifeAtttMap[iPropID] = iLifeAttID;
	s_lifeAtt2PropMap[iLifeAttID] = iPropID;
}

int Prop2LifeAtt(int iPropID)
{
	map<int, int>::iterator it = s_prop2LifeAtttMap.find(iPropID);
	if(it == s_prop2LifeAtttMap.end() )
	{
		return -1;
	}

	int iResult = it->second;

	return iResult;
}

int LifeAtt2Prop(int iLifeAtt)
{
	map<int, int>::iterator it = s_lifeAtt2PropMap.find(iLifeAtt);
	if(it == s_lifeAtt2PropMap.end() )
	{
		return -1;
	}

	int iResult = it->second;

	return iResult;
}


PropObserveSystem::PropObserveSystem()
{
}

PropObserveSystem::~PropObserveSystem()
{
	IEntity* pEntity = getEntityFromHandle(m_hEntity);
	if(pEntity)
	{
		unRegisterActorFinish();
	}
}


bool PropObserveSystem::create(IEntity* pEntity, const std::string& strData)
{
	assert(pEntity);
	m_hEntity = pEntity->getHandle();

	if(!s_bSelfPropInited)
	{
		makePropLifeAttPair(PROP_ENTITY_EXP, GSProto::en_LifeAtt_Exp);
		makePropLifeAttPair(PROP_ACTOR_GOLD, GSProto::en_LifeAtt_Gold);
		makePropLifeAttPair(PROP_ACTOR_SILVER, GSProto::en_LifeAtt_Silver);
		makePropLifeAttPair(PROP_ENTITY_HEROEXP, GSProto::en_LifeAtt_HeroExp);

		// 战斗属性
		makePropLifeAttPair(PROP_ENTITY_MAXHP, GSProto::en_LifeAtt_MaxHP);
		makePropLifeAttPair(PROP_ENTITY_ATT, GSProto::en_LifeAtt_Att);
		makePropLifeAttPair(PROP_ENTITY_DOGE, GSProto::en_LifeAtt_Doge);
		makePropLifeAttPair(PROP_ENTITY_HIT, GSProto::en_LifeAtt_Hit);
		makePropLifeAttPair(PROP_ENTITY_ANTIKNOCK, GSProto::en_LifeAtt_AntiKnock);
		makePropLifeAttPair(PROP_ENTITY_KNOCK, GSProto::en_LifeAtt_Knock);
		makePropLifeAttPair(PROP_ENTITY_BLOCK, GSProto::en_LifeAtt_Block);
		makePropLifeAttPair(PROP_ENTITY_WRECK, GSProto::en_LifeAtt_Wreck);
		makePropLifeAttPair(PROP_ENTITY_ARMOR, GSProto::en_LifeAtt_Armor);
		makePropLifeAttPair(PROP_ENTITY_SUNDER, GSProto::en_LifeAtt_Sunder);
		makePropLifeAttPair(PROP_ENTITY_INITANGER, GSProto::en_LifeAtt_InitAnger);
		makePropLifeAttPair(PROP_ACTOR_FSCHIPCOUNT, GSProto::en_LifeAtt_FSChipCount);

		makePropLifeAttPair(PROP_ENTITY_PHYSTRENGTH, GSProto::en_LifeAtt_PhyStrength);
		makePropLifeAttPair(PROP_ENTITY_HEROCVTCOUNT, GSProto::en_LifeAtt_HeroConvertCount);

		makePropLifeAttPair(PROP_ENTITY_FUNCTIONMASK, GSProto::en_LifeAtt_FunctionMask);
		makePropLifeAttPair(PROP_ENTITY_HONOR, GSProto::en_LifeAtt_Honor);
		makePropLifeAttPair(PROP_ENTITY_HP, GSProto::en_LifeAtt_HP);
		
		makePropLifeAttPair(PROP_ENTITY_PHYSTRENGTHLIMIT, GSProto::en_LifeAtt_PhyStrengthLimit);

		makePropLifeAttPair(PROP_ACTOR_VIPLEVEL, GSProto::en_LifeAtt_VIPLevel);
		makePropLifeAttPair(PROP_ENTITY_ANGER, GSProto::en_LifeAtt_Anger);
		makePropLifeAttPair(PROP_ENTITY_FIGHTVALUE, GSProto::en_LifeAtt_FightValue);

		makePropLifeAttPair(PROP_HERO_HASFIGHTSOUL, GSProto::en_LifeAtt_HasFightSoul);
		makePropLifeAttPair(PROP_ACTOR_VIPEXP,GSProto::en_LifeAtt_VIPEXP);
		makePropLifeAttPair(PROP_ACTOR_NOTICE,GSProto::en_LifeAtt_Notice);

		makePropLifeAttPair(PROP_ENTITY_SKILLDAMAGE,GSProto::en_LifeAtt_SkillDamage);

		makePropLifeAttPair(PROP_ENTITY_SKILLDEF, GSProto::en_LifeAtt_SkillDef);

		makePropLifeAttPair(PROP_HERO_HASEQUIP, GSProto::en_LifeAtt_HasEquip);
		
		makePropLifeAttPair(PROP_HERO_PRICE, GSProto::en_LifeAtt_Price);

		makePropLifeAttPair(PROP_ENTITY_ACTOR_HEAD, GSProto::en_LifeAtt_actorhead);
		makePropLifeAttPair(PROP_ENTITY_ACTOR_HEADTYPE, GSProto::en_LifeAtt_actorheadtype);
		
		makePropLifeAttPair(PROP_ENTITY_ACTOR_VIGOR,  GSProto::en_LifeAtt_actorVigor);

		makePropLifeAttPair(PROP_ENTITY_QUALITY, GSProto::en_LifeAtt_Quality);
		makePropLifeAttPair(PROP_ENTITY_DEF, GSProto::en_LifeAtt_Def);
		makePropLifeAttPair(PROP_ENTITY_STRENGTHBUYTIMES, GSProto::en_LifeAtt_LeftPhyStrength);



		makePropLifeAttPair(PROP_ACTOR_SHOWKNOCK_DAMAGE, GSProto::en_LifeAtt_KnockDamage);
		makePropLifeAttPair(PROP_ACTOR_SHOWKNOCK_XIXUE, GSProto::en_LifeAtt_KnockXiXue);
		makePropLifeAttPair(PROP_ACTOR_SHOWCONATTACK, GSProto::en_LifeAtt_ConAttackRate);

		makePropLifeAttPair(PROP_ACTOR_LEGION_CONTRIBUTE, GSProto::en_LifeAtt_Contribute);

		makePropLifeAttPair(PROP_ACTOR_SHOWFIRSTPAY, GSProto::en_LifeAtt_ShowFristPayBtn);

		
		

		
		//s_selfNotifyProp[PROP_ENTITY_EXP] = 1; 经验/等级变化走独立协议
		s_selfNotifyProp[PROP_ACTOR_GOLD] = 1;		
		s_selfNotifyProp[PROP_ACTOR_SILVER] = 1;		
		s_selfNotifyProp[PROP_ENTITY_HEROEXP] = 1;	
		s_selfNotifyProp[PROP_ACTOR_FSCHIPCOUNT] = 1;	
		s_selfNotifyProp[PROP_ENTITY_HEROCVTCOUNT] = 1;

		s_selfNotifyProp[PROP_ENTITY_MAXHP] = 1;	
		s_selfNotifyProp[PROP_ENTITY_ATT] = 1;
		s_selfNotifyProp[PROP_ENTITY_FUNCTIONMASK] = 1;
		s_selfNotifyProp[PROP_ENTITY_PHYSTRENGTH] = 1;
		s_selfNotifyProp[PROP_ENTITY_PHYSTRENGTHLIMIT] = 1;
		s_selfNotifyProp[PROP_ACTOR_VIPLEVEL] = 1;
		s_selfNotifyProp[PROP_ENTITY_HONOR] = 1;
		s_selfNotifyProp[PROP_ENTITY_FIGHTVALUE] = 1;
		s_selfNotifyProp[PROP_HERO_HASFIGHTSOUL] = 1;
		s_selfNotifyProp[PROP_ACTOR_NOTICE] = 1;
		s_selfNotifyProp[PROP_ENTITY_ARMOR] = 1;
		s_selfNotifyProp[PROP_ENTITY_SUNDER] = 1;
		s_selfNotifyProp[PROP_HERO_HASEQUIP] = 1;
		
		s_selfNotifyProp[PROP_HERO_PRICE] = 1;

		s_selfNotifyProp[PROP_ENTITY_ACTOR_HEAD] = 1;
		s_selfNotifyProp[PROP_ENTITY_ACTOR_HEADTYPE] = 1;
		s_selfNotifyProp[PROP_ENTITY_ACTOR_VIGOR] = 1;
		s_selfNotifyProp[PROP_ENTITY_QUALITY] = 1;
		s_selfNotifyProp[PROP_ENTITY_DEF] = 1;
		s_selfNotifyProp[PROP_ENTITY_STRENGTHBUYTIMES] = 1;
		s_selfNotifyProp[PROP_ACTOR_VIPEXP] = 1;
		
		s_selfNotifyProp[PROP_ACTOR_LEGION_CONTRIBUTE] = 1;
		s_selfNotifyProp[PROP_ACTOR_SHOWFIRSTPAY] = 1;
		
		s_bSelfPropInited = true;
	}

	return true;
}

bool PropObserveSystem::createComplete()
{
	registerActorFinish();
	
	return true;
}



void PropObserveSystem::onEventPropChg(EventArgs& args)
{
	if(!isMasterCreateFinish() )
	{
		return;
	}

	EventArgs_PropChange& propChgArgs = (EventArgs_PropChange&)args;
	if(s_selfNotifyProp[propChgArgs.iPropID] != 1)
	{
		return;
	}

	// 通知变化
	GSProto::Cmd_Sc_AttChg scMsg;
	scMsg.set_dwobjectid(m_hEntity);
	
	GSProto::PropItem* pScPropItem = scMsg.mutable_szchglifeatt()->Add();

	int iLifeAtt = Prop2LifeAtt(propChgArgs.iPropID);
	assert(iLifeAtt >= 0);
	pScPropItem->set_ilifeattid(iLifeAtt);
	pScPropItem->set_ivalue(propChgArgs.iValue);

	GSProto::SCMessage pkg;
	HelpMakeScMsg(pkg, GSProto::CMD_ATT_CHG, scMsg);

	// 发送
	IEntity* pSender = getEntityFromHandle(m_hEntity);
	assert(pSender);

	int iClassID = pSender->getProperty(PROP_ENTITY_CLASS, 0);
	if(GSProto::en_class_Actor != iClassID)
	{
		HEntity hMaster = pSender->getProperty(PROP_ENTITY_MASTER, 0);
		IEntity* pMaster = getEntityFromHandle(hMaster);
		assert(pMaster);

		int iMasterClass = pMaster->getProperty(PROP_ENTITY_CLASS, 0);
		assert(iMasterClass == GSProto::en_class_Actor);

		pSender = pMaster;
	}

	pSender->sendMessage(pkg);
}


bool PropObserveSystem::isMasterCreateFinish()
{
	IEntity* pEntity = getEntityFromHandle(m_hEntity);
	if(!pEntity) return false;

	int iClassID = pEntity->getProperty(PROP_ENTITY_CLASS, 0);
	if(iClassID == GSProto::en_class_Actor)
	{
		return pEntity->getProperty(PROP_ACTOR_CREATEFINISH, 0) != 0;
	}
	else if(iClassID == GSProto::en_class_Hero)
	{
		HEntity hMaster = pEntity->getProperty(PROP_ENTITY_MASTER, 0);
		IEntity* pMaster = getEntityFromHandle(hMaster);
		if(!pMaster) return false;

		return pMaster->getProperty(PROP_ACTOR_CREATEFINISH, 0) != 0;
	}

	return false;
}


void PropObserveSystem::registerActorFinish()
{
	IEntity* pEntity = getEntityFromHandle(m_hEntity);
	if(!pEntity) return;

	int iClassID = pEntity->getProperty(PROP_ENTITY_CLASS, 0);
	if(iClassID == GSProto::en_class_Actor)
	{
		//pEntity->getEventServer()->subscribeEvent(EVENT_ENTITY_SENDACTOR_TOCLIENT, this, &PropObserveSystem::onActorCreateFinish);
		pEntity->getEventServer()->subscribeEvent(EVENT_ENTITY_PROPCHANGE, this, &PropObserveSystem::onEventPropChg);
	}
	else if(iClassID == GSProto::en_class_Hero)
	{
		HEntity hMaster = pEntity->getProperty(PROP_ENTITY_MASTER, 0);
		IEntity* pMaster = getEntityFromHandle(hMaster);
		if(pMaster)
		{
			// 如果Master不是actor,不处理
			int iMasterClassID = pMaster->getProperty(PROP_ENTITY_CLASS, 0);
			if(iMasterClassID == GSProto::en_class_Actor)
			{
				//pMaster->getEventServer()->subscribeEvent(EVENT_ENTITY_SENDACTOR_TOCLIENT, this, &PropObserveSystem::onActorCreateFinish);
				pEntity->getEventServer()->subscribeEvent(EVENT_ENTITY_PROPCHANGE, this, &PropObserveSystem::onEventPropChg);
			}
		}
	}
}

void PropObserveSystem::unRegisterActorFinish()
{
	IEntity* pEntity = getEntityFromHandle(m_hEntity);
	if(!pEntity) return;

	int iClassID = pEntity->getProperty(PROP_ENTITY_CLASS, 0);
	if(iClassID == GSProto::en_class_Actor)
	{
		//pEntity->getEventServer()->unsubscribeEvent(EVENT_ENTITY_SENDACTOR_TOCLIENT, this, &PropObserveSystem::onActorCreateFinish);
		pEntity->getEventServer()->unsubscribeEvent(EVENT_ENTITY_PROPCHANGE, this, &PropObserveSystem::onEventPropChg);
	}
	else if(iClassID == GSProto::en_class_Hero)
	{
		HEntity hMaster = pEntity->getProperty(PROP_ENTITY_MASTER, 0);
		IEntity* pMaster = getEntityFromHandle(hMaster);
		if(pMaster)
		{
			// 如果Master不是actor,不处理
			int iMasterClassID = pMaster->getProperty(PROP_ENTITY_CLASS, 0);
			if(iMasterClassID == GSProto::en_class_Actor)
			{
				//pMaster->getEventServer()->unsubscribeEvent(EVENT_ENTITY_SENDACTOR_TOCLIENT, this, &PropObserveSystem::onActorCreateFinish);
				pEntity->getEventServer()->unsubscribeEvent(EVENT_ENTITY_PROPCHANGE, this, &PropObserveSystem::onEventPropChg);
			}
		}
	}
}


const std::vector<Uint32>& PropObserveSystem::getSupportMessage()
{
	static std::vector<Uint32> resultList;

	return resultList;
}


