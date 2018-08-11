#include "DungeonServerPch.h"
#include "DungeonFactory.h"
#include "IGodAnimalSystem.h"
#include "DungeonSystem.h"
#include "DropFactory.h"
#include "IHeroSystem.h"
#include "IFormationSystem.h"
#include "IEquipBagSystem.h"

extern "C" IObject* createDungeonSystem()
{
	return new DungeonSystem;
}


DungeonSystem::DungeonSystem():m_iCurFightingScene(0), m_bCanFp(false),b_ActorIsFight(false),m_iLastFightSecond(0)
{
}

DungeonSystem::~DungeonSystem()
{
	IEntity* pEntity = getEntityFromHandle(m_hEntity);
	if(pEntity)
	{
		pEntity->getEventServer()->unsubscribeEvent(EVENT_ENTITY_SENDACTOR_TOCLIENT, this, &DungeonSystem::onEventSendToClient);
	}
}

Uint32 DungeonSystem::getSubsystemID() const
{
	return IID_IDungeonSystem;
}

Uint32 DungeonSystem::getMasterHandle()
{
	return m_hEntity;
}

bool DungeonSystem::create(IEntity* pEntity, const std::string& strData)
{
	m_hEntity = pEntity->getHandle();
	
	initDungeonSysData(strData);

	// 注册事件
	pEntity->getEventServer()->subscribeEvent(EVENT_ENTITY_SENDACTOR_TOCLIENT, this, &DungeonSystem::onEventSendToClient);

	m_saoDangPointResume.init(m_hEntity);

	ITimeRangeValue* pTimeRangeValue = getComponent<ITimeRangeValue>(COMPNAME_TimeRangeValue, IID_ITimeRangeValue);
	assert(pTimeRangeValue);

	pTimeRangeValue->addRangeValue(&m_saoDangPointResume, "ResumeSaoDang");
	
	return true;
}

void DungeonSystem::initDungeonSysData(const string& strData)
{
	if(strData.size() > 0)
	{
		ServerEngine::JceToObj(strData, m_dungeonSysData);
		return;
	}
	
	IDungeonFactory* pDungeonFactory = getComponent<IDungeonFactory>(COMPNAME_DungeonFactory, IID_IDungeonFactory);
	assert(pDungeonFactory);

	int iFirstSectionID = pDungeonFactory->getFirstSection();
	assert(iFirstSectionID > 0);

	// 这里只push了普通关卡
	m_dungeonSysData.sectionList.push_back(iFirstSectionID);

	// 填充第一个关卡
	const DungeonSection* pSection = pDungeonFactory->querySection(iFirstSectionID);
	assert(pSection && pSection->sceneIDList.size() > 0);

	int iFirstSceneID = pSection->sceneIDList.front();

	ServerEngine::SceneRecord tmpRecord;
	tmpRecord.iSceneID = iFirstSceneID;
	m_dungeonSysData.m_mapSceneRecord[iFirstSceneID] = tmpRecord;

	m_dungeonSysData.newSceneList[iFirstSceneID] = true;

	// 装备关卡处理
	vector<int> equipDungeonIDList = pDungeonFactory->getSectionList(GSProto::en_SectionType_Equip);
	for(size_t i = 0; i < equipDungeonIDList.size(); i++)
	{
		const DungeonSection* pEquipSectionCfg = pDungeonFactory->querySection(equipDungeonIDList[i]);
		assert(pEquipSectionCfg);

		ServerEngine::SceneRecord tmpEquipSceneRecord;
		tmpEquipSceneRecord.iSceneID = pEquipSectionCfg->sceneIDList.front();
		m_dungeonSysData.m_mapSceneRecord[tmpEquipSceneRecord.iSceneID] = tmpEquipSceneRecord;
	}
}

bool DungeonSystem::createComplete()
{
	return true;
}

const std::vector<Uint32>& DungeonSystem::getSupportMessage()
{
	static vector<Uint32> resultList;

	if(resultList.size() == 0)
	{
		resultList.push_back(GSProto::CMD_QUERY_DSECTIONDETAIL);
		resultList.push_back(GSProto::CMD_DUNGEON_FIGHT);
		resultList.push_back(GSProto::CMD_GET_SECTION_AWARD);

		resultList.push_back(GSProto::CMD_DUNGEON_SAODANG);

		resultList.push_back(GSProto::CMD_EQUIPDUNGEON_QUERY_EQUIPINFO);
		resultList.push_back(GSProto::CMD_EQUIPDUNGEON_FP);
	}

	return resultList;
}

void DungeonSystem::onMessage(QxMessage* pMessage)
{
	assert(pMessage->dwMsgLen == sizeof(GSProto::CSMessage) );
	const GSProto::CSMessage& msg = *(const GSProto::CSMessage*)(pMessage->pMsgDataBuff);

	switch(msg.icmd() )
	{
		case GSProto::CMD_QUERY_DSECTIONDETAIL:
			onQuerySectionMsg(msg);
			break;

		case GSProto::CMD_DUNGEON_FIGHT:
			onDungeonFightMsg(msg);
			break;

		case GSProto::CMD_GET_SECTION_AWARD:
			onGetSectionAwardMsg(msg);
			break;

		case GSProto::CMD_DUNGEON_SAODANG:
			onReqSaoDangMsg(msg);
			break;

		case GSProto::CMD_EQUIPDUNGEON_QUERY_EQUIPINFO:
			onReqQueryFpEquipList(msg);
			break;

		case GSProto::CMD_EQUIPDUNGEON_FP:
			onReqExecFp(msg);
			break;
	}
}


void DungeonSystem::onReqQueryFpEquipList(const GSProto::CSMessage& message)
{
	if(!m_bCanFp) return;

	GSProto::CMD_EQUIPDUNGEON_QUERY_EQUIPINFO_SC scMsg;
	for(size_t i = 0; i < m_fpEquipList.size(); i++)
	{
		IItem* pEquip = getItemFromHandle(m_fpEquipList[i]);
		assert(pEquip);

		int iBaseID = pEquip->getProperty(PROP_ITEM_BASEID, 0);
		scMsg.add_szequipidlist(iBaseID);
	}

	IEntity* pMaster = getEntityFromHandle(m_hEntity);
	assert(pMaster);

	pMaster->sendMessage(GSProto::CMD_EQUIPDUNGEON_QUERY_EQUIPINFO, scMsg);
}


int DungeonSystem::calcFpCostGold()
{
	// 首次免费
	if(m_canGetedPosList.size() >= m_fpEquipList.size() )
	{
		return 0;
	}

	IGlobalCfg *pGlobal = getComponent<IGlobalCfg>(COMPNAME_GlobalCfg, IID_IGlobalCfg);
	assert(pGlobal);

	if(m_fpEquipList.size() - m_canGetedPosList.size() == 1)
	{
		int iVIPLevel = pGlobal->getInt("翻牌免费2次VPP", 6);

		IEntity* pMaster = getEntityFromHandle(m_hEntity);
		assert(pMaster);

		int iCurVIPLV = pMaster->getProperty(PROP_ACTOR_VIPLEVEL, 0);
		if(iCurVIPLV >= iVIPLevel)
		{
			return 0;
		}
	}

	int iCostGold = pGlobal->getInt("翻牌消耗金币", 100);

	return iCostGold;
}


