#include "HeroSystemPch.h"
#include "HeroSystem.h"
#include "IFormationSystem.h"
#include "ISkillSystem.h"
#include "ErrorCodeDef.h"
#include "IHeroEquipSystem.h"
#include "IItemFactory.h"
#include "IItemSystem.h"
#include "IJZEntityFactory.h"
#include "IEquipBagSystem.h"
#include "HeroTallentFactory.h"
#include "HeroTallentSystem.h"

extern "C" IObject* createHeroSystem()
{
	return new HeroSystem;
}

extern int Prop2LifeAtt(int iPropID);

HeroSystem::HeroSystem():m_iSrcHeroID(0), m_iConvtTargetHeroID(0), m_iConvrtCount(0)
{
}

HeroSystem::~HeroSystem()
{
	IEntity* pEntity = getEntityFromHandle(m_hEntity);
	if(pEntity)
	{
		pEntity->getEventServer()->unsubscribeEvent(EVENT_ENTITY_SENDACTOR_TOCLIENT, this, &HeroSystem::onEventActorCreateFinish);	
	}

	for(size_t i = 0; i < m_heroList.size(); i++)
	{
		IEntity* pHero = getEntityFromHandle(m_heroList[i]);
		if(!pHero) continue;
		
		delete pHero;
	}

	/*ITimeRangeValue* pTimeRangeValue = getComponent<ITimeRangeValue>(COMPNAME_TimeRangeValue, IID_ITimeRangeValue);
	assert(pTimeRangeValue);

	pTimeRangeValue->removeRangeValue(&m_heroConvertData);*/

	
	ITimerComponent* timeComponent = getComponent<ITimerComponent>(COMPNAME_TimeAxis,IID_ITimerComponent);
	assert(timeComponent);
	timeComponent->killTimer( m_CheckHeroTimerHandle);
	
}

Uint32 HeroSystem::getSubsystemID() const
{
	return IID_IHeroSystem;
}


Uint32 HeroSystem::getMasterHandle()
{
	return m_hEntity;
}

bool HeroSystem::create(IEntity* pEntity, const std::string& strData)
{
	assert(pEntity);
	m_hEntity = pEntity->getHandle();

	// 初始化英雄数据
	if(strData.size() > 0)
	{
		initHeroData(strData);
	}

	pEntity->getEventServer()->subscribeEvent(EVENT_ENTITY_SENDACTOR_TOCLIENT, this, &HeroSystem::onEventActorCreateFinish);

	ITimerComponent* timeComponent = getComponent<ITimerComponent>(COMPNAME_TimeAxis,IID_ITimerComponent);
	assert(timeComponent);
	m_CheckHeroTimerHandle = timeComponent->setTimer(this,1,60*1000,"HeroSystem");
	
	return true;
}

void HeroSystem::initHeroData(const std::string& strData)
{
	ServerEngine::HeroSystemData tmpSaveData;
	ServerEngine::JceToObj(strData, tmpSaveData);

	IEntityFactory* pEntityFactory = getComponent<IEntityFactory>(COMPNAME_EntityFactory, IID_IEntityFactory);
	assert(pEntityFactory);

	for(size_t i = 0; i < tmpSaveData.heroList.size(); i++)
	{
		ServerEngine::RoleSaveData& roleData = tmpSaveData.heroList[i];
		roleData.basePropData.roleIntPropset[PROP_ENTITY_MASTER] = m_hEntity;
		
		string strTmpHeroData = ServerEngine::JceToStr(roleData);
		IEntity* pTmpHero = pEntityFactory->createEntity("Hero", strTmpHeroData);
		assert(pTmpHero);

		m_heroList.push_back(pTmpHero->getHandle() );
	}

	m_heroConvertData.initData(tmpSaveData.heroConvertData, m_hEntity, PROP_ENTITY_HEROCVTCOUNT);
	//m_heroSoulList = tmpSaveData.heroSoulData.heroSoulList;
}


bool HeroSystem::createComplete()
{
	return true;
}

void HeroSystem::onEventActorCreateFinish(EventArgs& args)
{
	// 通知客户端
	GSProto::Cmd_Sc_HeroList scCmd_Sc_HeroList;
	for(size_t i = 0; i < m_heroList.size(); i++)
	{
		HEntity hTmpHero = m_heroList[i];
		IEntity* pHero = getEntityFromHandle(hTmpHero);
		assert(pHero);

		GSProto::HeroBaseData* pHeroBaseData = scCmd_Sc_HeroList.add_szherolist();
		assert(pHeroBaseData);
		
		packHeroBaseData(pHero, *pHeroBaseData);	
	}

	
	/*for(map<int, int>::iterator it = m_heroSoulList.begin(); it != m_heroSoulList.end(); it++)
	{
		GSProto::HeroSoulInfo* pNewSoul = scCmd_Sc_HeroList.mutable_szsoullist()->Add();
		pNewSoul->set_iheroid(it->first);
		pNewSoul->set_isoulcount(it->second);
	}*/

	GSProto::SCMessage pkg;
	HelpMakeScMsg(pkg, GSProto::CMD_HERO_LIST, scCmd_Sc_HeroList);

	IEntity* pEntity = getEntityFromHandle(m_hEntity);
	assert(pEntity);

	pEntity->sendMessage(pkg);
	
	checkNotice();
}


void HeroSystem::packHeroBaseData(IEntity* pHero, GSProto::HeroBaseData& scHeroBase)
{
	int iTmpHeroID = pHero->getProperty(PROP_ENTITY_BASEID, 0);
	int iLevel = pHero->getProperty(PROP_ENTITY_LEVEL, 0);
	int iLevelStep = pHero->getProperty(PROP_ENTITY_LEVELSTEP, 0);
	int iFightValue = pHero->getProperty(PROP_ENTITY_FIGHTVALUE, 0);
	int iMaxHP = pHero->getProperty(PROP_ENTITY_MAXHP, 0);
	int iAtt = pHero->getProperty(PROP_ENTITY_ATT, 0);

	scHeroBase.set_dwobjectid(pHero->getHandle() );
	scHeroBase.set_iheroid(iTmpHeroID);
	scHeroBase.set_ilevel(iLevel);
	scHeroBase.set_ilevelstep(iLevelStep);
	scHeroBase.set_ifightvalue(iFightValue);
	scHeroBase.set_imaxhp(iMaxHP);
	scHeroBase.set_iatt(iAtt);

	static int s_notifyHeroProp[] = {PROP_HERO_HASFIGHTSOUL, PROP_HERO_HASEQUIP, PROP_ENTITY_ARMOR, PROP_ENTITY_SUNDER,PROP_HERO_PRICE, 
		PROP_ENTITY_HEROEXP, PROP_ENTITY_QUALITY, PROP_ENTITY_DEF,};

	for(size_t i = 0; i < sizeof(s_notifyHeroProp)/sizeof(int); i++)
	{
		int iLifeAtt = Prop2LifeAtt(s_notifyHeroProp[i]);
		int iValue = pHero->getProperty(s_notifyHeroProp[i], 0);
		GSProto::PropItem* pNewProp = scHeroBase.mutable_szproplist()->Add();
		pNewProp->set_ilifeattid(iLifeAtt);
		pNewProp->set_ivalue(iValue);
	}

	HeroTallentSystem* pHeroTallentSys = static_cast<HeroTallentSystem*>(pHero->querySubsystem(IID_IHeroTallentSystem));
	assert(pHeroTallentSys);

	pHeroTallentSys->fillHeroTallent(scHeroBase);
	
}

const std::vector<Uint32>& HeroSystem::getSupportMessage()
{
	static std::vector<Uint32> resultList;

	if(resultList.size() == 0)
	{
		resultList.push_back(GSProto::CMD_GET_HERODESC);
		resultList.push_back(GSProto::CMD_HERO_CONVERT);
		resultList.push_back(GSProto::CMD_HHYPER_LINK);

		resultList.push_back(GSProto::CMD_QUERY_HERO_LEVELINFO);
		resultList.push_back(GSProto::CMD_HERO_LEVELUP);
		resultList.push_back(GSProto::CMD_QUERY_HEROTIP);

		//resultList.push_back(GSProto::CMD_QUERY_HERO_LEVELSTEP);
		resultList.push_back(GSProto::CMD_HERO_LEVELSTEPUP);

		resultList.push_back(GSProto::CMD_QUERY_HEROPROGRESS);
		resultList.push_back(GSProto::CMD_HERO_SANGONG);

		resultList.push_back(GSProto::CMD_QUERY_HEROCONVERT);

		//add by hyf 英雄出售
		resultList.push_back(GSProto::CMD_HEROSYSTEM_SELLHERO);
		resultList.push_back(GSProto::CMD_HERO_COVERT_OPEN);

		resultList.push_back(GSProto::CMD_HERO_CONVERT_CONFIRM);
		resultList.push_back(GSProto::CMD_HERO_TALLENT_LEVELUP);


		resultList.push_back(GSProto::CMD_HERO_CALLHERO);
		resultList.push_back(GSProto::CMD_HERO_EQUIP_FAVORITE);
		resultList.push_back(GSProto::CMD_HERO_COMBINE_FAVORITE);
		resultList.push_back(GSProto::CMD_HERO_QUALITY_UP);
		resultList.push_back(GSProto::CMD_QUERY_SANGONG);
	}

	return resultList;
}

void HeroSystem::onMessage(QxMessage* pMessage)
{
	assert(pMessage->dwMsgLen == sizeof(GSProto::CSMessage) );
	const GSProto::CSMessage& msg = *(const GSProto::CSMessage*)(pMessage->pMsgDataBuff);

	switch(msg.icmd() )
	{
		case GSProto::CMD_GET_HERODESC:
			onGetHeroDescMsg(msg);
			break;

		case GSProto::CMD_HERO_CONVERT:
			onHeroConvertMsg(msg);
			break;

		case GSProto::CMD_QUERY_HEROCONVERT:
			onReqQueryConvert(msg);

		case GSProto::CMD_HHYPER_LINK:
			onHyperLinkMsg(msg);
			break;

		case GSProto::CMD_QUERY_HERO_LEVELINFO:
			onQueryHeroLevelInfo(msg);
			break;

		case GSProto::CMD_HERO_LEVELUP:
			onHeroLevelUp(msg);
			break;

		case GSProto::CMD_QUERY_HEROTIP:
			onQueryHeroTip(msg);
			break;

		//case GSProto::CMD_QUERY_HERO_LEVELSTEP:
			//onQueryLevelStepInfo(msg);
		//	break;

		case GSProto::CMD_HERO_LEVELSTEPUP:
			onExeLevelStepUp(msg);
			break;

		case GSProto::CMD_QUERY_HEROPROGRESS:
			onQueryProgress(msg);
			break;

		case GSProto::CMD_HERO_SANGONG:
			onReqHeroSangong(msg);
			break;

		case GSProto::CMD_HERO_CONVERT_CONFIRM:
			//onReqConvertConfirm(msg);
			break;

		case GSProto::CMD_HERO_TALLENT_LEVELUP:
			onReqTallentUpgrade(msg);
			break;

		case GSProto::CMD_HEROSYSTEM_SELLHERO:
		     {
			 	//onReqSellHero(msg);
			 }break;

		case GSProto::CMD_HERO_COVERT_OPEN:
			{
				onCheckOpenCovertView(msg);
			}break;

		case GSProto::CMD_HERO_CALLHERO:
			onReqCallHero(msg);
			break;

		case GSProto::CMD_HERO_EQUIP_FAVORITE:
			onReqEquipFavorite(msg);
			break;

		case GSProto::CMD_HERO_COMBINE_FAVORITE:
			onReqCombineFavorite(msg);
			break;

		case GSProto::CMD_HERO_QUALITY_UP:
			onReqHeroQualityUp(msg);
			break;

		case GSProto::CMD_QUERY_SANGONG:
			onReqQuerySangong(msg);
			break;
	}
}





void HeroSystem::onReqHeroQualityUp(const GSProto::CSMessage& message)
{
	GSProto::CMD_HERO_QUALITY_UP_CS req;
	if(!req.ParseFromString(message.strmsgbody() ) )
	{
		return;
	}

	HEntity hHero = req.dwheroobjectid();
	

	IEntity* pHero = getEntityFromHandle(hHero);
	if(!pHero) return;

	if( (Uint32)pHero->getProperty(PROP_ENTITY_MASTER, 0) != m_hEntity)
	{
		return;
	}

	HeroTallentSystem* pHeroTallentSys = static_cast<HeroTallentSystem*>(pHero->querySubsystem(IID_IHeroTallentSystem));
	assert(pHeroTallentSys);

	pHeroTallentSys->upgradeQuality();
}


