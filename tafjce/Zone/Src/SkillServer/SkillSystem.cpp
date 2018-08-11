#include "SkillServerPch.h"
#include "Skill.h"
#include "SkillSystem.h"
//#include "IMagicGhostSystem.h"

extern "C" IObject* createSkillSystem()
{
	return new SkillSystem;
}

extern bool isGameServer();

SkillSystem::SkillSystem():m_iCurrentRound(-1)
{
}


SkillSystem::~SkillSystem()
{
	for(MapSkill::iterator it = m_skillMap.begin(); it != m_skillMap.end(); it++)
	{
		ISkill* pSkill = *it;
		delete pSkill;
	}

	for(MapSkill::iterator it = m_tempSkillMap.begin(); it != m_tempSkillMap.end(); it++)
	{
		ISkill* pSkill = *it;
		delete pSkill;
	}

	IEntity* pEntity = getEntityFromHandle(m_hEntity);
	if(pEntity)
	{
		pEntity->getEventServer()->unsubscribeEvent(EVENT_ENTITY_PREBEGIN_FIGHT, this, &SkillSystem::onEventPreBeginFight);
		pEntity->getEventServer()->unsubscribeEvent(EVENT_ENTITY_LEVELSTEPCHG, this, &SkillSystem::onEventLevelStepChg);
	}
}

Uint32 SkillSystem::getSubsystemID() const
{
	return IID_ISkillSystem;
}

Uint32 SkillSystem::getMasterHandle()
{
	return m_hEntity;
}

bool SkillSystem::create(IEntity* pEntity, const std::string& strData)
{
	PROFILE_MONITOR("SkillSystem::create");
	m_hEntity = pEntity->getHandle();
	
	return true;
}

bool SkillSystem::initSkillList(const vector<int>& skillIDList)
{
	ISkillFactory* pSkillFactory = getComponent<ISkillFactory>(COMPNAME_SkillFactory, IID_ISkillFactory);
	assert(pSkillFactory);

	for(size_t i = 0; i < skillIDList.size(); i++)
	{
		int iTmpSkillID = skillIDList[i];
		const ISkill* pSkillProto = pSkillFactory->getSkillPrototype(iTmpSkillID);
		assert(pSkillProto);

		ISkill* pNewSkill = pSkillProto->clone();
		assert(pNewSkill);

		bool bResult = pNewSkill->init(this);
		assert(bResult);

		m_skillMap.push_back(pNewSkill);
		//m_skillMap[iTmpSkillID] = pNewSkill;
	}
	
	return true;
}

bool SkillSystem::initHeroAndGoldAnimalSkill()
{
	//ServerEngine::SkillSystemSaveData tmpData;
	//ServerEngine::JceToObj(strData, tmpData);
	IEntity* pEntity = getEntityFromHandle(m_hEntity);
	assert(pEntity);

	int iBaseID = pEntity->getProperty(PROP_ENTITY_BASEID, 0);
	int iClassID = pEntity->getProperty(PROP_ENTITY_CLASS, 0);

	string strTbName;
	if(GSProto::en_class_Hero == iClassID)
	{
		strTbName = TABLENAME_Hero;
	}
	else if(GSProto::en_class_GodAnimal == iClassID)
	{
		strTbName = TABLENAME_GodAnimal;
	}
	else
	{
		assert(false);
		return false;
	}

	ITable* pHeroTb = getCompomentObjectManager()->findTable(strTbName.c_str() );
	assert(pHeroTb);

	int iRecord = pHeroTb->findRecord(iBaseID);
	assert(iRecord >= 0);

	string strSkillList = pHeroTb->getString(iRecord, "技能列表");
	vector<int> skillIDList = TC_Common::sepstr<int>(strSkillList, "#");
	
	bool bResult = initSkillList(skillIDList);

	return bResult;
}