void DungeonSystem::onReqExecFp(const GSProto::CSMessage& message)
{
	if(!m_bCanFp) return;

	if(m_fpEquipList.size() == 0) return;

	if(m_canGetedPosList.size() == 0) return;

	/*IGlobalCfg *pGlobal = getComponent<IGlobalCfg>(COMPNAME_GlobalCfg, IID_IGlobalCfg);
	assert(pGlobal);*/

	GSProto::CMD_EQUIPDUNGEON_FP_CS req;
	if(!req.ParseFromString(message.strmsgbody() ) )
	{
		return;
	}

	int iReqPos = req.ipos();
	
	int iCostGold = calcFpCostGold();//pGlobal->getInt("翻牌消耗金币", 100);*/

	IEntity* pMaster = getEntityFromHandle(m_hEntity);
	assert(pMaster);

	int iTkGold = pMaster->getProperty(PROP_ACTOR_GOLD, 0);
	if(iTkGold < iCostGold)
	{
		pMaster->sendErrorCode(ERROR_NEED_GOLD);
		return;
	}

	pMaster->changeProperty(PROP_ACTOR_GOLD, 0-iCostGold, GSProto::en_Reason_EquipFB_FPCostGold);

	IRandom* pRandom = getComponent<IRandom>("Random", IID_IMiniAprRandom);
	assert(pRandom);

	
	int iRandV = pRandom->random() % m_canGetedPosList.size();
	int iPos = m_canGetedPosList[iRandV];
	m_canGetedPosList.erase(m_canGetedPosList.begin() + iRandV);

	IEquipBagSystem* pEquipBagSys = static_cast<IEquipBagSystem*>(pMaster->querySubsystem(IID_IEquipBagSystem));
	assert(pEquipBagSys);

	pEquipBagSys->addEquipByHandle(m_fpEquipList[iPos], GSProto::en_Reason_EquipFB_FPCostGold, false);

	GSProto::CMD_EQUIPDUNGEON_FP_SC scMsg;
	scMsg.set_ipos(iReqPos);
	GSProto::EquipInfo* pScEquipInfo = scMsg.mutable_equipinfo();

	IItem* pEquip = getItemFromHandle(m_fpEquipList[iPos]);
	assert(pEquip);

	pEquip->packScEquipInfo(*pScEquipInfo, iPos);

	int iNextCostGold = calcFpCostGold();
	scMsg.set_inextcostgold(iNextCostGold);

	pMaster->sendMessage(GSProto::CMD_EQUIPDUNGEON_FP, scMsg);
}


void DungeonSystem::onReqSaoDangMsg(const GSProto::CSMessage& message)
{
	GSProto::CMD_DUNGEON_SAODANG_CS req;
	if(!req.ParseFromString(message.strmsgbody() ) )
	{
		return;
	}

	int iSceneID = req.isceneid();

	IEntity* pMaster = getEntityFromHandle(m_hEntity);
	assert(pMaster);
	int iActorLevel = pMaster->getProperty( PROP_ENTITY_LEVEL, 0);
	IGlobalCfg *pGlobal = getComponent<IGlobalCfg>(COMPNAME_GlobalCfg, IID_IGlobalCfg);
	assert(pGlobal);
	int iSaoTangOpenLevel = pGlobal->getInt("扫荡开启等级", 15);
	if(iActorLevel < iSaoTangOpenLevel )
	{
		pMaster->sendErrorCode(ERROR_DUNGON_SAODANG_OPENLEVEL);
		return;
	}
	
	// 判断是否3星
	int iPassStar = 0;
	if(!getSceneStar(iSceneID, iPassStar) || (iPassStar != GSProto::en_SceneStar_3) )
	{
		pMaster->sendErrorCode(ERROR_CANNOTSAODANG_STAR3);
		return;
	}

	int iCostPhyStrength = 0;
	bool bHasPassLimit = false;
	if(!checkSceneFightCond(iSceneID, iCostPhyStrength, bHasPassLimit) )
	{
		return;
	}
	
	// 判断扫荡次数是否足够
	int iSaoDangLimit = pMaster->getProperty(PROP_ENTITY_SAODANGLIMI, 0);
	int iSaoDangPoint = pMaster->getProperty(PROP_ENTITY_SAODANGPOINT, 0);
	if( (iSaoDangLimit != 0) && (iSaoDangPoint <= 0) )
	{
		pMaster->sendErrorCode(ERROR_SAODANG_NOPOINT);
		return;
	}
	
	// 扣除体力、扣除扫荡次数
	// 增加挑战次数吧
	if(bHasPassLimit)
	{
		getDayResetValue(m_dungeonSysData.m_mapSceneRecord[iSceneID].passTimes).iValue++;
	}

	
	// 扣除体力,记录日志
	PLAYERLOG(pMaster)<<"ChgPhystrength|"<<iCostPhyStrength<<"|"<<"DungeonSaoDang"<<endl;
	pMaster->changeProperty(PROP_ENTITY_PHYSTRENGTH, 0-iCostPhyStrength, GSProto::en_Reason_DungeonFight);

	// 扣除扫荡点
	pMaster->changeProperty(PROP_ENTITY_SAODANGPOINT, -1, GSProto::en_Reason_DungeonFight);

	// 计算奖励
	GSProto::Cmd_Sc_CommFightResult scCommFightResult;
	sceneAwardAndEvent(iSceneID, scCommFightResult, false, false, false);

	// 通知客户端
	GSProto::CMD_DUNGEON_SAODANG_SC scMsg;
	*scMsg.mutable_awardinfo() = *scCommFightResult.mutable_awardresult();
	scMsg.set_iherogetexp(scCommFightResult.iherogetexp() );
	*scMsg.mutable_szheroaward() = *scCommFightResult.mutable_szheroaward();
	scMsg.set_bcanfp(scCommFightResult.bcanfp());
	scMsg.set_icostgold( scCommFightResult.icostgold());
	scMsg.set_iextraexp(scCommFightResult.iextraexp());
	
	
	pMaster->sendMessage(GSProto::CMD_DUNGEON_SAODANG, scMsg);

	//任务系统
/*	{
		EventArgs args;
		args.context.setInt("times",1);
		args.context.setInt("entity",m_hEntity);
		pMaster->getEventServer()->setEvent(EVENT_ENTITY_PASSSCENE_COUNT, args);
	}*/
}


void DungeonSystem::onGetSectionAwardMsg(const GSProto::CSMessage& message)
{
	GSProto::Cmd_Cs_GetSectionAward csReq;
	if(!csReq.ParseFromString(message.strmsgbody() ) )
	{
		return;
	}

	IEntity* pEntity = getEntityFromHandle(m_hEntity);
	assert(pEntity);

	int iSectionID = csReq.isectionid();

	DungeonFactory* pDungeonFactory = static_cast<DungeonFactory*>(getComponent<IDungeonFactory>(COMPNAME_DungeonFactory, IID_IDungeonFactory));
	assert(pDungeonFactory);

	const DungeonSection* pSection = pDungeonFactory->querySection(iSectionID);
	if(!pSection)
	{
		return;
	}

	// 是否完美通关了
	if(!isSectionPerfectPass(iSectionID) )
	{
		pEntity->sendErrorCode(ERROR_DUNGEON_NOPERFECT);
		return;
	}

	// 是否已经领取
	if(m_dungeonSysData.perfectPassSectionMap.find(iSectionID) != m_dungeonSysData.perfectPassSectionMap.end() )
	{
		pEntity->sendErrorCode(ERROR_SECTION_GETED);
		return;
	}

	// 如果英雄背包满了，不能领取
	IHeroSystem* pHeroSys = static_cast<IHeroSystem*>(pEntity->querySubsystem(IID_IHeroSystem));
	assert(pHeroSys);

	if(pHeroSys->checkHeroFull() )
	{
		pEntity->sendErrorCode(ERROR_TOUCH_MAXHERO);
		return;
	}

	
	//关闭上浮
	
	CloseAttCommUP close(m_hEntity);

	m_dungeonSysData.perfectPassSectionMap[iSectionID] = true;

	for(size_t i = 0; i < pSection->perfectIDList.size(); i++)
	{
		const PerfectAwardCfg* pPerfectAwardCfg = pDungeonFactory->queryPerfectAward(pSection->perfectIDList[i]);
		assert(pPerfectAwardCfg);

		doAwardPerfect(pPerfectAwardCfg);
	}

	
	// 通知领取成功
	GSProto::CMD_GET_SECTION_AWARD_SC scMsg;
	scMsg.set_iisok(1);
	scMsg.set_isectionid(iSectionID);
	
	pEntity->sendMessage(GSProto::CMD_GET_SECTION_AWARD, scMsg);
}


int LifeAtt2Prop(int iLifeAtt);

