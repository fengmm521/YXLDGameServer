#include "FightServerPch.h"
#include "FightFactory.h"
#include "FightSystem.h"
#include "IFormationSystem.h"
#include "OutPrxImp.h"
#include "IGodAnimalSystem.h"
#include "Fight.h"
#include "IJZMessageLayer.h"

extern "C" IComponent* createFightFactory(Int32)
{
	return new FightFactory;
}

extern int Prop2LifeAtt(int iPropID);
extern void onSkillEffectChangeHPProcess(HEntity hGiver, HEntity hEntity, int iChgValue, const EffectContext& ctx);


FightFactory::FightFactory():m_pCurPVEFightCtx(NULL), m_bContinue(false), m_pContinueDataAll(NULL), 
	m_pContinueBattleData(NULL), m_iRound(-1)
{
}

FightFactory::~FightFactory()
{
}


BattleWholeDataGuard::BattleWholeDataGuard(GSProto::FightDataAll* pFightDataAll)
{
	FightFactory* pFightFactory = static_cast<FightFactory*>(getComponent<IFightFactory>(COMPNAME_FightFactory, IID_IFightFactory) );
	assert(pFightFactory);

	pFightFactory->m_pContinueDataAll = pFightDataAll;
}

BattleWholeDataGuard::~BattleWholeDataGuard()
{
	FightFactory* pFightFactory = static_cast<FightFactory*>(getComponent<IFightFactory>(COMPNAME_FightFactory, IID_IFightFactory) );
	assert(pFightFactory);

	pFightFactory->m_pContinueDataAll = NULL;
}



void FightFactory::queryBattleRecord(const ServerEngine::PKFight& fightKey, DelegateQueryFight cb)
{
	IMessageLayer* pMsgLayer = getComponent<IMessageLayer>(COMPNAME_MessageLayer, IID_IMessageLayer);
	assert(pMsgLayer);

	pMsgLayer->AsyncGetFightRecord(fightKey, new FightRecordQueryCb(fightKey, cb) );
}

void FightFactory::saveBattleRecord(const ServerEngine::BattleData& data)
{
	IMessageLayer* pMsgLayer = getComponent<IMessageLayer>(COMPNAME_MessageLayer, IID_IMessageLayer);
	assert(pMsgLayer);

	pMsgLayer->AsyncSaveFightRecord(data, new FightRecordSaveCb);
}

string FightFactory::makeFightStringKey(const ServerEngine::PKFight& key)
{
	uuid_t itemuuid;
	memcpy(itemuuid, &(key.u64LowUUID), sizeof(key.u64LowUUID) );
	memcpy( (char*)itemuuid+8, &(key.u64HighUUID), sizeof(key.u64HighUUID) );

	char szUUIDString[128] = {0};
	uuid_unparse(itemuuid, szUUIDString);
	char szResultData[128] = {0};
	snprintf(szResultData, sizeof(szResultData), "%s*%d", szUUIDString, key.iWorldID);

	return szResultData;
}

bool FightFactory::decodeFightStringKey(const std::string& strKey, ServerEngine::PKFight& key)
{
	size_t sPos = strKey.find("*");
	if(sPos == string::npos)
	{
		return false;
	}

	string strUUID = strKey.substr(0, sPos);
	string strWorldID = strKey.substr(sPos+1);

	uuid_t itemuuid;
	uuid_parse(strUUID.c_str(), itemuuid);
	int iWorldID = AdvanceAtoi(strWorldID);

	memcpy(&key.u64LowUUID, itemuuid, sizeof(key.u64LowUUID) );
	memcpy(&key.u64HighUUID, ((char*)itemuuid)+8, sizeof(key.u64HighUUID) );
	key.iWorldID = iWorldID;

	return true;
}


void FightFactory::generateFightKey(int iWorldID, ServerEngine::BattleData& data)
{
	IGlobalCfg* pGlobCfg = getComponent<IGlobalCfg>("GlobalCfg", IID_IGlobalCfg);
	assert(pGlobCfg);

	uuid_t itemuuid;
	uuid_generate(itemuuid);

	assert(sizeof(itemuuid) == 16);
	memcpy(&data.FightKey.u64LowUUID, itemuuid, sizeof(data.FightKey.u64LowUUID) );
	memcpy(&data.FightKey.u64HighUUID, ((char*)itemuuid)+8, sizeof(data.FightKey.u64HighUUID) );
	data.FightKey.iWorldID = iWorldID;
}


int FightFactory::continueMultiPVEFight(ServerEngine::BattleData& battleInfo, const ServerEngine::ActorCreateContext& roleInfo, vector<int> monsterGrpList, const ServerEngine::FightContext& ctx)
{
	BattleRandSetGuard randValueListGuard(ctx.randValueList);

	m_bContinue = true;
	m_pContinueBattleData = &battleInfo;
	
	int iResult = doMultiPVEFight(battleInfo, roleInfo, monsterGrpList, ctx);
	m_pContinueBattleData = NULL;
	m_bContinue = false;

	return iResult;
}


void FightFactory::fillReplaceAction(GSProto::FightDataAll& data, const vector<HEntity>& memberList, bool bLastFight)
{	
	GSProto::FightAction* pNewAction = data.mutable_szfightaction()->Add();
	pNewAction->set_iactiontype(GSProto::en_FightAction_ReplaceMember);
	
	GSProto::ReplaceMemberAction* pReplaceMemberAction = pNewAction->mutable_replacememberaction();
	pReplaceMemberAction->set_battackerreplace(false);
	pReplaceMemberAction->set_bfinalfight(bLastFight);
	
	for(int i = 0; i< (int)memberList.size(); i++)
	{
		if(isTargetPos(i) && getEntityFromHandle(memberList[i]))
		{
			fillSingleFightObj(memberList[i], *(pReplaceMemberAction->add_sznewfightobject() ) );
		}
	}

	triggerBeginResultSet();
}


bool FightFactory::initlize(const PropertySet& propSet)
{
	ITable* pTable = getCompomentObjectManager()->findTable("ActorSkill");
	assert(pTable);

	for(int i = 0; i < pTable->getRecordCount(); i++)
	{
		ActorSkillCfg tmpSkillCfg;
		tmpSkillCfg.iActorSkillID = pTable->getInt(i, "技能ID");
		tmpSkillCfg.iCost = pTable->getInt(i, "消耗能量");

		string strEffect = pTable->getString(i, "效果");
		tmpSkillCfg.effectIDList = TC_Common::sepstr<int>(strEffect, "#");
		tmpSkillCfg.iCondLevel = pTable->getInt(i, "开启等级");

		m_actorSkillMap[tmpSkillCfg.iActorSkillID] = tmpSkillCfg;
	}

	return true;
}


void FightFactory::appendDlgList(GSProto::FightDataAll& fightAllData, const vector<int>& dlgList)
{
	for(size_t i = 0; i < dlgList.size(); i++)
	{
		int iIndex = fightAllData.szfightaction_size();

		GSProto::FightAction* pNewAction = fightAllData.add_szfightaction();
		pNewAction->set_iactiontype(GSProto::en_FightAction_Dlg);
		pNewAction->set_iactionindex(iIndex);

		GSProto::DialogAction* pDlgAction = pNewAction->mutable_dlgaction();
		pDlgAction->set_idlgid(dlgList[i]);
	}
}


int FightFactory::doMultiPVEFight(ServerEngine::BattleData& battleInfo, const ServerEngine::ActorCreateContext& roleInfo, vector<int> monsterGrpList, const ServerEngine::FightContext& ctx)
{
	PROFILE_MONITOR("doMultiPVEFight");

	BattleFightLogHelper guard_logHelp(string("doMultiPVEFight") + roleInfo.strAccount);

	// 设置随机数Cache
	BattleRandomCache randCache(battleInfo);

	BattleFightCtxGuard ctxGuard( (ServerEngine::FightContext&)ctx);

	IJZEntityFactory* pEntityFactory = getComponent<IJZEntityFactory>(COMPNAME_EntityFactory, IID_IJZEntityFactory);
	assert(pEntityFactory);

	FDLOG("Rand")<<"Start Create Ghost"<<endl;
	string strGhostCreateInfo = ServerEngine::JceToStr(roleInfo);
	IEntity* pGhost = pEntityFactory->getEntityFactory()->createEntity("Ghost", strGhostCreateInfo);
	if(!pGhost) return en_FightResult_SysError;

	int iFightValue = pGhost->getProperty(PROP_ENTITY_FIGHTVALUE, 0);
	pGhost->setProperty(PROP_ENTITY_FIGHTVALUE_PREFIGHT, iFightValue);

	FDLOG("Rand")<<"End Create Ghost"<<endl;

	int iWorldID = pGhost->getProperty(PROP_ACTOR_WORLD, 0);

	vector<HEntity> createdList;
	vector<int> fightList = monsterGrpList;
	int iLoopCount = 0;
	GSProto::FightDataAll orgData;

	IGlobalCfg* pGlobalCfg = getComponent<IGlobalCfg>(COMPNAME_GlobalCfg, IID_IGlobalCfg);
	assert(pGlobalCfg);
	int iInitItemCount = pGlobalCfg->getInt("战斗初始药物数量", 1);
	if(ctx.iFightType == GSProto::en_FightType_FirstFight)
	{
		iInitItemCount = 3;
	}
	if(ctx.iFightType == GSProto::en_FightType_DreamLand)
	{
		ServerEngine::DreamLandFightCtx dreamLandCtx;
		assert(ctx.context.find("DreamLandFightCtx") != ctx.context.end() );
		ServerEngine::JceToObj(ctx.context.at("DreamLandFightCtx"), dreamLandCtx);
		iInitItemCount = dreamLandCtx.iItemCount;

		
	}
	
	orgData.set_icuritemcount(iInitItemCount);
	orgData.set_iinititemcount(iInitItemCount);

	BattleWholeDataGuard wholeDataGuard(&orgData);

	if(monsterGrpList.size() == 0)
	{
		orgData.set_bonlyonefight(true);
	}
	else
	{
		orgData.set_bonlyonefight(false);
	}

	// 先添加开场对话
	if(ctx.bBeginDlg && (ctx.dlgList.size() > 0) )
	{
		appendDlgList(orgData, ctx.dlgList);
	}
	
	while(fightList.size() > 0)
	{
		int iMonsterGrpID = fightList.front();
		fightList.erase(fightList.begin() );

		vector<HEntity> monsterList = pEntityFactory->createMonsterGrp(iMonsterGrpID);		
		vector<HEntity> memberList;
		memberList.resize(GSProto::MAX_BATTLE_MEMBER_SIZE, 0);

		FDLOG("Rand")<<"Start Fill Member"<<endl;
		fillGhost(memberList, pGhost->getHandle(), true);

		FDLOG("Rand")<<"Start Fill Monster"<<endl;
		fillMonsterGrp(memberList, monsterList);

		std::copy(memberList.begin(), memberList.end(), std::inserter(createdList, createdList.end() ) );

		vector<HEntity> externList;
		fillExternList(pGhost->getHandle(), true, externList);

		// 填充新上场的怪物Action
		if(iLoopCount > 0)
		{
			fillReplaceAction(orgData, memberList, (fightList.size() == 0) );
			if( (fightList.size() == 0) && !ctx.bBeginDlg) appendDlgList(orgData, ctx.dlgList);
		}
		
		//ServerEngine::BattleData tmpBattleInfo;
		int iResult = _doCommonFight(memberList, battleInfo, ctx, externList, iWorldID, iLoopCount, orgData);
		
		iLoopCount++;
		if(iResult != en_FightResult_OK)
		{
			FDLOG("Fight")<<orgData.DebugString()<<endl;
			cleanFightMemList(createdList);
			return iResult;
		}

		// 如果攻击方失败了，直接返回
		if(!battleInfo.bAttackerWin)
		{
			FDLOG("Fight")<<orgData.DebugString()<<endl;
			cleanFightMemList(createdList);
			return iResult;
		}
	}
	
	cleanFightMemList(createdList);

	FDLOG("Fight")<<orgData.DebugString()<<endl;

	return en_FightResult_OK;
}