bool SkillSystem::initMonsterSkill()
{
	IEntity* pEntity = getEntityFromHandle(m_hEntity);
	assert(pEntity);

	int iBaseID = pEntity->getProperty(PROP_ENTITY_BASEID, 0);

	ITable* pMonsterTb = getCompomentObjectManager()->findTable("Monster");
	assert(pMonsterTb);

	int iRecord = pMonsterTb->findRecord(iBaseID);
	assert(iRecord >= 0);

	vector<int> skillIDList;

	int iCommAttSkillID = pMonsterTb->getInt(iRecord, "普攻技能");
	assert(iCommAttSkillID > 0);
	skillIDList.push_back(iCommAttSkillID);

	int iTallentSkillID = pMonsterTb->getInt(iRecord, "天赋技能");
	if(iTallentSkillID > 0)
	{
		skillIDList.push_back(iTallentSkillID);
	}

	int iActiveSkillID = pMonsterTb->getInt(iRecord, "主动技能");
	if(iActiveSkillID)
	{
		skillIDList.push_back(iActiveSkillID);
	}

	bool bResult = initSkillList(skillIDList);

	return bResult;
}


void SkillSystem::getTallentSkillList(vector<int>& skillList)
{
	for(MapSkill::iterator it = m_skillMap.begin(); it != m_skillMap.end(); it++)
	{
		const ISkill* pSkill = *it;
		if(pSkill && (en_SkillType_Talent == pSkill->getSkillType() ) )
		{
			skillList.push_back(pSkill->getSkillID() );
		}
	}
}


bool SkillSystem::createComplete()
{
	PROFILE_MONITOR("SkillSystem::createComplete");

	IEntity* pEntity = getEntityFromHandle(m_hEntity);
	assert(pEntity);

	pEntity->getEventServer()->subscribeEvent(EVENT_ENTITY_PREBEGIN_FIGHT, this, &SkillSystem::onEventPreBeginFight);
	pEntity->getEventServer()->subscribeEvent(EVENT_ENTITY_LEVELSTEPCHG, this, &SkillSystem::onEventLevelStepChg);

	int iClassID = pEntity->getProperty(PROP_ENTITY_CLASS, 0);

	vector<int> skillIDList;
	if(GSProto::en_class_Monster == iClassID)
	{
		return initMonsterSkill();	
	}
	else if( (iClassID == GSProto::en_class_Hero) || (iClassID == GSProto::en_class_GodAnimal) )
	{
		return initHeroAndGoldAnimalSkill();
	} 
	
	return true;
}

void SkillSystem::onEventLevelStepChg(EventArgs& args)
{
	int iOldLevelStep = args.context.getInt("oldlevelstep");
	int iNewLevelStep = args.context.getInt("newlevelstep");
	// 通知被动技能，如果刚好开启,使用, 如果降阶,取消效果
	IEntity* pEntity = getEntityFromHandle(m_hEntity);
	assert(pEntity);

	for(MapSkill::iterator it = m_skillMap.begin(); it != m_skillMap.end(); it++)
	{
		Skill* pSkill = static_cast<Skill*>(*it);
		assert(pSkill);

		pSkill->onLevelStepChg(iOldLevelStep, iNewLevelStep);
	}

}


const std::vector<Uint32>& SkillSystem::getSupportMessage()
{
	static std::vector<Uint32> result;


	return result;
}

void SkillSystem::onMessage(QxMessage* pMessage)
{	
}


void SkillSystem::onEventPreBeginFight(EventArgs& args)
{
	// 初始化怒气
	IEntity* pEntity = getEntityFromHandle(m_hEntity);
	assert(pEntity);

	// 如果是在多轮战斗中，不重置
	//int iLoopCount = args.context.getInt("loopcount", 0);
	//if(0 != iLoopCount) return;
	/*if(0 == iLoopCount)
	{
		int iInitAnger = pEntity->getProperty(PROP_ENTITY_INITANGER, 0);
		pEntity->setProperty(PROP_ENTITY_ANGER, iInitAnger);
	}*/

	// 神兽怒气特殊处理
	int iClassID = pEntity->getProperty(PROP_ENTITY_CLASS, 0);
	if(GSProto::en_class_GodAnimal == iClassID)
	{
		const vector<HEntity>* pMemmberList = (const vector<HEntity>*)args.context.getInt64("memberlist", 0);
		assert(pMemmberList);

		for(size_t i = 0; i < pMemmberList->size(); i++)
		{
			HEntity hTmp = (*pMemmberList)[i];
			IEntity* pTmp = getEntityFromHandle(hTmp);
			if(!pTmp) continue;

			pTmp->getEventServer()->unsubscribeEvent(EVENT_ENTITY_USESKILL, this, &SkillSystem::onMemberUseSkill);
			pTmp->getEventServer()->subscribeEvent(EVENT_ENTITY_USESKILL, this, &SkillSystem::onMemberUseSkill);
		}
	}
}