void DungeonSystem::doAwardPerfect(const PerfectAwardCfg* pPerfectCfg)
{
	assert(pPerfectCfg);

	IEntity* pActor = getEntityFromHandle(m_hEntity);
	assert(pActor);

	IHeroSystem* pHeroSys = static_cast<IHeroSystem*>(pActor->querySubsystem(IID_IHeroSystem));
	assert(pHeroSys);

	IGodAnimalSystem* pGodAnimalSys = static_cast<IGodAnimalSystem*>(pActor->querySubsystem(IID_IGodAnimalSystem));
	assert(pGodAnimalSys);

	IItemSystem* pItemSys = static_cast<IItemSystem*>(pActor->querySubsystem(IID_IItemSystem));
	assert(pItemSys);
	
	switch(pPerfectCfg->iType)
	{

		case GSProto::en_ShopType_LifeAttType:	
			{
				if(pPerfectCfg->iItemID == GSProto::en_LifeAtt_Exp)
				{
					pActor->addExp(pPerfectCfg->iCount);
				}
				else
				{
					int iPropID = LifeAtt2Prop(pPerfectCfg->iItemID);
					assert(iPropID >= 0);
					pActor->changeProperty(iPropID, pPerfectCfg->iCount, GSProto::en_Reason_PerfectPassSection);
				}
			}
			break;
			
		case GSProto::en_Shop_GoodType_Hero:
			pHeroSys->addHeroWithLevelStep(pPerfectCfg->iItemID, pPerfectCfg->iLevelStep, true, GSProto::en_Reason_PerfectPassSection);
			break;

		case GSProto::en_Shop_GoodType_GodAnimal:
			pGodAnimalSys->addGodAnimalWithLevelStep(pPerfectCfg->iItemID, true, pPerfectCfg->iLevelStep);
			break;

		case GSProto::en_Shop_GoodType_Item:
			pItemSys->addItem(pPerfectCfg->iItemID, pPerfectCfg->iCount, GSProto::en_Reason_PerfectPassSection);
			break;
			
	}
}



struct DungeonFightCb
{
	DungeonFightCb(HEntity hEntity, int iSceneID):m_hEntity(hEntity), m_iSceneID(iSceneID){}

	void operator() (int iRet, const ServerEngine::BattleData& data)
	{
		IEntity* pEntity = getEntityFromHandle(m_hEntity);
		if(!pEntity) return;

		DungeonSystem* pDungeonSys = static_cast<DungeonSystem*>(pEntity->querySubsystem(IID_IDungeonSystem));
		if(!pDungeonSys) return;

		pDungeonSys->processFightResult(m_iSceneID, iRet, data);
	}

	HEntity m_hEntity;
	int m_iSceneID;
};


void DungeonSystem::onDungeonFightMsg(const GSProto::CSMessage& message)
{
	GSProto::Cmd_Cs_DungeonFight reqMsg;
	if(!reqMsg.ParseFromString(message.strmsgbody() ) )
	{
		return;
	}
	
	IEntity* pEntity = getEntityFromHandle(m_hEntity);
	assert(pEntity);

	if(b_ActorIsFight)
	{
		IGlobalCfg* pGlobalCfg = getComponent<IGlobalCfg>(COMPNAME_GlobalCfg, IID_IGlobalCfg);
		assert(pGlobalCfg);

		// 验证CD时间
		Uint32 dwCurTime = time(0);
		Uint32 dwPVECD = (Uint32)pGlobalCfg->getInt("PVECD", 2);
		if(dwCurTime < (m_iLastFightSecond + dwPVECD) )
		{
			pEntity->sendErrorCode(ERROR_FIGHT_TOO_FIGHT);
			return;
		}
	}

	int iSceneID = reqMsg.isceneid();

	int iCostPhyStrength = 0;
	bool bHasPassLimit = false;
	if(!checkSceneFightCond(iSceneID, iCostPhyStrength, bHasPassLimit) )
	{
		return;
	}

	// 增加挑战次数吧
	if(bHasPassLimit)
	{
		getDayResetValue(m_dungeonSysData.m_mapSceneRecord[iSceneID].passTimes).iValue++;
	}
	
	// 扣除体力,记录日志
	//PLAYERLOG(pEntity)<<"ChgPhystrength|"<<iCostPhyStrength<<"|"<<"DungeonFight"<<endl;
	//pEntity->changeProperty(PROP_ENTITY_PHYSTRENGTH, 0-iCostPhyStrength, GSProto::en_Reason_DungeonFight);

	IFightSystem* pFightSys = static_cast<IFightSystem*>(pEntity->querySubsystem(IID_IFightSystem));
	assert(pFightSys);

	
	ServerEngine::FightContext ctx;
	ctx.iFightType = GSProto::en_FightType_Scene;
	ctx.iSceneID = iSceneID;

	// 未通关，才播放剧情
	IDungeonFactory* pDungeonFactory = getComponent<IDungeonFactory>(COMPNAME_DungeonFactory, IID_IDungeonFactory);
	assert(pDungeonFactory);
	
	const DungeonScene* pDungeonScene = pDungeonFactory->queryScene(iSceneID);
	assert(pDungeonScene);

	// 如果是装备FB修改战斗类型
	const DungeonSection* pDungeonSection = pDungeonFactory->querySection(pDungeonScene->iOwnSectionID);
	assert(pDungeonSection);

	if(pDungeonSection->iSectionType == GSProto::en_SectionType_Equip)
	{
		ctx.iFightType = GSProto::en_FightType_EquipScene;
	}
	
	int iTmpPassStar = 0;
	if(!getSceneStar(iSceneID, iTmpPassStar) || (iTmpPassStar == GSProto::en_SceneStar_Null) )
	{
		ctx.dlgList = pDungeonScene->dlgList;
		ctx.bBeginDlg = pDungeonScene->bBeginDlg;
	}

	m_iCurFightingScene = iSceneID;
	b_ActorIsFight = true;
	m_iLastFightSecond = time(0);
	pFightSys->AsyncMultPVEFight(DungeonFightCb(m_hEntity, iSceneID), pDungeonScene->monsterGrpList, ctx);
}


bool DungeonSystem::checkSceneFightCond(int iSceneID, int& iCostPhyStrength, bool& bHasPassLimit)
{
	IEntity* pEntity = getEntityFromHandle(m_hEntity);
	assert(pEntity);

	// 验证是否开启
	if(!isSceneOpened(iSceneID) )
	{
		return false;
	}

	IDungeonFactory* pDungeonFactory = getComponent<IDungeonFactory>(COMPNAME_DungeonFactory, IID_IDungeonFactory);
	assert(pDungeonFactory);
	
	const DungeonScene* pDungeonScene = pDungeonFactory->queryScene(iSceneID);
	assert(pDungeonScene);

	// 是否要求英雄背包满
	IHeroSystem* pHeroSys = static_cast<IHeroSystem*>(pEntity->querySubsystem(IID_IHeroSystem));
	assert(pHeroSys);

	if(pDungeonScene->bConditionAddHero && pHeroSys->checkHeroFull() )
	{
		pEntity->sendErrorCode(ERROR_TOUCH_MAXHERO);
		return false;
	}

	map<int, ServerEngine::SceneRecord>::iterator it = m_dungeonSysData.m_mapSceneRecord.find(iSceneID);
	assert(it != m_dungeonSysData.m_mapSceneRecord.end() );
	
	ServerEngine::SceneRecord& refSceneRecord = it->second;

	// 验证战斗次数是否有限制
	if(pDungeonScene->iLimitPassCount > 0)
	{
		bHasPassLimit = true;
		if(getDayResetValue(refSceneRecord.passTimes).iValue >= pDungeonScene->iLimitPassCount)
		{
			pEntity->sendErrorCode(ERROR_DUNGEON_COUNTLIMIT);
			return false;
		}
	}

	// 验证体力是否足够
	IGlobalCfg* pGlobalCfg = getComponent<IGlobalCfg>(COMPNAME_GlobalCfg, IID_IGlobalCfg);
	assert(pGlobalCfg);

	int iNeedPhyStrength = pGlobalCfg->getInt("关卡消耗体力", 2);

	int iTkPhystrength = pEntity->getProperty(PROP_ENTITY_PHYSTRENGTH, 0);
	if(iTkPhystrength < iNeedPhyStrength)
	{
		//pEntity->sendErrorCode(ERROR_NEED_PHYSTRENGTH);
		IItemSystem *pItemSystem = static_cast<IItemSystem*>(pEntity->querySubsystem(IID_IItemSystem));
		assert(pItemSystem);
		pItemSystem->onReqBuyPhyStength();
		return false;
	}

	iCostPhyStrength = iNeedPhyStrength;

	return true;
}