bool FightFactory::isTargetPos(int iPos)
{
	if( (iPos >= GSProto::MAX_BATTLE_MEMBER_SIZE/2) && (iPos < GSProto::MAX_BATTLE_MEMBER_SIZE) )
	{
		return true;
	}

	if(iPos == GSProto::TARGET_GOLDANIMAL_POS) return true;

	return false;
}


BattleFightCtxGuard::BattleFightCtxGuard(ServerEngine::FightContext& ctx)
{
	FightFactory* pFightFactory = static_cast<FightFactory*>(getComponent<IFightFactory>(COMPNAME_FightFactory, IID_IFightFactory) );
	assert(pFightFactory);

	pFightFactory->m_pCurPVEFightCtx = &ctx;
	
}

BattleFightCtxGuard::~BattleFightCtxGuard()
{
	FightFactory* pFightFactory = static_cast<FightFactory*>(getComponent<IFightFactory>(COMPNAME_FightFactory, IID_IFightFactory) );
	assert(pFightFactory);

	pFightFactory->m_pCurPVEFightCtx = NULL;
	
}


int FightFactory::doPVEFight(ServerEngine::BattleData& battleInfo, const ServerEngine::ActorCreateContext& roleInfo, int iMonsterGrpID, const ServerEngine::FightContext& ctx)
{
	PROFILE_MONITOR("doPVEFight");

	BattleFightLogHelper guard_logHelp(string("doPVEFight")+roleInfo.strAccount);

	// 设置随机数Cache
	BattleRandomCache randCache(battleInfo);

	BattleFightCtxGuard ctxGuard( (ServerEngine::FightContext&)ctx);
	
	IJZEntityFactory* pEntityFactory = getComponent<IJZEntityFactory>(COMPNAME_EntityFactory, IID_IJZEntityFactory);
	assert(pEntityFactory);

	FDLOG("Rand")<<"Start Create Ghost"<<endl;
	string strGhostCreateInfo = ServerEngine::JceToStr(roleInfo);
	IEntity* pGhost = pEntityFactory->getEntityFactory()->createEntity("Ghost", strGhostCreateInfo);
	if(!pGhost)
	{
		return en_FightResult_SysError;
	}

	int iFightValue = pGhost->getProperty(PROP_ENTITY_FIGHTVALUE, 0);
	pGhost->setProperty(PROP_ENTITY_FIGHTVALUE_PREFIGHT, iFightValue);

	FDLOG("Rand")<<"End Create Ghost"<<endl;

	int iWorldID = pGhost->getProperty(PROP_ACTOR_WORLD, 0);
	vector<HEntity> monsterList = pEntityFactory->createMonsterGrp(iMonsterGrpID);

	FDLOG("Rand")<<"Start Fill Member"<<endl;
	vector<HEntity> memberList;
	memberList.resize(GSProto::MAX_BATTLE_MEMBER_SIZE, 0);
	fillGhost(memberList, pGhost->getHandle(), true);

	FDLOG("Rand")<<"Start Fill Monster"<<endl;
	fillMonsterGrp(memberList, monsterList);

	// 神兽填充到externList,暂无;
	vector<HEntity> externList;
	fillExternList(pGhost->getHandle(), true, externList);
	GSProto::FightDataAll wholeData;

	IGlobalCfg* pGlobalCfg = getComponent<IGlobalCfg>(COMPNAME_GlobalCfg, IID_IGlobalCfg);
	assert(pGlobalCfg);

	int iInitItemCount = pGlobalCfg->getInt("战斗初始药物数量", 1);
	if(ctx.iFightType == GSProto::en_FightType_FirstFight)
	{
		iInitItemCount = 3;
	}
	BattleWholeDataGuard wholeDataGuard(&wholeData);
	wholeData.set_icuritemcount(iInitItemCount);
	wholeData.set_iinititemcount(iInitItemCount);

	FDLOG("Rand")<<"ReadyFight"<<endl;
	int iResult = _doCommonFight(memberList, battleInfo, ctx, externList, iWorldID, 0,  wholeData);
	

	cleanFightMemList(memberList);

	FDLOG("Fight")<<wholeData.DebugString()<<endl;

	return iResult;
}

BattleRandSetGuard::BattleRandSetGuard(const vector<taf::Int64>& valueList)
{
	IRandom* pRandom = getComponent<IRandom>("Random", IID_IMiniAprRandom);
	assert(pRandom);
	
	list<long int> cacheList;
	std::copy(valueList.begin(), valueList.end(), std::inserter(cacheList, cacheList.end() ) );
	pRandom->setRandomValue(cacheList);
}

BattleRandSetGuard::~BattleRandSetGuard()
{
	IRandom* pRandom = getComponent<IRandom>("Random", IID_IMiniAprRandom);
	assert(pRandom);

	pRandom->clearValueList();
}

int FightFactory::continuePVEFight(ServerEngine::BattleData& battleInfo, const ServerEngine::ActorCreateContext& roleInfo, int iMonsterGrpID, const ServerEngine::FightContext& ctx)
{
	BattleRandSetGuard randValueListGuard(ctx.randValueList);

	m_bContinue = true;
	m_pContinueBattleData = &battleInfo;
	int iResult = doPVEFight(battleInfo, roleInfo, iMonsterGrpID, ctx);
	m_pContinueBattleData = NULL;
	m_bContinue = false;

	return iResult;
}

int FightFactory::continuePVPFight(ServerEngine::BattleData& battleInfo, const ServerEngine::ActorCreateContext& roleInfo, const ServerEngine::ActorCreateContext& targetRileInfo, const ServerEngine::FightContext& ctx)
{
	BattleRandSetGuard randValueListGuard(ctx.randValueList);

	m_bContinue = true;
	m_pContinueBattleData = &battleInfo;

	int iResult = doPVPFight(battleInfo, roleInfo, targetRileInfo, ctx);

	m_pContinueBattleData = NULL;
	m_bContinue = false;

	return iResult;
}


bool FightFactory::canUseFightUseActorSkill(int iFightType)
{
	return (iFightType == GSProto::en_FightType_Scene) || (iFightType == GSProto::en_FightType_EquipScene) ||(iFightType==GSProto::en_FightType_ClimbTower) || (iFightType == GSProto::en_FightType_DreamLand);
}


void FightFactory::fillExternList(HEntity hGost, bool bAttacker, vector<HEntity>& externList)
{
	IEntity* pGhost = getEntityFromHandle(hGost);
	if(!pGhost) return;

	IGodAnimalSystem* pGodAnimalSys = static_cast<IGodAnimalSystem*>(pGhost->querySubsystem(IID_IGodAnimalSystem));
	assert(pGodAnimalSys);

	HEntity hActiveGodAnimal = pGodAnimalSys->getActiveGodAnimal();
	IEntity* pGodAnimal = getEntityFromHandle(hActiveGodAnimal);
	if(!pGodAnimal) return;

	externList.push_back(hActiveGodAnimal);
	if(bAttacker)
	{
		pGodAnimal->setProperty(PROP_ENTITY_BATTLEPOS, GSProto::ATT_GOLDANIMAL_POS);
	}
	else
	{
		pGodAnimal->setProperty(PROP_ENTITY_BATTLEPOS, GSProto::TARGET_GOLDANIMAL_POS);
	}
};


void FightFactory::fillGhost(vector<HEntity>& memberList, HEntity hGhost, bool bAttacker)
{
	IEntity* pGhost = getEntityFromHandle(hGhost);
	assert(pGhost);

	int iBeginPos = 0;
	if(!bAttacker) iBeginPos += GSProto::MAX_BATTLE_MEMBER_SIZE/2;

	IFormationSystem* pFormationSys = static_cast<IFormationSystem*>(pGhost->querySubsystem(IID_IFormationSystem));
	assert(pFormationSys);

	for(int i = 0; i < GSProto::MAX_BATTLE_MEMBER_SIZE/2; i++)
	{
		HEntity hTmpHero = pFormationSys->getEntityFromPos(i);
		if(0 == hTmpHero) continue;

		IEntity* pTmpHero = getEntityFromHandle(hTmpHero);
		assert(pTmpHero);

		pTmpHero->setProperty(PROP_ENTITY_BATTLEPOS, iBeginPos + i);
		memberList[iBeginPos + i] = hTmpHero;
	}
}

void FightFactory::fillMonsterGrp(vector<HEntity>& memberList, const vector<HEntity>& monsterList)
{
	for(size_t i = 0; i < monsterList.size(); i++)
	{
		if(0 == monsterList[i])
		{
			continue;
		}
		
		IEntity* pMonster = getEntityFromHandle(monsterList[i]);
		assert(pMonster);

		pMonster->setProperty(PROP_ENTITY_BATTLEPOS, (Int32)(GSProto::MAX_BATTLE_MEMBER_SIZE/2 + i) );
		memberList[GSProto::MAX_BATTLE_MEMBER_SIZE/2 + i] = monsterList[i];
	}
}