void HeroSystem::onReqCombineFavorite(const GSProto::CSMessage& message)
{
	GSProto::CMD_HERO_COMBINE_FAVORITE_CS req;
	if(!req.ParseFromString(message.strmsgbody() ) )
	{
		return;
	}

	int iItemID = req.iitemid();

	HeroTallentFactory* pHeroTallentFactory = getComponent<HeroTallentFactory>(COMPNAME_HeroTallentFactory, IID_IHeroTallentFactory);
	assert(pHeroTallentFactory);

	// 如果身上已经有了了,不能合成
	IEntity* pEntity = getEntityFromHandle(m_hEntity);
	assert(pEntity);

	IItemSystem* pItemSystem = static_cast<IItemSystem*>(pEntity->querySubsystem(IID_IItemSystem));
	assert(pItemSystem);

	/*if(pItemSystem->getItemCount(iItemID) > 0)
	{
		return;
	}*/

	int iTkSilver = pEntity->getProperty(PROP_ACTOR_SILVER, 0);

	map<int, int> consumeMap;
	bool bResult = checkFavorite(iItemID, 1, iTkSilver, true, consumeMap);
	if(!bResult)
	{
		pEntity->sendErrorCode(ERROR_COMBINE_XIHAO_NEEDITEM);
		return;
	}

	// 扣除消耗，
	consumeFavoriteCombine(iItemID, 1, true);

	// 创建合成品
	pItemSystem->addItem(iItemID, 1, GSProto::en_Reason_CombineFavorite);

	GSProto::CMD_HERO_COMBINE_FAVORITE_SC scMsg;
	scMsg.set_bresult(true);
	scMsg.set_iitemid(iItemID);

	pEntity->sendMessage(GSProto::CMD_HERO_COMBINE_FAVORITE, scMsg);
}

bool HeroSystem::checkFavorite(int iItemID, int iCount, int& iTkSilver, bool bFirstLv, map<int, int>& consumeMap)
{
	HeroTallentFactory* pHeroTallentFactory = getComponent<HeroTallentFactory>(COMPNAME_HeroTallentFactory, IID_IHeroTallentFactory);
	assert(pHeroTallentFactory);

	IEntity* pEntity = getEntityFromHandle(m_hEntity);
	assert(pEntity);

	IItemSystem* pItemSystem = static_cast<IItemSystem*>(pEntity->querySubsystem(IID_IItemSystem));
	assert(pItemSystem);

	int iOwnCount = 0;
	if(!bFirstLv)
	{
		iOwnCount = pItemSystem->getItemCount(iItemID);
		iOwnCount -= consumeMap[iItemID];
		if(iOwnCount < 0) iOwnCount = 0;
		
		if(iOwnCount >= iCount)
		{
			consumeMap[iItemID] += iCount;
			return true;
		}
	}

	int iLackCount = iCount - iOwnCount;
	consumeMap[iItemID] += iOwnCount;

	const FavoriteCombine* pCombineInfo = pHeroTallentFactory->queryFavoriteCombineInfo(iItemID);
	if(!pCombineInfo)
	{
		return false;
	}

	iTkSilver -= pCombineInfo->iNeedSilver * iCount;
	if(iTkSilver < 0)
	{
		pEntity->sendErrorCode(ERROR_NEED_SILVER);
		return false;
	}
	
	for(map<int, int>::const_iterator it = pCombineInfo->needItemMap.begin(); it != pCombineInfo->needItemMap.end(); it++)
	{
		int iTmpItemID = it->first;
		int iTmpCount = it->second;
		iTmpCount = iTmpCount * iLackCount;

		if(!checkFavorite(iTmpItemID, iTmpCount, iTkSilver, false, consumeMap) )
		{
			return false;
		}
	}

	return true;
}


void HeroSystem::consumeFavoriteCombine(int iItemID, int iCount, bool bFirstLv)
{
	HeroTallentFactory* pHeroTallentFactory = getComponent<HeroTallentFactory>(COMPNAME_HeroTallentFactory, IID_IHeroTallentFactory);
	assert(pHeroTallentFactory);

	IEntity* pEntity = getEntityFromHandle(m_hEntity);
	assert(pEntity);

	IItemSystem* pItemSystem = static_cast<IItemSystem*>(pEntity->querySubsystem(IID_IItemSystem));
	assert(pItemSystem);

	int iOwnCount = 0;
	if(!bFirstLv)
	{
		iOwnCount = pItemSystem->getItemCount(iItemID);
		if(iOwnCount >= iCount)
		{
			bool bResult = pItemSystem->removeItem(iItemID, iCount, GSProto::en_Reason_CombineFavorite);
			assert(bResult);
			return;
		}
	}

	int iLackCount = iCount - iOwnCount;

	// 先消耗已有的吧
	if(iOwnCount > 0)
	{
		bool bResult = pItemSystem->removeItem(iItemID, iOwnCount, GSProto::en_Reason_CombineFavorite);
		assert(bResult);
	}

	const FavoriteCombine* pCombineInfo = pHeroTallentFactory->queryFavoriteCombineInfo(iItemID);
	if(!pCombineInfo)
	{
		assert(false);
		return;
	}

	int iTkSilver = pEntity->getProperty(PROP_ACTOR_SILVER, 0);
	assert(iTkSilver >= pCombineInfo->iNeedSilver * iCount);
	pEntity->changeProperty(PROP_ACTOR_SILVER,0 - pCombineInfo->iNeedSilver * iCount, GSProto::en_Reason_CombineFavorite);

	for(map<int, int>::const_iterator it = pCombineInfo->needItemMap.begin(); it != pCombineInfo->needItemMap.end(); it++)
	{
		int iTmpItemID = it->first;
		int iTmpCount = it->second;
		iTmpCount = iTmpCount * iLackCount;

		consumeFavoriteCombine(iTmpItemID, iTmpCount, false);
	}
}



void HeroSystem::onReqEquipFavorite(const GSProto::CSMessage& message)
{
	GSProto::CMD_HERO_EQUIP_FAVORITE_CS req;
	if(!req.ParseFromString(message.strmsgbody() ) )
	{
		return;
	}

	HEntity hHero = req.dwheroobjectid();
	IEntity* pHero = getEntityFromHandle(hHero);
	if(!pHero) return;

	if( (Uint32)pHero->getProperty(PROP_ENTITY_MASTER, 0) != (Uint32)m_hEntity)
	{
		return;
	}

	int iItemID = req.iitemid();
	int iPos = req.ipos();

	HeroTallentSystem* pHeroTallentSys = static_cast<HeroTallentSystem*>(pHero->querySubsystem(IID_IHeroTallentSystem));
	assert(pHeroTallentSys);

	pHeroTallentSys->equipFavorite(iPos, iItemID);
}


HEntity HeroSystem::callHero(int iHeroID)
{
	IEntity* pMaster = getEntityFromHandle(m_hEntity);
	assert(pMaster);

	// 判断是否身上已经有这类英雄
	for(size_t i = 0; i < m_heroList.size(); i++)
	{
		IEntity* pTmpHero = getEntityFromHandle(m_heroList[i]);
		if(!pTmpHero) continue;

		int iTmpHeroID = pTmpHero->getProperty(PROP_ENTITY_BASEID, 0);
		if(iTmpHeroID == iHeroID)
		{
			pMaster->sendErrorCode(ERROR_HERO_EXIST);
			return 0;
		}
	}
	

	int iHeroItemID = getHeroItemID(iHeroID);
	if(0 == iHeroItemID)
	{
		return 0;
	}

	IItemSystem* pItemSystem = static_cast<IItemSystem*>(pMaster->querySubsystem(IID_IItemSystem));
	assert(pItemSystem);

	int iOwnItemCount = pItemSystem->getItemCount(iHeroItemID);
	int iNeedCount = getHeroCallNeedCount(iHeroID);

	if(iOwnItemCount < iNeedCount)
	{
		pMaster->sendErrorCode(ERROR_CALLHERO_NEEDITEM);
		return 0;
	}

	ITable* pHeroTable = getCompomentObjectManager()->findTable(TABLENAME_Hero);
	assert(pHeroTable);

	int iRecord = pHeroTable->findRecord(iHeroID);
	if(-1 == iRecord)
	{
		return 0;
	}
	int iNeedSilver = pHeroTable->getInt(iRecord, "召唤所需铜币");

	int iTkSilver = pMaster->getProperty(PROP_ACTOR_SILVER, 0);
	if(iTkSilver < iNeedSilver)
	{
		pMaster->sendErrorCode(ERROR_NEED_SILVER);
		return 0;
	}
	// 扣除魂魄
	//en_Reason_CallHero
	if(!pItemSystem->removeItem(iHeroItemID, iNeedCount, GSProto::en_Reason_CallHero) )
	{
		return 0;
	}

	// 扣钱
	pMaster->changeProperty(PROP_ACTOR_SILVER, 0-iNeedSilver, GSProto::en_Reason_CallHero);

	HEntity hNewHero = addHero(iHeroID, false, GSProto::en_Reason_CallHero);

	GSProto::CMD_HERO_CALLHERO_SC scMsg;
	scMsg.set_iheroid(iHeroID);
	scMsg.set_dwnewheroid(hNewHero);

	pMaster->sendMessage(GSProto::CMD_HERO_CALLHERO, scMsg);

	return hNewHero;
}


void HeroSystem::onReqCallHero(const GSProto::CSMessage& message)
{
	GSProto::CMD_HERO_CALLHERO_CS req;
	if(!req.ParseFromString(message.strmsgbody() ) )
	{
		return;
	}

	int iHeroID = req.iheorid();
	
	ITable* pHeroTable = getCompomentObjectManager()->findTable(TABLENAME_Hero);
	assert(pHeroTable);

	int iRecord = pHeroTable->findRecord(iHeroID);
	if(iRecord < 0) return ;
	
	callHero(iHeroID);
}

int HeroSystem::getHeroCallNeedCount(int iHeroID)
{
	ITable* pHeroTable = getCompomentObjectManager()->findTable(TABLENAME_Hero);
	assert(pHeroTable);
	
	int iRecord = pHeroTable->findRecord(iHeroID);
	if(iRecord < 0) return 0;

	int iCount = pHeroTable->getInt(iRecord, "召唤所需魂魄");

	return iCount;
}



void HeroSystem::onReqTallentUpgrade(const GSProto::CSMessage& message)
{
	// 天赋升级
	GSProto::CMD_HERO_TALLENT_LEVELUP_CS req;
	if(!req.ParseFromString(message.strmsgbody() ) )
	{
		return;
	}

	Uint32 dwHeroID = req.dwheroobjectid();
	int iTallentID = req.itallentid();

	IEntity* pHero = getEntityFromHandle(dwHeroID);
	if(!pHero) return;

	if( (Uint32)pHero->getProperty(PROP_ENTITY_MASTER, 0) != (Uint32)m_hEntity)
	{
		return;
	}

	HeroTallentSystem* pHeroTallentSys = static_cast<HeroTallentSystem*>(pHero->querySubsystem(IID_IHeroTallentSystem));
	assert(pHeroTallentSys);

	int iTallentLevel = 0;
	if(!pHeroTallentSys->getTallentLevel(iTallentID, iTallentLevel) )
	{
		return;
	}

	if(!pHeroTallentSys->canTallentUpgrade(iTallentID, iTallentLevel, true) )
	{
		return;
	}

	if(!pHeroTallentSys->consumeTalllentUpgrade(iTallentID, iTallentLevel) )
	{
		return;
	}

	// 升级吧
	pHeroTallentSys->tallentUpgrade(iTallentID);

	// 材料有扣除，可能导致其他技能变化
	pHeroTallentSys->notifyUpgradeFlagChg();

	//任务- 技能升级
	{
		IEntity *pEntity = getEntityFromHandle(m_hEntity);
		EventArgs args;
		args.context.setInt("times",1);
		args.context.setInt("entity",m_hEntity);
		pEntity->getEventServer()->setEvent(EVENT_ENTITY_TASK_HEROSKILL_LEVELUP, args);
	}
}