bool DungeonSystem::canSkipCurScene()
{
	int iPassStar = 0;
	if(getSceneStar(m_iCurFightingScene, iPassStar) && (GSProto::en_SceneStar_3 == iPassStar) )
	{
		return true;
	}

	return false;
}

void DungeonSystem::processFightResult(int iScenID, int iRet, const ServerEngine::BattleData& data)
{
	// 这个数值仅在战斗回调处理前有效
	m_iCurFightingScene = 0;
	b_ActorIsFight = false;
	
	IGlobalCfg* pGlobalCfg = getComponent<IGlobalCfg>(COMPNAME_GlobalCfg, IID_IGlobalCfg);
	assert(pGlobalCfg);
	IEntity* pEntity = getEntityFromHandle(m_hEntity);
	assert(pEntity);
	
	if(iRet == en_FightResult_SysError)
	{
		SvrErrLog("Scene Fight Error|%d", iScenID);
		//CloseAttCommUP close(m_hEntity);
		//int iNeedPhyStrength = pGlobalCfg->getInt("关卡消耗体力", 2);
		//pEntity->changeProperty(PROP_ENTITY_PHYSTRENGTH, iNeedPhyStrength, GSProto::en_Reason_DungeonPhyStrengthReback);

		return;
	}




	// 首次通关才检测
	int iFirstSceneID = pGlobalCfg->getInt("第一个关卡", 1);
	if( (data.bQuit != 0) && (iFirstSceneID == iScenID) )
	{
		int iOldStar = GSProto::en_SceneStar_Null;
		if(!getSceneStar(iScenID, iOldStar) || (iOldStar == GSProto::en_SceneStar_Null) )
		{
			pEntity->sendErrorCode(ERROR_CANNOTQUIT_FIRSTSCENE);
			return;
		}
	}

	// 回一个quit吧
	if(data.bQuit != 0)
	{
		pEntity->sendMessage(GSProto::CMD_FIGHT_QUIT);
		((ServerEngine::BattleData&)data).bQuitSuccess = 1;
	}

	IDungeonFactory* pDungeonFactory = getComponent<IDungeonFactory>(COMPNAME_DungeonFactory, IID_IDungeonFactory);
	assert(pDungeonFactory);
	
	const DungeonScene* pDungeonScene = pDungeonFactory->queryScene(iScenID);
	assert(pDungeonScene);
	
	//副本关闭上浮
	CloseAttCommUP close(m_hEntity);

	IFightSystem* pFightSys = static_cast<IFightSystem*>(pEntity->querySubsystem(IID_IFightSystem));
	assert(pFightSys);

	if(!data.bAttackerWin)
	{
		CloseAttCommUP close(m_hEntity);
		
		GSProto::Cmd_Sc_CommFightResult scFightResult;
		scFightResult.set_isceneid(iScenID);
		scFightResult.set_iissuccess(0);

		pEntity->sendMessage(GSProto::CMD_COMM_FIGHTRESULT, scFightResult);
		pFightSys->sendBattleFin();

		// 体力归还
		//int iNeedPhyStrength = pGlobalCfg->getInt("关卡消耗体力", 2);
		//pEntity->changeProperty(PROP_ENTITY_PHYSTRENGTH, iNeedPhyStrength, GSProto::en_Reason_DungeonPhyStrengthReback);

		// 通关次数回退
		if(pDungeonScene->iLimitPassCount > 0)
		{
			map<int, ServerEngine::SceneRecord>::iterator it = m_dungeonSysData.m_mapSceneRecord.find(iScenID);
			assert(it != m_dungeonSysData.m_mapSceneRecord.end() );
			ServerEngine::SceneRecord& refSceneRecord = it->second;
			getDayResetValue(refSceneRecord.passTimes).iValue--;
		}
		
		return;
	}

	int iNeedPhyStrength = pGlobalCfg->getInt("关卡消耗体力", 2);
	pEntity->changeProperty(PROP_ENTITY_PHYSTRENGTH, 0-iNeedPhyStrength, GSProto::en_Reason_DungeonPhyStrengthReback);
	
	// 胜利了，计算奖励
	GSProto::Cmd_Sc_CommFightResult scCommFightResult;
	scCommFightResult.set_isceneid(iScenID);
	scCommFightResult.set_iissuccess(1);
	scCommFightResult.set_istar(data.iStar);

	// 判断是否首次通关
	bool bFirstPassScene = true;
	int iOldStar = GSProto::en_SceneStar_Null;
	if(getSceneStar(iScenID, iOldStar) && (iOldStar != GSProto::en_SceneStar_Null) )
	{
		bFirstPassScene = false;
	}
	
	// 更新场景评星
	bool bPerfectPassSection = updateSceneStar(iScenID, data.iStar);

	bool bFirstPerfectPass = false;
	if( (data.iStar == GSProto::en_SceneStar_3) && (data.iStar != iOldStar) )
	{
		bFirstPerfectPass = true;
	}

	sceneAwardAndEvent(iScenID, scCommFightResult, bPerfectPassSection, bFirstPassScene, bFirstPerfectPass);
	
	// 通知客户端战斗结果和奖励结果
	pEntity->sendMessage(GSProto::CMD_COMM_FIGHTRESULT, scCommFightResult);
	pFightSys->sendBattleFin();

	// 如果是新通关一个章节，需要通知客户端做表现
	if(bFirstPassScene && getSectionHavePass(pDungeonScene->iOwnSectionID) )
	{
		GSProto::CMD_NEWPASS_SECTION_SC scMsg;
		scMsg.set_ipassedsectionid(pDungeonScene->iOwnSectionID);
		pEntity->sendMessage(GSProto::CMD_NEWPASS_SECTION, scMsg);
	}

	
}


void DungeonSystem::doFpGenerate(const DungeonScene* pDungeonScene)
{
	m_bCanFp = true;

	int iEliteEquip = randomFpEquip(pDungeonScene->eliteEquipList);
	

	assert(iEliteEquip >= 0);

	IItemFactory* pItemFactory = getComponent<IItemFactory>(COMPNAME_ItemFactory, IID_IItemFactory);
	assert(pItemFactory);

	vector<HItem> itemList;
	if(pItemFactory->createItemFromID(iEliteEquip, 1, itemList) )
	{
		assert(itemList.size() == 1);
		m_fpEquipList.push_back(itemList[0]);
	}

	for(int i = 0; i < 3; i++)
	{
		int iNormalEquip = randomFpEquip(pDungeonScene->normalEquipList);
		itemList.clear();

		if(pItemFactory->createItemFromID(iNormalEquip, 1, itemList) )
		{
			assert(itemList.size() == 1);
			m_fpEquipList.push_back(itemList[0]);
		}
	}
	
	for(int i = 0; i < 4; i ++)m_canGetedPosList.push_back(i);
}

int DungeonSystem::randomFpEquip(const vector<ChanceCfg>& chanceList)
{
	int iSumChance = 0;
	for(int i = 0; i < (int)chanceList.size(); i++)
	{
		iSumChance += chanceList[i].iChance;
	}

	IRandom* pRandom = getComponent<IRandom>("Random", IID_IMiniAprRandom);
	assert(pRandom);

	assert(iSumChance >= 0);
	int iRandV = pRandom->random() % iSumChance;

	int iSum = 0;
	for(int i = 0; i < (int)chanceList.size(); i++)
	{
		iSum += chanceList[i].iChance;
		if(iRandV < iSum)
		{
			return chanceList[i].iID;
			break;
		}
	}

	return 0;
}