bool FightFactory::isAttackerFirstByFightValue(const vector<HEntity>& memberList)
{
	double dAttackSideValue = 0.0;
	double dTargetSideValue = 0.0;
	for(size_t i = 0; i < memberList.size(); i++)
	{
		HEntity hTmp = memberList[i];
		IEntity* pTmp = getEntityFromHandle(hTmp);
		if(!pTmp) continue;

		int iTmpFightValue = pTmp->getProperty(PROP_ENTITY_FIGHTVALUE, 0);

		if(i < GSProto::MAX_BATTLE_MEMBER_SIZE/2)
		{
			dAttackSideValue += iTmpFightValue;
		}
		else
		{
			dTargetSideValue += iTmpFightValue;
		}
	}

	return dAttackSideValue > dTargetSideValue;
}


bool FightFactory::isAttackerSideFirstMove(const ServerEngine::FightContext& ctx, const vector<HEntity>& memberList)
{
	if(ctx.iFightType == GSProto::en_FightType_CampBattleFactory)
	{
		return isAttackerFirstByFightValue(memberList);
	}

	return true;
}


void BattleRandomRcvCb::operator()(long int iValue)
{
	m_randomValueList.push_back(iValue);
	FDLOG("Rand")<<iValue<<endl;
}


BattleRandomCache::BattleRandomCache(ServerEngine::BattleData& data):m_battleData(data)
{
	IRandom* pRandom = getComponent<IRandom>("Random", IID_IMiniAprRandom);
	assert(pRandom);

	FDLOG("Rand")<<"Begin----"<<endl;
	pRandom->setRandRcv(BattleRandomRcvCb(m_battleData.randValueList) );
}

BattleRandomCache::~BattleRandomCache()
{
	IRandom* pRandom = getComponent<IRandom>("Random", IID_IMiniAprRandom);
	assert(pRandom);

	pRandom->clearRandRcv();

	FDLOG("Rand")<<"End----"<<endl;
}


void FightFactory::triggerBeginResultSet()
{
	// 统计下Action的Index,方便测试
	if(m_pContinueDataAll)
	{
		int iTmpIndex = m_pContinueDataAll->szfightaction_size() - 1;
		m_pContinueDataAll->mutable_szfightaction()->rbegin()->set_iactionindex(iTmpIndex);
	}
	
	if(!m_pCurPVEFightCtx) return;

	if(!m_pContinueDataAll) return;

	// 没药，返回
	if(m_pContinueDataAll->icuritemcount() == 0)
	{
		return;
	}

	// 找到Action索引
	int iActionIndex = m_pContinueDataAll->szfightaction_size();
	if(iActionIndex == 0) return;

	iActionIndex--;

	// 找到Result索引, Result和Action处理有差别，部分Action可能没有Result,这时认为是0
	GSProto::FightAction* pCurAction = &(*(m_pContinueDataAll->mutable_szfightaction()->rbegin()));
	int iResultIndex = pCurAction->szresultset_size();

	if(iResultIndex > 0) iResultIndex--;

	int iActorLevel = getLeftActorLevel(m_memberList);
	
	//vector<ServerEngine::FightUseItem>& refUseItemActionList = m_pCurPVEFightCtx->useItemActionList;

	// 过程中可能会导致迭代器删除，改为List处理
	std::list<ServerEngine::FightUseItem> backUpUseItemActionList;
	std::copy(m_pCurPVEFightCtx->useItemActionList.begin(), m_pCurPVEFightCtx->useItemActionList.end(), std::inserter(backUpUseItemActionList, backUpUseItemActionList.end() ) );
	for(list<ServerEngine::FightUseItem>::iterator it = backUpUseItemActionList.begin(); it != backUpUseItemActionList.end();)
	{
		if(m_pContinueDataAll->icuritemcount() == 0)
		{
			break;
		}
	
		ServerEngine::FightUseItem refUseItem = *it;
		
		const ActorSkillCfg* pActorSkillCfg = getActorSkillCfg(refUseItem.iActorSkillID);
		if(!pActorSkillCfg)
		{
			FDLOG("ActorSkillError")<<refUseItem.iActorSkillID<<endl;
			continue;
		}
		
		//if( (refUseItem.iActionIndex == iActionIndex) && (refUseItem.iResultsetIndex == iResultIndex) )
		if(isActionResultHit(iActionIndex, iResultIndex, refUseItem) )
		{
			backUpUseItemActionList.erase(it++);
			
			if( (m_pContinueDataAll->icuritemcount() >= pActorSkillCfg->iCost) && (iActorLevel >= pActorSkillCfg->iCondLevel) )
			{
				useActorSkill(refUseItem);
			}
			
			/*if(refUseItem.iTargetPos >= GSProto::MAX_BATTLE_MEMBER_SIZE/2)
			{
				continue;
			}*/

			FDLOG("UseItem")<<"TryUse|"<<iActionIndex<<"|"<<iResultIndex<<"|"<<refUseItem.iActionIndex<<"|"<<refUseItem.iResultsetIndex<<endl;

			//useFightItem(refUseItem);
		}
		else
		{
			it++;
		}
	}

	m_pCurPVEFightCtx->useItemActionList.clear();
	std::copy(backUpUseItemActionList.begin(), backUpUseItemActionList.end(), std::inserter(m_pCurPVEFightCtx->useItemActionList, m_pCurPVEFightCtx->useItemActionList.end() ) );
}

int FightFactory::getLeftActorLevel(const vector<HEntity>& memberList)
{
	for(int i = 0; i < GSProto::MAX_BATTLE_MEMBER_SIZE/2; i++)
	{
		IEntity* pEntity = getEntityFromHandle(memberList[i]);
		if(!pEntity) continue;

		int iClassID = pEntity->getProperty(PROP_ENTITY_CLASS, 0);
		if(GSProto::en_class_Hero == iClassID)
		{
			IEntity* pMaster = getEntityFromHandle(pEntity->getProperty(PROP_ENTITY_MASTER, 0) );
			assert(pMaster);

			int iLevel = pMaster->getProperty(PROP_ENTITY_LEVEL, 0);

			return iLevel;
		}
	}

	return 0;
}

const ActorSkillCfg* FightFactory::getActorSkillCfg(int iActorSkillID)
{
	map<int, ActorSkillCfg>::iterator it = m_actorSkillMap.find(iActorSkillID);
	if(it != m_actorSkillMap.end() )
	{
		return &(it->second);
	}

	return NULL;
}


bool FightFactory::isActionResultHit(int iActionIndex, int iResultsetIndex, const ServerEngine::FightUseItem& useItemCtx)
{
	if(0 == useItemCtx.iPost)
	{
		if( (iActionIndex == useItemCtx.iActionIndex) && (iResultsetIndex == useItemCtx.iResultsetIndex) )
		{
			return true;
		}
		else
		{
			return false;
		}
	}

	if(1 == useItemCtx.iPost)
	{
		// 已经是下一个ResultSet了OK
		if( (iActionIndex == useItemCtx.iActionIndex) && (iResultsetIndex == (useItemCtx.iResultsetIndex + 1) ) )
		{
			return true;
		}

		// 有可能ResultIndex是上一个Action最后一个, 那么，在这个Action的第一个ResultSet触发
		if(iActionIndex == (useItemCtx.iActionIndex + 1) )
		{
			const GSProto::FightAction& preAction = m_pContinueDataAll->szfightaction(useItemCtx.iActionIndex);
			if( (preAction.szresultset_size() == (useItemCtx.iResultsetIndex + 1) ) && (0 == iResultsetIndex) )
			{
				return true;
			}
		}
	}

	return false;
}

void FightFactory::fillAddBuffToAction(GSProto::FightAction* pAction, HEntity hTarget, EventArgs& args)
{
	IBuffFactory* pBuffFactory = getComponent<IBuffFactory>(COMPNAME_BuffFactory, IID_IBuffFactory);
	assert(pBuffFactory);

	int iBuffID = args.context.getInt("buffid", 0);
	bool bFilled = args.context.getInt("filled", 0) == 1;
	if(bFilled)
	{
		return;
	}

	args.context.setInt("filled", 1);

	const IEntityBuff* pBuffProto = pBuffFactory->getBuffPrototype(iBuffID);
	if(!pBuffProto) return;

	if(!pBuffProto->isShowClient() ) return;

	IEntity* pTarget = getEntityFromHandle(hTarget);
	assert(pTarget);
	
	int iTargetPos = pTarget->getProperty(PROP_ENTITY_BATTLEPOS, 0);

	google::protobuf::RepeatedPtrField<GSProto::FightResulSet>* pszResultSet = pAction->mutable_szresultset();
	assert(pszResultSet->size() > 0);

	GSProto::FightResulSet* pCurResultSet = &(*pszResultSet->rbegin() );

	// 新增一个Result
	GSProto::FightResult* pNewResult = pCurResultSet->mutable_szresult()->Add();
	assert(pNewResult);

	pNewResult->set_iresulttype(GSProto::en_FightResult_AddBuff);
	GSProto::FightAddBuffResult* pNewAddBuffResult = pNewResult->mutable_addbuffresult();
	pNewAddBuffResult->set_itargetpos(iTargetPos);
	pNewAddBuffResult->set_ibuffid(iBuffID);
}