/*void HeroSystem::onReqConvertConfirm(const GSProto::CSMessage& message)
{
	if( (0 == m_iConvtTargetHeroID) || (0 == m_iConvrtCount) )
	{
		return;
	}

	if(getHeroSoulCount(m_iSrcHeroID) < m_iConvrtCount)
	{
		return;
	}

	removeHeroSoul(m_iSrcHeroID, m_iConvrtCount);
	addHeroSoul(m_iConvtTargetHeroID, m_iConvrtCount);

	IEntity* pEntity = getEntityFromHandle(m_hEntity);
	assert(pEntity);

	pEntity->sendMessage(GSProto::CMD_HERO_CONVERT_CONFIRM);

	PLAYERLOG(pEntity)<<"ConvertConfirm|"<<m_iSrcHeroID<<"|"<<m_iConvtTargetHeroID<<"|"<<m_iConvrtCount<<endl;
}*/


//add by hyf 英雄出售
/*void HeroSystem::onReqSellHero(const GSProto::CSMessage& message)
{
	GSProto::CMD_HEROSYSTEM_SELLHERO_CS req;
	if(!req.ParseFromString(message.strmsgbody()))
	{
		return;
	}

	int iHeroID = req.iheroid();
	int iCount = req.isoulcount();

	if(getHeroSoulCount(iHeroID) < iCount)
	{
		return;
	}

	// 删除魂魄
	removeHeroSoul(iHeroID, iCount);
	
	IGlobalCfg* pGlobalCfg = getComponent<IGlobalCfg>(COMPNAME_GlobalCfg, IID_IGlobalCfg);
	assert(pGlobalCfg);

	int iSingleSilver = pGlobalCfg->getInt("英雄魂魄单价", 1000);
	double dTotalSilver = (double)iSingleSilver * (double)iCount;
	if(dTotalSilver >= GSProto::MAX_MONEY_VALUE)
	{
		dTotalSilver = GSProto::MAX_MONEY_VALUE;
	}

	IEntity *pEntity = getEntityFromHandle(m_hEntity);
	assert(pEntity);
	
	pEntity->changeProperty(PROP_ACTOR_SILVER, (int)dTotalSilver, GSProto::en_Reason_SELLHERO);

	pEntity->sendMessage(GSProto::CMD_HEROSYSTEM_SELLHERO);*/

	
	/*std::set<HEntity> comsumeList;
	for(int i = 0; i < req.szdwobjectid().size(); i++)
	{
		comsumeList.insert(req.szdwobjectid().Get(i) );
	}

	//
	if(0 == comsumeList.size()) return;
	
	int iTotalPrice = 0;


	for(std::set<HEntity>::iterator iter = comsumeList.begin(); iter != comsumeList.end(); ++iter)
	{
		IEntity* pHero = getEntityFromHandle(*iter);
		if(!pHero) return;
		HEntity hMaster = pHero->getProperty(PROP_ENTITY_MASTER, 0);
		if(hMaster != m_hEntity)
		{
			return;
		}
		
		IHeroEquipSystem* pHeroEquipSystem = static_cast<IHeroEquipSystem*>(pHero->querySubsystem(IID_IHeroEqupSystem));
		assert(pHeroEquipSystem);

		IHeroRealEquipSystem* pHeroRealEquipSys = static_cast<IHeroRealEquipSystem*>(pHero->querySubsystem(IID_IHeroRealEquupSystem) );
		assert(pHeroRealEquipSys);
		if(pHeroEquipSystem->hasFightSoul()||pHeroRealEquipSys->hasEquip())
		{

			pEntity->sendErrorCode(ERROR_HAVEEQUIPORSOUL_CANNOT_SELL);
			return ;
		}
		
	}
	
	GSProto::CMD_DELETE_HEROS_SC s2cMsg;

	//扣除英雄 
	for(std::set<HEntity>::iterator iter = comsumeList.begin(); iter != comsumeList.end(); ++iter)
	{
		IEntity* pHero = getEntityFromHandle(*iter);
		if(!pHero) return;
		
		int iHeroPrice = pHero->getProperty(PROP_HERO_PRICE, 0);

		HEROLOG(pHero)<<"SellHero"<<endl; 
		
		
		bool res = destroyHero(*iter, false);
		if(res)
		{
			s2cMsg.add_szheroobjecetid(*iter);
			
			iTotalPrice += iHeroPrice;
		}
		
	}

	pEntity->changeProperty(PROP_ACTOR_SILVER, iTotalPrice, GSProto::en_Reason_SELLHERO);
	PLAYERLOG(pEntity)<<"SellHero get Silver|"<<iTotalPrice<<endl;

	pEntity->sendMessage(GSProto::CMD_DELETE_HEROS, s2cMsg);*/
//}

void HeroSystem::onCheckOpenCovertView(const GSProto::CSMessage& message)
{
	// 未开启转换功能，不能使用
	IEntity* pEntity = getEntityFromHandle(m_hEntity);
	assert(pEntity);

	GSProto::COVERHEROVIEM iOpenId = GSProto::enOpenCovert;
	/*if(!pEntity->isFunctionOpen(GSProto::en_Function_HeroCvt) )
	{
		iOpenId = GSProto::enOpenSell;
	}*/

	GSProto::CMD_HERO_COVERT_OPEN_SC s2cMsg;
	s2cMsg.set_iopenid(iOpenId);
	pEntity->sendMessage(GSProto::CMD_HERO_COVERT_OPEN,s2cMsg);
}


void HeroSystem::onReqQuerySangong(const GSProto::CSMessage& message)
{
	GSProto::CMD_QUERY_SANGONG_CS req;
	if(!req.ParseFromString(message.strmsgbody() ) )
	{
		return;
	}

	HEntity hHero = req.dwheroobjectid();
	int iSumExp = calcSangoHeroExp(hHero);

	GSProto::CMD_QUERY_SANGONG_SC scMsg;
	scMsg.set_dwheroobjectid(hHero);
	scMsg.set_isilver(iSumExp);

	IEntity* pActor = getEntityFromHandle(m_hEntity);
	assert(pActor);

	pActor->sendMessage(GSProto::CMD_QUERY_SANGONG, scMsg);
}


void HeroSystem::onReqHeroSangong(const GSProto::CSMessage& message)
{
	GSProto::CMD_HERO_SANGONG_CS req;
	if(!req.ParseFromString(message.strmsgbody() ) )
	{
		return;
	}

	Uint32 dwHeroObjectID = req.dwobjectid();
	IEntity* pHero = getEntityFromHandle(dwHeroObjectID);
	if(!pHero) return;

	HEntity hMaster = pHero->getProperty(PROP_ENTITY_MASTER, 0);
	if(hMaster != m_hEntity)
	{
		return;
	}

	IEntity* pActor = getEntityFromHandle(m_hEntity);
	assert(pActor);

	// 身上有武魂，不能散功
	IHeroEquipSystem* pHeroEquipSys = static_cast<IHeroEquipSystem*>(pHero->querySubsystem(IID_IHeroEqupSystem) );
	assert(pHeroEquipSys);

	if(pHeroEquipSys->hasFightSoul() )
	{
		pActor->sendErrorCode(ERROR_HERO_CANNOTSANGO_FIGHTSOUL);
		return;
	}
	
	// 身上有装备，不能散功, 去掉这个限制
	/*IHeroRealEquipSystem* pHeroRealEquipSys = static_cast<IHeroRealEquipSystem*>(pHero->querySubsystem(IID_IHeroRealEquupSystem) );
	if(pHeroRealEquipSys->hasEquip() )
	{
		pActor->sendErrorCode(ERROR_CANNOTSANGO_EQUIP);
		return;
	}*/
	
	// 执行散功
	int iSumExp = calcSangoHeroExp(dwHeroObjectID);

	// 判断铜币是否足够
	int iTkSilver = pActor->getProperty(PROP_ACTOR_SILVER, 0);
	if(iTkSilver < iSumExp)
	{
		pActor->sendErrorCode(ERROR_NEED_SILVER);
		return;
	}

	pActor->changeProperty(PROP_ACTOR_SILVER, 0-iSumExp, GSProto::en_Reason_SanGongConsume);
	
	int iTkHeroExp = pActor->getProperty(PROP_ENTITY_HEROEXP, 0);

	pActor->changeProperty(PROP_ENTITY_HEROEXP, iSumExp, 0);

	pHero->setProperty(PROP_ENTITY_LEVEL, 1);
	pHero->setProperty(PROP_ENTITY_HEROEXP, 0);

	// 通知客户端
	
	sendHeroUpdateInfo(dwHeroObjectID);
	
	GSProto::CMD_HERO_SANGONG_SC scMsg;
	scMsg.set_dwobjectid(dwHeroObjectID);
	scMsg.set_irebackheroexp(iSumExp);

	pActor->sendMessage(GSProto::CMD_HERO_SANGONG, scMsg);

	
	// 记录日志
	HEROLOG(pHero)<<"SanGong|"<<iSumExp<<endl;
	PLAYERLOG(pActor)<<"SanGong|"<<iSumExp<<"|"<<iTkHeroExp<<"|"<<iSumExp<<endl;
}


int HeroSystem::calcSangoHeroExp(HEntity hHero)
{
	IEntity* pHero = getEntityFromHandle(hHero);
	if(!pHero) return 0;

	int iLevel = pHero->getProperty(PROP_ENTITY_LEVEL, 0);
	int iCurHeroExp = pHero->getProperty(PROP_ENTITY_HEROEXP, 0);

	int iSumExp = iCurHeroExp;
	ITable* pTable = getCompomentObjectManager()->findTable("HeroLevelExp");
	assert(pTable);
	for(int i = 1; i < iLevel; i++)
	{
		int iRecord = pTable->findRecord(i);
		assert(iRecord >= 0);

		int iTmpExp = pTable->getInt(iRecord, "修为");
		iSumExp += iTmpExp;
	}

	return iSumExp;	
}


void HeroSystem::onQueryProgress(const GSProto::CSMessage& message)
{
	GSProto::CMD_QUERY_HEROPROGRESS_CS req;
	if(!req.ParseFromString(message.strmsgbody() ) )
	{
		return;
	}

	HEntity hHero = req.dwheroobjectid();
	sendHeroProgress(hHero);
}

void HeroSystem::sendHeroProgress(HEntity hHero)
{
	IEntity* pHero = getEntityFromHandle(hHero);
	if(!pHero) return;
	
	if( (Uint32)pHero->getProperty(PROP_ENTITY_MASTER, 0) != m_hEntity)
	{
		return;
	}

	GSProto::CMD_QUERY_HEROPROGRESS_SC scMsg;
	int iCurProgress = pHero->getProperty(PROP_HERO_LVSTEPPROGRESS, 0);
	int iCurLevelStep = pHero->getProperty(PROP_ENTITY_LEVELSTEP, 0);

	IJZEntityFactory* pJZEntityFactory = getComponent<IJZEntityFactory>(COMPNAME_EntityFactory, IID_IJZEntityFactory);
	assert(pJZEntityFactory);

	int iPreNeedProgress = 0;
	if(iCurLevelStep > 0)
	{
		const LevelStepGrowCfg* pPreLevelStepCfg = pJZEntityFactory->queryLevelStepCfg(iCurLevelStep-1);
		if(pPreLevelStepCfg) iPreNeedProgress = pPreLevelStepCfg->iNeedProgress;
	}

	const LevelStepGrowCfg* pLevelStepCfg = pJZEntityFactory->queryLevelStepCfg(iCurLevelStep);
	if(!pLevelStepCfg)
	{
		scMsg.set_icurprogress(iCurProgress - iPreNeedProgress);
		scMsg.set_ineedprogress(iCurProgress - iPreNeedProgress);
	}
	else
	{
		scMsg.set_icurprogress(iCurProgress - iPreNeedProgress);
		scMsg.set_ineedprogress(pLevelStepCfg->iNeedProgress - iPreNeedProgress);
		//int iTmpProgress = (int)((double)iCurProgress*100/pLevelStepCfg->iNeedProgress);
		//scMsg.set_iprogresspercent(iTmpProgress);
	}
	scMsg.set_dwheroobjectid(hHero);

	IEntity* pEntity = getEntityFromHandle(m_hEntity);
	assert(pEntity);

	//add by hyf
	fillStepLevelUpNotice( *scMsg.mutable_herolist());

	pEntity->sendMessage(GSProto::CMD_QUERY_HEROPROGRESS, scMsg);
}