void SkillSystem::onMemberUseSkill(EventArgs& args)
{
	// 反击不加
	bool bBackAttack = args.context.getInt("isBackAttack");
	if(bBackAttack) return;

	HEntity hUserEntity = args.context.getInt("entity");
	IEntity* pUserEntity = getEntityFromHandle(hUserEntity);
	assert(pUserEntity);


	IGlobalCfg* pGlobalCfg = getComponent<IGlobalCfg>(COMPNAME_GlobalCfg, IID_IGlobalCfg);
	assert(pGlobalCfg);

	int iSkillType = args.context.getInt("SkillType");
	
	IEntity* pEntity = getEntityFromHandle(m_hEntity);
	assert(pEntity);

	int iSelfPos = pEntity->getProperty(PROP_ENTITY_BATTLEPOS, 0);
	int iUserPos = pUserEntity->getProperty(PROP_ENTITY_BATTLEPOS, 0);
	
	int iAddValue = 0;
	if( (en_SkillType_ComAttack == iSkillType) || (en_SkillType_SmallActive == iSkillType) )
	{
		iAddValue = pGlobalCfg->getInt("神兽怒气增加值", 5);
	}
	else if(en_SkillType_Active == iSkillType)
	{
		// 只有我方英雄释放技能才加
		if(GSProto::en_class_Hero != pUserEntity->getProperty(PROP_ENTITY_CLASS, 0) )
		{
			return;
		}
	
		if(iUserPos / (GSProto::MAX_BATTLE_MEMBER_SIZE/2) == (iSelfPos - GSProto::MAX_BATTLE_MEMBER_SIZE) )
		{
			iAddValue = pGlobalCfg->getInt("神兽绝技怒气增加值", 25);
		}
	}

	if(0 == iAddValue) return;

	pEntity->changeProperty(PROP_ENTITY_ANGER, iAddValue, 0);

	int iMaxAngerValue = pGlobalCfg->getInt("最大怒气", 100);
	if(pEntity->getProperty(PROP_ENTITY_ANGER, 0) > iMaxAngerValue)
	{
		pEntity->setProperty(PROP_ENTITY_ANGER, iMaxAngerValue);
	}

	int iResultAnger = pEntity->getProperty(PROP_ENTITY_ANGER, 0);
	FIGHTLOG<<"GodAnimal|AddAnger|"<<iSelfPos<<"|"<<iAddValue<<"|"<<iResultAnger<<endl;
}

int SkillSystem::getSelectedActiveSkill()
{
	int iResultSkillID = _getTopSkillByType(en_SkillType_Active, true);
	
	return iResultSkillID;
}

int SkillSystem::_getTopSkillByType(int iSkillType, bool bActive)
{
	// 选择开启品阶最高的
	Skill* pTopSkill = NULL;
	for(MapSkill::iterator it = m_skillMap.begin(); it != m_skillMap.end(); it++)
	{
		Skill* pTmpSkill = static_cast<Skill*>(*it);
		assert(pTmpSkill);

		if(pTmpSkill->getSkillType() != iSkillType)
		{
			continue;
		}

		if(bActive && !pTmpSkill->isActive() )
		{
			continue;
		}

		if(!pTopSkill || (pTmpSkill->m_iActiveLevelStep > pTopSkill->m_iActiveLevelStep) )
		{
			pTopSkill = pTmpSkill;
		}
	}

	if(!pTopSkill) return 0;

	return pTopSkill->getSkillID();
}