void FightFactory::fillPropChgToAction(GSProto::FightAction* pAction, EventArgs_FightPropChg& fightPropChgArgs)
{
	// 除了HP和怒气,其他属性下降不通知
	if(fightPropChgArgs.iChgValue < 0)
	{
		if( (fightPropChgArgs.iPropID != PROP_ENTITY_HP) && (fightPropChgArgs.iPropID != PROP_ENTITY_ANGER) )
		{
			return;
		}
	}

	// 通知客户端需要映射下
	IEffectFactory* pEffectFactory = getComponent<IEffectFactory>(COMPNAME_EffectFactory, IID_IEffectFactory);
	assert(pEffectFactory);
	
	int iShowPropID = 0;
	if(!pEffectFactory->isPropNotify(fightPropChgArgs.iPropID, iShowPropID) )
	{
		return;
	}

	if(fightPropChgArgs.bFilledResult) return;
	
	fightPropChgArgs.bFilledResult = true;
	
	google::protobuf::RepeatedPtrField<GSProto::FightResulSet>* pszResultSet = pAction->mutable_szresultset();

	assert(pszResultSet->size() > 0);
	
	GSProto::FightResulSet* pCurResultSet = &(*pszResultSet->rbegin() );

	IEntity* pTarget = getEntityFromHandle(fightPropChgArgs.hEntity);
	assert(pTarget);

	int iTargetPos = pTarget->getProperty(PROP_ENTITY_BATTLEPOS, 0);

	// 新增一个Result
	GSProto::FightResult* pNewResult = pCurResultSet->mutable_szresult()->Add();
	assert(pNewResult);

	if(!fightPropChgArgs.bRelive)
	{
		pNewResult->set_iresulttype(GSProto::en_FightResult_PropChg);
		GSProto::FightPropChgResult* pPropChgResult = pNewResult->mutable_propchgresult();

		
		pPropChgResult->set_itargetpos(iTargetPos);
		int iLifeAtt = Prop2LifeAtt(iShowPropID);
		assert(iLifeAtt >= 0);
		pPropChgResult->set_ilifeatt(iLifeAtt);
		pPropChgResult->set_ichgvalue(fightPropChgArgs.iChgValue);
		pPropChgResult->set_iresultvalue(fightPropChgArgs.iValue);
	}
	else
	{
		pNewResult->set_iresulttype(GSProto::en_FightResult_Relive);
		GSProto::ReliveResult* pReliveResult = pNewResult->mutable_reliveresult();

		pReliveResult->set_itargetpos(iTargetPos);
		int iLifeAtt = Prop2LifeAtt(iShowPropID);
		assert(iLifeAtt >= 0);
		pReliveResult->set_ilifeatt(iLifeAtt);
		pReliveResult->set_ichgvalue(fightPropChgArgs.iChgValue);
		pReliveResult->set_iresultvalue(fightPropChgArgs.iValue);
	}
}


bool FightFactory::useActorSkill(const ServerEngine::FightUseItem& useItemCtx)
{
	// 选择一个Giver
	HEntity hGiver;
	for(size_t i = 0; i < GSProto::MAX_BATTLE_MEMBER_SIZE/2; i++)
	{
		HEntity hTmpEntity = m_memberList[i];
		IEntity* pTmpEntity = getEntityFromHandle(hTmpEntity);
		if(!pTmpEntity) continue;

		int iTmpHP = pTmpEntity->getProperty(PROP_ENTITY_HP, 0);
		if(0 >= iTmpHP) continue;

		hGiver = hTmpEntity;
		break;
	}

	IEntity* pGiver = getEntityFromHandle(hGiver);
	assert(pGiver);

	const ActorSkillCfg* pActorSkillCfg = getActorSkillCfg(useItemCtx.iActorSkillID);
	assert(pActorSkillCfg);

	// 扣除能量
	int iLeftItemCount = m_pContinueDataAll->icuritemcount() - pActorSkillCfg->iCost;
	m_pContinueDataAll->set_icuritemcount(iLeftItemCount);

	// 处理Resultset位置
	//PropChgResultListener propListener(hGiver);
	GSProto::FightAction* outFightAction = NULL; 
	GSProto::FightResulSet* outResultSet = NULL;
	if(useItemCtx.iActionIndex == (m_pContinueDataAll->szfightaction_size() - 1) )
	{
		outFightAction = &(*(m_pContinueDataAll->mutable_szfightaction()->rbegin() ) );
		google::protobuf::RepeatedPtrField<GSProto::FightResulSet>* pszResultSet = outFightAction->mutable_szresultset();

		if(pszResultSet->size() == 0)
		{
			outResultSet = pszResultSet->Add();

			// 加HP,触发事件(客户端需要放到新Result里)
			PropChgResultListener propChgListener(pGiver->getHandle() );
			BuffAddListener buffAddListener(m_memberList);
			
			useActorEffect(pGiver, pActorSkillCfg->effectIDList);
		}
		else
		{
			int iConditionSize = 1;
			if(useItemCtx.iPost != 0) iConditionSize++;
			assert(pszResultSet->size() >= iConditionSize);
			vector<GSProto::FightResulSet> backupResultsetList;
			std::copy(pszResultSet->rbegin(), pszResultSet->rbegin()+iConditionSize, std::inserter(backupResultsetList, backupResultsetList.end() ) );

			// 删除最后2个Result
			for(int i = 0; i < iConditionSize; i++)
			{
				pszResultSet->RemoveLast();
			}
			
			outResultSet = pszResultSet->Add();

			// 加HP,触发事件(客户端需要放到新Result里)
			PropChgResultListener propChgListener(pGiver->getHandle() );
			BuffAddListener buffAddListener(m_memberList);
			
			useActorEffect(pGiver, pActorSkillCfg->effectIDList);

			// 修正HP,再回填,只有Pos需要处理
			if(useItemCtx.iPost != 0)
			{
				fixResultSetHP(backupResultsetList[0]);
			}
			std::copy(backupResultsetList.begin(), backupResultsetList.end(), google::protobuf::RepeatedFieldBackInserter(pszResultSet) );
		}
	}
	else
	{
		// 处理跨Action的情况
		assert(useItemCtx.iPost == 1);
		outFightAction = m_pContinueDataAll->mutable_szfightaction(useItemCtx.iActionIndex);
		
		// 备份最后一个Action, 必须删除，否则事件触发会有问题
		GSProto::FightAction backupAction = *(m_pContinueDataAll->mutable_szfightaction()->rbegin() );
		m_pContinueDataAll->mutable_szfightaction()->RemoveLast();

		// 备份当前最后一个ResultSet，插入吃药的在前面
		bool bBackupResultset = outFightAction->szresultset_size() > 0;
		GSProto::FightResulSet backupResultset;
		if(bBackupResultset)
		{
			// 备份，删除最后一个ResultSet
			backupResultset = *(outFightAction->mutable_szresultset()->rbegin() );
			outFightAction->mutable_szresultset()->RemoveLast();
			outResultSet = outFightAction->mutable_szresultset()->Add();
		}
		else
		{
			outResultSet = outFightAction->mutable_szresultset()->Add();
		}

		// 加HP,触发事件(客户端需要放到新Result里)
		PropChgResultListener propChgListener(pGiver->getHandle() );
		BuffAddListener buffAddListener(m_memberList);
		
		useActorEffect(pGiver, pActorSkillCfg->effectIDList);
		//EffectContext tmpEffectCtx;
		//onSkillEffectChangeHPProcess(pTarget->getHandle(), pTarget->getHandle(), iAddHP, tmpEffectCtx);

		// 如果ResultSet有备份，写回去
		if(bBackupResultset)
		{
			// 修正HP再回填
			fixResultSetHP(backupResultset);
			*(outFightAction->mutable_szresultset()->Add()) = backupResultset;
		}

		// 写回去原来的Action
		*m_pContinueDataAll->mutable_szfightaction()->Add() = backupAction;
	}

	// 原来的ActionIndex 由于插入一个新的吃药ResultSet,这里要为原来的重新Trigger一次
	triggerBeginResultSet();

	GSProto::FightResult* pNewResult = outResultSet->add_szresult();
	pNewResult->set_iresulttype(GSProto::en_FightResult_UseActorSkill);

	GSProto::FightUseActorSkillResult* pUseItemResult = pNewResult->mutable_useactorskillresult();
	pUseItemResult->set_iresultitemcount(m_pContinueDataAll->icuritemcount() );

	// 记录
	m_pContinueBattleData->useItemList.push_back(useItemCtx);
	
	return true;
}


void FightFactory::useActorEffect(IEntity* pGiver, const vector<int>& effectList)
{
	IEffectSystem* pEffectSys = static_cast<IEffectSystem*>(pGiver->querySubsystem(IID_IEffectSystem) );
	assert(pEffectSys);

	EffectContext tmpContext;
	tmpContext.setInt(PROP_EFFECT_GIVER, (Int32)pGiver->getHandle() );
	tmpContext.setInt64(PROP_EFFECT_FIGHTMEMBERLIST, (Int64)&m_memberList);
	for(size_t i = 0; i < effectList.size(); i++)
	{
		int iEffectID = effectList[i];
		pEffectSys->addEffect(iEffectID,  tmpContext);
	}
}


bool FightFactory::useFightItem(const ServerEngine::FightUseItem& useItem)
{
	/*IGlobalCfg* pGlobalCfg = getComponent<IGlobalCfg>(COMPNAME_GlobalCfg, IID_IGlobalCfg);
	assert(pGlobalCfg);

	int iResumePercent = pGlobalCfg->getInt("PVE药物恢复万份比", 3000);

	IEntity* pTarget = getEntityFromHandle(m_memberList[useItem.iTargetPos]);
	if(!pTarget) return false;

	// 如果目标死亡,continue
	int iTargetHP = pTarget->getProperty(PROP_ENTITY_HP, 0);
	if(0 == iTargetHP)
	{
		FDLOG("UseItem")<<"Fail|"<<useItem.iActionIndex<<"|"<<useItem.iResultsetIndex<<endl;
		return false;
	}

	FDLOG("UseItem")<<"UseSuccess|"<<useItem.iActionIndex<<"|"<<useItem.iResultsetIndex<<endl;
	int iTargetMaxHP = pTarget->getProperty(PROP_ENTITY_MAXHP, 0);
	int iAddHP = (int)( (double)iTargetMaxHP * (double)iResumePercent/10000);
	pTarget->changeProperty(PROP_ENTITY_HP, iAddHP, 0);


	// 记录
	m_pContinueBattleData->useItemList.push_back(useItem);

	// 减少数目
	int iCurItemCount = m_pContinueDataAll->icuritemcount();
	m_pContinueDataAll->set_icuritemcount(iCurItemCount-1);

	_useItemImp(useItem, pTarget, iAddHP);*/

	return true;
}