/*void HeroSystem::onQueryLevelStepInfo(const GSProto::CSMessage& message)
{
	GSProto::Cmd_Cs_QueryHeroLevelStep reqMsg;
	if(!reqMsg.ParseFromString(message.strmsgbody() ) )
	{
		return;
	}

	HEntity hTargetHero = reqMsg.dwhostheroid();
	set<HEntity> comsumeList;
	for(int i = 0; i < reqMsg.szconsumehero().size(); i++)
	{
		comsumeList.insert(reqMsg.szconsumehero().Get(i) );
	}

	if(comsumeList.size() == 0)
	{
		return;
	}
	
	int iCostSilver = 0;
	int iSumProgress = 0;
	int iResultLevelStep = 0;

	IEntity* pTargetHero = getEntityFromHandle(hTargetHero);
	if(!pTargetHero) return;

	IEntity* pEntity = getEntityFromHandle(m_hEntity);
	assert(pEntity);

	int iResultLevel = 0;
	int iResultHeroExp = 0;
	if(!checkLevelStepCost(hTargetHero, comsumeList, true, iCostSilver, iSumProgress, iResultLevelStep, iResultLevel, iResultHeroExp) )
	{
		return;
	}

	GSProto::Cmd_Sc_QueryHeroLevelStep scMsg;
	scMsg.set_dwhostheroid(hTargetHero);
	scMsg.set_ineedsilver(iCostSilver);

	//packHeroBaseData(pTargetHero,  *scMsg.mutable_curinfo()  );

	ServerEngine::RoleSaveData tmpSaveData;
	pTargetHero->packSaveData(tmpSaveData);

	tmpSaveData.basePropData.roleIntPropset[PROP_ENTITY_LEVELSTEP] = iResultLevelStep;
	tmpSaveData.basePropData.roleIntPropset[PROP_ENTITY_LEVEL] = iResultLevel;
	
	IJZEntityFactory* pJZEntityFactory = getComponent<IJZEntityFactory>(COMPNAME_EntityFactory, IID_IJZEntityFactory);
	assert(pJZEntityFactory);
	
	int iPreNeedProgress = 0;
	int iCurLevelStep = pTargetHero->getProperty(PROP_ENTITY_LEVELSTEP, 0);
	if(iCurLevelStep > 0)
	{
		const LevelStepGrowCfg* pPreLevelStepCfg = pJZEntityFactory->queryLevelStepCfg(iCurLevelStep-1);
		if(pPreLevelStepCfg) iPreNeedProgress = pPreLevelStepCfg->iNeedProgress;
	}

	scMsg.set_ipostprogress(iSumProgress - iPreNeedProgress);

	string strData = ServerEngine::JceToStr(tmpSaveData);
	IEntity* pNewHero = pJZEntityFactory->getEntityFactory()->createEntity("Hero", strData);
	assert(pNewHero);
	
	packHeroBaseData(pNewHero,  *scMsg.mutable_postinfo()  );
	delete pNewHero;

	GSProto::SCMessage pkg;
	HelpMakeScMsg(pkg, GSProto::CMD_QUERY_HERO_LEVELSTEP, scMsg);

	pEntity->sendMessage(pkg);
}*/

void HeroSystem::onExeLevelStepUp(const GSProto::CSMessage& message)
{
	GSProto::CMD_HERO_LEVELSTEPUP_CS reqMsg;
	if(!reqMsg.ParseFromString(message.strmsgbody() ) )
	{
		return;
	}

	HEntity hTargetHero = reqMsg.dwhostheroid();
	
	doHeroLevelStepUp(hTargetHero, true);
}

void HeroSystem::onQueryHeroTip(const GSProto::CSMessage& message)
{
	GSProto::Cmd_Cs_QueryHeroTip reqMsg;
	if(!reqMsg.ParseFromString(message.strmsgbody() ) )
	{
		return;
	}

	HEntity hHero = reqMsg.dwheroobjectid();

	IEntity* pHero = getEntityFromHandle(hHero);
	if(!pHero) return;

	if(m_hEntity != (HEntity)pHero->getProperty(PROP_ENTITY_MASTER, 0))
	{
		return;
	}

	GSProto::Cmd_Sc_QueryHeroTip scMsg;
	scMsg.set_dwobjejctid(hHero);
	
	ISkillSystem* pSkillSys = static_cast<ISkillSystem*>(pHero->querySubsystem(IID_ISkillSystem));
	assert(pSkillSys);

	int iSkillID = pSkillSys->getSelectedActiveSkill();
	scMsg.set_iskillid(iSkillID);

	IEntity* pEntity = getEntityFromHandle(m_hEntity);
	assert(pEntity);

	GSProto::SCMessage pkg;
	HelpMakeScMsg(pkg, GSProto::CMD_QUERY_HEROTIP, scMsg);

	pEntity->sendMessage(pkg);
}

bool HeroSystem::doHeroLevelStepUp(HEntity hTargetHero,  bool bNotifyMsg)
{
	IEntity* pTargetHero = getEntityFromHandle(hTargetHero);
	if(!pTargetHero) return false;

	if( (Uint32)pTargetHero->getProperty(PROP_ENTITY_MASTER, 0) != (Uint32)m_hEntity)
	{
		return false;
	}

	int iHeroID = pTargetHero->getProperty(PROP_ENTITY_BASEID, 0);

	IEntity* pEntity = getEntityFromHandle(m_hEntity);
	assert(pEntity);

	int iLevelStep = pTargetHero->getProperty(PROP_ENTITY_LEVELSTEP, 0);

	IJZEntityFactory* pJZEntityFactory = getComponent<IJZEntityFactory>(COMPNAME_EntityFactory, IID_IJZEntityFactory);
	assert(pJZEntityFactory);

	const LevelStepGrowCfg* pLevelStepCfg = pJZEntityFactory->queryLevelStepCfg(iLevelStep);
	assert(pLevelStepCfg);

	// 判断是否达到顶级了
	if(!pJZEntityFactory->queryLevelStepCfg(iLevelStep + 1) )
	{
		pEntity->sendErrorCode(ERROR_HERO_MAXLEVELSTEP);
		return false;
	}

	int iOwnCount = getHeroSoulCount(iHeroID);
	if(iOwnCount < pLevelStepCfg->iNeedSoulCount)
	{
		pEntity->sendErrorCode(ERROR_HERO_LACKSOUL);
		return false;
	}

	int iTkSilver = pEntity->getProperty(PROP_ACTOR_SILVER, 0);
	if(iTkSilver < pLevelStepCfg->iNeedSilver)
	{
		pEntity->sendErrorCode(ERROR_NEED_SILVER);
		return false;
	}

	/*if(!removeHeroSoul(iHeroID, pLevelStepCfg->iNeedSoulCount) )
	{
		return false;
	}*/


	IItemSystem* pItemSystem = static_cast<IItemSystem*>(pEntity->querySubsystem(IID_IItemSystem));
	assert(pItemSystem);
	
	int iItemID = getHeroItemID(iHeroID);

	if(!pItemSystem->removeItem(iItemID, pLevelStepCfg->iNeedSoulCount, GSProto::en_Reason_HeroLevelStepUp) )
	{
		return false;
	}

	pEntity->changeProperty(PROP_ACTOR_SILVER, 0-pLevelStepCfg->iNeedSilver, GSProto::en_Reason_HeroLevelStepUp);


	GSProto::CMD_HERO_LEVELSTEPUP_SC scMsg;
	scMsg.set_dwheroobjectid(hTargetHero);
	
	scMsg.mutable_oldstate()->set_ilevelstep(pTargetHero->getProperty(PROP_ENTITY_LEVELSTEP, 0) );
	scMsg.mutable_oldstate()->set_ihpgrow(pTargetHero->getProperty(PROP_ENTITY_HPGROW, 0) );
	scMsg.mutable_oldstate()->set_iattgrow(pTargetHero->getProperty(PROP_ENTITY_ATTGROW, 0) );
	scMsg.mutable_oldstate()->set_idefgrow(pTargetHero->getProperty(PROP_ENTITY_DEFGROW, 0) );

	int iOldMaxHP = pTargetHero->getProperty(PROP_ENTITY_MAXHP, 0);
	int iOldAtt = pTargetHero->getProperty(PROP_ENTITY_ATT, 0);
	int iOldDef = pTargetHero->getProperty(PROP_ENTITY_DEF, 0);

	// 修改等阶
	pTargetHero->setProperty(PROP_ENTITY_LEVELSTEP, iLevelStep+1);

	// 触发事件
	EventArgs args;
	args.context.setInt("entity", hTargetHero);
	args.context.setInt("oldlevelstep", iLevelStep);
	args.context.setInt("newlevelstep", iLevelStep+1);
	pTargetHero->getEventServer()->setEvent(EVENT_ENTITY_CALCGROW, args);
	pTargetHero->getEventServer()->setEvent(EVENT_ENTITY_LEVELSTEPCHG, args);

	scMsg.mutable_newstate()->set_ilevelstep(pTargetHero->getProperty(PROP_ENTITY_LEVELSTEP, 0) );
	scMsg.mutable_newstate()->set_ihpgrow(pTargetHero->getProperty(PROP_ENTITY_HPGROW, 0) );
	scMsg.mutable_newstate()->set_iattgrow(pTargetHero->getProperty(PROP_ENTITY_ATTGROW, 0) );
	scMsg.mutable_newstate()->set_idefgrow(pTargetHero->getProperty(PROP_ENTITY_DEFGROW, 0) );

	int iAddMaxHP = pTargetHero->getProperty(PROP_ENTITY_MAXHP, 0) - iOldMaxHP;
	int iAddAtt = pTargetHero->getProperty(PROP_ENTITY_ATT, 0) - iOldAtt;
	int iAddDef = pTargetHero->getProperty(PROP_ENTITY_DEF, 0) - iOldDef;
	scMsg.set_ihpaddvalue(iAddMaxHP);
	scMsg.set_iattaddvalue(iAddAtt);
	scMsg.set_idefaddvalue(iAddDef);

	sendHeroUpdateInfo(hTargetHero);
	pEntity->sendMessage(GSProto::CMD_HERO_LEVELSTEPUP, scMsg);

	HEROLOG(pTargetHero)<<"LevelStepUp|"<<iLevelStep<<"|"<<(iLevelStep+1)<<endl;

	notifyLevelStepUp(iHeroID, iLevelStep, iLevelStep+1);

	return true;
}


int HeroSystem::getHeroSoulCount(int iHeroID)
{
	/*map<int, int>::iterator it = m_heroSoulList.find(iHeroID);
	if(it != m_heroSoulList.end() )
	{
		return it->second;
	}

	return 0;*/

	int iItemID = getHeroItemID(iHeroID);
	
	IEntity* pMaster = getEntityFromHandle(m_hEntity);
	assert(pMaster);

	IItemSystem* pItemSystem = static_cast<IItemSystem*>(pMaster->querySubsystem(IID_IItemSystem) );
	assert(pItemSystem);

	int iResultCount = pItemSystem->getItemCount(iItemID);

	return iResultCount;
}


/*void HeroSystem::addHeroSoul(int iHeroID, int iCount)
{
	m_heroSoulList[iHeroID] += iCount;

	GSProto::CMD_HERO_SOULCHG_SC scMsg;
	GSProto::HeroSoulChg* pNewChg = scMsg.mutable_szchglist()->Add();

	pNewChg->set_iheroid(iHeroID);
	pNewChg->set_ichgcount(iCount);
	pNewChg->set_icurcount(m_heroSoulList[iHeroID]);

	IEntity* pEntity = getEntityFromHandle(m_hEntity);
	assert(pEntity);

	pEntity->sendMessage(GSProto::CMD_HERO_SOULCHG, scMsg);

	PLAYERLOG(pEntity)<<"AddHeroSoul|"<<iHeroID<<"|"<<iCount<<endl;
}*/