void DungeonSystem::sceneAwardAndEvent(int iSceneID, GSProto::Cmd_Sc_CommFightResult& scCommFightResult, bool bPerfectPassSection, bool bFirstPassScene, bool bFirstPerfectPass)
{
	m_bCanFp = false;
	m_fpEquipList.clear();
	m_canGetedPosList.clear();

	IEntity* pEntity = getEntityFromHandle(m_hEntity);
	assert(pEntity);

	IDungeonFactory* pDungeonFactory = getComponent<IDungeonFactory>(COMPNAME_DungeonFactory, IID_IDungeonFactory);
	assert(pDungeonFactory);
	
	const DungeonScene* pDungeonScene = pDungeonFactory->queryScene(iSceneID);
	assert(pDungeonScene);

	IGlobalCfg* pGlobal = getComponent<IGlobalCfg>(COMPNAME_GlobalCfg, IID_IGlobalCfg);
	assert(pGlobal);

	int iExtraExp = 0;
	processDungeonAward(iSceneID, *scCommFightResult.mutable_awardresult(), bFirstPassScene, bFirstPerfectPass,iExtraExp);
	
	scCommFightResult.set_iextraexp(iExtraExp);
	
	// 执行奖励(modifyed by feiwu, 放到升级后面吧)
	IDropFactory* pDropFactory = getComponent<IDropFactory>(COMPNAME_DropFactory, IID_IDropFactory);
	assert(pDropFactory);

	IRandom* pRandom = getComponent<IRandom>("Random", IID_IMiniAprRandom);
	assert(pRandom);
	
	pDropFactory->excuteDrop(m_hEntity, *scCommFightResult.mutable_awardresult(), GSProto::en_Reason_DungeonFight);

	// 出战英雄获得额外修为
	if(pDungeonScene->iHeroGetExp > 0)
	{
		doHeroExpAward(scCommFightResult, pDungeonScene->iHeroGetExp);
	}

	// 触发翻牌检测
	int iPassStar = 0;
	if( (pDungeonScene->fpChanceList.size() > 0) && getSceneStar(iSceneID, iPassStar) )
	{
		map<int, int>::const_iterator it = pDungeonScene->fpChanceList.find(iPassStar);
		if(it != pDungeonScene->fpChanceList.end() )
		{
			int iChance = it->second;
			int iRandV = pRandom->random() % 10000;
			if(iRandV < iChance)
			{
				scCommFightResult.set_bcanfp(true);

				//int iCostGold = pGlobal->getInt("翻牌消耗金币", 100);
				scCommFightResult.set_icostgold(0);
				doFpGenerate(pDungeonScene);
			}
		}
	}

	// 触发通关场景事件
	{
		EventArgs args;
		args.context.setInt("entity", m_hEntity);
		args.context.setInt("sceneid", iSceneID);
		pEntity->getEventServer()->setEvent(EVENT_ENTITY_PASSSCENE, args);
	}

	if(bPerfectPassSection)
	{
		EventArgs args;
		args.context.setInt("entity", m_hEntity);
		args.context.setInt("sectionid", pDungeonScene->iOwnSectionID);
		args.context.setInt("firstperfectpass", bFirstPerfectPass);
		pEntity->getEventServer()->setEvent(EVENT_ENTITY_PERFECT_PASSSECTION, args);
	}

	// 发送战斗奖励消息的部分，挪到外面了
	
	//任务 通关关卡
 	{
		EventArgs args;
		args.context.setInt("times",1);
		args.context.setInt("entity",m_hEntity);
		pEntity->getEventServer()->setEvent(EVENT_ENTITY_PASSSCENE_COUNT, args);
	}
}


void DungeonSystem::doHeroExpAward(GSProto::Cmd_Sc_CommFightResult& scCommFightResult, int iAddHeroExp)
{
	IEntity* pEntity = getEntityFromHandle(m_hEntity);
	assert(pEntity);

	IFormationSystem* pFormationSys = static_cast<IFormationSystem*>(pEntity->querySubsystem(IID_IFormationSystem));
	assert(pFormationSys);

	ITable* pHeroExpTb = getCompomentObjectManager()->findTable(TABLENAME_HeroLevelExp);
	assert(pHeroExpTb);

	vector<HEntity> formationEntityList;
	pFormationSys->getEntityList(formationEntityList);
	scCommFightResult.set_iherogetexp(iAddHeroExp);
	
	for(size_t i = 0; i < formationEntityList.size(); i++)
	{	
		IEntity* pHero = getEntityFromHandle(formationEntityList[i]);
		if(!pHero) continue;
			
		GSProto::HeroExpAwardDetail* pScNewHeroAward = scCommFightResult.add_szheroaward();
		pScNewHeroAward->set_dwheroobjectid(pHero->getHandle() );
		
		int iOldLv = pHero->getProperty(PROP_ENTITY_LEVEL, 0);
		pHero->addExp(iAddHeroExp);
		int iTmpHeroLevel = pHero->getProperty(PROP_ENTITY_LEVEL, 0);

		pScNewHeroAward->set_blevelup(iOldLv != iTmpHeroLevel);

		// 获取经验升级百分比
		int iTmpCurHeroExp = pHero->getProperty(PROP_ENTITY_HEROEXP, 0);
		int iTmpRecord = pHeroExpTb->findRecord(iTmpHeroLevel);
		assert(iTmpRecord >= 0);

		int iNeedHeroExp = pHeroExpTb->getInt(iTmpRecord, "修为");
		int iPercent = (int)( (double)iTmpCurHeroExp*100/(double)iNeedHeroExp);
		
		pScNewHeroAward->set_iheroexppercent(iPercent);
	}
}


void DungeonSystem::processDungeonAward(int iScenID, GSProto::FightAwardResult& awardResult, bool bFirstPass, bool bFirstPerfectPass, int& extraExp)
{
	IDungeonFactory* pDungeonFactory = getComponent<IDungeonFactory>(COMPNAME_DungeonFactory, IID_IDungeonFactory);
	assert(pDungeonFactory);

	IDropFactory* pDropFactory = getComponent<IDropFactory>(COMPNAME_DropFactory, IID_IDropFactory);
	assert(pDropFactory);

	const DungeonScene* pDungeonScene = pDungeonFactory->queryScene(iScenID);
	assert(pDungeonScene);

	vector<int> useDropList;
	if(bFirstPass && (pDungeonScene->firstDropIDList.size() > 0) )
	{
		useDropList = pDungeonScene->firstDropIDList;
	}
	else if(bFirstPerfectPass)
	{
		useDropList = pDungeonScene->firstPerfectIDList;
	}
	else
	{
		useDropList = pDungeonScene->dropIDList;
	}

	for(size_t i = 0; i < useDropList.size(); i++)
	{
		pDropFactory->calcDrop(useDropList[i], awardResult);
	}

	// 银币和经验处理(合并)
	int iExp = pDungeonScene->iExp;

	IVIPFactory *pVipFactory = getComponent<IVIPFactory>(COMPNAME_VIPFactory, IID_IVIPFactory);
	assert(pVipFactory);
	int iPecent = pVipFactory->getVipPropByHEntity(m_hEntity,VIP_PROP_FB_ADDEXP);
	extraExp = iExp*((double)iPecent/10000);
	iExp += extraExp;
		
	pDropFactory->addPropToResult(awardResult, GSProto::en_LifeAtt_Exp, iExp);
	pDropFactory->addPropToResult(awardResult, GSProto::en_LifeAtt_Silver, pDungeonScene->iSilver);

	// 引导英雄升阶特殊关卡
	IGlobalCfg* pGlobalCfg = getComponent<IGlobalCfg>(COMPNAME_GlobalCfg, IID_IGlobalCfg);
	int iSpecialSceneID = pGlobalCfg->getInt("英雄升阶引导关卡", 0);
	if( (iSpecialSceneID == iScenID) && bFirstPass)
	{
		addSpecialGuideHero(awardResult);
	}
}