int SkillSystem::getStageSkill()
{
	int iResultSkillID = _getTopSkillByType(en_SkillType_Stage, true);
	
	return iResultSkillID;
}

void SkillSystem::packSaveData(string& data)
{
	// 和策划确认，技能无需存档了，直接从配置读取
	
	/*ServerEngine::SkillSystemSaveData tmpSaveData;

	for(MapSkill::iterator it = m_skillMap.begin(); it != m_skillMap.end(); it++)
	{
		ISkill* pTmpSkill = *it;
		assert(pTmpSkill);
		tmpSaveData.skillList.push_back(pTmpSkill->getSkillID() );
	}

	data = ServerEngine::JceToStr(tmpSaveData);*/
}

ISkill* SkillSystem::getSkill(int iSkillID)
{
	for(MapSkill::iterator it = m_skillMap.begin(); it != m_skillMap.end(); it++)
	{
		ISkill* pTmpSkill = *it;
		assert(pTmpSkill);

		if(pTmpSkill->getSkillID() == iSkillID)
		{
			return pTmpSkill;
		}
	}

	return NULL;
}

ISkill* SkillSystem::selectSkill(const vector<HEntity>& memberList)
{
	// 眩晕，直接返回
	IEntity* pEntity = getEntityFromHandle(m_hEntity);
	assert(pEntity);
	
	bool bDizz = pEntity->getProperty(PROP_ENTITY_DIZZ, 0) != 0;
	if(bDizz) return NULL;

	// 混乱之后，只能用普攻
	bool bConfuse = pEntity->getProperty(PROP_ENTITY_CONFUSE, 0) != 0;
	if(bConfuse)
	{
		vector<int> tmpSkillList = getSkillListByType(en_SkillType_ComAttack, true);
		assert(tmpSkillList.size() == 1);
		int iCommSkillID = tmpSkillList[0];
		ISkill* pCommSkill = getSkill(iCommSkillID);
		assert(pCommSkill);

		return pCommSkill;
	}
	
	// 看看选中的当前主动技能能否使用
	int iActiveSkillID = getSelectedActiveSkill();

	ISkill* pActiveSkill = getSkill(iActiveSkillID);
	assert(pActiveSkill);

	if(pActiveSkill->canUseSkill(memberList) )
	{
		return pActiveSkill;
	}

	// 神兽只有主动技能和登场技能，无普攻技能
	int iClassID = pEntity->getProperty(PROP_ENTITY_CLASS, 0);
	if(GSProto::en_class_GodAnimal == iClassID)
	{
		return NULL;
	}

	// 有概率使用小技能?
	vector<int> smallSkillList = getSkillListByType(en_SkillType_SmallActive, true);
	if(smallSkillList.size() > 0)
	{
		int iSmallSkillID = smallSkillList[0];

		ISkillFactory* pSkillFactory = getComponent<ISkillFactory>(COMPNAME_SkillFactory, IID_ISkillFactory);
		assert(pSkillFactory);
		
		Skill* pSmallSkill = (Skill*)(pSkillFactory->getSkillPrototype(iSmallSkillID) );
		assert(pSmallSkill);

		IRandom* pRandom = getComponent<IRandom>("Random", IID_IMiniAprRandom);
		assert(pRandom);

		int iRandV = pRandom->random() % 10000;
		if(iRandV < pSmallSkill->m_iSmallSkillChance)
		{
			return getSkill(iSmallSkillID);
		}
	}

	vector<int> tmpSkillList = getSkillListByType(en_SkillType_ComAttack, true);
	assert(tmpSkillList.size() == 1);
	
	int iCommSkillID = tmpSkillList[0];

	ISkill* pCommSkill = getSkill(iCommSkillID);
	assert(pCommSkill);

	return pCommSkill;
}