/*bool HeroSystem::removeHeroSoul(int iHeroID, int iCount)
{
	if(m_heroSoulList.find(iHeroID) == m_heroSoulList.end() )
	{
		return false;
	}

	if(m_heroSoulList[iHeroID] < iCount)
	{
		return false;
	}

	m_heroSoulList[iHeroID] -= iCount;

	int iLeftCount = m_heroSoulList[iHeroID];
	if(m_heroSoulList[iHeroID] == 0)
	{
		m_heroSoulList.erase(iHeroID);
	}

	// 通知客户端
	GSProto::CMD_HERO_SOULCHG_SC scMsg;
	GSProto::HeroSoulChg* pNewChg = scMsg.mutable_szchglist()->Add();

	pNewChg->set_iheroid(iHeroID);
	pNewChg->set_ichgcount(-iCount);
	pNewChg->set_icurcount(iLeftCount);

	IEntity* pEntity = getEntityFromHandle(m_hEntity);
	assert(pEntity);

	pEntity->sendMessage(GSProto::CMD_HERO_SOULCHG, scMsg);

	PLAYERLOG(pEntity)<<"RemoveHeroSoul|"<<iHeroID<<"|"<<iCount<<endl;

	return true;
}*/


void HeroSystem::notifyLevelStepUp(int iHeroID, int iPreLevelStep, int iCurLevelStep)
{
	if(iPreLevelStep == iCurLevelStep)
	{
		return;
	}

	for(int i = iPreLevelStep + 1; i <= iCurLevelStep; i++)
	{
		_notifyLevelStepUp(iHeroID, i);
	}
}

void HeroSystem::_notifyLevelStepUp(int iHeroID, int iLevelStep)
{
	IGlobalCfg* pGlobalCfg = getComponent<IGlobalCfg>(COMPNAME_GlobalCfg, IID_IGlobalCfg);
	assert(pGlobalCfg);

	string strNotifyLevelStep = pGlobalCfg->getString("英雄升阶提示", "30#40");
	vector<int> hitList = TC_Common::sepstr<int>(strNotifyLevelStep, "#");

	if(find(hitList.begin(), hitList.end(), iLevelStep) == hitList.end() )
	{
		return;
	}

	GSProto::CMD_MARQUEE_SC scMsg;
	scMsg.set_marqueeid(GSProto::en_marQueue_HeroLevelStep);

	IEntity* pActor = getEntityFromHandle(m_hEntity);
	assert(pActor);

	string strActorName = pActor->getProperty(PROP_ENTITY_NAME, "");
	scMsg.add_szstrmessage(strActorName);

	// 升阶前
	{
		stringstream ss;
		ss<<iHeroID<<"#"<<iLevelStep-1;
		scMsg.add_szstrmessage(ss.str() );
	}

	// 升阶后
	{
		stringstream ss;
		ss<<iHeroID<<"#"<<iLevelStep;
		scMsg.add_szstrmessage(ss.str() );
	}

	IJZMessageLayer* pMsgLayer = getComponent<IJZMessageLayer>(COMPNAME_MessageLayer, IID_IJZMessageLayer);
	assert(pMsgLayer);

	GSProto::SCMessage pkg;
	HelpMakeScMsg(pkg, GSProto::CMD_MARQUEE, scMsg);
	
	pMsgLayer->broadcastMsg(pkg);
}



void HeroSystem::gmSetAllHeroLevelStep(int iLevelStep)
{
	IJZEntityFactory* pJZEntityFactory = getComponent<IJZEntityFactory>(COMPNAME_EntityFactory, IID_IJZEntityFactory);
	assert(pJZEntityFactory);

	// 策划配置是从1开始的
	const LevelStepGrowCfg* pLevelStepCfg = pJZEntityFactory->queryLevelStepCfg(iLevelStep);
	if(!pLevelStepCfg) return;

	for(size_t i = 0; i < m_heroList.size(); i++)
	{
		IEntity* pHero = getEntityFromHandle(m_heroList[i]);
		if(!pHero) continue;

		int iOldLevelStep = pHero->getProperty(PROP_ENTITY_LEVELSTEP, 0);
		pHero->setProperty(PROP_ENTITY_LEVELSTEP, iLevelStep);
		pHero->setProperty(PROP_HERO_LVSTEPPROGRESS, pLevelStepCfg->iNeedProgress);
		
		EventArgs args;
		args.context.setInt("entity", pHero->getHandle() );
		args.context.setInt("oldlevelstep", iOldLevelStep);
		args.context.setInt("newlevelstep", iLevelStep);
		pHero->getEventServer()->setEvent(EVENT_ENTITY_CALCGROW, args);
		pHero->getEventServer()->setEvent(EVENT_ENTITY_LEVELSTEPCHG, args);

		sendHeroUpdateInfo(pHero->getHandle() );

		HEROLOG(pHero)<<"GMLevelStepUp|"<<iOldLevelStep<<"|"<<iLevelStep<<endl;
	}
}

void HeroSystem::gmSetAllHeroLevel(int iLevel)
{	
	for(size_t i = 0; i < m_heroList.size(); i++)
	{
		IEntity* pHero = getEntityFromHandle(m_heroList[i]);
		if(!pHero) continue;

		pHero->setProperty(PROP_ENTITY_LEVEL, iLevel);
	}
}


void HeroSystem::onQueryHeroLevelInfo(const GSProto::CSMessage& message)
{
	GSProto::Cmd_Cs_QueryHeroLevelInfo reqQuery;
	if(!reqQuery.ParseFromString(message.strmsgbody() ) )
	{
		return;
	}

	HEntity hHero = reqQuery.dwheroobjectid();
	IEntity* pHero = getEntityFromHandle(hHero);
	if(!pHero) return;

	if( (Uint32)pHero->getProperty(PROP_ENTITY_MASTER, 0) != (Uint32)m_hEntity)
	{
		return;
	}

	GSProto::Cmd_Sc_QueryHeroLevelInfo scMsg;
	packLevelInfo(hHero, scMsg.mutable_levelinfo() );
	//add by hyf 
	fillLevelUpNotice( *scMsg.mutable_herolist());
	
	IEntity* pEntity = getEntityFromHandle(m_hEntity);
	assert(pEntity);
	

	pEntity->sendMessage(GSProto::CMD_QUERY_HERO_LEVELINFO, scMsg);
}

void HeroSystem::packLevelInfo(HEntity hHero, GSProto::HeroLevelUpInfo* pLevelInfo)
{
	IEntity* pHero = getEntityFromHandle(hHero);
	assert(pHero);

	IEntity* pEntity = getEntityFromHandle(m_hEntity);
	assert(pEntity);

	int iHeroCurHeroExp = pHero->getProperty(PROP_ENTITY_HEROEXP, 0);
	int iTkExp = pEntity->getProperty(PROP_ENTITY_HEROEXP, 0);

	pLevelInfo->set_dwheroobjectid(hHero);
	pLevelInfo->set_icurheroexp(iHeroCurHeroExp);
	pLevelInfo->set_iactortkheroexp(iTkExp);

	ITable* pHeroExpTb = getCompomentObjectManager()->findTable(TABLENAME_HeroLevelExp);
	assert(pHeroExpTb);
	int iHeroLv = pHero->getProperty(PROP_ENTITY_LEVEL, 0);
	int iLvRecord = pHeroExpTb->findRecord(iHeroLv);
	assert(iLvRecord >= 0);

	int iNeedHeroExp = pHeroExpTb->getInt(iLvRecord, "修为");
	pLevelInfo->set_ineedheroexp(iNeedHeroExp);

	if(pHeroExpTb->findRecord(iHeroLv + 1) < 0)
	{
		int iNextLvMaxHP = pHero->getProperty(PROP_ENTITY_MAXHP, 0);
		int iNextLvAtt = pHero->getProperty(PROP_ENTITY_ATT, 0);
		int iNextDef = pHero->getProperty(PROP_ENTITY_DEF, 0);
		pLevelInfo->set_inextmaxhp(iNextLvMaxHP);
		pLevelInfo->set_inextatt(iNextLvAtt);
		pLevelInfo->set_inextlv(iHeroLv);
		pLevelInfo->set_inextdef(iNextDef);
		return;
	}

	ServerEngine::RoleSaveData tmpData;
	pHero->packSaveData(tmpData);

	tmpData.basePropData.roleIntPropset[PROP_ENTITY_LEVEL] = iHeroLv + 1;

	IEntityFactory* pEntityFactory = getComponent<IEntityFactory>(COMPNAME_EntityFactory, IID_IEntityFactory);
	assert(pEntityFactory);
	string strTmpData = ServerEngine::JceToStr(tmpData);
	IEntity* pTmpHero = pEntityFactory->createEntity("Hero", strTmpData);
	assert(pTmpHero);

	int iNextLvMaxHP = pTmpHero->getProperty(PROP_ENTITY_MAXHP, 0);
	int iNextLvAtt = pTmpHero->getProperty(PROP_ENTITY_ATT, 0);
	int iNextLvDef = pTmpHero->getProperty(PROP_ENTITY_DEF, 0);
	pLevelInfo->set_inextmaxhp(iNextLvMaxHP);
	pLevelInfo->set_inextatt(iNextLvAtt);
	pLevelInfo->set_inextdef(iNextLvDef);
	pLevelInfo->set_inextlv(iHeroLv+1);
	
	delete pTmpHero;
}


void HeroSystem::onHeroLevelUp(const GSProto::CSMessage& message)
{
	GSProto::Cmd_Cs_HeroLevelUp reqMsg;
	if(!reqMsg.ParseFromString(message.strmsgbody() ) )
	{
		return;
	}
	
	HEntity hHero = reqMsg.dwheroobjectid();
	IEntity* pHero = getEntityFromHandle(hHero);
	if(!pHero) return;

	HEntity hMaster = pHero->getProperty(PROP_ENTITY_MASTER, 0);
	if(hMaster != m_hEntity)
	{
		return;
	}

	IEntity* pEntity = getEntityFromHandle(m_hEntity);
	assert(pEntity);


	int iCurHeroLv = pHero->getProperty(PROP_ENTITY_LEVEL, 0);
	ITable* pHeroLvExpTb = getCompomentObjectManager()->findTable("HeroLevelExp");
	assert(pHeroLvExpTb);

	// 判断下，人物等级120级以下，英雄等级不能超过(人物等级X3)
	int iLevel = pEntity->getProperty(PROP_ENTITY_LEVEL, 0);
	if(iCurHeroLv >= iLevel * 2)
	{
		pEntity->sendErrorCode(ERROR_HEROLEVEL_3ACTORLEVEL);
		return;
	}
	
	if(pHeroLvExpTb->findRecord(iCurHeroLv + 1) < 0)
	{
		pEntity->sendErrorCode(ERROR_HERO_TOUCHMAXLV);
		return;
	}

	int iRecord = pHeroLvExpTb->findRecord(iCurHeroLv);

	// 是否存在修为?
	int iAcotorTkHeroExp = pEntity->getProperty(PROP_ENTITY_HEROEXP, 0);
	if(0 == iAcotorTkHeroExp)
	{
		pEntity->sendErrorCode(ERROR_NO_HEROEXP);
		return;
	}

	int iNeedHeroExp = pHeroLvExpTb->getInt(iRecord, "修为");
	int iHeroTkExp = pHero->getProperty(PROP_ENTITY_HEROEXP, 0);

	int iNeedCostHeroExp = iNeedHeroExp - iHeroTkExp;
	if(iAcotorTkHeroExp < iNeedCostHeroExp)
	{
		// 增加修为
		pHero->changeProperty(PROP_ENTITY_HEROEXP, iAcotorTkHeroExp, GSProto::en_Reason_HeroLevelUp);

		// 扣除消耗
		pEntity->changeProperty(PROP_ENTITY_HEROEXP,0 - iAcotorTkHeroExp, GSProto::en_Reason_HeroLevelUp);
	}
	else
	{
		// 升级
		pHero->setProperty(PROP_ENTITY_HEROEXP, 0);
		pHero->changeProperty(PROP_ENTITY_LEVEL, 1, GSProto::en_Reason_HeroLevelUp);

		// 扣除消耗
		pEntity->changeProperty(PROP_ENTITY_HEROEXP, 0-iNeedCostHeroExp, GSProto::en_Reason_HeroLevelUp);

		{
			EventArgs args;
			args.context.setInt("HeroLevel",pHero->getProperty( PROP_ENTITY_LEVEL,0 ));
			args.context.setInt("entity",m_hEntity);
			pEntity->getEventServer()->setEvent(EVENT_ENTITY_TASKFINISH_HEROLEVEL, args);
		}
	}

	sendHeroUpdateInfo(hHero);

	{
		GSProto::Cmd_Sc_HeroLevelUp scMsg;
		packLevelInfo(hHero, scMsg.mutable_levelinfo() );
		//add by hyf 
		//提示可升级英雄
		fillLevelUpNotice( *scMsg.mutable_herolist());
		pEntity->sendMessage(GSProto::CMD_HERO_LEVELUP, scMsg);
	}
	
	{
		int iNewLv = pHero->getProperty(PROP_ENTITY_LEVEL, 0);
		int iNewHeroExp = pHero->getProperty(PROP_ENTITY_HEROEXP, 0);
		HEROLOG(pHero)<<"LevelUp|"<<iCurHeroLv<<"|"<<iNewLv<<"|"<<iHeroTkExp<<"|"<<iNewHeroExp<<endl;
	}
	
	//任务系统
	{
		EventArgs args;
		args.context.setInt("times",1);
		args.context.setInt("entity",m_hEntity);
		pEntity->getEventServer()->setEvent(EVENT_ENTITY_TASK_HEROLEVELUP, args);
	}
	
}