void DungeonSystem::addSpecialGuideHero(GSProto::FightAwardResult& awardResult)
{
	IEntity* pEntity = getEntityFromHandle(m_hEntity);
	assert(pEntity);

	IFormationSystem* pFormationSys = static_cast<IFormationSystem*>(pEntity->querySubsystem(IID_IFormationSystem));
	assert(pFormationSys);

	vector<HEntity> heroList;
	pFormationSys->getEntityList(heroList);

	ITable* pHeroTb = getCompomentObjectManager()->findTable(TABLENAME_Hero);
	assert(pHeroTb);

	for(size_t i = 0; i < heroList.size(); i++)
	{
		IEntity* pTmpHero = getEntityFromHandle(heroList[i]);
		if(!pTmpHero) continue;

		int iHeroID = pTmpHero->getProperty(PROP_ENTITY_BASEID, 0);
		GSProto::FightAwardHero* pNewAwardHero = awardResult.mutable_szawardherolist()->Add();
		pNewAwardHero->set_iheroid(iHeroID);
		pNewAwardHero->set_ilevelstep(0);
		pNewAwardHero->set_icount(1);

		int iRecord = pHeroTb->findRecord(iHeroID);
		assert(iRecord >= 0);

		int iQuality = pHeroTb->getInt(iRecord, "初始品质");
		pNewAwardHero->set_iquality(iQuality);
	}
}


void DungeonSystem::processNewPassScene(int iSceneID, int iStar)
{
	//assert(m_dungeonSysData.m_mapSceneRecord.find(iSceneID) != m_dungeonSysData.m_mapSceneRecord.end() );
	//ServerEngine::SceneRecord& sceneRecord = m_dungeonSysData.m_mapSceneRecord[iSceneID];
	//sceneRecord.iStarLevel = iStar;

	IEntity* pEntity = getEntityFromHandle(m_hEntity);
	assert(pEntity);

	PLAYERLOG(pEntity)<<"NewPassScene|"<<iSceneID<<"|"<<iStar<<endl;

	IDungeonFactory* pDungeonFactory = getComponent<IDungeonFactory>(COMPNAME_DungeonFactory, IID_IDungeonFactory);
	assert(pDungeonFactory);

	int iNextSceneID = pDungeonFactory->getNextScene(iSceneID);
	if(iNextSceneID > 0)
	{
		ServerEngine::SceneRecord tmpNewRecord;
		tmpNewRecord.iSceneID = iNextSceneID;
		tmpNewRecord.iStarLevel = GSProto::en_SceneStar_Null;
		m_dungeonSysData.m_mapSceneRecord[iNextSceneID] = tmpNewRecord;
		m_dungeonSysData.newSceneList[iNextSceneID] = true;
		PLAYERLOG(pEntity)<<"AddNewScene|"<<iNextSceneID<<endl;
	}
	else
	{
		int iOwnSectionID = pDungeonFactory->getSceneOwnScetion(iSceneID);
		int iNextSectionID = pDungeonFactory->getNextSection(iOwnSectionID);
		if(iNextSectionID > 0)
		{
			m_dungeonSysData.sectionList.push_back(iNextSectionID);

			// 填充第一个关卡
			const DungeonSection* pSection = pDungeonFactory->querySection(iNextSectionID);
			assert(pSection && pSection->sceneIDList.size() > 0);

			int iFirstSceneID = pSection->sceneIDList.front();
			ServerEngine::SceneRecord tmpRecord;
			tmpRecord.iSceneID = iFirstSceneID;
			m_dungeonSysData.m_mapSceneRecord[iFirstSceneID] = tmpRecord;
			m_dungeonSysData.newSceneList[iFirstSceneID] = true;
			
			sendSectionUpdate(iNextSectionID);
			PLAYERLOG(pEntity)<<"AddNewScene|"<<iFirstSceneID<<endl;
			{
				EventArgs args;
				args.context.setInt("dungeonId", iOwnSectionID);
				pEntity->getEventServer()->setEvent(EVENT_ENTITY_FINISHDUNGEONCONDITION, args);
			}

		}
		
	}

 	//任务 通关关卡
 	/*{
		EventArgs args;
		args.context.setInt("times",1);
		args.context.setInt("entity",m_hEntity);
		pEntity->getEventServer()->setEvent(EVENT_ENTITY_PASSSCENE_COUNT, args);
	}*/
 		
}

bool DungeonSystem::updateSceneStar(int iSceneID, int iStar)
{
	assert(m_dungeonSysData.m_mapSceneRecord.find(iSceneID) != m_dungeonSysData.m_mapSceneRecord.end() );
	ServerEngine::SceneRecord& sceneRecord = m_dungeonSysData.m_mapSceneRecord[iSceneID];

	// 新通关一个，开启下一个
	if(sceneRecord.iStarLevel == GSProto::en_SceneStar_Null)
	{
		processNewPassScene(iSceneID, iStar);
	}

	// 是否需要更新评分
	if(sceneRecord.iStarLevel >= iStar)
	{
		return false;
	}

	
	IDungeonFactory* pDungeonFactory = getComponent<IDungeonFactory>(COMPNAME_DungeonFactory, IID_IDungeonFactory);
	assert(pDungeonFactory);
	
	const DungeonScene* pDungeonScene = pDungeonFactory->queryScene(iSceneID);
	assert(pDungeonScene);

	int iPreSectionStar = getSectionStar(pDungeonScene->iOwnSectionID);

	// 更新评分
	sceneRecord.iStarLevel = iStar;

	int iPostSectonStar = getSectionStar(pDungeonScene->iOwnSectionID);

	// 章节评分变化，通知客户端更新
	if(iPreSectionStar != iPostSectonStar)
	{
		sendSectionUpdate(pDungeonScene->iOwnSectionID);
	}

	if(iPostSectonStar == GSProto::en_SceneStar_3)
	{
		return true;
	}
	
	return false;
}


bool DungeonSystem::isSceneOpened(int iSceneID)
{
	return m_dungeonSysData.m_mapSceneRecord.find(iSceneID) != m_dungeonSysData.m_mapSceneRecord.end();
}


void DungeonSystem::onQuerySectionMsg(const GSProto::CSMessage& message)
{
	GSProto::Cmd_Cs_QueryDSectionDetail reqMsg;
	if(!reqMsg.ParseFromString(message.strmsgbody() ) )
	{
		return;
	}

	IEntity* pEntity = getEntityFromHandle(m_hEntity);
	assert(pEntity);
	
	int iQuerySectionID = reqMsg.isectionid();
	if(!isSectionOpend(iQuerySectionID) )
	{
		pEntity->sendErrorCode(ERROR_DUNGEON_SECTION_NOTOPEN);
		return;
	}

	//下发给客户端
	IDungeonFactory* pDungeonFactory = getComponent<IDungeonFactory>(COMPNAME_DungeonFactory, IID_IDungeonFactory);
	assert(pDungeonFactory);

	const DungeonSection* pDungeonSectionInfo = pDungeonFactory->querySection(iQuerySectionID);
	if(!pDungeonSectionInfo) return;

	GSProto::Cmd_Sc_QueryDSectionDetail scMsg;

	scMsg.set_isectionid(iQuerySectionID);
	for(size_t i = 0; i < pDungeonSectionInfo->sceneIDList.size(); i++)
	{
		int iTmpSceneID = pDungeonSectionInfo->sceneIDList[i];

		GSProto::DungeonSceneDetail* pTmpDetail = scMsg.mutable_szdungeonscene()->Add();
		if(!fillScSceneDetail(pTmpDetail, iTmpSceneID) )
		{
			scMsg.mutable_szdungeonscene()->RemoveLast();
			break;
		}
		
		m_dungeonSysData.newSceneList.erase(iTmpSceneID);
	}

	scMsg.set_isectionsize((int)pDungeonSectionInfo->sceneIDList.size() );

	IGlobalCfg* pGlobalCfg = getComponent<IGlobalCfg>(COMPNAME_GlobalCfg, IID_IGlobalCfg);
	assert(pGlobalCfg);
	
	int iNeedPhyStrength = pGlobalCfg->getInt("关卡消耗体力", 2);
	
	
	scMsg.set_icostphystrength(iNeedPhyStrength);

	// 扫荡信息
	IEntity* pMaster = getEntityFromHandle(m_hEntity);
	assert(pMaster);

	m_saoDangPointResume.checkResume(time(0) );
	
	int iSaoDangLimit = pMaster->getProperty(PROP_ENTITY_SAODANGLIMI, 0);
	if(0 == iSaoDangLimit)
	{
		scMsg.set_bsaodanghavelimit(false);
	}
	else
	{
		scMsg.set_bsaodanghavelimit(true);
		int iSaoDangPoint = pMaster->getProperty(PROP_ENTITY_SAODANGPOINT, 0);
		if(0 == iSaoDangPoint)
		{
			scMsg.set_bshowsaodangcd(true);
			scMsg.set_isaodangcdsecond(getSaoDangCD() );
		}
		else
		{
			scMsg.set_bshowsaodangcd(false);
			scMsg.set_ileftsaodangcount(iSaoDangPoint);
			scMsg.set_isaodanglimitcount(iSaoDangLimit);
		}
	}

	GSProto::SCMessage pkg;
	HelpMakeScMsg(pkg, GSProto::CMD_QUERY_DSECTIONDETAIL, scMsg);
	pEntity->sendMessage(pkg);
}