void FightFactory::_useItemImp(const ServerEngine::FightUseItem& useItemCtx, IEntity* pTarget, int iAddHP)
{
	GSProto::FightAction* outFightAction = NULL; 
	GSProto::FightResulSet* outResultSet = NULL;

	// 没有跨Action的情况
	if(useItemCtx.iActionIndex == (m_pContinueDataAll->szfightaction_size() - 1) )
	{
		outFightAction = &(*(m_pContinueDataAll->mutable_szfightaction()->rbegin() ) );
		google::protobuf::RepeatedPtrField<GSProto::FightResulSet>* pszResultSet = outFightAction->mutable_szresultset();

		if(pszResultSet->size() == 0)
		{
			outResultSet = pszResultSet->Add();

			// 加HP,触发事件(客户端需要放到新Result里)
			PropChgResultListener propChgListener(pTarget->getHandle() );
			EffectContext tmpEffectCtx;
			onSkillEffectChangeHPProcess(pTarget->getHandle(), pTarget->getHandle(), iAddHP, tmpEffectCtx);
		}
		else
		{
			int iConditionSize = 1;
			if(useItemCtx.iPost != 0) iConditionSize++;
			assert(pszResultSet->size() >= iConditionSize);
			vector<GSProto::FightResulSet> backupResultsetList;
			std::copy(pszResultSet->rbegin(), pszResultSet->rbegin()+iConditionSize, std::inserter(backupResultsetList, backupResultsetList.end() ) );

			// 删除最后2个Result
			for(int i = 0; i < iConditionSize; i++)
			{
				pszResultSet->RemoveLast();
			}
			
			outResultSet = pszResultSet->Add();

			// 加HP,触发事件(客户端需要放到新Result里)
			PropChgResultListener propChgListener(pTarget->getHandle() );
			EffectContext tmpEffectCtx;
			onSkillEffectChangeHPProcess(pTarget->getHandle(), pTarget->getHandle(), iAddHP, tmpEffectCtx);

			// 修正HP,再回填,只有Pos需要处理
			if(useItemCtx.iPost != 0)
			{
				//fixResultSetHP(pTarget, backupResultsetList[0]);
			}
			std::copy(backupResultsetList.begin(), backupResultsetList.end(), google::protobuf::RepeatedFieldBackInserter(pszResultSet) );
		}
	}
	else
	{
		// 处理跨Action的情况
		assert(useItemCtx.iPost == 1);
		outFightAction = m_pContinueDataAll->mutable_szfightaction(useItemCtx.iActionIndex);
		
		// 备份最后一个Action, 必须删除，否则事件触发会有问题
		GSProto::FightAction backupAction = *(m_pContinueDataAll->mutable_szfightaction()->rbegin() );
		m_pContinueDataAll->mutable_szfightaction()->RemoveLast();

		// 备份当前最后一个ResultSet，插入吃药的在前面
		bool bBackupResultset = outFightAction->szresultset_size() > 0;
		GSProto::FightResulSet backupResultset;
		if(bBackupResultset)
		{
			// 备份，删除最后一个ResultSet
			backupResultset = *(outFightAction->mutable_szresultset()->rbegin() );
			outFightAction->mutable_szresultset()->RemoveLast();
			outResultSet = outFightAction->mutable_szresultset()->Add();
		}
		else
		{
			outResultSet = outFightAction->mutable_szresultset()->Add();
		}

		// 加HP,触发事件(客户端需要放到新Result里)
		PropChgResultListener propChgListener(pTarget->getHandle() );
		EffectContext tmpEffectCtx;
		onSkillEffectChangeHPProcess(pTarget->getHandle(), pTarget->getHandle(), iAddHP, tmpEffectCtx);

		// 如果ResultSet有备份，写回去
		if(bBackupResultset)
		{
			// 修正HP再回填
			//fixResultSetHP(pTarget, backupResultset);
			*(outFightAction->mutable_szresultset()->Add()) = backupResultset;
		}

		// 写回去原来的Action
		*m_pContinueDataAll->mutable_szfightaction()->Add() = backupAction;
	}

	// 原来的ActionIndex 由于插入一个新的吃药ResultSet,这里要为原来的重新Trigger一次
	triggerBeginResultSet();

	GSProto::FightResult* pNewResult = outResultSet->add_szresult();
	pNewResult->set_iresulttype(GSProto::en_FightResult_UseActorSkill);

	GSProto::FightUseActorSkillResult* pUseItemResult = pNewResult->mutable_useactorskillresult();
	pUseItemResult->set_iresultitemcount(m_pContinueDataAll->icuritemcount() );

}

void FightFactory::fixResultSetHP(GSProto::FightResulSet& refResultSet)
{
	for(size_t i = 0; i < m_memberList.size(); i++)
	{
		IEntity* pTmpMember = getEntityFromHandle(m_memberList[i]);
		if(!pTmpMember) continue;

		GSProto::FightResult* pNewResult =refResultSet.add_szresult();
		pNewResult->set_iresulttype(GSProto::en_FightResult_SyncHP);
		
		int iTargetPos = pTmpMember->getProperty(PROP_ENTITY_BATTLEPOS, 0);
		int iHP = pTmpMember->getProperty(PROP_ENTITY_HP, 0);
		int iAnger = pTmpMember->getProperty(PROP_ENTITY_ANGER, 0);

		GSProto::FightSyncHPResult* pSyncHPResult = pNewResult->mutable_synchpresult();
		pSyncHPResult->set_itargetpos(iTargetPos);
		pSyncHPResult->set_ihp(iHP);
		pSyncHPResult->set_ianger(iAnger);
	}
}


int FightFactory::getCurrentRound()
{
	return m_iRound;
}


int FightFactory::_doCommonFight(const vector<HEntity>& memberList, ServerEngine::BattleData& data, const ServerEngine::FightContext& ctx, const vector<HEntity>& externList, int iWorldID, int iLoopCount, GSProto::FightDataAll& wholeData)
{
	PROFILE_MONITOR("_doCommonFight");

	FDLOG("Rand")<<"Start CommFight"<<endl;
	m_memberList = memberList;

	bool bAttackerFirstMove = false;
	if(GSProto::en_FightType_Boss == ctx.iFightType)
	{
		bAttackerFirstMove = true;
	}
	else
	{
		bAttackerFirstMove = isAttackerSideFirstMove(ctx, memberList);
	}

	// 每次进战斗触发,因为每次换人，会导致memberlist变化，逻辑层需要知道
	_triggerBeginFight(memberList, ctx, externList, iLoopCount);
	
	// 登场技能使用(PROP_ENTITY_FIGHTVALUE)
	if(iLoopCount == 0)
	{
		doOnStageSkill(memberList, bAttackerFirstMove, externList, wholeData);
	}

	int iRound = 0;
	vector<HEntity> szSortArray[2];
	sortAttackList(memberList, bAttackerFirstMove, szSortArray[0], szSortArray[1]);
	
	_triggerBeginRound(memberList, iRound, externList);

	if(iLoopCount == 0)
	{
		fillBattleMember(wholeData, memberList, externList, ctx);
	}

	FDLOG("Rand")<<"Start Round Fight"<<endl;
	
	int iDoor = 0;
	while(!checkFinish(memberList, data, iRound, ctx) )
	{
		vector<HEntity> tmpExternList = externList;
		while(!checkFinish(memberList, data, iRound, ctx) )
		{
			if(!procGoldAnimalFight(iRound, memberList, bAttackerFirstMove, tmpExternList, externList, wholeData) ) break;
		}

		if(checkFinish(memberList, data, iRound, ctx) )
		{
			break;
		}
	
		// 如果一轮打完，重新排下一轮
		if( (szSortArray[0].size() == 0) && (szSortArray[1].size() == 0) )
		{
			_triggerEndRound(memberList, iRound, externList);
			iDoor = 0; 
			sortAttackList(memberList, bAttackerFirstMove, szSortArray[0], szSortArray[1]);
			_triggerBeginRound(memberList, ++iRound, externList);
		}

		if(szSortArray[iDoor].size() == 0)
		{
			iDoor = 1 - iDoor;
		}

		HEntity hTmpEntity;
		while(szSortArray[iDoor].size() > 0)
		{
			IEntity* pCheckEntity = getEntityFromHandle(szSortArray[iDoor].front() );
			assert(pCheckEntity);

			szSortArray[iDoor].erase(szSortArray[iDoor].begin() );

			int iCheckHP = pCheckEntity->getProperty(PROP_ENTITY_HP, 0);
			if(iCheckHP > 0)
			{
				hTmpEntity = pCheckEntity->getHandle();
				break;
			}
		}

		// 切换为下次对方出手
		iDoor = 1 - iDoor;
	
		IEntity* pTmpEntity = getEntityFromHandle(hTmpEntity);
		if(pTmpEntity)
		{
			ISkillSystem* pSkillSystem = static_cast<ISkillSystem*>(pTmpEntity->querySubsystem(IID_ISkillSystem));
			assert(pSkillSystem);

			// 调整下顺序，delBuff可能会在PreAttack事件触发，表现上需要在技能释放前
			{
				DelBuffActionListener tmpDelBuffListner(wholeData, memberList, externList);
				_triggerPreAttack(hTmpEntity, iRound);
			}
			
			{
				SkillActionListener skillAListener(wholeData, hTmpEntity, memberList, externList);
				pSkillSystem->useSkill(memberList);
				
			}
			
			{
				DelBuffActionListener tmpDelBuffListner(wholeData, memberList, externList);
				_triggerPostAttack(hTmpEntity, iRound);
			}
		}
	}

	// 填充成员结束状态信息和胜负
	wholeData.set_iisattackerwin((int)data.bAttackerWin);

	fillMemberFinStatus(memberList, wholeData);

	_triggerEndFight(memberList, ctx, externList);

	generateFightKey(iWorldID, data);
	wholeData.SerializeToString(&data.strBattleBody);

	// 填充星级
	data.iStar = calcFightStar(wholeData);
	
	return en_FightResult_OK;
}


void FightFactory::fillMemberFinStatus(const vector<HEntity>& memberList, GSProto::FightDataAll& data)
{
	for(int i = 0; i < data.szfightobject_size(); i++)
	{
		GSProto::FightObj* pFightObj = data.mutable_szfightobject(i);
		int iPos = pFightObj->ipos();
		if(iPos >= (int)memberList.size() )
		{
			continue;
		}

		// 由于后面引入多拨战斗,这里要continue
		HEntity hTmpEntity = memberList[iPos];
		IEntity* pTmpEntity = getEntityFromHandle(hTmpEntity);
		if(!pTmpEntity) continue;
		
		int iFinHP = pTmpEntity->getProperty(PROP_ENTITY_HP, 0);
		pFightObj->set_ifinhp(iFinHP);
	}
}