bool SkillSystem::useSkill(const vector<HEntity>& memberList)
{
	assert(memberList.size() == GSProto::MAX_BATTLE_MEMBER_SIZE);

	ISkill* pSkill = selectSkill(memberList);
	if(!pSkill) return false;
	
	EffectContext preContext;
	bool bResult = pSkill->useSkill(memberList, preContext);

	return bResult;
}

void SkillSystem::backAttack(HEntity hTarget)
{
	vector<int> skillList = getSkillListByType(en_SkillType_ComAttack, true);
	assert(skillList.size() == 1);

	int iSkillID = skillList[0];

	ISkill* pSkill = getSkill(iSkillID);
	assert(pSkill);

	EffectContext preContext;
	preContext.setInt(PROP_EFFECT_FIXTARGET, hTarget);
	preContext.setInt(PROP_EFFECT_ISBACKATTACK, 1);
	preContext.setInt(PROP_EFFECT_FIXHITRESULT, GSProto::en_SkillHitResult_Hit);
	

	vector<HEntity> emptyList;
	pSkill->useSkill(emptyList, preContext);
}

vector<int> SkillSystem::getSkillListByType(int iSkillType, bool bOnlyActive)
{
	ISkillFactory* pSkillFactory = getComponent<ISkillFactory>("SkillFactory", IID_ISkillFactory);
	assert(pSkillFactory);

	vector<int> resultList;
	for(MapSkill::iterator it = m_skillMap.begin(); it != m_skillMap.end(); it++)
	{
		Skill* pTmpSkill = static_cast<Skill*>(*it);
		assert(pTmpSkill);
		
		int iSkillID = pTmpSkill->getSkillID();	
		
		if(bOnlyActive && !pTmpSkill->isActive() )
		{
			continue;
		}
		
		if(pTmpSkill->getSkillType() != iSkillType)
		{
			continue;
		}
		
		resultList.push_back(iSkillID);
	}
	
	return resultList;
}

bool SkillSystem::addTempSkill(int iSkillID)
{
	ISkillFactory* pSkillFactory = getComponent<ISkillFactory>("SkillFactory", IID_ISkillFactory);
	assert(pSkillFactory);

	/*MapSkill::iterator it = m_tempSkillMap.find(iSkillID);
	if(it != m_tempSkillMap.end() )
	{
		return true;
	}*/

	for(MapSkill::iterator it = m_tempSkillMap.begin(); it != m_tempSkillMap.end(); it++)
	{
		ISkill* pTmpSkill = *it;
		if(pTmpSkill->getSkillID() == iSkillID)
		{
			return true;
		}
	}
	
	const ISkill* pSkillProto = pSkillFactory->getSkillPrototype(iSkillID);
	if(!pSkillProto)
	{
		return false;
	}

	ISkill* pNewSkill = pSkillProto->clone();
	assert(pNewSkill);

	pNewSkill->init(this);
	m_tempSkillMap.push_back(pNewSkill);
	//std::pair<MapSkill::iterator, bool> result = m_tempSkillMap.insert(std::make_pair(iSkillID, pNewSkill) );
	//assert(result.second);
	//it = result.first;

    return true;
}

bool SkillSystem::useTempSkill(int iSkillID, const vector<HEntity>& memberList, const EffectContext& preContext)
{
	ISkillFactory* pSkillFactory = getComponent<ISkillFactory>("SkillFactory", IID_ISkillFactory);
	assert(pSkillFactory);

	ISkill* pUseSkill = NULL;
	for(MapSkill::iterator it = m_tempSkillMap.begin(); it != m_tempSkillMap.end(); it++)
	{
		ISkill* pTmpSkill = *it;
		if(pTmpSkill->getSkillID() == iSkillID)
		{
			pUseSkill = pTmpSkill;
			break;
		}
	}

	assert(pUseSkill);

	if(!pUseSkill->canUseSkill(memberList) )
	{
		return false;
	}

	pUseSkill->useSkill(memberList, preContext);

	return true;
}

vector<ISkill*> SkillSystem::getSkillList(bool bOnlyActive)
{
	return m_skillMap;
}