bool DungeonSystem::fillScSceneDetail(GSProto::DungeonSceneDetail* pTmpDetail, int iSceneID)
{
	IDungeonFactory* pDungeonFactory = getComponent<IDungeonFactory>(COMPNAME_DungeonFactory, IID_IDungeonFactory);
	assert(pDungeonFactory);

	const DungeonScene* pTmpScene = pDungeonFactory->queryScene(iSceneID);
	if(!pTmpScene) return false;

	map<int, ServerEngine::SceneRecord>::iterator it = m_dungeonSysData.m_mapSceneRecord.find(iSceneID);
	if(it == m_dungeonSysData.m_mapSceneRecord.end() )
	{
		pTmpDetail->set_isceneid(iSceneID);
		pTmpDetail->set_istarlevel(0);
		pTmpDetail->set_bnewopen(false);
		pTmpDetail->set_bisopend(false);
	
		return true;
	}

	ServerEngine::SceneRecord& refSceneRecord = it->second;
	
	pTmpDetail->set_isceneid(iSceneID);
	pTmpDetail->set_istarlevel(refSceneRecord.iStarLevel);
	pTmpDetail->set_bnewopen(m_dungeonSysData.newSceneList.find(iSceneID) != m_dungeonSysData.newSceneList.end() );

	// 填充FB次数限制
	if(0 == pTmpScene->iLimitPassCount)
	{
		pTmpDetail->set_bhaspasslimit(false);
	}
	else
	{
		int iLeftPassCount = pTmpScene->iLimitPassCount - getDayResetValue(refSceneRecord.passTimes).iValue;
		if(iLeftPassCount < 0) iLeftPassCount = 0;
	
		pTmpDetail->set_bhaspasslimit(true);
		pTmpDetail->set_ileftcount(iLeftPassCount);
		pTmpDetail->set_itotalcount(pTmpScene->iLimitPassCount);
	}

	// 设置开启标记
	pTmpDetail->set_bisopend(true);

	return true;
}


int DungeonSystem::getSaoDangCD()
{
	IEntity* pMaster = getEntityFromHandle(m_hEntity);
	assert(pMaster);

	m_saoDangPointResume.checkResume(time(0) );

	IGlobalCfg* pGlobalCfg = getComponent<IGlobalCfg>(COMPNAME_GlobalCfg, IID_IGlobalCfg);
	assert(pGlobalCfg);
	
	int iCDSecond = pGlobalCfg->getInt("扫荡恢复时间", 60);

	int iLastResumeTime = pMaster->getProperty(PROP_ENTITY_LASTSAODANG_RESUMETIME, 0);

	if( (Uint32)time(0) > ( (Uint32)iLastResumeTime + iCDSecond) )
	{
		return 0;
	}

	return (int)(iLastResumeTime + iCDSecond - time(0) );
}

int DungeonSystem::getSectionStar(int iSectionID)
{
	// 检查最小的星级
	DungeonFactory* pDungeonFactory = static_cast<DungeonFactory*>(getComponent<IDungeonFactory>(COMPNAME_DungeonFactory, IID_IDungeonFactory));
	assert(pDungeonFactory);

	const DungeonSection* pDungeonSection = pDungeonFactory->querySection(iSectionID);
	if(!pDungeonSection) return 0;

	int iMinStar = 3;
	bool bPassed = false;
	for(size_t i = 0; i < pDungeonSection->sceneIDList.size(); i++)
	{
		int iTmpStar = 0;
		if(getSceneStar(pDungeonSection->sceneIDList[i], iTmpStar) )
		{
			if(iTmpStar > 0) bPassed = true;
		
			if( (iTmpStar < iMinStar) && (iTmpStar > 0) )
			{
				iMinStar = iTmpStar;
			}
			else if(0 == iTmpStar)
			{
				if(bPassed) return GSProto::en_SceneStar_1;
			}
		}
		else
		{
			if(bPassed) return GSProto::en_SceneStar_1;
		}
	}

	if(!bPassed) return 0;
	
	return iMinStar;
}

void DungeonSystem::onEventSendToClient(EventArgs& args)
{
	DungeonFactory* pDungeonFactory = static_cast<DungeonFactory*>(getComponent<IDungeonFactory>(COMPNAME_DungeonFactory, IID_IDungeonFactory));
	assert(pDungeonFactory);

	GSProto::Cmd_Sc_DungeonSectionList scMsg;
	for(size_t i = 0; i < m_dungeonSysData.sectionList.size(); i++)
	{
		int iTmpSectionID = m_dungeonSysData.sectionList[i];
		GSProto::DungeonSection* pTmpSecction = scMsg.mutable_szdungeonsection()->Add();

		fillScSection(pTmpSecction, iTmpSectionID);
	}

	GSProto::SCMessage pkg;
	HelpMakeScMsg(pkg, GSProto::CMD_DUNGEON_SECTIONLIST, scMsg);

	IEntity* pEntity = getEntityFromHandle(m_hEntity);
	assert(pEntity);

	pEntity->sendMessage(pkg);
}

bool DungeonSystem::isSectionPerfectPass(int iSectionID)
{	
	IDungeonFactory* pDungeonFactory = getComponent<IDungeonFactory>(COMPNAME_DungeonFactory, IID_IDungeonFactory);
	assert(pDungeonFactory);

	const DungeonSection* pSection = pDungeonFactory->querySection(iSectionID);
	if(!pSection) return false;

	const vector<int>& sceneList = pSection->sceneIDList;

	for(size_t i = 0; i < sceneList.size(); i++)
	{
		int iTmpSceneID = sceneList[i];
		int iTmpStar = 0;
		if(!getSceneStar(iTmpSceneID, iTmpStar) || (GSProto::en_SceneStar_3 != iTmpStar) )
		{
			return false;
		}
	}

	return true;
}


void DungeonSystem::fillScSection(GSProto::DungeonSection* pTmpSecction, int iSectionID)
{
	DungeonFactory* pDungeonFactory = static_cast<DungeonFactory*>(getComponent<IDungeonFactory>(COMPNAME_DungeonFactory, IID_IDungeonFactory));
	assert(pDungeonFactory);

	
	pTmpSecction->set_isectionid(iSectionID);
	pTmpSecction->set_iisperfectpass((int)isSectionPerfectPass(iSectionID) );
			
	const DungeonSection* pSectionCfg = pDungeonFactory->querySection(iSectionID);
	assert(pSectionCfg);
	for(vector<int>::const_iterator it = pSectionCfg->perfectIDList.begin(); it != pSectionCfg->perfectIDList.end(); it++)
	{
		pTmpSecction->add_szperfectidlist(*it);
	}

	bool bGeted = m_dungeonSysData.perfectPassSectionMap.find(iSectionID) != m_dungeonSysData.perfectPassSectionMap.end();
	pTmpSecction->set_bgeted(bGeted);

	int iSectionStar = getSectionStar(iSectionID);
	pTmpSecction->set_istar(iSectionStar);
}