void FightFactory::fillBattleMember(GSProto::FightDataAll& data, const vector<HEntity>& memberList, const vector<HEntity>& externList, const ServerEngine::FightContext& ctx)
{
	data.set_ifighttype(ctx.iFightType);
	data.set_isceneid(ctx.iSceneID);

	bool bLeftIsActor = false;
	bool bRightIsActor = false;

	HEntity hLeftActor;
	HEntity hRightActor;
	for(size_t i = 0; i < memberList.size(); i++)
	{
		HEntity hEntity = memberList[i];
		IEntity* pEntity = getEntityFromHandle(hEntity);
		if(!pEntity) continue;

		fillSingleFightObj(hEntity, *(data.mutable_szfightobject()->Add() ) );

		int iClassID = pEntity->getProperty(PROP_ENTITY_CLASS, 0);
		if(!bLeftIsActor && (i < GSProto::MAX_BATTLE_MEMBER_SIZE/2) && (GSProto::en_class_Hero == iClassID) )
		{
			bLeftIsActor = true;
			hLeftActor = pEntity->getProperty(PROP_ENTITY_MASTER, 0);
		}

		if(!bRightIsActor && (i >= GSProto::MAX_BATTLE_MEMBER_SIZE/2) && (GSProto::en_class_Hero == iClassID) )
		{
			bRightIsActor = true;
			hRightActor = pEntity->getProperty(PROP_ENTITY_MASTER, 0);
		}
	}

	for(size_t i = 0; i < externList.size(); i++)
	{
		HEntity hEntity = externList[i];
		IEntity* pEntity = getEntityFromHandle(hEntity);
		if(!pEntity) continue;

		fillSingleFightObj(hEntity, *(data.mutable_szfightobject()->Add() ) );
	}

	// 如果是PVP需要记录双方头像
	if(bLeftIsActor && bRightIsActor)
	{
		IEntity* pLeftActor = getEntityFromHandle(hLeftActor);
		IEntity* pRightActor = getEntityFromHandle(hRightActor);
		assert(pLeftActor && pRightActor);
		
		GSProto::FightHead* pLeftHead = data.mutable_lefthead();
		GSProto::FightHead* pRightHead = data.mutable_righthead();

		assert(pLeftHead && pRightHead);

		IFormationSystem* pLeftFormationSys = static_cast<IFormationSystem*>(pLeftActor->querySubsystem(IID_IFormationSystem));
		assert(pLeftFormationSys);

		IFormationSystem* pRightFormationSys = static_cast<IFormationSystem*>(pRightActor->querySubsystem(IID_IFormationSystem));
		assert(pRightFormationSys);

		pLeftHead->set_iheadid(pLeftFormationSys->getVisibleHeroID() );
		pLeftHead->set_stractorname(pLeftActor->getProperty(PROP_ENTITY_NAME, "") );
		pLeftHead->set_ilevel(pLeftActor->getProperty(PROP_ENTITY_LEVEL, 1));
		//pLeftHead->set_ifightvalue(pLeftActor->getProperty(PROP_ENTITY_FIGHTVALUE, 1));
		

		pRightHead->set_iheadid(pRightFormationSys->getVisibleHeroID() );
		pRightHead->set_stractorname(pRightActor->getProperty(PROP_ENTITY_NAME, "") );
		pRightHead->set_ilevel(pRightActor->getProperty(PROP_ENTITY_LEVEL, 1));
		//pRightHead->set_ifightvalue(pRightActor->getProperty(PROP_ENTITY_FIGHTVALUE, 1));
		
	}
}

void FightFactory::fillSingleFightObj(HEntity hEntity, GSProto::FightObj& fightObj)
{
	IEntity* pEntity = getEntityFromHandle(hEntity);
	assert(pEntity);

	int iClass = pEntity->getProperty(PROP_ENTITY_CLASS, 0);
	fightObj.set_iobjecttype(iClass);

	int iPos = pEntity->getProperty(PROP_ENTITY_BATTLEPOS, 0);
	fightObj.set_ipos(iPos);

	int iBaseID = pEntity->getProperty(PROP_ENTITY_BASEID, 0);
	int iLevel = pEntity->getProperty(PROP_ENTITY_LEVEL, 0);
	int iLevelStep = pEntity->getProperty(PROP_ENTITY_LEVELSTEP, 0);
	int iMaxHP = pEntity->getProperty(PROP_ENTITY_MAXHP, 0);
	int iAnger = pEntity->getProperty(PROP_ENTITY_ANGER, 0);
	int iCurHP = pEntity->getProperty(PROP_ENTITY_HP, 0);
	int iQuality = pEntity->getProperty(PROP_ENTITY_QUALITY, 0);

	if(GSProto::en_class_Hero == iClass)
	{
		GSProto::FightHero* pHero = fightObj.mutable_hero();
		pHero->set_ibaseid(iBaseID);
		pHero->set_ilevel(iLevel);
		pHero->set_ilevelstep(iLevelStep);
		pHero->set_imaxhp(iMaxHP);
		pHero->set_iinitanger(iAnger);
		pHero->set_ihp(iCurHP);
		pHero->set_iquality(iQuality);
	}
	else if(GSProto::en_class_Monster == iClass)
	{
		GSProto::FightMonster* pMonster = fightObj.mutable_monster();
		pMonster->set_ibaseid(iBaseID);
		pMonster->set_ilevel(iLevel);
		pMonster->set_ilevelstep(iLevelStep);
		pMonster->set_imaxhp(iMaxHP);
		pMonster->set_iinitanger(iAnger);
		pMonster->set_ihp(iCurHP);
	}
	else if(GSProto::en_class_GodAnimal == iClass)
	{
		GSProto::FightGodAnimal* pGodAnimal = fightObj.mutable_godanimal();
		pGodAnimal->set_ibaseid(iBaseID);
		pGodAnimal->set_ilevel(iLevel);
		pGodAnimal->set_ilevelstep(iLevelStep);
		pGodAnimal->set_imaxhp(iMaxHP);
		pGodAnimal->set_iinitanger(iAnger);
	}
}

void FightFactory::_triggerBeginFight(const vector<HEntity>& memberList, const ServerEngine::FightContext& ctx, const vector<HEntity>& externList, int iLoopCount)
{
	vector<HEntity> tmpList = memberList;
	std::copy(externList.begin(), externList.end(), std::inserter(tmpList, tmpList.end() ) );

	set<HEntity> masterList;
	int attHEntity;
	for(size_t i = 0; i < tmpList.size(); i++)
	{
		HEntity hEntity = tmpList[i];
		IEntity* pEntity = getEntityFromHandle(hEntity);
		if(!pEntity) continue;

		HEntity hMaster = pEntity->getProperty(PROP_ENTITY_MASTER, 0);
		if(0 != hMaster) 
		{
			masterList.insert(hMaster);
			if( i < GSProto::MAX_BATTLE_MEMBER_SIZE/2)
			{
				attHEntity = hMaster;
			}
		}

		EventArgs args;
		args.context.setInt("entity", hEntity);
		args.context.setInt("loopcount", iLoopCount);
		args.context.setInt64("memberlist", (Int64)&memberList);
		args.context.setInt64("clientParam", (Int64)&ctx);
		
		pEntity->getEventServer()->setEvent(EVENT_ENTITY_PREBEGIN_FIGHT, args);
		pEntity->getEventServer()->setEvent(EVENT_ENTITY_BEGIN_FIGHT, args);
	}

	for(set<HEntity>::iterator it = masterList.begin(); it != masterList.end(); it++)
	{
		HEntity hEntity = *it;
		IEntity* pEntity = getEntityFromHandle(hEntity);
		if(!pEntity) continue;

		EventArgs args;
		args.context.setInt("entity", hEntity);
		args.context.setInt64("memberlist", (Int64)&memberList);
		args.context.setInt64("clientParam", (Int64)&ctx);
		args.context.setInt64("attHEnttiy",	(Int64)attHEntity);
		
		pEntity->getEventServer()->setEvent(EVENT_ENTITY_PREBEGIN_FIGHT, args);
		pEntity->getEventServer()->setEvent(EVENT_ENTITY_BEGIN_FIGHT, args);
	}
}

void FightFactory::_triggerEndFight(const vector<HEntity>& memberList, const ServerEngine::FightContext& ctx, const vector<HEntity>& externList)
{
	
	vector<HEntity> tmpList = memberList;
	std::copy(externList.begin(), externList.end(), std::inserter(tmpList, tmpList.end() ) );

	set<HEntity> masterList;
	for(size_t i = 0; i < tmpList.size(); i++)
	{
		HEntity hEntity = tmpList[i];
		IEntity* pEntity = getEntityFromHandle(hEntity);
		if(!pEntity) continue;

		HEntity hMaster = pEntity->getProperty(PROP_ENTITY_MASTER, 0);
		if(0 != hMaster) masterList.insert(hMaster);

		EventArgs args;
		args.context.setInt("entity", hEntity);
		args.context.setInt("entity", hEntity);
		args.context.setInt64("memberlist", (Int64)&memberList);
		args.context.setInt64("clientParam", (Int64)&ctx);
		
		pEntity->getEventServer()->setEvent(EVENT_ENTITY_END_FIGHT, args);
	}

	for(set<HEntity>::iterator it = masterList.begin(); it != masterList.end(); it++)
	{
		HEntity hEntity = *it;
		IEntity* pEntity = getEntityFromHandle(hEntity);
		if(!pEntity) continue;

		EventArgs args;
		args.context.setInt("entity", hEntity);
		args.context.setInt("entity", hEntity);
		args.context.setInt64("memberlist", (Int64)&memberList);
		args.context.setInt64("clientParam", (Int64)&ctx);
		
		pEntity->getEventServer()->setEvent(EVENT_ENTITY_END_FIGHT, args);
	}
}

void FightFactory::_triggerBeginRound(const vector<HEntity>& memberList, int iRound, const vector<HEntity>& externList)
{
	EventArgs args;
	args.context.setInt("round", iRound);

	vector<HEntity> tmpList = memberList;
	std::copy(externList.begin(), externList.end(), std::inserter(tmpList, tmpList.end() ) );
	
	for(size_t i = 0; i < tmpList.size(); i++)
	{
		HEntity hEntity = tmpList[i];
		IEntity* pEntity = getEntityFromHandle(hEntity);
		if(!pEntity) continue;
		
		args.context.setInt("entity", hEntity);
		pEntity->getEventServer()->setEvent(EVENT_ENTITY_BEGIN_ROUND, args);
	}

	m_iRound = iRound;
}