void HeroSystem::onHyperLinkMsg(const GSProto::CSMessage& message)
{
	GSProto::Cmd_Cs_HyperLink reqMsg;
	if(!reqMsg.ParseFromString(message.strmsgbody() ) )
	{
		return;
	}

	IEntity* pEntity = getEntityFromHandle(m_hEntity);
	assert(pEntity);

	IScriptEngine* pScriptEngine = getComponent<IScriptEngine>(COMPNAME_ScriptEngine, IID_IScriptEngine);
	assert(pScriptEngine);


	EventArgs args;
	args.context.setString("msg", reqMsg.strhyperlinkcmd().c_str() );
	pScriptEngine->runFunction("OnHyperLinkMessage", pEntity, "IEntity", &args, "EventArgs");
}


void HeroSystem::onReqQueryConvert(const GSProto::CSMessage& message)
{
	// 未开启转换功能，不能使用
	IEntity* pEntity = getEntityFromHandle(m_hEntity);
	assert(pEntity);

	/*if(!pEntity->isFunctionOpen(GSProto::en_Function_HeroCvt) )
	{
		pEntity->sendErrorCode(ERROR_HERO_CONVERT_NOOPEN);
		return;
	}*/

	pEntity->sendMessage(GSProto::CMD_QUERY_HEROCONVERT);
}


void HeroSystem::onHeroConvertMsg(const GSProto::CSMessage& message)
{
	// 未开启转换功能，不能使用
	IEntity* pEntity = getEntityFromHandle(m_hEntity);
	assert(pEntity);

	/*if(!pEntity->isFunctionOpen(GSProto::en_Function_HeroCvt) )
	{
		pEntity->sendErrorCode(ERROR_HERO_CONVERT_NOOPEN);
		return;
	}*/
	
	GSProto::CMD_HERO_CONVERT_CS reqMsg;
	if(!reqMsg.ParseFromString(message.strmsgbody() ) )
	{
		return;
	}

	int iHeroID = reqMsg.iheroid();
	int iCount = reqMsg.icount();

	//数量不够
	if(getHeroSoulCount(iHeroID) < iCount)
	{
		return;
	}

	IGlobalCfg* pGlobalCfg = getComponent<IGlobalCfg>(COMPNAME_GlobalCfg, IID_IGlobalCfg);
	assert(pGlobalCfg);
	
	int iSingleSilver = pGlobalCfg->getInt("英雄转换单价", 1000);
	int iTotalSilver = iSingleSilver * iCount;

	// 验证银币是否足够
	int iTkSilver = pEntity->getProperty(PROP_ACTOR_SILVER, 0);
	if(iTkSilver < iTotalSilver)
	{
		pEntity->sendErrorCode(ERROR_NEED_SILVER);
		return;
	}

	// 转换结果
	HeroTallentFactory* pHeroTallentFactory = getComponent<HeroTallentFactory>("HeroTallentFactory", IID_IHeroTallentFactory);
	assert(pHeroTallentFactory);

	const HeroConvertCfg* pHeroCvtCfg = pHeroTallentFactory->queryConvertCfg(iHeroID);
	assert(pHeroCvtCfg);

	IRandom* pRandom = getComponent<IRandom>(COMPNAME_Random, IID_IMiniAprRandom);
	assert(pRandom);

	map<int, int> targetHeroList;
	int iTotalChance = 0;
	for(size_t i = 0; i < m_heroList.size(); i++)
	{
		IEntity* pTmpHero = getEntityFromHandle(m_heroList[i]);
		if(!pTmpHero) continue;

		int iTmpHeroID = pTmpHero->getProperty(PROP_ENTITY_BASEID, 0);
		if(pHeroCvtCfg->targetChanceList.find(iTmpHeroID) != pHeroCvtCfg->targetChanceList.end() )
		{
			targetHeroList[iTmpHeroID] = pHeroCvtCfg->targetChanceList.at(iTmpHeroID);
			iTotalChance += pHeroCvtCfg->targetChanceList.at(iTmpHeroID);
		}
	}

	if(0 == iTotalChance)
	{
		return;
	}

	int iTargetHeroID = 0;

	int iRandV = pRandom->random() % iTotalChance;
	int iSumV = 0;
	for(map<int, int>::iterator it = targetHeroList.begin(); it != targetHeroList.end(); it++)
	{
		int iBaseID = it->first;
		int iTmpChance = it->second;

		iSumV += iTmpChance;
		if(iRandV < iSumV)
		{
			iTargetHeroID = iBaseID;
			break;
		}
	}

	if(0 == iTargetHeroID)
	{
		return;
	}

	// 消耗银币
	pEntity->changeProperty(PROP_ACTOR_SILVER, 0-iTotalSilver, GSProto::en_Reason_HeroConvert);

	m_iSrcHeroID = iHeroID;
	m_iConvtTargetHeroID = iTargetHeroID;
	m_iConvrtCount = iCount;

	// 通知客户端
	GSProto::CMD_HERO_CONVERT_SC scMsg;
	scMsg.set_inewheroid(m_iConvtTargetHeroID);

	pEntity->sendMessage(GSProto::CMD_HERO_CONVERT, scMsg);

	PLAYERLOG(pEntity)<<"ConvertTry|"<<m_iSrcHeroID<<"|"<<m_iConvtTargetHeroID<<"|"<<m_iConvrtCount<<endl;
}

void HeroSystem::onGetHeroDescMsg(const GSProto::CSMessage& message)
{
	GSProto::Cmd_Cs_GetHeroDesc reqMsg;
	if(!reqMsg.ParseFromString(message.strmsgbody() ) )
	{
		return;
	}

	HEntity hQueryHero = reqMsg.dwobjectid();
	IEntity* pHero = getEntityFromHandle(hQueryHero);
	if(!pHero) return;

	HEntity hMaster = pHero->getProperty(PROP_ENTITY_MASTER, 0);
	if(hMaster != m_hEntity)
	{
		return;
	}

	GSProto::Cmd_Sc_GetHeroDesc scMsg;
	scMsg.set_dwobjectid(hQueryHero);

	int iAttGrow = pHero->getProperty(PROP_ENTITY_ATTGROW, 0);
	int iMaxHPGrow = pHero->getProperty(PROP_ENTITY_HPGROW, 0);
	int iDefGrow = pHero->getProperty(PROP_ENTITY_DEFGROW, 0);
	scMsg.set_iattgrow(iAttGrow);
	scMsg.set_imaxhpgrow(iMaxHPGrow);
	scMsg.set_idefgrow(iDefGrow);

	// 填充属性
	//static int s_szNotifyProp[] = {PROP_ENTITY_MAXHP, PROP_ENTITY_ATT, PROP_ENTITY_DOGE, PROP_ENTITY_HIT, PROP_ENTITY_ANTIKNOCK,
	//PROP_ENTITY_KNOCK, PROP_ENTITY_BLOCK, PROP_ENTITY_WRECK, PROP_ENTITY_ARMOR, PROP_ENTITY_SUNDER, PROP_ENTITY_INITANGER,};

	static map<int, int> s_mapProp;
	static map<int, int> s_mapGrowProp;
	if(s_mapProp.size() == 0)
	{
		s_mapProp[PROP_ENTITY_MAXHP] = PROP_ENTITY_BASEMAXHP;
		s_mapGrowProp[PROP_ENTITY_MAXHP] = PROP_ENTITY_HPGROW;
		
		s_mapProp[PROP_ENTITY_ATT] = PROP_ENTITY_BASEATT;
		s_mapGrowProp[PROP_ENTITY_ATT] = PROP_ENTITY_ATTGROW;
		
		s_mapProp[PROP_ENTITY_DOGE] = PROP_ENTITY_BASEDOGE;
		
		s_mapProp[PROP_ENTITY_HIT] = PROP_ENTITY_BASEHIT;
		s_mapGrowProp[PROP_ENTITY_HIT] = PROP_ENTITY_HITGROW;
		
		s_mapProp[PROP_ENTITY_ANTIKNOCK] = PROP_ENTITY_BASEANTIKNOCK;
		
		s_mapProp[PROP_ENTITY_KNOCK] = PROP_ENTITY_BASEKNOCK;
		s_mapGrowProp[PROP_ENTITY_KNOCK] = PROP_ENTITY_KNOCKGROW;
		
		
		s_mapProp[PROP_ENTITY_BLOCK] = PROP_ENTITY_BASEBLOCK;

		s_mapProp[PROP_ENTITY_WRECK] = PROP_ENTITY_BASEWRECK;
		s_mapGrowProp[PROP_ENTITY_WRECK] = PROP_ENTITY_WRECKGROW;

		s_mapProp[PROP_ENTITY_DEF] = PROP_ENTITY_BASEDEF;
		s_mapGrowProp[PROP_ENTITY_DEF] = PROP_ENTITY_DEFGROW;
		
		
		s_mapProp[PROP_ENTITY_ARMOR] = PROP_ENTITY_BASEARMOR;
		s_mapProp[PROP_ENTITY_SUNDER] = PROP_ENTITY_BASESUNDER;
		s_mapProp[PROP_ENTITY_INITANGER] = PROP_ENTITY_BASEINITANGER;
		//s_mapProp[PROP_ENTITY_SKILLDEF] = PROP_ENTITY_SKILLDEF;
		
	}

	int iLevel = pHero->getProperty(PROP_ENTITY_LEVEL, 0);
	for(map<int, int>::iterator it = s_mapProp.begin(); it != s_mapProp.end(); it++)
	{
		int iPropID = it->first;
		int iLifeAttID = Prop2LifeAtt(iPropID);
		assert(iLifeAttID >= 0);

		int iGrowValue = 0;
		if(s_mapGrowProp.find(it->first) != s_mapGrowProp.end() )
		{
			iGrowValue = pHero->getProperty(s_mapGrowProp[it->first], 0);
		}
		
		int iBaseValue = pHero->getProperty(it->second, 0);
		
		int iBasePropFix = 10000;
		if( (PROP_ENTITY_BASEMAXHP == it->second) || (PROP_ENTITY_BASEATT == it->second) || (PROP_ENTITY_BASEDEF == it->second) )
		{
			iBasePropFix = pHero->getProperty(PROP_ENTITY_BASEPROP_FIXPARAM, 0);
		}

		iBaseValue = (int) ((double)iBaseValue * (double)iBasePropFix/10000 + (double)iLevel * (double)iGrowValue/10000);
		
		{
			// 绝技防御特殊处理
			if(iPropID == PROP_ENTITY_SKILLDEF)
			{
				iBaseValue = 0;
			}
			GSProto::PropItem* pNewPropItem = scMsg.mutable_szproplist()->Add();
			pNewPropItem->set_ilifeattid(iLifeAttID);
			pNewPropItem->set_ivalue(iBaseValue);

			
		}
		{
			int iTotalValue = pHero->getProperty(iPropID, 0);
			GSProto::PropItem* pNewAddPropItem = scMsg.mutable_szpropaddvaluelist()->Add();
			pNewAddPropItem->set_ilifeattid(iLifeAttID);
			pNewAddPropItem->set_ivalue(iTotalValue - iBaseValue);
		}
	}

	// 填充技能
	ISkillSystem* pHeroSkillSys = static_cast<ISkillSystem*>(pHero->querySubsystem(IID_ISkillSystem));
	assert(pHeroSkillSys);

	vector<ISkill*> heroSkillList = pHeroSkillSys->getSkillList(false);
	for(size_t i = 0; i < heroSkillList.size(); i++)
	{
		ISkill* pTmpSkill = heroSkillList[i];
		assert(pTmpSkill);

		// 过滤普攻
		if(en_SkillType_ComAttack == pTmpSkill->getSkillType() )
		{
			continue;
		}
		
		GSProto::SkillInfo* pNewSkillInfo = scMsg.mutable_szskilllist()->Add();
		pNewSkillInfo->set_iskillid(pTmpSkill->getSkillID() );
		pNewSkillInfo->set_bactive(pTmpSkill->isActive() );
	}

	GSProto::SCMessage pkg;
	HelpMakeScMsg(pkg, GSProto::CMD_GET_HERODESC, scMsg);

	IEntity* pEntity = getEntityFromHandle(m_hEntity);
	assert(pEntity);

	pEntity->sendMessage(pkg);
}

	