void DungeonSystem::sendSectionUpdate(int iSectionID)
{
	DungeonFactory* pDungeonFactory = static_cast<DungeonFactory*>(getComponent<IDungeonFactory>(COMPNAME_DungeonFactory, IID_IDungeonFactory));
	assert(pDungeonFactory);

	GSProto::Cmd_Sc_SectionUpdate scMsg;

	GSProto::DungeonSection* pTmpSecction = scMsg.mutable_updateinfo();
	fillScSection(pTmpSecction, iSectionID);
	
	GSProto::SCMessage pkg;
	HelpMakeScMsg(pkg, GSProto::CMD_SECTION_UPDATE, scMsg);

	IEntity* pEntity = getEntityFromHandle(m_hEntity);
	assert(pEntity);

	pEntity->sendMessage(pkg);
}

void DungeonSystem::packSaveData(string& data)
{
	// 为Ghost 打包，无需关卡数据
	IEntity* pEntity = getEntityFromHandle(m_hEntity);
	assert(pEntity);

	bool bForGhost = pEntity->getProperty(PROP_ENTITY_PACKFORGHOST, 0) == 1;
	if(bForGhost) return;

	data = ServerEngine::JceToStr(m_dungeonSysData);
}

bool DungeonSystem::getSceneStar(int iSceneID, int& iPassStar)
{		
	map<int, ServerEngine::SceneRecord>::iterator it = m_dungeonSysData.m_mapSceneRecord.find(iSceneID);
	if(it == m_dungeonSysData.m_mapSceneRecord.end() )
	{
		return false;
	}

	const ServerEngine::SceneRecord& record = it->second;	
	iPassStar = record.iStarLevel;

	return true;
}

bool DungeonSystem::isSectionOpend(int iSectionID)
{
	IDungeonFactory* pDungeonFactory = getComponent<IDungeonFactory>(COMPNAME_DungeonFactory, IID_IDungeonFactory);
	assert(pDungeonFactory);

	const DungeonSection* pDungeonSection = pDungeonFactory->querySection(iSectionID);
	if(!pDungeonSection) return false;

	if(pDungeonSection->iSectionType == GSProto::en_SectionType_Equip)
	{
		return true;
	}

	const vector<int>& refSectionList = m_dungeonSysData.sectionList;

	bool bFind = std::find(refSectionList.begin(), refSectionList.end(), iSectionID) != refSectionList.end();

	return bFind;
}

int DungeonSystem::getCurSectionID()
{
	assert(m_dungeonSysData.sectionList.size() > 0);

	return m_dungeonSysData.sectionList.back();
}

bool DungeonSystem::getSectionHavePass(int iSectionID)
{
   IDungeonFactory* pDungeonFactory = getComponent<IDungeonFactory>(COMPNAME_DungeonFactory, IID_IDungeonFactory);
	assert(pDungeonFactory);

	const DungeonSection* pSection = pDungeonFactory->querySection(iSectionID);
	if(!pSection) return false;

	const vector<int>& sceneList = pSection->sceneIDList;

	for(size_t i = 0; i < sceneList.size(); i++)
	{
		int iTmpSceneID = sceneList[i];
		int iTmpStar = 0;
		if( (!getSceneStar(iTmpSceneID, iTmpStar)) || (iTmpStar == 0))
		{
			return false;
		}
	}

	return true;
}

bool DungeonSystem::getSeceneHavePass(int isceneID)
{
	map<int, ServerEngine::SceneRecord>::iterator it = m_dungeonSysData.m_mapSceneRecord.find(isceneID);
	return (it != m_dungeonSysData.m_mapSceneRecord.end() );
}


//上浮开关

CloseAttCommUP::CloseAttCommUP(HEntity hEntity)
{
	m_pEntity = getEntityFromHandle(hEntity);
	
	if( m_pEntity->getProperty(PROP_ENTITY_ISClOSEATTCOMMUPORNOT,0) > 0)
	{
		int iCloseCount =  m_pEntity->getProperty(PROP_ENTITY_ISClOSEATTCOMMUPORNOT,0) + 1;
		m_pEntity->setProperty(PROP_ENTITY_ISClOSEATTCOMMUPORNOT,iCloseCount);
		return;
	}
	m_pEntity->setProperty(PROP_ENTITY_ISClOSEATTCOMMUPORNOT,1);
	GSProto::CMD_ATT_COMEUP_SC s2cMsg;
	s2cMsg.set_biscomeup(false);
	m_pEntity->sendMessage(GSProto::CMD_ATT_COMEUP,s2cMsg);
}

CloseAttCommUP::~CloseAttCommUP()
{
	int iCloseCount =  m_pEntity->getProperty(PROP_ENTITY_ISClOSEATTCOMMUPORNOT,0) - 1;
	m_pEntity->setProperty(PROP_ENTITY_ISClOSEATTCOMMUPORNOT,iCloseCount);
	if(iCloseCount>0)
	{
		return;
	}
	GSProto::CMD_ATT_COMEUP_SC s2cMsg;
	s2cMsg.set_biscomeup(true);
	m_pEntity->sendMessage(GSProto::CMD_ATT_COMEUP,s2cMsg);

	m_pEntity = NULL;
}


TimeRangeExecute SaoDangPointResume::getExecuteDelegate()
{
	return TimeRangeExecute(this, &SaoDangPointResume::doResume);
}

TimeRangeCheck SaoDangPointResume::getCheckDelegate()
{
	return TimeRangeCheck(this, &SaoDangPointResume::checkResume);
}

bool SaoDangPointResume::checkResume(Uint32 dwTime)
{
	IEntity* pEntity = getEntityFromHandle(m_hEntity);
	assert(pEntity);

	int iSaoDangLimit = pEntity->getProperty(PROP_ENTITY_SAODANGLIMI, 0);
	int iLastSaoDangRefreshTime = pEntity->getProperty(PROP_ENTITY_LASTSAODANG_RESUMETIME, 0);
	if(0 == iLastSaoDangRefreshTime)
	{
		pEntity->setProperty(PROP_ENTITY_SAODANGPOINT, iSaoDangLimit);
		pEntity->setProperty(PROP_ENTITY_LASTSAODANG_RESUMETIME, (int)time(0) );
		
		return true;
	}

	IGlobalCfg* pGlobalCfg = getComponent<IGlobalCfg>(COMPNAME_GlobalCfg, IID_IGlobalCfg);
	assert(pGlobalCfg);
	
	int iNeedSecond = pGlobalCfg->getInt("扫荡恢复时间", 60);

	// 未到时间，不处理
	if( (Uint32)time(0) < ( (Uint32)iLastSaoDangRefreshTime + iNeedSecond) )
	{
		return false;
	}

	// 设置下上次刷新时间
	Uint32 dwPassSecond = time(0) - iLastSaoDangRefreshTime;
	Uint32 dwCount = dwPassSecond/(Uint32)iNeedSecond;
	pEntity->setProperty(PROP_ENTITY_LASTSAODANG_RESUMETIME, (int)(iLastSaoDangRefreshTime + dwCount * iNeedSecond) );

	
	// 如果无限制，无需更新， 也不扣除扫荡点
	if(0 == iSaoDangLimit) return true;

	// 满了，也不处理
	int iSaoDangPoint = pEntity->getProperty(PROP_ENTITY_SAODANGPOINT, 0);
	if(iSaoDangPoint >= iSaoDangLimit)
	{
		return true;
	}

	// 增加扫荡点
	int iResultSaoDangPoint = std::min(iSaoDangPoint + (int)dwCount, iSaoDangLimit);
	
	pEntity->setProperty(PROP_ENTITY_SAODANGPOINT, iResultSaoDangPoint);

	PLAYERLOG(pEntity)<<"ResumeSaoDang|"<<(iResultSaoDangPoint - iSaoDangPoint)<<"|"<<iResultSaoDangPoint<<endl;

	return true;
}

void SaoDangPointResume::doResume(Uint32 dwTime)
{
	// nothing
}