void FightFactory::_triggerEndRound(const vector<HEntity>& memberList, int iRound, const vector<HEntity>& externList)
{
	EventArgs args;
	args.context.setInt("round", iRound);

	vector<HEntity> tmpList = memberList;
	std::copy(externList.begin(), externList.end(), std::inserter(tmpList, tmpList.end() ) );

	for(vector<HEntity>::const_iterator it = tmpList.begin(); it != tmpList.end(); it++)
	{
		HEntity hEntity = *it;
		IEntity* pEntity = getEntityFromHandle(hEntity);
		if(!pEntity) continue;
		
		args.context.setInt("entity", hEntity);
		pEntity->getEventServer()->setEvent(EVENT_ENTITY_END_ROUND, args);
	}
}

void FightFactory::_triggerPreAttack(HEntity hEntity, int iRound)
{
	IEntity* pEntity = getEntityFromHandle(hEntity);
	if(!pEntity) return;

	EventArgs args;
	args.context.setInt("entity", hEntity);
	args.context.setInt("round", iRound);
	pEntity->getEventServer()->setEvent(EVENT_ENTITY_PRE_ATTACK, args);
}

void FightFactory::_triggerPostAttack(HEntity hEntity, int iRound)
{
	IEntity* pEntity = getEntityFromHandle(hEntity);
	if(!pEntity) return;

	EventArgs args;
	args.context.setInt("entity", hEntity);
	args.context.setInt("round", iRound);
	pEntity->getEventServer()->setEvent(EVENT_ENTITY_POST_ATTACK, args);	
}


bool FightFactory::procGoldAnimalFight(int iRound, const vector<HEntity>& memberList, bool bAttackerFirstMove, vector<HEntity>& externList, const vector<HEntity>& orgExternList, GSProto::FightDataAll& wholeData)
{
	if(externList.size() == 0)
	{
		return false;
	}
	
	HEntity hTmpMem = externList.front();
	externList.erase(externList.begin() );
	IEntity* pTmpMem = getEntityFromHandle(hTmpMem);
	assert(pTmpMem);

	ISkillSystem* pSkillSystem = static_cast<ISkillSystem*>(pTmpMem->querySubsystem(IID_ISkillSystem));
	assert(pSkillSystem);

	{
		DelBuffActionListener tmpDelBuffListner(wholeData, memberList, orgExternList);
		_triggerPreAttack(hTmpMem, iRound);
	}


	{
		SkillActionListener skillAListener(wholeData, hTmpMem, memberList, orgExternList);
		pSkillSystem->useSkill(memberList);
	}
	
	{
		DelBuffActionListener tmpDelBuffListner(wholeData, memberList, orgExternList);
		_triggerPostAttack(hTmpMem, iRound);
	}

	return true;
}

void FightFactory::sortAttackList(const vector<HEntity>& memberList, bool bAttackerFirst, vector<HEntity>& firsts, vector<HEntity>& second)
{
	static int s_szSortIndex[] = {0, 1, 2, 3, 4, 5, 6, 7, 8,};
	int iParam = 0;

	if(!bAttackerFirst)
	{
		iParam = 1;
	}

	for(int i = 0; i < (int)(sizeof(s_szSortIndex)/sizeof(int)); i++)
	{
		int iFirstPos = (GSProto::MAX_BATTLE_MEMBER_SIZE/2) * iParam + s_szSortIndex[i];
		IEntity* pFirstEntity = getEntityFromHandle(memberList[iFirstPos]);
		if(pFirstEntity && (pFirstEntity->getProperty(PROP_ENTITY_HP, 0) > 0) )
		{
			
firsts.push_back(pFirstEntity->getHandle() );
		}

		int iSecondPos = (GSProto::MAX_BATTLE_MEMBER_SIZE/2) * (1 - iParam) + s_szSortIndex[i];
		IEntity* pSecondEntity = getEntityFromHandle(memberList[iSecondPos]);
		if(pSecondEntity && (pSecondEntity->getProperty(PROP_ENTITY_HP, 0) > 0) )
		{
			second.push_back(pSecondEntity->getHandle() );
		}
	}
}


bool FightFactory::checkFinish(const vector<HEntity>& memberList, ServerEngine::BattleData& data, int iRound, const ServerEngine::FightContext& ctx)
{
	double dAttackerHP = 0.0;
	double dTargetHP = 0.0;

	for(size_t i = 0; i < memberList.size(); i++)
	{
		IEntity* pTmpEntity = getEntityFromHandle(memberList[i]);
		if(!pTmpEntity) continue;

		int iHP = pTmpEntity->getProperty(PROP_ENTITY_HP, 0);

		if(i < GSProto::MAX_BATTLE_MEMBER_SIZE/2)
		{
			dAttackerHP += iHP;
		}
		else
		{
			dTargetHP += iHP;
		}
	}

	if(dAttackerHP <= 0.0)
	{
		data.bAttackerWin = false;
		return true;
	}
	else if(dTargetHP <= 0.0)
	{
		data.bAttackerWin = true;
		return true;
	}

	if(iRound >= GSProto::MAX_FIGHTROUND_NUM)
	{
		data.bAttackerWin = false;
		return true;
	}
	
	return false;
}

int FightFactory::calcFightStar(GSProto::FightDataAll& data)
{
	const ::google::protobuf::RepeatedPtrField<GSProto::FightObj>& refObjectList = data.szfightobject();

	int iWinDeadCount = 0;
	for(int i = 0; i < refObjectList.size(); i++)
	{
		const GSProto::FightObj& refTmpObj = refObjectList.Get(i);
		
		// 活着的和神兽不算
		if( (refTmpObj.ifinhp() > 0) || (refTmpObj.ipos() >= GSProto::MAX_BATTLE_MEMBER_SIZE) )
		{
			continue;
		}


		bool bWinSide = (refTmpObj.ipos() < GSProto::MAX_BATTLE_MEMBER_SIZE/2) && (data.iisattackerwin() );
		bWinSide = bWinSide || ((refTmpObj.ipos() >= GSProto::MAX_BATTLE_MEMBER_SIZE/2) && !data.iisattackerwin() );
		
		if(bWinSide) iWinDeadCount++;
	}

	if(iWinDeadCount >= GSProto::en_SceneStar_3)
	{
		return GSProto::en_SceneStar_1;
	}

	return GSProto::en_SceneStar_3 - iWinDeadCount;
}

void FightFactory::doOnStageSkill(const vector<HEntity>& memberList, bool bAttackerFirst, const vector<HEntity>& externList, GSProto::FightDataAll& wholeData)
{
	for(int i = 0; i < GSProto::MAX_BATTLE_MEMBER_SIZE/2; i++)
	{
		int iPos = (1 - (int)bAttackerFirst) * (GSProto::MAX_BATTLE_MEMBER_SIZE/2) + i;
		HEntity hEntity = memberList[iPos];
		if(0 == hEntity) continue;

		//GSProto::FightAction* pNewAction =  wholeData.mutable_szfightaction()->Add();

		// modified by feiwu, 只有神兽的登场技能展示20140928
		//SkillActionListener skillAListener(wholeData, hEntity, memberList, externList);
		useStateSkill(hEntity, memberList);
	}

	for(int i = 0; i < GSProto::MAX_BATTLE_MEMBER_SIZE/2; i++)
	{
		int iPos = ((int)bAttackerFirst) * (GSProto::MAX_BATTLE_MEMBER_SIZE/2) + i;
		HEntity hEntity = memberList[iPos];
		if(0 == hEntity) continue;

		//GSProto::FightAction* pNewAction =  wholeData.mutable_szfightaction()->Add();
		// modified by feiwu, 只有神兽的登场技能展示20140928
		//SkillActionListener skillAListener(wholeData, hEntity, memberList, externList);
		
		useStateSkill(hEntity, memberList);
	}

	for(size_t i = 0; i < externList.size(); i++)
	{
		HEntity hEntity = externList[i];
		if(0 == hEntity) continue;

		//GSProto::FightAction* pNewAction =  wholeData.mutable_szfightaction()->Add();
		SkillActionListener skillAListener(wholeData, hEntity, memberList, externList);
		
		useStateSkill(hEntity, memberList);
	}
}

void FightFactory::useStateSkill(HEntity hEntity, const vector<HEntity>& memberList)
{
	IEntity* pEntity = getEntityFromHandle(hEntity);
	assert(pEntity);

	ISkillSystem* pSkillSystem = static_cast<ISkillSystem*>(pEntity->querySubsystem(IID_ISkillSystem));
	assert(pSkillSystem);

	int iTmpSkillID = pSkillSystem->getStageSkill();
	if(iTmpSkillID > 0)
	{
		pSkillSystem->addTempSkill(iTmpSkillID);
		pSkillSystem->useTempSkill(iTmpSkillID, memberList);
	}
}