vector<HEntity> HeroSystem::getHeroList()
{
	return m_heroList;
}

HEntity HeroSystem::addHero(int iHeroID, bool bNotifyError, int iReason)
{
	ITable* pHeroTb = getCompomentObjectManager()->findTable(TABLENAME_Hero);
	assert(pHeroTb);

	int iRecord = pHeroTb->findRecord(iHeroID);
	assert(iRecord >= 0);
	
	int iInitLevelStep = pHeroTb->getInt(iRecord, "初始星级");

	return _addHero(iHeroID, iInitLevelStep, bNotifyError, iReason);
}

int HeroSystem::calcHeroSoulCount(int iLevelStep)
{
	IJZEntityFactory* pJZEntityFactory = getComponent<IJZEntityFactory>(COMPNAME_EntityFactory, IID_IJZEntityFactory);
	assert(pJZEntityFactory);

	const LevelStepGrowCfg* pLevelStepCfg = pJZEntityFactory->queryLevelStepCfg(iLevelStep);
	assert(pLevelStepCfg);

	return pLevelStepCfg->iExtraSoulCount;
}

int HeroSystem::getHeroCountByQuality(int iQuality) 
{
	int iCount = 0; 
	for(size_t i = 0; i < m_heroList.size(); ++i)
	{
		HEntity hEntity = m_heroList[i];
		IEntity *pHeroEntity = getEntityFromHandle(hEntity);
		assert(pHeroEntity);
		int iHeroQuality = pHeroEntity->getProperty( PROP_ENTITY_QUALITY, 0);
		if(iQuality <= iHeroQuality)
		{
			iCount++;
		}
	}
	return iCount;
}

int HeroSystem::getHeroCount()
{
	return m_heroList.size();
}


HEntity HeroSystem::_addHero(int iHeroID, int iLevelStep, bool bNotifyError, int iReason)
{
	IEntity* pEntity = getEntityFromHandle(m_hEntity);
	assert(pEntity);

	IItemSystem* pItemSystem = static_cast<IItemSystem*>(pEntity->querySubsystem(IID_IItemSystem));
	assert(pItemSystem);

	ITable* pTable = getCompomentObjectManager()->findTable(TABLENAME_Hero);
	assert(pTable);

	int iRecord = pTable->findRecord(iHeroID);
	assert(iRecord >= 0);
	
	// 判断身上是否已经有相同ID的英雄了, 如果当前的英雄等阶比要加入的低，将二者互换下
	for(size_t i = 0; i < m_heroList.size(); i++)
	{
		IEntity* pTmpHero = getEntityFromHandle(m_heroList[i]);
		assert(pTmpHero);

		int iTmpHeroID = pTmpHero->getProperty(PROP_ENTITY_BASEID, 0);
		if(iTmpHeroID == iHeroID)
		{
			int iHeroItemID = getHeroItemID(iHeroID);
			int iTmpLevelStep = pTmpHero->getProperty(PROP_ENTITY_LEVELSTEP, 0);
			if(iTmpLevelStep <= iLevelStep)
			{
				pTmpHero->setProperty(PROP_ENTITY_LEVELSTEP, iLevelStep);
				sendHeroUpdateInfo(pTmpHero->getHandle() );
				
				int iSoulCount = calcHeroSoulCount(iTmpLevelStep);

				//下行通用提示
				/*if( iReason != GSProto::en_Reason_ShopSystem_RandomHero )
				{
					GSProto::CMD_COMMON_GETHERO_SC scMsg;
					scMsg.set_iherobaseid( iHeroID);
					scMsg.set_igetherosoul( iSoulCount);
					scMsg.set_iherooldstep(iTmpLevelStep);
					scMsg.set_iheronewstep(iLevelStep);
				
					pEntity->sendMessage( GSProto::CMD_COMMON_GETHERO,  scMsg);
				}*/

				CloseAttCommUP close(m_hEntity);
				EventArgs args;
				args.context.setInt("entity", m_hEntity);
				args.context.setInt("itemid", iHeroItemID);
				args.context.setInt("itemcount", iSoulCount);
				args.context.setInt("heroId", iHeroID);
				pEntity->getEventServer()->setEvent(EVENT_ENTITY_ADDHEROSOUL_FROMHERO, args);
				pItemSystem->addItem(iHeroItemID, iSoulCount, GSProto::en_Reason_AddHero);
					
				
				/*if( iReason == GSProto::en_Reason_ShopSystem_RandomHero ||  iReason == GSProto::en_Reason_ClimTowerChallengeAward )
				{
				    CloseAttCommUP close(m_hEntity);
					EventArgs args;
					args.context.setInt("entity", m_hEntity);
					args.context.setInt("itemid", iHeroItemID);
					args.context.setInt("itemcount", iSoulCount);
					args.context.setInt("heroId", iHeroID);
					pEntity->getEventServer()->setEvent(EVENT_ENTITY_ADDHEROSOUL_FROMHERO, args);
					pItemSystem->addItem(iHeroItemID, iSoulCount, GSProto::en_Reason_AddHero);
				}
				else
				{
					pItemSystem->addItem(iHeroItemID, iSoulCount, GSProto::en_Reason_AddHero);
				}*/
			}
			else
			{
				 
				int iSoulCount = calcHeroSoulCount(iLevelStep);

				 CloseAttCommUP close(m_hEntity);
					EventArgs args;
					args.context.setInt("entity", m_hEntity);
					args.context.setInt("itemid", iHeroItemID);
					args.context.setInt("itemcount", iSoulCount);
					args.context.setInt("heroId", iHeroID);
					pEntity->getEventServer()->setEvent(EVENT_ENTITY_ADDHEROSOUL_FROMHERO, args);
					pItemSystem->addItem(iHeroItemID, iSoulCount, GSProto::en_Reason_AddHero);

				/*
				if( iReason == GSProto::en_Reason_ShopSystem_RandomHero  ||  iReason == GSProto::en_Reason_ClimTowerChallengeAward)
				{
					CloseAttCommUP close(m_hEntity);
					EventArgs args;
					args.context.setInt("entity", m_hEntity);
					args.context.setInt("itemid", iHeroItemID);
					args.context.setInt("itemcount", iSoulCount);
					args.context.setInt("heroId", iHeroID);
					pEntity->getEventServer()->setEvent(EVENT_ENTITY_ADDHEROSOUL_FROMHERO, args);
					pItemSystem->addItem(iHeroItemID, iSoulCount, GSProto::en_Reason_AddHero);
				}
				else
				{
					pItemSystem->addItem(iHeroItemID, iSoulCount, GSProto::en_Reason_AddHero);
				}
				*/
			}
			return 0;
		}
	}

	ServerEngine::RoleSaveData roleSaveData;
	fillNewHeroData(roleSaveData, iHeroID);
	roleSaveData.basePropData.roleIntPropset[PROP_ENTITY_LEVELSTEP] = iLevelStep;

	// 如果初始等阶不为0, 需要设置进度
	IJZEntityFactory* pJZEntityFactory = getComponent<IJZEntityFactory>(COMPNAME_EntityFactory, IID_IJZEntityFactory);
	assert(pJZEntityFactory);

	const LevelStepGrowCfg* pLevelStepCfg = pJZEntityFactory->queryLevelStepCfg(iLevelStep - 1);
	if(pLevelStepCfg)
	{
		roleSaveData.basePropData.roleIntPropset[PROP_HERO_LVSTEPPROGRESS] = pLevelStepCfg->iNeedProgress;
	}

	IEntityFactory* pEntityFactory = getComponent<IEntityFactory>(COMPNAME_EntityFactory, IID_IEntityFactory);
	assert(pEntityFactory);

	string strHeroData = ServerEngine::JceToStr(roleSaveData);
	IEntity* pTmpHero = pEntityFactory->createEntity("Hero", strHeroData);
	assert(pTmpHero);

	m_heroList.push_back(pTmpHero->getHandle() );

	// 触发新增英雄事件
	{
		EventArgs args;
		args.context.setInt("entity", m_hEntity);
		args.context.setInt("hero", pTmpHero->getHandle() );
		pEntity->getEventServer()->setEvent(EVENT_ENTITY_NEWHERO, args);
	}
	
	// 通知客户端消息
	sendHeroUpdateInfo(pTmpHero->getHandle() );

	HEROLOG(pTmpHero)<<"Add|"<<iReason<<endl;

	return pTmpHero->getHandle();
}



HEntity HeroSystem::addHeroWithLevelStep(int iHeroID, int iLevelStep, bool bNotifyError, int iReason)
{
	return _addHero(iHeroID, iLevelStep, bNotifyError, iReason);
}


bool HeroSystem::checkHeroFull()
{
	//return m_heroList.size() >= (GSProto::MAX_HERO_COUNT - 10);

	return false;
}


void HeroSystem::fillNewHeroData(ServerEngine::RoleSaveData& roleSaveData, int iHeroBaseID)
{
	roleSaveData.basePropData.roleIntPropset[PROP_ENTITY_MASTER] = m_hEntity;
	roleSaveData.basePropData.roleIntPropset[PROP_ENTITY_BASEID] = iHeroBaseID;
	roleSaveData.basePropData.roleIntPropset[PROP_ENTITY_LEVEL] = 1;

	ITable* pTable = getCompomentObjectManager()->findTable(TABLENAME_Hero);
	assert(pTable);

	int iRecord = pTable->findRecord(iHeroBaseID);
	assert(iRecord >= 0);

	/*ServerEngine::SkillSystemSaveData skillSysData;
	string strSkillList = pTable->getString(iRecord, "技能列表");
	skillSysData.skillList = TC_Common::sepstr<int>(strSkillList, "#");
	roleSaveData.subsystemData[IID_ISkillSystem] = ServerEngine::JceToStr(skillSysData);*/

	// 设置UUID吧
	uuid_t itemuuid;
	uuid_generate(itemuuid);
	char szUUIDString[1024] = {0};
	uuid_unparse_upper(itemuuid, szUUIDString);
	roleSaveData.basePropData.roleStringPropset[PROP_ENTITY_UUID] = szUUIDString;

	// 设置Quality
	int iQuality = pTable->getInt(iRecord, "初始品质");
	roleSaveData.basePropData.roleIntPropset[PROP_ENTITY_QUALITY] = iQuality;
}

void HeroSystem::sendHeroUpdateInfo(HEntity hHero)
{
	IEntity* pHero = getEntityFromHandle(hHero);
	assert(pHero);

	GSProto::Cmd_Sc_HeroUpdate scMsg;
	GSProto::HeroBaseData* pHeroBaseData = scMsg.mutable_herodata();
	
	packHeroBaseData(pHero, *pHeroBaseData);

	GSProto::SCMessage pkg;
	HelpMakeScMsg(pkg, GSProto::CMD_HERO_UPDATE, scMsg);

	IEntity* pMaster = getEntityFromHandle(m_hEntity);
	assert(pMaster);

	pMaster->sendMessage(pkg);
}