int FightFactory::doPVPFight(ServerEngine::BattleData& battleInfo, const ServerEngine::ActorCreateContext& attRoleInfo, const ServerEngine::ActorCreateContext& targetRileInfo, const ServerEngine::FightContext& ctx)
{
	PROFILE_MONITOR("doPVPFight");

	BattleFightLogHelper guard_logHelp(string("doPVPFight") + attRoleInfo.strAccount);

	// 设置随机数Cache
	BattleRandomCache randCache(battleInfo);

	BattleFightCtxGuard ctxGuard( (ServerEngine::FightContext&)ctx);
	
	IJZEntityFactory* pEntityFactory = getComponent<IJZEntityFactory>(COMPNAME_EntityFactory, IID_IJZEntityFactory);
	assert(pEntityFactory);

	string strAttCreateData = ServerEngine::JceToStr(attRoleInfo);
	IEntity* pAttackGhost = pEntityFactory->getEntityFactory()->createEntity("Ghost", strAttCreateData);

	int iFightValue = pAttackGhost->getProperty(PROP_ENTITY_FIGHTVALUE, 0);
	pAttackGhost->setProperty(PROP_ENTITY_FIGHTVALUE_PREFIGHT, iFightValue);

	int iWorldID = pAttackGhost->getProperty(PROP_ACTOR_WORLD, 0);

	string strTargetCreateData = ServerEngine::JceToStr(targetRileInfo);
	IEntity* pTargetGhost = pEntityFactory->getEntityFactory()->createEntity("Ghost", strTargetCreateData);

	assert(pAttackGhost && pTargetGhost);

	int iTargetFightValue = pTargetGhost->getProperty(PROP_ENTITY_FIGHTVALUE, 0);
	pTargetGhost->setProperty(PROP_ENTITY_FIGHTVALUE_PREFIGHT, iTargetFightValue);

	vector<HEntity> memberList;
	memberList.resize(GSProto::MAX_BATTLE_MEMBER_SIZE, 0);
	fillGhost(memberList, pAttackGhost->getHandle(), true);
	fillGhost(memberList, pTargetGhost->getHandle(), false);

	// 神兽填充到externList,暂无;
	vector<HEntity> externList;

	bool bAttackerFirstMove = isAttackerSideFirstMove(ctx, memberList);
	if(bAttackerFirstMove)
	{
		fillExternList(pAttackGhost->getHandle(), true, externList);
		fillExternList(pTargetGhost->getHandle(), false, externList);
	}
	else
	{
		fillExternList(pTargetGhost->getHandle(), false, externList);
		fillExternList(pAttackGhost->getHandle(), true, externList);
	}

	IGlobalCfg* pGlobalCfg = getComponent<IGlobalCfg>(COMPNAME_GlobalCfg, IID_IGlobalCfg);
	assert(pGlobalCfg);

	int iInitItemCount = pGlobalCfg->getInt("战斗初始药物数量", 1);
	if(ctx.iFightType == GSProto::en_FightType_FirstFight)
	{
		iInitItemCount = 3;
	}
	if(ctx.iFightType == GSProto::en_FightType_DreamLand)
	{
		ServerEngine::DreamLandFightCtx dreamLandCtx;
		assert(ctx.context.find("DreamLandFightCtx") != ctx.context.end() );
		ServerEngine::JceToObj(ctx.context.at("DreamLandFightCtx"), dreamLandCtx);
		iInitItemCount = dreamLandCtx.iItemCount;

		
	}

	GSProto::FightDataAll wholeData;
	BattleWholeDataGuard wholeDataGuard(&wholeData);
	wholeData.set_icuritemcount(iInitItemCount);
	wholeData.set_iinititemcount(iInitItemCount);
		
	int iResult = _doCommonFight(memberList, battleInfo, ctx, externList, iWorldID, 0, wholeData);

	cleanFightMemList(memberList);

	FDLOG("Fight")<<wholeData.DebugString()<<endl;

	return iResult;
}

void FightFactory::cleanFightMemList(const vector<HEntity>& memberList)
{
	for(size_t i = 0; i < memberList.size(); i++)
	{
		HEntity hMem = memberList[i];
		IEntity* pMem = getEntityFromHandle(hMem);
		if(!pMem) continue;

		int iClassID = pMem->getProperty(PROP_ENTITY_CLASS, 0);
		if(GSProto::en_class_Hero == iClassID)
		{
			HEntity hMaster = pMem->getProperty(PROP_ENTITY_MASTER, 0);
			IEntity* pMaster = getEntityFromHandle(hMaster);
			delete pMaster;
		}
		else if(GSProto::en_class_Monster == iClassID)
		{
			delete pMem;
		}
	}
}


class FightPVECallback:public ServerEngine::FightPrxCallback
{
public:

	FightPVECallback(DelegatePVEFight cb):m_cb(cb){}

	 virtual void callback_doPVEFight(taf::Int32 ret,  const ServerEngine::BattleData& battleInfo)
	 {
	 	m_cb(ret, battleInfo);
	 }
     
     virtual void callback_doPVEFight_exception(taf::Int32 ret)
     {
     	FDLOG("FightPVECallback")<<"callback_doPVEFight_exception"<<ret<<endl;
     	ServerEngine::BattleData nullBattle;
     	m_cb(en_FightResult_SysError, nullBattle);
     }

	 virtual void callback_doMultiPVEFight(taf::Int32 ret,  const ServerEngine::BattleData& battleInfo)
	 {
	 	m_cb(ret, battleInfo);
	 }
    
     virtual void callback_doMultiPVEFight_exception(taf::Int32 ret)
     {
     	FDLOG("FightPVECallback")<<"callback_doMultiPVEFight_exception"<<ret<<endl;
     	ServerEngine::BattleData nullBattle;
     	m_cb(en_FightResult_SysError, nullBattle);
     }

	 virtual void callback_continuePVEFight(taf::Int32 ret,  const ServerEngine::BattleData& battleInfo)
	 {
	 	m_cb(ret, battleInfo);
	 }
      
     virtual void callback_continuePVEFight_exception(taf::Int32 ret)
     { 
     	FDLOG("FightPVECallback")<<"callback_continuePVEFight_exception"<<ret<<endl;
    	ServerEngine::BattleData nullBattle;
     	m_cb(en_FightResult_SysError, nullBattle);
	 }

     virtual void callback_continueMultiPVEFight(taf::Int32 ret,  const ServerEngine::BattleData& battleInfo)
     {
     	m_cb(ret, battleInfo);
     }

	 virtual void callback_continueMultiPVEFight_exception(taf::Int32 ret)
	 {
	 	FDLOG("FightPVECallback")<<"callback_continueMultiPVEFight_exception"<<ret<<endl;
	 	ServerEngine::BattleData nullBattle;
     	m_cb(en_FightResult_SysError, nullBattle);
	 }

private:

	DelegatePVEFight m_cb;
};


class FightPVPCallback:public ServerEngine::FightPrxCallback
{
public:

	FightPVPCallback(DelegatePVPFight cb):m_cb(cb){}

	virtual void callback_doPVPFight(taf::Int32 ret,  const ServerEngine::BattleData& battleInfo)
	{
		m_cb(ret, (ServerEngine::BattleData&)battleInfo);
	}
  
    virtual void callback_doPVPFight_exception(taf::Int32 ret)
    {
    	ServerEngine::BattleData nullBattle;
     	m_cb(en_FightResult_SysError, nullBattle);
    }

	virtual void callback_continuePVPFight(taf::Int32 ret,  const ServerEngine::BattleData& battleInfo)
	{
		m_cb(ret, (ServerEngine::BattleData&)battleInfo);
	}
    
    virtual void callback_continuePVPFight_exception(taf::Int32 ret)
    {
    	ServerEngine::BattleData nullBattle;
     	m_cb(en_FightResult_SysError, nullBattle);
   	}

private:

	DelegatePVPFight m_cb;
};

void FightFactory::AsyncPVEFight(DelegatePVEFight cb, const ServerEngine::ActorCreateContext& attacker, int iMonsterGrpID, const ServerEngine::FightContext& ctx)
{
	IJZMessageLayer* pMsgLayer = getComponent<IJZMessageLayer>(COMPNAME_MessageLayer, IID_IJZMessageLayer);
	assert(pMsgLayer);

	try
	{
		pMsgLayer->AsyncPVEFight(new FightPVECallback(cb), attacker, iMonsterGrpID, ctx);
	}
	catch(...)
	{
		ServerEngine::BattleData nullBattle;
     	      cb(en_FightResult_SysError, nullBattle);
	}
}


void FightFactory::AsyncMultiPVEFight(DelegatePVEFight cb,  const ServerEngine::ActorCreateContext& roleInfo, vector<int> monsterGrpList, const ServerEngine::FightContext& ctx)
{
	IJZMessageLayer* pMsgLayer = getComponent<IJZMessageLayer>(COMPNAME_MessageLayer, IID_IJZMessageLayer);
	assert(pMsgLayer);

	try
	{
		pMsgLayer->AsyncMultiPVEFight(new FightPVECallback(cb), roleInfo, monsterGrpList, ctx);
	}
	catch(...)
	{
		ServerEngine::BattleData nullBattle;
     	cb(en_FightResult_SysError, nullBattle);
	}
}



void FightFactory::AsyncPVPFight(DelegatePVPFight cb, const ServerEngine::ActorCreateContext& attacker, const ServerEngine::ActorCreateContext& target, const ServerEngine::FightContext& ctx)
{
	IJZMessageLayer* pMsgLayer = getComponent<IJZMessageLayer>(COMPNAME_MessageLayer, IID_IJZMessageLayer);
	assert(pMsgLayer);

	try
	{
		pMsgLayer->AsyncPVPFight(new FightPVPCallback(cb), attacker, target, ctx);
	}
	catch(...)
	{
		ServerEngine::BattleData nullBattle;
     	cb(en_FightResult_SysError, nullBattle);
	}
}

void FightFactory::AsyncContinuePVEFight(DelegatePVEFight cb, const ServerEngine::ActorCreateContext& attacker, int iMonsterGrpID, const ServerEngine::FightContext& ctx)
{
	IJZMessageLayer* pMsgLayer = getComponent<IJZMessageLayer>(COMPNAME_MessageLayer, IID_IJZMessageLayer);
	assert(pMsgLayer);

	try
	{
		pMsgLayer->AsyncContinuePVEFight(new FightPVECallback(cb), attacker, iMonsterGrpID, ctx);
	}
	catch(...)
	{
		ServerEngine::BattleData nullBattle;
     	cb(en_FightResult_SysError, nullBattle);
	}
}

void FightFactory::AsyncContinuePVPFight(DelegatePVPFight cb, const ServerEngine::ActorCreateContext& roleInfo, const ServerEngine::ActorCreateContext& targetRileInfo, const ServerEngine::FightContext& ctx)
{
	IJZMessageLayer* pMsgLayer = getComponent<IJZMessageLayer>(COMPNAME_MessageLayer, IID_IJZMessageLayer);
	assert(pMsgLayer);

	try
	{
		pMsgLayer->AsyncContinuePVPFight(new FightPVPCallback(cb), roleInfo, targetRileInfo, ctx);
	}
	catch(...)
	{
		ServerEngine::BattleData nullBattle;
     	cb(en_FightResult_SysError, nullBattle);
	}
}


void FightFactory::AsyncContinueMultiPVEFight(DelegatePVEFight cb, const ServerEngine::ActorCreateContext& roleInfo, vector<int> monsterGrpList, const ServerEngine::FightContext& ctx)
{
	IJZMessageLayer* pMsgLayer = getComponent<IJZMessageLayer>(COMPNAME_MessageLayer, IID_IJZMessageLayer);
	assert(pMsgLayer);

	try
	{
		pMsgLayer->AsyncContinueMultiPVEFight(new FightPVECallback(cb), roleInfo, monsterGrpList, ctx);
	}
	catch(...)
	{
		ServerEngine::BattleData nullBattle;
     	cb(en_FightResult_SysError, nullBattle);
	}
}


EventServer* FightFactory::getEventServer()
{
	return &m_eventServer;
}


const vector<HEntity>* FightFactory::getMemberList()
{
	if(!m_pCurPVEFightCtx) return NULL;

	return &m_memberList;
}