bool HeroSystem::destroyHero(HEntity hHero, bool bNotifyError)
{
	// 目前只是销毁，后续处理等待策划出细节
	IEntity* pHero = getEntityFromHandle(hHero);
	if(!pHero)
	{
		return false;
	}

	// 验证合法性
	int iClassID = pHero->getProperty(PROP_ENTITY_CLASS, 0);
	if(GSProto::en_class_Hero != iClassID)
	{
		return false;
	}

	HEntity hMaster = pHero->getProperty(PROP_ENTITY_MASTER, 0);
	if(hMaster != m_hEntity)
	{
		return false;
	}

	vector<HEntity>::iterator it = std::find(m_heroList.begin(), m_heroList.end(), hHero);
	if(it == m_heroList.end() )
	{
		return false;
	}

	HEROLOG(pHero)<<"destroy"<<endl;

	IEntity* pEntity = getEntityFromHandle(m_hEntity);
	assert(pEntity);

	//触发销毁英雄事件
	{
		EventArgs args;
		args.context.setInt("entity", m_hEntity);
		args.context.setInt("hero", hHero);
		pEntity->getEventServer()->setEvent(EVENT_ENTITY_DELHERO, args);
	}

	m_heroList.erase(it);
	pHero->Release();

	if(bNotifyError)
	{
		
		GSProto::Cmd_Sc_HeroDel scDel;
		scDel.set_dwobjectid(hHero);
		
		// 通知客户端
		GSProto::SCMessage pkg;
		HelpMakeScMsg(pkg, GSProto::CMD_HERO_DEL, scDel);
		pEntity->sendMessage(pkg);
	}
	return true;
}


int HeroSystem::getHeroItemID(int iHeroID)
{
	ITable* pHeroTable = getCompomentObjectManager()->findTable(TABLENAME_Hero);
	assert(pHeroTable);

	int iRecord = pHeroTable->findRecord(iHeroID);
	if(iRecord < 0) return 0;

	int iItemID = pHeroTable->getInt(iRecord, "英雄魂魄物品ID");

	return iItemID;
}


void HeroSystem::packSaveData(string& data)
{
	// 由于英雄数量太多，需要针对Ghost打包做优化
	IEntity* pEntity = getEntityFromHandle(m_hEntity);
	assert(pEntity);

	bool bForGhost = pEntity->getProperty(PROP_ENTITY_PACKFORGHOST, 0) == 1;
	IFormationSystem* pFormationSys = static_cast<IFormationSystem*>(pEntity->querySubsystem(IID_IFormationSystem));
	assert(pFormationSys);
	
	ServerEngine::HeroSystemData tmpSaveData;
	for(size_t i = 0; i < m_heroList.size(); i++)
	{
		IEntity* pHero = getEntityFromHandle(m_heroList[i]);
		assert(pHero);

		if(bForGhost && (pFormationSys->getEntityPos(pHero->getHandle() ) < 0 ) )
		{
			continue;
		}

		tmpSaveData.heroList.push_back(ServerEngine::RoleSaveData() );
		pHero->packSaveData(tmpSaveData.heroList.back() );
	}

	m_heroConvertData.fillData(tmpSaveData.heroConvertData);
	
	//tmpSaveData.heroSoulData.heroSoulList = m_heroSoulList;

	data = ServerEngine::JceToStr(tmpSaveData);
}

HEntity HeroSystem::getHeroFromUUID(const string& strUUID)
{
	for(size_t i = 0; i < m_heroList.size(); i++)
	{
		IEntity* pHero = getEntityFromHandle(m_heroList[i]);
		assert(pHero);

		string strTmpUUID = pHero->getProperty(PROP_ENTITY_UUID, "");
		if(strTmpUUID == strUUID)
		{
			return pHero->getHandle();
		}
	}

	return 0;
}

void HeroSystem::checkNotice()
{
	IEntity* pEntity = getEntityFromHandle(m_hEntity);
			
	bool resCanLevelUp = noticeHeroLevelUp();
	pEntity->chgNotice(GSProto::en_NoticeGuid_LevelUp,resCanLevelUp);
	
	bool resCanStepLevelUp = noticeHeroStepLevelUp();
	pEntity->chgNotice(GSProto::en_NoticeGuid_StepLevelUp,resCanStepLevelUp);

}

void HeroSystem::onTimer(int nEventId)
{
	
	if(1 == nEventId)
	{
		checkNotice();
	}
	
}

bool HeroSystem::noticeHeroLevelUp()	
{
	IEntity* pEntity = getEntityFromHandle(m_hEntity);
	assert(pEntity);
	
	IFormationSystem* pFormationSys = static_cast<IFormationSystem*> (pEntity->querySubsystem(IID_IFormationSystem));
	assert(pFormationSys);
	std::vector < HEntity > heroList;
	pFormationSys->getEntityList(heroList);

	for(size_t i = 0; i < heroList.size(); i++)
	{
		HEntity hTmpHero = heroList[i];
		IEntity* pHero = getEntityFromHandle(hTmpHero);
		assert(pHero);
		bool res = heroCanLevelUp(pHero);
		if(res)
		{
			return true;
		}
	}
	return false;
}

bool HeroSystem::noticeHeroStepLevelUp()
{
	IEntity* pEntity = getEntityFromHandle(m_hEntity);
	assert(pEntity);
	
	IFormationSystem* pFormationSys = static_cast<IFormationSystem*> (pEntity->querySubsystem(IID_IFormationSystem));
	assert(pFormationSys);
	std::vector < HEntity > heroList;
	pFormationSys->getEntityList(heroList);

	for(size_t i = 0; i < heroList.size(); i++)
	{
		HEntity hTmpHero = heroList[i];
		IEntity* pHero = getEntityFromHandle(hTmpHero);
		assert(pHero);
		bool res = herocanStepLevelUp(pHero);
		if(res)
		{
			return true;
		}
	}
	return false;
}

bool HeroSystem::heroCanLevelUp(IEntity* pHero)
{
	IEntity* pEntity = getEntityFromHandle(m_hEntity);
	assert(pEntity);
	
	int iCurHeroLv = pHero->getProperty(PROP_ENTITY_LEVEL, 0);
	int iLevel = pEntity->getProperty(PROP_ENTITY_LEVEL, 0);
	
	//高于英雄3级不能升级
	if(iCurHeroLv >= iLevel * 2)
	{
			return false;
	}
	
	//最高级不能升级
	ITable* pHeroLvExpTb = getCompomentObjectManager()->findTable("HeroLevelExp");
	assert(pHeroLvExpTb);
	if(pHeroLvExpTb->findRecord(iCurHeroLv + 1) < 0)
	{
		return  false;
	}
	
	// 是否存在修为?
	int iAcotorTkHeroExp = pEntity->getProperty(PROP_ENTITY_HEROEXP, 0);
	if(0 == iAcotorTkHeroExp)
	{
		return false;
	}
	//修为够升一级提醒
	
	int iRecord = pHeroLvExpTb->findRecord(iCurHeroLv);
	
	int iNeedHeroExp = pHeroLvExpTb->getInt(iRecord, "修为");
	if(iAcotorTkHeroExp < iNeedHeroExp )
	{
		return false;
	}
	
	return true;
}

bool HeroSystem::herocanStepLevelUp(IEntity* pHero)	
{
	int iHeroBaseID = pHero->getProperty(PROP_ENTITY_BASEID, 0);
	
	IJZEntityFactory* pJZEntityFactory = getComponent<IJZEntityFactory>(COMPNAME_EntityFactory, IID_IJZEntityFactory);
	assert(pJZEntityFactory);
	
	// 判断等阶是否达到最大值了
	int iOldLevelStep = pHero->getProperty(PROP_ENTITY_LEVELSTEP, 0);
	if(!pJZEntityFactory->queryLevelStepCfg(iOldLevelStep + 1) )
	{
		return false;
	}

	for(size_t i = 0; i < m_heroList.size(); ++i)
	{
		HEntity hTmpHero = m_heroList[i];
		if(hTmpHero == pHero->getHandle())
		{
			continue;
		}
		
		IEntity* pTempHero = getEntityFromHandle(hTmpHero);
		assert(pTempHero);

		int iCurHeroBaseID = pTempHero->getProperty(PROP_ENTITY_BASEID, 0);
		if(iHeroBaseID == iCurHeroBaseID)
		{
			IHeroEquipSystem* pHeroEquipSystem = static_cast<IHeroEquipSystem*>(pTempHero->querySubsystem(IID_IHeroEqupSystem));
			assert(pHeroEquipSystem);

			IHeroRealEquipSystem* pHeroRealEquipSys = static_cast<IHeroRealEquipSystem*>(pTempHero->querySubsystem(IID_IHeroRealEquupSystem) );
			assert(pHeroRealEquipSys);
			
			if(!pHeroEquipSystem->hasFightSoul() && !pHeroRealEquipSys->hasEquip())
			{
				return true;
			}
		}
	}
	
	return false;
}

void HeroSystem::fillLevelUpNotice(GSProto::NoticeHeroList& heroList)
{
	IEntity* pEntity = getEntityFromHandle(m_hEntity);
	assert(pEntity);
	
	IFormationSystem* pFormationSys = static_cast<IFormationSystem*>(pEntity->querySubsystem(IID_IFormationSystem));
	assert(pFormationSys);
	vector<HEntity> heroVec;
	pFormationSys->getEntityList(heroVec);
	bool bHave = false;
	for(size_t i = 0; i < heroVec.size(); i++)
	{
		HEntity hTmpHero = heroVec[i];
		IEntity* pHero = getEntityFromHandle(hTmpHero);
		assert(pHero);
		bool res = heroCanLevelUp(pHero);
		if(res)
		{
			heroList.add_szdwheroobjectid(hTmpHero);
			bHave = true;
		}
	}
	
   pEntity->chgNotice(GSProto::en_NoticeGuid_LevelUp,bHave);
}

void HeroSystem::fillStepLevelUpNotice(GSProto::NoticeHeroList& heroList)
{
	IEntity* pEntity = getEntityFromHandle(m_hEntity);
	assert(pEntity);

	bool bHave = false;
		
	IFormationSystem* pFormationSys = static_cast<IFormationSystem*> (pEntity->querySubsystem(IID_IFormationSystem));
	assert(pFormationSys);
	std::vector < HEntity > heroVec;
	pFormationSys->getEntityList(heroVec);
	for(size_t i = 0; i < heroVec.size(); i++)
	{
		HEntity hTmpHero = heroVec[i];
		IEntity* pHero = getEntityFromHandle(hTmpHero);
		assert(pHero);
		bool res = herocanStepLevelUp(pHero);
		if(res)
		{
			heroList.add_szdwheroobjectid(hTmpHero);
			bHave = true;
		}
	}
	
	pEntity->chgNotice(GSProto::en_NoticeGuid_StepLevelUp,bHave);
	
}


void HeroSystem::fillFemaleHeroList(vector<ServerEngine::FemailHeroDesc>& femaleList)
{
	for(size_t i = 0; i < m_heroList.size(); i++)
	{
		IEntity* pHero = getEntityFromHandle(m_heroList[i]);
		if(!pHero) continue;

		int iSex = pHero->getProperty(PROP_ENTITY_SEX, 0);
		if(iSex == GSProto::en_Sex_FeMale)
		{
			ServerEngine::FemailHeroDesc tmpHeroDesc;
			tmpHeroDesc.iHeroID = pHero->getProperty(PROP_ENTITY_BASEID, 0);
			tmpHeroDesc.iFightValue = pHero->getProperty(PROP_ENTITY_FIGHTVALUE, 0);
			femaleList.push_back(tmpHeroDesc);
		}
	}
}


void HeroSystem::GMOnlineGetRoleHeroData(string& strJson)
{

	strJson = "{\"cmd\":\"rolehero\",\"data\":[";

	for(size_t i = 0; i < m_heroList.size(); i++)
	{
		IEntity* pHero = getEntityFromHandle(m_heroList[i]);
		if(!pHero) continue;
		
		int iHeroId =  pHero->getProperty(PROP_ENTITY_BASEID, 0);
		int iLevel = pHero->getProperty(PROP_ENTITY_LEVEL, 0);
		int iLevelStep = pHero->getProperty(PROP_ENTITY_LEVELSTEP, 0)+1;
		
		strJson+="{\"heroid\":\""+TC_I2S(iHeroId)+"\","+"\"level\":\""+TC_I2S(iLevel)+ "\"," + "\"levelstep\":\""+TC_I2S(iLevelStep)+"\"}";
		
		if( i != m_heroList.size()-1) strJson+=",";
	}
	strJson += "]}";
	
	//cout<<strJson.c_str()<<endl;
}





