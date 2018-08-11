#include "ManorSystemPch.h"
#include "ManorSystem.h"
#include "ManorFactory.h"
#include "IJZEntityFactory.h"
#include "IModifyDelegate.h"

extern "C" IObject* createManorSystem()
{
	return new ManorSystem;
}

extern bool SaveOneSubsystemData(IEntity *pEntity, ServerEngine::PIRole& roleInfo, Uint32 dwSubsystemID);

ManorSystem::ManorSystem():
	iLootCount(0)
{ 
}

ManorSystem::~ManorSystem()
{
	ITimerComponent* timeComponent = getComponent<ITimerComponent>(COMPNAME_TimeAxis,IID_ITimerComponent);
	assert(timeComponent);
	timeComponent->killTimer( m_CheckVigorTimerHandle);
	timeComponent->killTimer( m_CheckHeroSoulAndTieJiangPu);
	
}

Uint32 ManorSystem::getSubsystemID() const
{
	return IID_IManorSystem;
}

Uint32 ManorSystem::getMasterHandle()
{
	return m_hEntity;
}

void ManorSystem::initMachineData(string strData)
{
	initManorSystemData();
	vector<int> dataVec = TC_Common::sepstr<int>(strData, "#");
	assert(dataVec.size() == 4);
	
	int iBaseSilver = dataVec[0];
	int iBaseHeroExp = dataVec[1];
	int iSilverLevel = dataVec[2];
	int iHeroExpLevel = dataVec[3];

	//这里记录机器人的固定产出，有点牵强
	m_manorSystemData.iProductSivlerBeforLevelUp  = iBaseSilver;
	m_manorSystemData.iProductHeroExpBeforLevelUp = iBaseHeroExp;
	m_manorSystemData.iBeLootSilver = 0;
	m_manorSystemData.iBeLootHeroExp = 0;
	//所有矿等级固定
	size_t dbSize = m_manorSystemData.silverResVec.size();
	for(size_t i = 0; i < dbSize; ++i)
	{   
		ServerEngine::ManorResData& resDBData = m_manorSystemData.silverResVec[i];
		resDBData.iResLevel = iSilverLevel;
	}
	size_t heroExpDBSize = m_manorSystemData.heroExpResVec.size();
	for(size_t i = 0; i < heroExpDBSize; ++i)
	{
		ServerEngine::ManorResData& heroExpDBData = m_manorSystemData.heroExpResVec[i];
		heroExpDBData.iResLevel = iHeroExpLevel;
	}
	
}

void ManorSystem::resetMachineData()
{
	m_manorSystemData.iBeLootSilver = 0;
	m_manorSystemData.iBeLootHeroExp = 0;
}


bool ManorSystem::create(IEntity* pEntity, const std::string& strData)
{
	m_hEntity = pEntity->getHandle();
	if(strData.size() > 0)
	{
		ServerEngine::JceToObj(strData, m_manorSystemData);
	}
	else
	{
		initManorSystemData();
	}

	//初始化精力
	ITimerComponent* timeComponent = getComponent<ITimerComponent>(COMPNAME_TimeAxis,IID_ITimerComponent);
	assert(timeComponent);
	m_CheckVigorTimerHandle = timeComponent->setTimer(this,1,5*1000,"ManorSystem");
	m_CheckHeroSoulAndTieJiangPu = timeComponent->setTimer(this,2,5*1000,"ManorSystem");
	//checkVigor();
	gmFlag = false;
	return true;
}

void  ManorSystem::onTimer(int nEventId)
{
	IEntity *pEntity = getEntityFromHandle(m_hEntity);
	assert(pEntity);
	if( pEntity->getProperty( PROP_ENTITY_CLASS, 0) != GSProto::en_class_Actor)
	{
		return;
	}
	if(1 == nEventId)
	{
		checkVigor();
	}
	else if(2 == nEventId)
	{
		CheckWuHunDianAndTieJiangPuBuild();
	}

}

void ManorSystem::checkVigor()
{
	IZoneTime *pZone = getComponent<IZoneTime>(COMPNAME_ZoneTime, IID_IZoneTime);
	assert(pZone);
	IEntity* pEntity = getEntityFromHandle(m_hEntity);
	assert(pEntity);
	int iVigor = pEntity->getProperty(PROP_ENTITY_ACTOR_VIGOR, 0);
	IGlobalCfg *global = getComponent<IGlobalCfg>(COMPNAME_GlobalCfg, IID_IGlobalCfg);
	assert(global);
	int iVigorLimit = global->getInt("领地体力上限", 30);
	int iAddOneVigor = global->getInt("领地恢复1点体力时间间隔",1800);
	int iRemaind = pZone->GetCurSecond() - m_manorSystemData.iVigorLastSecond;
	int iVigorCount = iRemaind/iAddOneVigor + iVigor;
	if ( iVigorCount > iVigor)
	{
		iVigorCount = std::min(iVigorLimit, iVigorCount);
		int iChange = iVigorCount - iVigor;
		pEntity->changeProperty(PROP_ENTITY_ACTOR_VIGOR,iChange ,GSProto::en_Reason_RECOVER_VIGOR);
		
		PLAYERLOG(this)<<"ChgProperty|"<<GSProto::en_LifeAtt_actorVigor<<"|"<<iChange<<"|"<<GSProto::en_Reason_RECOVER_VIGOR<<endl;
		
		int imodSecond = iRemaind % iAddOneVigor;
		m_manorSystemData.iVigorLastSecond = pZone->GetCurSecond() - imodSecond;
	}
	//检查提示放在这里
	checkNotice();
}

void ManorSystem::checkNotice()
{
	IEntity *pEntity = getEntityFromHandle(m_hEntity);
	assert(pEntity);
	
	bool bHaveNewLog = m_manorSystemData.bHaveNewLog;

	IZoneTime *pZone = getComponent<IZoneTime>(COMPNAME_ZoneTime, IID_IZoneTime);
	assert(pZone);
	ManorFactory * pManorFactory = getComponent<ManorFactory>(COMPNAME_ManorFactory, IID_IManorFactory);
	assert(pManorFactory);
	IGlobalCfg * pGlobal = getComponent<IGlobalCfg>(COMPNAME_GlobalCfg, IID_IGlobalCfg);
	assert(pGlobal);
	ServerEngine::ManorWuHunAndItemData& WuHundbData = m_manorSystemData.wuHunDianData;
	HeroSoulOrItem wuHunDian;
	bool res = pManorFactory->getActorWuHunDianByLevel(wuHunDian,WuHundbData.level);
	assert(res);

	//武魂殿
	bool bHaveWuHun = false;
	if(WuHundbData.bIsProduct)
	{
		int iProductSecond =  wuHunDian.iProductSecond - ( pZone->GetCurSecond() - WuHundbData.beginProductSecond);
		if(iProductSecond <= 0)
		{
			bHaveWuHun = true;
		}
	}

	//铁匠铺
	ServerEngine::ManorWuHunAndItemData& TieJiangPudbData = m_manorSystemData.tieJiangPuData;
	HeroSoulOrItem tieJiangPu;
	bool res2 = pManorFactory->getActorTieJiangPuByLevel(tieJiangPu, TieJiangPudbData.level);
	assert(res2);
	bool bHaveItem = false;
	if(TieJiangPudbData.bIsProduct)
	{
		int iProductSecond =  tieJiangPu.iProductSecond - ( pZone->GetCurSecond() - TieJiangPudbData.beginProductSecond);
		if(iProductSecond <= 0)
		{
			bHaveItem = true;
		}
	}

	pEntity->chgNotice( GSProto::en_NoticeGuid_ManorHaveWuHun, bHaveWuHun);
	pEntity->chgNotice( GSProto::en_NoticeGuid_ManorHaveItem, bHaveItem);
	pEntity->chgNotice( GSProto::en_NoticeGuid_ManorHaveNewLog, bHaveNewLog);
	
}

void ManorSystem::checkRes()
{
	ManorFactory* pManorFactory = getComponent<ManorFactory>(COMPNAME_ManorFactory, IID_IManorFactory);
	assert(pManorFactory);

	vector<ManorRes> silverResVec;
	bool res = pManorFactory->getActorOpenSilverResList(silverResVec, m_hEntity);
	assert(res);
	size_t dbSize = m_manorSystemData.silverResVec.size();
	if(dbSize == silverResVec.size()) return;
	for(size_t i = dbSize; i < silverResVec.size(); ++i)
	{   
		ManorRes manorRes = silverResVec[i];
		ServerEngine::ManorResData resDBData;
		resDBData.ResId = manorRes.iResId;
		resDBData.iResLevel = 0;
		resDBData.bResIsBuild = false;
		m_manorSystemData.silverResVec.push_back(resDBData);
	}

	vector<ManorRes> heroExpResVec;
	bool res2 = pManorFactory->getActorOPenHeroExpResList(heroExpResVec, m_hEntity);
	assert(res2);
	size_t heroExpDBSize = m_manorSystemData.heroExpResVec.size();
	if(heroExpDBSize == heroExpResVec.size()) return;
	for(size_t i = heroExpDBSize; i < heroExpResVec.size(); ++i)
	{
		ManorRes heroExpRes = heroExpResVec[i];
		ServerEngine::ManorResData heroExpDBData;
		heroExpDBData.ResId = heroExpRes.iResId;
		heroExpDBData.iResLevel = 0;
		heroExpDBData.bResIsBuild = false;
		m_manorSystemData.heroExpResVec.push_back(heroExpDBData);
	}
}

void ManorSystem::initManorSystemData()
{
	checkRes();
	//铁匠铺
	m_manorSystemData.tieJiangPuData.bResIsBuild = false;
	m_manorSystemData.tieJiangPuData.bIsProduct = false;
	m_manorSystemData.tieJiangPuData.level = 1;
	
	m_manorSystemData.wuHunDianData.bResIsBuild = false;
	m_manorSystemData.wuHunDianData.bIsProduct = false;
	m_manorSystemData.wuHunDianData.level = 1;
	m_manorSystemData.bIsBeLoot = false;
	//add for notice
	m_manorSystemData.bHaveNewLog = false;
	//m_manorSystemData.bGuideOver = false;
	IGlobalCfg *pGlobal = getComponent<IGlobalCfg>(COMPNAME_GlobalCfg, IID_IGlobalCfg);
	assert(pGlobal);
	m_manorSystemData.iProductSivlerBeforLevelUp  =  pGlobal->getInt("引导时铜币产出", 5000);
	m_manorSystemData.iProductHeroExpBeforLevelUp = pGlobal->getInt("引导时修为产出", 5000);
	IZoneTime *pZone = getComponent<IZoneTime>(COMPNAME_ZoneTime, IID_IZoneTime);
	assert(pZone);
//	m_manorSystemData.dwHarvestSecond = pZone->GetCurSecond();
	m_manorSystemData.iVigorLastSecond = 0;
}

bool ManorSystem::createComplete()
{
	checkVigor();
	return true;
}

const std::vector<Uint32>& ManorSystem::getSupportMessage()
{
	static vector<Uint32> msgCmdList;
	if(0 == msgCmdList.size())
	{
		msgCmdList.push_back(GSProto::CMD_MANOR_QUERY);				
		msgCmdList.push_back(GSProto::CMD_MANOR_RES_USE_GOLD_LEVELUP);		
		msgCmdList.push_back(GSProto::CMD_MANOR_RES_COMMON_LEVELUP);		
		msgCmdList.push_back(GSProto::CMD_MANOR_RES_HARVEST);	        		
		msgCmdList.push_back(GSProto::CMD_MANOR_WUHUNDIAN_QUERY);	   			
		msgCmdList.push_back(GSProto::CMD_MANOR_WUHUNDIAN_LEVELUP );			
		msgCmdList.push_back(GSProto::CMD_MANOR_WUHUNDIAN_BEGIN_PRODUCT ); 		
		msgCmdList.push_back(GSProto::CMD_MANOR_WUHUNDIAN_WUXIEKEJIE);		
		msgCmdList.push_back(GSProto::CMD_MANOR_WUHUNDIAN_REFRESH);		
		msgCmdList.push_back(GSProto::CMD_MANOR_TIEJIANGPU_QUERY);			
		msgCmdList.push_back(GSProto::CMD_MANOR_TIEJIANGPU_LEVELUP);		
		msgCmdList.push_back(GSProto::CMD_MANOR_TIEJIANGPU_REFRESH);	
		msgCmdList.push_back(GSProto::CMD_MANOR_TIEJIANGPU_BEGIN_PRODUCT);
		msgCmdList.push_back(GSProto::CMD_MANOR_TIEJIANGPU_WUXIEKEJI);	
		msgCmdList.push_back(GSProto::CMD_MANOR_QUERY_LOG);			
		msgCmdList.push_back(GSProto::CMD_MANOR_REVENGE);				
		msgCmdList.push_back(GSProto::CMD_MANOR_PUBLISH);				
		msgCmdList.push_back(GSProto::CMD_MANOR_WUHUNDIAN_BULID_ADDSPEED);
		msgCmdList.push_back(GSProto::CMD_MANOR_TIEJIANGPU_BULID_ADDSPEED);
		msgCmdList.push_back(GSProto::CMD_MANOR_REQUEST_LOOT);
		msgCmdList.push_back(GSProto::CMD_MANOR_REQUEST_LOOT_BATTLE);
		msgCmdList.push_back(GSProto::CMD_MANOR_WUHUNDIAN_ONEKEY_HARVEST);
		msgCmdList.push_back(GSProto::CMD_MANOR_TIEJIANGPU_ONEKEY_HARVEST);
		msgCmdList.push_back(GSProto::CMD_MANORSYSTEM_QUERY_PROTECT);
		msgCmdList.push_back(GSProto::CMD_MANORSYTEM_BUY_PROTECT);

	}
	return msgCmdList;
}

void ManorSystem::onMessage(QxMessage* pMessage)
{
	assert(pMessage->dwMsgLen == sizeof(GSProto::CSMessage));
	const GSProto::CSMessage& msg =*(const GSProto::CSMessage*)(pMessage->pMsgDataBuff);
	checkReset();
	switch(msg.icmd())
	{
		case GSProto::CMD_MANOR_QUERY:
		{
			onQueryManorInfo(msg);
		}break;
		
		case GSProto::CMD_MANOR_RES_USE_GOLD_LEVELUP:
		{
			onResLevelUpUseGold(msg);
		}break;
		
		case GSProto::CMD_MANOR_RES_COMMON_LEVELUP:
		{
			onResLevelUpCommon(msg);
		}break;		
		
		case GSProto::CMD_MANOR_RES_HARVEST:
		{
			onHarvestRes(msg);
		}break;		
			        		
		case GSProto::CMD_MANOR_WUHUNDIAN_QUERY:
		{
			bool res = wuHunDianAndTieJiangPuIsOpen(true);
			if(res)
			{
				onQueryWuHunDian(msg);
			}
			
		}break;		
			   			
		case GSProto::CMD_MANOR_WUHUNDIAN_LEVELUP :
		{
			bool res = wuHunDianAndTieJiangPuIsOpen(true);
			if(res)
			{
				onWuHunDianLevelUp(msg);
			}
		}break;		
				
		case GSProto::CMD_MANOR_WUHUNDIAN_BEGIN_PRODUCT:
		{
			bool res = wuHunDianAndTieJiangPuIsOpen(true);
			if(res)
			{
				onWuHunDianBeginProduct(msg);
			}
			
		}break;		
			
		case GSProto::CMD_MANOR_WUHUNDIAN_WUXIEKEJIE:
		{
			bool res = wuHunDianAndTieJiangPuIsOpen(true);
			if(res)
			{
				onWuHunDianSetHeroWuxiekeji(msg);
			}
		}break;		
			
		case GSProto::CMD_MANOR_WUHUNDIAN_REFRESH:
		{
			bool res = wuHunDianAndTieJiangPuIsOpen(true);
			if(res)
			{
				onWuHunDianRefresh(msg);
			}
		}break;		
			
		case GSProto::CMD_MANOR_TIEJIANGPU_QUERY:
		{
			bool res = wuHunDianAndTieJiangPuIsOpen();
			if(res)
			{
				onQueryTieJiangPu(msg);
			}
		}break;		
					
		case GSProto::CMD_MANOR_TIEJIANGPU_LEVELUP:
		{
			bool res = wuHunDianAndTieJiangPuIsOpen();
			if(res)
			{
				onTieJiangPuLevelUp(msg);
			}
		}break;		
				
		case GSProto::CMD_MANOR_TIEJIANGPU_REFRESH:
		{
			bool res = wuHunDianAndTieJiangPuIsOpen();
			if(res)
			{
				onTieJiangPuRefresh(msg);
			}
		}break;		
			
		case GSProto::CMD_MANOR_TIEJIANGPU_BEGIN_PRODUCT:
		{
			bool res = wuHunDianAndTieJiangPuIsOpen();
			if(res)
			{
				onTieJiangPuBeginProduct(msg);
			}
		}break;		
		
		case GSProto::CMD_MANOR_TIEJIANGPU_WUXIEKEJI:
		{
			bool res = wuHunDianAndTieJiangPuIsOpen();
			if(res)
			{
				onTieJiangPuSetItemWuxiekeji(msg);
			}
		}break;		
			
		case GSProto::CMD_MANOR_QUERY_LOG:
		{
			onQueryManorLog(msg);
		}break;		
					
		case GSProto::CMD_MANOR_REVENGE:
		{
			onReqRevenge(msg);
		}break;		
						
		case GSProto::CMD_MANOR_PUBLISH:
		{
			onPublishReward( msg);
		}break;		
						
		case GSProto::CMD_MANOR_WUHUNDIAN_BULID_ADDSPEED:
		{
			bool res = wuHunDianAndTieJiangPuIsOpen(true);
			if(res)
			{
				onWuHunDianBulidAddSpeed(msg);
			}
			
		}break;		
		
		case GSProto::CMD_MANOR_TIEJIANGPU_BULID_ADDSPEED:
		{
			bool res = wuHunDianAndTieJiangPuIsOpen();
			if(res)
			{
				onTieJiangPuBulidAddSpeed(msg);
			}
		}break;		

		case GSProto::CMD_MANOR_REQUEST_LOOT:
		{
			onRequestLoot( msg);
		}break;

		case GSProto::CMD_MANOR_REQUEST_LOOT_BATTLE:
		{
			onRequestLootBattle(msg);
		}break;

		case GSProto::CMD_MANOR_WUHUNDIAN_ONEKEY_HARVEST:
		{
			bool res = wuHunDianAndTieJiangPuIsOpen(true);
			if(res)
			{
				onRequestOneKeyHarvestWuHunDian(msg);
			}
			
		}break;

		case GSProto::CMD_MANOR_TIEJIANGPU_ONEKEY_HARVEST:
		{
			bool res = wuHunDianAndTieJiangPuIsOpen();
			if(res)
			{
				onRequestOneKeyHarvestTieJiangPu(msg);
			}
		}break;

		case GSProto::CMD_MANORSYSTEM_QUERY_PROTECT:
		{
				onQueryProtectInfo(msg);
		}break;

		case GSProto::CMD_MANORSYTEM_BUY_PROTECT:
		{
				onBuyProtect(msg);
		}break;
	}
}

void ManorSystem::clearProtectItemCD()
{
	IZoneTime *pZone = getComponent<IZoneTime>(COMPNAME_ZoneTime, IID_IZoneTime);
	assert(pZone);
	
	ManorFactory *pManorFactory =static_cast<ManorFactory*>( getComponent<IManorFactory>(COMPNAME_ManorFactory, IID_IManorFactory));
	assert(pManorFactory);

	int iCurSecond = pZone->GetCurSecond();

	ServerEngine::ManorProtectData& protectData = m_manorSystemData.protectData;
	vector<ServerEngine::TimeResetValue>::iterator iter = protectData.haveBuyProtectList.begin();
	for(; iter != protectData.haveBuyProtectList.end(); )
	{
		const ServerEngine::TimeResetValue& timeResetValue = *iter;
		int iId = timeResetValue.iValue;
		int iBeginCd = timeResetValue.dwLastChgTime;
		
		ManorProtect protect ;
		bool res = pManorFactory->getManorProtectById( iId, protect);
		assert(res);
		
		int iRemaindSecond = iCurSecond - iBeginCd;
		bool bHaveCd = (iRemaindSecond < protect.iBuycdSecond);
		if(!bHaveCd)
		{
			protectData.haveBuyProtectList.erase(iter);
		}
		else
		{
			++iter;
		}
	}
}

void ManorSystem::onQueryProtectInfo(const GSProto::CSMessage& msg)
{

	//清理数据
	clearProtectItemCD();
	
	pushProtectInfo();
	
	
}

void ManorSystem::pushProtectInfo()
{
	IEntity* pEntity = getEntityFromHandle(m_hEntity);
	assert(pEntity);

	IZoneTime *pZone = getComponent<IZoneTime>(COMPNAME_ZoneTime, IID_IZoneTime);
	assert(pZone);

	ManorFactory *pManorFactory =static_cast<ManorFactory*>( getComponent<IManorFactory>(COMPNAME_ManorFactory, IID_IManorFactory));
	assert(pManorFactory);

	GSProto::CMD_MANORSYSTEM_QUERY_PROTECT_SC scMsg;
	int iCurSecond = pZone->GetCurSecond();

	const ServerEngine::ManorProtectData& protectData = m_manorSystemData.protectData;
	
	for(size_t i = 0; i < protectData.haveBuyProtectList.size(); ++i)
	{
		const ServerEngine::TimeResetValue& timeResetValue = protectData.haveBuyProtectList[i];
		int iId = timeResetValue.iValue;
		int iBeginCd = timeResetValue.dwLastChgTime;
		
		GSProto::ProtectDetail & detail = *(scMsg.add_szprotectdetail());
		detail.set_iprotectid(iId );

		ManorProtect protect;
		bool res = pManorFactory->getManorProtectById( iId,protect);
		assert(res);
		
		int iRemaindSecond = iCurSecond - iBeginCd;
		bool bHaveCd = (iRemaindSecond < protect.iBuycdSecond);
		if(bHaveCd)
		{
			detail.set_iremaindsecond(protect.iBuycdSecond - iRemaindSecond);
		}
	}

	pEntity->sendMessage(GSProto::CMD_MANORSYSTEM_QUERY_PROTECT, scMsg);
}


void ManorSystem::onBuyProtect(const GSProto::CSMessage& msg)
{
	GSProto::CMD_MANORSYTEM_BUY_PROTECT_CS csMsg;
	if(! csMsg.ParseFromString(msg.strmsgbody()))
	{
		return;
	}

	
	clearProtectItemCD();
	
	int iProtectId = csMsg.iprotectid();
	
	IEntity* pEntity = getEntityFromHandle(m_hEntity);
	assert(pEntity);
	
	ManorFactory *pManorFactory =static_cast<ManorFactory*>( getComponent<IManorFactory>(COMPNAME_ManorFactory, IID_IManorFactory));
	assert(pManorFactory);
	
	ManorProtect protect;
	bool res = pManorFactory->getManorProtectById( iProtectId, protect);
	if(!res)
	{
		return;
	}

	//是否还在CD
	ServerEngine::ManorProtectData& protectData = m_manorSystemData.protectData;
	for(size_t i = 0; i < protectData.haveBuyProtectList.size(); ++i)
	{
		ServerEngine::TimeResetValue& timeResetValue = protectData.haveBuyProtectList[i];
		if(timeResetValue.iValue == iProtectId)
		{
			return;
		}
	}

	int iPrice = protect.iPrice;
	int iProtectSecond = protect.iProtectSecond;

	int iActorHave = pEntity->getProperty(PROP_ACTOR_GOLD, 0);
	if(iActorHave < iPrice)
	{
		pEntity->sendErrorCode( ERROR_NEED_GOLD);
		return;
	}

	//扣钱
	assert(iPrice >=0);
	pEntity->changeProperty( PROP_ACTOR_GOLD, 0-iPrice, GSProto::en_Reason_Manor_Protect);

	//给保护
	//int iRemainSecond = getRemaindProtectSecond(m_manorSystemData);
	IZoneTime *pZone = getComponent<IZoneTime>(COMPNAME_ZoneTime, IID_IZoneTime);
	assert(pZone);
	int iCurSecond = pZone->GetCurSecond();
	int iProtectEndSecond = protectData.iBeginProtectSecond + protectData.iHaveBuyProtectSecond;
	int iRemaindSecond = iProtectEndSecond - iCurSecond;
	if(iRemaindSecond > 0)
	{
		protectData.iHaveBuyProtectSecond += iProtectSecond;
	}
	else
	{
		protectData.iHaveBuyProtectSecond = iProtectSecond;
		protectData.iBeginProtectSecond = time(0);
	}

	ServerEngine::TimeResetValue timeResetValue;
	timeResetValue.iValue = iProtectId;
	timeResetValue.dwLastChgTime = time(0);
	protectData.haveBuyProtectList.push_back(timeResetValue);

	//下行推送
	pushProtectInfo();
	
}

bool ManorSystem::wuHunDianAndTieJiangPuIsOpen(bool bIsWuHunDian /*=false*/)
{
	IEntity *pEntity = getEntityFromHandle(m_hEntity);
	assert(pEntity);
	IGlobalCfg *pGlobal = getComponent<IGlobalCfg>(COMPNAME_GlobalCfg, IID_IGlobalCfg);
	assert(pGlobal);
	int iNeedLevel = pGlobal->getInt("武魂殿铁匠铺开启等级", 7);
	int iActorLevel = pEntity->getProperty(PROP_ENTITY_LEVEL,1);
	if(iNeedLevel > iActorLevel)
	{
		if(bIsWuHunDian)
		{
			pEntity->sendErrorCode(ERROR_MANOR_OPEN_LEVEL);
		}
		else
		{
			pEntity->sendErrorCode(ERROR_MANOR_TIEJIANGPU_OPENLEVEL);
		}
		
		return false;
	}

	
	return true;
}

void ManorSystem::pushManorInfo2Client()
{
	IEntity* pEntity = getEntityFromHandle(m_hEntity);
	assert(pEntity);
	checkRes();
	IZoneTime *pZone = getComponent<IZoneTime>(COMPNAME_ZoneTime, IID_IZoneTime);
	assert(pZone);
	ManorFactory * pManorFactory = getComponent<ManorFactory>(COMPNAME_ManorFactory, IID_IManorFactory);
	assert(pManorFactory);
	GSProto::CMD_MANOR_QUERY_SC scMsg;
	//填充铜矿信息s
	fillResData(m_manorSystemData.silverResVec, GSProto::en_ManorResType_Silver, scMsg.mutable_szmanorsilverresdetail());
	//填充修为矿信息
	fillResData(m_manorSystemData.heroExpResVec, GSProto::en_ManorResType_HEROEXP, scMsg.mutable_szmanorheroexpresdetail());
	
	IGlobalCfg* pGlobal = getComponent<IGlobalCfg>(COMPNAME_GlobalCfg, IID_IGlobalCfg);
	assert(pGlobal);
	int iTotalBulidTeams = pGlobal->getInt("领地矿建队总数",3);
	scMsg.set_itotalbuildteams(iTotalBulidTeams);
	int nowbulidteams = getNewBuildTeams();
	scMsg.set_inowbulidteams(nowbulidteams);

	int iSilverSpeed = 0;
	int iHeroExpSeed = 0;
	int iSilverTotalOutPut = 0;
	int iHeroExpTotalPutPut = 0;
	getSilverSpeedAndTotolOutPut( iSilverSpeed, iSilverTotalOutPut);
	getHeroExpSpeedAndTotolOutPut( iHeroExpSeed, iHeroExpTotalPutPut);

	scMsg.set_isilverproductspeed( iSilverSpeed);
	scMsg.set_iheroexpproductspeed( iHeroExpSeed);
	scMsg.set_iheroexptotalproduct( iHeroExpTotalPutPut);
	scMsg.set_isilvertotalproduct(	iSilverTotalOutPut);

	//todo 铁匠铺
	CheckWuHunDianAndTieJiangPuBuild();
	fillTieJiangPuDetail( scMsg.mutable_tiejiangpudetail() );
	//武魂殿
	fillWuHunDianDetail( scMsg.mutable_wuhundiandetail() );
	//铜币
	int iNowSilverPutOut = calcSilverPutOut() + m_manorSystemData.iProductSivlerBeforLevelUp;
	iNowSilverPutOut = std::min(iNowSilverPutOut, iSilverTotalOutPut);
	iNowSilverPutOut = iNowSilverPutOut - m_manorSystemData.iBeLootSilver;
	//修为
	int iNowHeroExpPutOut = calcHeroExpPutOut() + m_manorSystemData.iProductHeroExpBeforLevelUp;
	iNowHeroExpPutOut = std::min(iNowHeroExpPutOut,iHeroExpTotalPutPut );
	iNowHeroExpPutOut = iNowHeroExpPutOut-m_manorSystemData.iBeLootHeroExp;
	/*if(!m_manorSystemData.bGuideOver)
	{
		
		iNowSilverPutOut = pGlobal->getInt("引导时铜币产出", 5000);
		iNowHeroExpPutOut = pGlobal->getInt("引导时修为产出", 5000);
	}*/
		
	scMsg.set_icurheroexpputout(iNowHeroExpPutOut );
	scMsg.set_icursilverputout(iNowSilverPutOut);
	
	//收取次数
	//int iHavestLimit = pGlobal->getInt("每日收取矿产次数",3);
	IVIPFactory* pVipFactory = getComponent<IVIPFactory>(COMPNAME_VIPFactory,IID_IVIPFactory);
	assert(pVipFactory);
	int iHavestLimit = pVipFactory->getVipPropByHEntity(m_hEntity, VIP_PROP_MANOR_HARVEST_TIMES);
	
	int iActorHaveHavest = m_manorSystemData.iResHarvestTimes;
	int iCanharvest = iHavestLimit - iActorHaveHavest;
	if( iCanharvest < 0) iCanharvest = 0;
	scMsg.set_icurcanhavesttimes(iCanharvest);
	scMsg.set_itotalhavesttimes(iHavestLimit);

	//剩余保护时间
	int iRemaindProtectSecond = getRemaindProtectSecond(m_manorSystemData);
	scMsg.set_iremaindprotectsecond( iRemaindProtectSecond );

	//收索消耗
	
	scMsg.set_isearchcost(searchCost(m_hEntity));

	pEntity->sendMessage(GSProto::CMD_MANOR_QUERY, scMsg);
}

int ManorSystem::searchCost(HEntity hEntity)
{
	IEntity* pEntity = getEntityFromHandle(hEntity);
	assert(pEntity);

	IGlobalCfg* pGlobal = getComponent<IGlobalCfg>(COMPNAME_GlobalCfg, IID_IGlobalCfg);
	assert(pGlobal);

	int iLootNeedSilver = pGlobal->getInt("查找掠夺对象消耗铜币",2000);
	int iKey = pGlobal->getInt("查找掠夺对象消耗铜币系数",20);
	int iLevel = pEntity->getProperty(PROP_ENTITY_LEVEL, 1);
	int iSearchCost = iLootNeedSilver + iKey*iLevel;
	return iSearchCost;
}

int ManorSystem::getRemaindProtectSecond(const ServerEngine::ManorSystemData& manorSystemData )
{
	IZoneTime *pZoneTime = getComponent<IZoneTime>(COMPNAME_ZoneTime, IID_IZoneTime);
	assert(pZoneTime);

	IGlobalCfg *pGlobal = getComponent<IGlobalCfg>(COMPNAME_GlobalCfg, IID_IGlobalCfg);
	assert(pGlobal);
	
	const ServerEngine::ManorProtectData& protectData = manorSystemData.protectData;

	int iCurSecond = pZoneTime->GetCurSecond();
	int iProtectEndSecond = protectData.iBeginProtectSecond + protectData.iHaveBuyProtectSecond;
	int iRemaindSecond = iProtectEndSecond - iCurSecond;

	iRemaindSecond = std::max(0, iRemaindSecond);

	int iActorProtectSecond = iCurSecond - manorSystemData.iLostBeLootSecond;
	int iSecond = pGlobal->getInt("领地被掠夺了的保护时间",7200);
	int iBeLootProtect = iSecond - iActorProtectSecond;
	
	iBeLootProtect = std::max(0,iBeLootProtect);
	iRemaindSecond += iBeLootProtect;
	
	return iRemaindSecond;
}


bool ManorSystem::manorBuildTeamsCanBulid()
{
	IGlobalCfg* pGlobal = getComponent<IGlobalCfg>(COMPNAME_GlobalCfg, IID_IGlobalCfg);
	assert(pGlobal);

	int iTotalBulidTeams = pGlobal->getInt("领地矿建队总数",3);
	int iNewBuild = getNewBuildTeams();
	assert(iTotalBulidTeams >= iNewBuild);
	return iTotalBulidTeams > iNewBuild;
}

int ManorSystem::getNewBuildTeams()
{
	IGlobalCfg* pGlobal = getComponent<IGlobalCfg>(COMPNAME_GlobalCfg, IID_IGlobalCfg);
	assert(pGlobal);
	
	int nowbulidteams = 0;
	for(size_t i = 0; i < m_manorSystemData.silverResVec.size(); ++i)
	{
		ServerEngine::ManorResData& data = m_manorSystemData.silverResVec[i];
		if(data.bResIsBuild) nowbulidteams++;
		
	}
	
	for(size_t i = 0; i < m_manorSystemData.heroExpResVec.size(); ++i)
	{
		ServerEngine::ManorResData& data = m_manorSystemData.heroExpResVec[i];
		if(data.bResIsBuild) nowbulidteams++;
	}

	ServerEngine::ManorWuHunAndItemData& dbWuHunDian = m_manorSystemData.wuHunDianData;
	if(dbWuHunDian.bResIsBuild)nowbulidteams++;

	ServerEngine::ManorWuHunAndItemData& tieJiangPu = m_manorSystemData.tieJiangPuData;
	if(tieJiangPu.bResIsBuild)nowbulidteams++;

	return nowbulidteams;
	
}

void ManorSystem::fillTieJiangPuDetail(GSProto::ManorResDetail* detail)
{
	
	IZoneTime *pZone = getComponent<IZoneTime>(COMPNAME_ZoneTime, IID_IZoneTime);
	assert(pZone);
	
	ManorFactory * pManorFactory = getComponent<ManorFactory>(COMPNAME_ManorFactory, IID_IManorFactory);
	assert(pManorFactory);
	ServerEngine::ManorWuHunAndItemData& dbData = m_manorSystemData.tieJiangPuData;
	HeroSoulOrItem tieJiangPu;
	bool res = pManorFactory->getActorTieJiangPuByLevel(tieJiangPu,dbData.level);
	assert(res);
	detail->set_iresid(-1);
	
	detail->set_irestype(GSProto::en_ManorResType_TIEJIANGPU);
	detail->set_bresinbuild(dbData.bResIsBuild);
	if(dbData.bResIsBuild)
	{
		int iLevelUpSecond =  tieJiangPu.iLevelUpSecond - (pZone->GetCurSecond() - dbData.beginLevelUpSecond);
		assert(iLevelUpSecond > 0);
		detail->set_iremainsecond(iLevelUpSecond);
	}
	
	//判断是否已经祭炼完成
	if(dbData.bIsProduct)
	{
		int iCurProductSecond = tieJiangPu.iProductSecond - (pZone->GetCurSecond() - dbData.beginProductSecond);
		detail->set_bcanharvest((iCurProductSecond <= 0));
	}
	detail->set_ireslevel(dbData.level);
}

void ManorSystem::fillWuHunDianDetail(GSProto::ManorResDetail* detail)
{
	IZoneTime *pZone = getComponent<IZoneTime>(COMPNAME_ZoneTime, IID_IZoneTime);
	assert(pZone);
	
	ManorFactory * pManorFactory = getComponent<ManorFactory>(COMPNAME_ManorFactory, IID_IManorFactory);
	assert(pManorFactory);
	ServerEngine::ManorWuHunAndItemData& dbData = m_manorSystemData.wuHunDianData;
	HeroSoulOrItem wuHunDian;
	bool res = pManorFactory->getActorWuHunDianByLevel(wuHunDian,dbData.level);
	assert(res);
	detail->set_iresid(-1);
	
	detail->set_irestype(GSProto::en_ManorResType_WUHUNDIAN);
	detail->set_bresinbuild(dbData.bResIsBuild);
	if(dbData.bResIsBuild)
	{
		int iLevelUpSecond =  wuHunDian.iLevelUpSecond - (pZone->GetCurSecond() - dbData.beginLevelUpSecond);
		assert(iLevelUpSecond > 0);
		detail->set_iremainsecond(iLevelUpSecond);
	}
	//判断是否已经祭炼完成
	if(dbData.bIsProduct)
	{
		int iCurProductSecond = wuHunDian.iProductSecond - (pZone->GetCurSecond() - dbData.beginProductSecond);
		detail->set_bcanharvest((iCurProductSecond <= 0));
	}
	detail->set_ireslevel(dbData.level);
}

void ManorSystem::onRequestOneKeyHarvestWuHunDian(const GSProto::CSMessage& msg)
{
	ManorFactory *pFact = getComponent<ManorFactory>(COMPNAME_ManorFactory, IID_IManorFactory);
	assert(pFact);
	IZoneTime *pZoneTime = getComponent<IZoneTime>(COMPNAME_ZoneTime, IID_IZoneTime);
	assert(pFact);
	ManorFactory * pManorFactory = getComponent<ManorFactory>(COMPNAME_ManorFactory, IID_IManorFactory);
	assert(pManorFactory);
	
	IEntity *pEntity = getEntityFromHandle(m_hEntity);
	assert(pEntity);
	
	ServerEngine::ManorWuHunAndItemData& dbData = m_manorSystemData.wuHunDianData;
	HeroSoulOrItem wuHunDian;
	bool res = pManorFactory->getActorWuHunDianByLevel(wuHunDian,dbData.level);
	assert(res);
	if(!dbData.bIsProduct)
	{
		return;
	}
	
	int iCurProductSecond = pZoneTime->GetCurSecond() - dbData.beginProductSecond;
	if(iCurProductSecond < wuHunDian.iProductSecond)
	{
		return;
	}
	//收获
	ITable* pTable = getCompomentObjectManager()->findTable(TABLENAME_Hero);
	assert(pTable);
	IItemSystem *pItemSystem = static_cast<IItemSystem*>(pEntity->querySubsystem(IID_IItemSystem));
	assert(pItemSystem);
	for(size_t i = 0; i < dbData.wuHunAndItemList.size(); ++i)
	{
		const ServerEngine::ManorWuHunAndItemUnit &unit = dbData.wuHunAndItemList[i];
		int iRemaind = unit.iCount - unit.iBeLootCount;
		assert(iRemaind >= 0);
		if(iRemaind >0)
		{
			int iRecord = pTable->findRecord(unit.id);
			assert(iRecord >= 0);
			int iHeroSoulId = pTable->getInt(iRecord, "英雄魂魄物品ID");
			assert(iHeroSoulId > 0);
			pItemSystem->addItem(iHeroSoulId, iRemaind,GSProto::en_Reason_ManorRes_Harvest);
		}
	}
	
	dbData.bIsProduct = false;
	dbData.wuHunAndItemList.clear();
	
	/*if(dbData.bResIsBuild)
	{
		int iLevelUpSecond =  wuHunDian.iLevelUpSecond - (pZoneTime->GetCurSecond() - dbData.beginLevelUpSecond);
		if(iLevelUpSecond <= 0)
		{
			dbData.bResIsBuild = false;
			dbData.level += 1;
		}
	}*/
/*	int count = pManorFactory->getActorWuHunDoxsCount(m_hEntity);
	vector<ManorItem> heroSoulList;
	pManorFactory->getWuHunByLevel(heroSoulList,count,dbData.level);
	m_manorSystemData.wuHunDianData.wuHunAndItemList.clear();
	for(size_t j= 0 ; j<  heroSoulList.size(); ++j)
	{
		ServerEngine::ManorWuHunAndItemUnit unit;
		ManorItem& item = heroSoulList[j];
		unit.id = item.iId;
		unit.iCount = item.iCount;
		unit.iBeLootCount = 0;
		unit.bWuXieKeji = false;
		m_manorSystemData.wuHunDianData.wuHunAndItemList.push_back(unit);
	}*/
	m_manorSystemData.wuHunDianData.wuHunAndItemList.clear();
	m_manorSystemData.wuHunDianData.befroreProductCache.clear();

	//更新客户端
	sendWuHunDian2Client();
}

void ManorSystem::onRequestOneKeyHarvestTieJiangPu(const GSProto::CSMessage& msg)
{
	ManorFactory *pFact = getComponent<ManorFactory>(COMPNAME_ManorFactory, IID_IManorFactory);
	assert(pFact);
	IZoneTime *pZoneTime = getComponent<IZoneTime>(COMPNAME_ZoneTime, IID_IZoneTime);
	assert(pFact);
	ManorFactory * pManorFactory = getComponent<ManorFactory>(COMPNAME_ManorFactory, IID_IManorFactory);
	assert(pManorFactory);
	
	IEntity *pEntity = getEntityFromHandle(m_hEntity);
	assert(pEntity);
	
	ServerEngine::ManorWuHunAndItemData& dbData = m_manorSystemData.tieJiangPuData;
	HeroSoulOrItem teijianPu;
	bool res = pManorFactory->getActorTieJiangPuByLevel(teijianPu,dbData.level);
	assert(res);
	if(!dbData.bIsProduct)
	{
		return;
	}
	
	int iCurProductSecond = pZoneTime->GetCurSecond() - dbData.beginProductSecond;
	if(iCurProductSecond < teijianPu.iProductSecond)
	{
		return;
	}
	//收获
	ITable* pTable = getCompomentObjectManager()->findTable(TABLENAME_Hero);
	assert(pTable);
	IItemSystem *pItemSystem = static_cast<IItemSystem*>(pEntity->querySubsystem(IID_IItemSystem));
	assert(pItemSystem);
	for(size_t i = 0; i < dbData.wuHunAndItemList.size(); ++i)
	{
		const ServerEngine::ManorWuHunAndItemUnit &unit = dbData.wuHunAndItemList[i];
		int iRemaind = unit.iCount - unit.iBeLootCount;
		assert(iRemaind >= 0);
		if(iRemaind >0)
		{
			pItemSystem->addItem(unit.id, iRemaind,GSProto::en_Reason_ManorRes_Harvest);
		}
	}
	
	dbData.bIsProduct = false;
	//dbData.wuHunAndItemList.clear();
	/*if(dbData.bResIsBuild)
	{
		int iLevelUpSecond =  teijianPu.iLevelUpSecond - (pZoneTime->GetCurSecond() - dbData.beginLevelUpSecond);
		if(iLevelUpSecond <= 0)
		{
			dbData.bResIsBuild = false;
			dbData.level += 1;
		}
	}*/
	/*int count = pManorFactory->getActorWuHunDoxsCount(m_hEntity);
	vector<ManorItem> itemList;
	pManorFactory->getTieJiangPuByLevel(itemList, count, dbData.level);
	m_manorSystemData.tieJiangPuData.wuHunAndItemList.clear();
	for(size_t j= 0 ; j< itemList.size(); ++j)
	{
		ServerEngine::ManorWuHunAndItemUnit unit;
		ManorItem& item = itemList[j];
		unit.id = item.iId;
		unit.iCount = item.iCount;
		unit.iBeLootCount = 0;
		unit.bWuXieKeji = false;
		m_manorSystemData.tieJiangPuData.wuHunAndItemList.push_back(unit);
	}*/
	dbData.wuHunAndItemList.clear();
	dbData.befroreProductCache.clear();
	//更新客户端
	sendTieJiangPu2Client();
}

void ManorSystem::onQueryManorInfo(const GSProto::CSMessage& msg)
{
	pushManorInfo2Client();
}

void ManorSystem::getSilverSpeedAndTotolOutPut(int& speed, int& totalOutPut)
{
	ManorFactory * pManorFactory = getComponent<ManorFactory>(COMPNAME_ManorFactory, IID_IManorFactory);
	assert(pManorFactory);
	
	
	for(size_t i = 0; i < m_manorSystemData.silverResVec.size(); ++i)
	{
		ServerEngine::ManorResData dbData = m_manorSystemData.silverResVec[i];
		ManorResUnit unit;
		bool res = pManorFactory->getActorSilverResByIdAndLevel(unit, dbData.ResId, dbData.iResLevel);
		assert(res);
		speed += unit.iProductRate;
		totalOutPut += unit.iResTotalProduct;
	}
	double TimeProductRateBase = getProductRateBase();
	speed = TimeProductRateBase*speed;
}

double ManorSystem::getProductRateBase()
{
	//IZoneTime *pZoneTime = getComponent<IZoneTime>(COMPNAME_ZoneTime, IID_IZoneTime);
	//assert(pZoneTime);
//	IGlobalCfg *pGlobal = getComponent<IGlobalCfg>(COMPNAME_GlobalCfg, IID_IGlobalCfg);
//	assert(pGlobal);
	
	//int halfBeginSecond = pGlobal->getInt("领地矿折半生产开始时间",  0);
	//int halfEndSecond = pGlobal->getInt("领地矿折半生产结束时间",28800);
//	int iCurSecond = pZoneTime->GetCurSecond();
	//int dayBeginSecond = pZoneTime->GetDayBeginSecond(iCurSecond);
	//int iHalfBeginSecond = dayBeginSecond + halfBeginSecond;
//	int iHalfEndSecond = dayBeginSecond + halfEndSecond;
	//if( (iCurSecond >= iHalfBeginSecond) && (iCurSecond <= iHalfEndSecond))
	//{
	//	return 0.5;
	//}
	//else
	//{
		return 1.0;
	//}
}

void ManorSystem::getHeroExpSpeedAndTotolOutPut(int& speed, int& totalOutPut)
{
	IZoneTime *pZoneTime = getComponent<IZoneTime>(COMPNAME_ZoneTime, IID_IZoneTime);
	assert(pZoneTime);
	IGlobalCfg *pGlobal = getComponent<IGlobalCfg>(COMPNAME_GlobalCfg, IID_IGlobalCfg);
	assert(pGlobal);
	ManorFactory * pManorFactory = getComponent<ManorFactory>(COMPNAME_ManorFactory, IID_IManorFactory);
	assert(pManorFactory);
	
	for(size_t i = 0; i < m_manorSystemData.heroExpResVec.size(); ++i)
	{
		ServerEngine::ManorResData dbData = m_manorSystemData.heroExpResVec[i];
		ManorResUnit unit;
		bool res = pManorFactory->getActorHeroExpResByIdAndLevel(unit, dbData.ResId, dbData.iResLevel);
		assert(res);
		speed += unit.iProductRate;
		totalOutPut += unit.iResTotalProduct;
	}
	
	double TimeProductRateBase = getProductRateBase();
	speed = TimeProductRateBase*speed;
}

void ManorSystem::fillResData(vector<ServerEngine::ManorResData>& resData, GSProto::ManorResType type, google::protobuf::RepeatedPtrField< ::GSProto::ManorResDetail >* manorRes)
{
	ManorFactory * pManorFactory = getComponent<ManorFactory>(COMPNAME_ManorFactory, IID_IManorFactory);
	assert(pManorFactory);
	IZoneTime *pZone = getComponent<IZoneTime>(COMPNAME_ZoneTime, IID_IZoneTime);
	assert(pZone);
	for(size_t i =0;i < resData.size(); ++i)
	{
		ServerEngine::ManorResData& dbData = resData[i];
		GSProto::ManorResDetail* resDetail = manorRes->Add();
		resDetail->set_iresid(dbData.ResId);
		resDetail->set_irestype(type);
		if(dbData.bResIsBuild)
		{
			//
			int iBuildTime = pZone->GetCurSecond() - dbData.beginLevelUpSecond;
			ManorResUnit unit;
			if(type == GSProto::en_ManorResType_Silver)
			{
				bool res = pManorFactory->getActorSilverResByIdAndLevel(unit, dbData.ResId, dbData.iResLevel);
				assert(res);
			}
			else
			{
				bool res = pManorFactory->getActorHeroExpResByIdAndLevel(unit, dbData.ResId, dbData.iResLevel);
				assert(res);
			}
			
			if(unit.iLevelUpSecond <= iBuildTime)
			{
				//检查是否建完
				dbData.bResIsBuild = false;
				resDetail->set_bresinbuild(dbData.bResIsBuild);
				if(dbData.iResLevel >= 1)
				{
					int iAfterLevelUpSecond = iBuildTime - unit.iLevelUpSecond;
					int iLevelUpEndSecond = pZone->GetCurSecond() - iAfterLevelUpSecond;
					
					if(type == GSProto::en_ManorResType_Silver)
					{
						m_manorSystemData.iProductSivlerBeforLevelUp+= calcProduct(dbData.beginProductSecond,unit, iLevelUpEndSecond);
						m_manorSystemData.iProductSivlerBeforLevelUp -= m_manorSystemData.iBeLootSilver;
						m_manorSystemData.iBeLootSilver = 0;
					}
					else
					{
						m_manorSystemData.iProductHeroExpBeforLevelUp +=  calcProduct(dbData.beginProductSecond,unit,iLevelUpEndSecond);
						m_manorSystemData.iProductHeroExpBeforLevelUp -=m_manorSystemData.iBeLootHeroExp;
						m_manorSystemData.iBeLootHeroExp = 0;
					}
					
			       	dbData.beginProductSecond = iLevelUpEndSecond;
				}
				dbData.iResLevel += 1;
				if(dbData.iResLevel == 1)
				{
					dbData.beginProductSecond = pZone->GetCurSecond();
				}			
			}
			else
			{
				resDetail->set_bresinbuild(dbData.bResIsBuild);
				//在建造中，下行剩余时间
				resDetail->set_iremainsecond(unit.iLevelUpSecond - iBuildTime);
			}
		}
		resDetail->set_ireslevel(dbData.iResLevel);
	}
}

int ManorSystem::calcSilverPutOut()
{
	ManorFactory * pManorFactory = getComponent<ManorFactory>(COMPNAME_ManorFactory, IID_IManorFactory);
	assert(pManorFactory);
	int productRes = 0;
	for(size_t i = 0; i < m_manorSystemData.silverResVec.size(); ++i)
	{
		const ServerEngine::ManorResData& resDBData = m_manorSystemData.silverResVec[i];
		if(resDBData.iResLevel == 0)  continue;
		int iBeginSecond = resDBData.beginProductSecond;
		ManorResUnit unit;
		bool res = pManorFactory->getActorSilverResByIdAndLevel(unit, resDBData.ResId, resDBData.iResLevel);
		assert(res);
		productRes += calcProduct(iBeginSecond,unit);
	}
	return productRes;
}

int ManorSystem::calcHeroExpPutOut()
{
	ManorFactory * pManorFactory = getComponent<ManorFactory>(COMPNAME_ManorFactory, IID_IManorFactory);
	assert(pManorFactory);
	int productRes = 0;
	for(size_t i = 0; i < m_manorSystemData.heroExpResVec.size(); ++i)
	{
		const ServerEngine::ManorResData& resDBData = m_manorSystemData.heroExpResVec[i];
		if(resDBData.iResLevel == 0) continue;
		int iBeginSecond = resDBData.beginProductSecond;
		ManorResUnit unit;
		bool res = pManorFactory->getActorHeroExpResByIdAndLevel(unit, resDBData.ResId, resDBData.iResLevel);
		assert(res);
		productRes += calcProduct(iBeginSecond,unit);
	}
	return productRes;
}

int ManorSystem::calcProduct(int iBeginSecond, ManorResUnit unit, int iEndProductSecond /*= 0*/)
{
	IZoneTime *pZoneTime = getComponent<IZoneTime>(COMPNAME_ZoneTime, IID_IZoneTime);
	assert(pZoneTime);
	IGlobalCfg *pGlobal = getComponent<IGlobalCfg>(COMPNAME_GlobalCfg, IID_IGlobalCfg);
	assert(pGlobal);
	ManorFactory * pManorFactory = getComponent<ManorFactory>(COMPNAME_ManorFactory, IID_IManorFactory);
	assert(pManorFactory);
	//int halfBeginSecond = pGlobal->getInt("领地矿折半生产开始时间",0);
//	int halfEndSecond = pGlobal->getInt("领地矿折半生产结束时间",28800);
	//一个周期是24小时
//	int iDaySecond = 24 * 3600;
	int iCurSecond = pZoneTime->GetCurSecond();
	if(iEndProductSecond != 0) iCurSecond = iEndProductSecond;
	
//	int TimeLineBegine = iBeginSecond;
	int nowProduct = 0;

	int iproductSecond = iCurSecond - iBeginSecond;
	if(iproductSecond<=0) return 0;
	nowProduct =  iproductSecond * (double)unit.iProductRate/3600.0;

	/*
	while((TimeLineBegine < iCurSecond) &&(nowProduct < unit.iResTotalProduct))
	{
		int dayBeginSecond = pZoneTime->GetDayBeginSecond(TimeLineBegine);
		int iHalfBeginSecond = dayBeginSecond + halfBeginSecond;
		int iHalfEndSecond = dayBeginSecond + halfEndSecond;

		if( iCurSecond < iHalfBeginSecond)
		{
			//当前查看的时间再特殊生产前
			int iproductSecond = iCurSecond - TimeLineBegine;
			assert(iproductSecond >=0);
			//FDLOG("ManorSystem")<<"Line == 914|"<<iproductSecond << " | " << TimeLineBegine <<endl;
			nowProduct +=  iproductSecond * (double)unit.iProductRate/3600.0;
			TimeLineBegine = iCurSecond;
			continue;
		}
		else if( (iHalfEndSecond >= iCurSecond) && (iHalfBeginSecond <= iCurSecond) )
		{
			if(TimeLineBegine < iHalfBeginSecond )
			{
				//在半量生产前
				int iproductSecond = iHalfBeginSecond - TimeLineBegine;
				assert(iproductSecond >=0);
				
				nowProduct +=  iproductSecond * (double)unit.iProductRate/3600.0;
				//在半量生产时间内
				int iproductSecond2 = iCurSecond - iHalfBeginSecond;
				assert(iproductSecond2 >=0);
				nowProduct +=  0.5*iproductSecond2 * (double)unit.iProductRate/3600.0;
				TimeLineBegine = iCurSecond;
				continue;
			}
			else if(TimeLineBegine >=  iHalfBeginSecond)
			{
				
				int iproductSecond = iCurSecond - TimeLineBegine;
				assert(iproductSecond >=0);
				nowProduct +=   0.5 * iproductSecond * (double)unit.iProductRate/3600.0;
				TimeLineBegine = iCurSecond;
				continue;
			}
		}
		else if(iCurSecond > iHalfEndSecond )
		{
			//在折半时间前
			if( TimeLineBegine < iHalfBeginSecond)
			{
				int beforHalfSecond = iHalfBeginSecond - TimeLineBegine;
				assert(beforHalfSecond >=0);
				nowProduct += beforHalfSecond * (double)unit.iProductRate/3600.0;
					
				int iMindSecond = iHalfEndSecond - iHalfBeginSecond;
				assert(iMindSecond >=0);
				nowProduct +=  0.5* iMindSecond * (double)unit.iProductRate/3600.0;
				
				if( iHalfBeginSecond + iDaySecond < iCurSecond)
				{
					int afterSecond = iDaySecond - halfEndSecond;
					nowProduct += afterSecond * (double)unit.iProductRate/3600.0;
					TimeLineBegine =  iHalfBeginSecond + iDaySecond;
				}
				else
				{	
					int afterSecond = iCurSecond - iHalfEndSecond;
					nowProduct += afterSecond * (double)unit.iProductRate/3600.0;
					TimeLineBegine =  iCurSecond;
				}
				continue;
			}

			if((TimeLineBegine >=iHalfBeginSecond) && (TimeLineBegine <= iHalfEndSecond) )
			{
				int iMindSecond = iHalfEndSecond - TimeLineBegine;
				assert(iMindSecond >=0);
				nowProduct +=  0.5* iMindSecond * (double)unit.iProductRate/3600.0;

				if( iHalfBeginSecond + iDaySecond < iCurSecond)
				{
					int afterSecond = iDaySecond - halfEndSecond;
					nowProduct += afterSecond * (double)unit.iProductRate/3600.0;
					TimeLineBegine =  iHalfBeginSecond + iDaySecond;
				}
				else
				{	
					int afterSecond = iCurSecond - iHalfEndSecond;
					nowProduct += afterSecond * (double)unit.iProductRate/3600.0;
					TimeLineBegine =  iCurSecond;
				}
				continue;
			}
			else
			{
	
				if( iHalfBeginSecond + iDaySecond < iCurSecond)
				{
					int afterSecond = iDaySecond - halfEndSecond ;
					assert(afterSecond>=0);
					nowProduct += afterSecond * (double)unit.iProductRate/3600.0;
					TimeLineBegine =  iHalfBeginSecond + iDaySecond;
					continue;
				}
				else
				{	
				
					int	afterSecond = iCurSecond - TimeLineBegine;
					assert(afterSecond>=0);
					nowProduct += afterSecond * (double)unit.iProductRate/3600.0;
					TimeLineBegine =  iCurSecond;
				}
				
				
			}
		}
	}*/
	
	assert(nowProduct >= 0);
	nowProduct = std::min(nowProduct,unit.iResTotalProduct);
	return nowProduct;
}

void ManorSystem::onResLevelUpUseGold(const GSProto::CSMessage& msg)
{
	GSProto::CMD_MANOR_RES_USE_GOLD_LEVELUP_CS csMsg;
	if(!csMsg.ParseFromString(msg.strmsgbody()))
	{
		return;
	}
	int resId = csMsg.iresid();
	GSProto::ManorResType type = csMsg.irestype();
	//在没有在升级
	ManorFactory * pManorFactory = getComponent<ManorFactory>(COMPNAME_ManorFactory, IID_IManorFactory);
	assert(pManorFactory);
	IZoneTime *pZoneTime = getComponent<IZoneTime>(COMPNAME_ZoneTime, IID_IZoneTime);
	assert(pZoneTime);
	IEntity *pEntity = getEntityFromHandle(m_hEntity);
	assert(pEntity);
	if(GSProto::en_ManorResType_Silver == type)
	{
		for(size_t i = 0; i < m_manorSystemData.silverResVec.size(); ++i)
		{
			ServerEngine::ManorResData dbData = m_manorSystemData.silverResVec[i];
			if(dbData.ResId == resId )
			{
				//是否已经修完
				if(dbData.bResIsBuild)
				{
					ManorResUnit unit;
					bool res = pManorFactory->getActorSilverResByIdAndLevel(unit, dbData.ResId, dbData.iResLevel);
					assert(res);
					int iLevelUpNeedSecond =unit.iLevelUpSecond-(pZoneTime->GetCurSecond() - dbData.beginLevelUpSecond);
					if(iLevelUpNeedSecond <= 0)
					{
						pEntity->sendErrorCode(ERROR_MANOR_SILVER_LEVELUPOVER);
						return;
					}
					int iNeedGold = iLevelUpNeedSecond/60;
					int modGold = iLevelUpNeedSecond%60;
					if(modGold > 0)
					{
						iNeedGold += 1;
					}
					IScriptEngine *pScript = getComponent<IScriptEngine>(COMPNAME_ScriptEngine, IID_IScriptEngine);
					assert(pScript);

					EventArgs args;
					args.context.setInt("entity", m_hEntity);
					args.context.setInt("cost", iNeedGold);
					args.context.setString("name",unit.strName.c_str());
					args.context.setInt("resId", dbData.ResId);

					pScript->runFunction("ManorSilverGoldAddSpeed",&args, "EventArgs");
					break;
				}
			}
		}
	}
	else if(GSProto::en_ManorResType_HEROEXP == type)
	{
		for(size_t i = 0; i < m_manorSystemData.heroExpResVec.size(); ++i)
		{
			ServerEngine::ManorResData dbData = m_manorSystemData.heroExpResVec[i];
			if(dbData.ResId == resId )
			{
				//是否已经修完
				if(dbData.bResIsBuild)
				{
					ManorResUnit unit;
					bool res = pManorFactory->getActorHeroExpResByIdAndLevel(unit,dbData.ResId,dbData.iResLevel);
					assert(res);
					int iLevelUpNeedSecond =unit.iLevelUpSecond-(pZoneTime->GetCurSecond() - dbData.beginLevelUpSecond);
					if(iLevelUpNeedSecond <= 0)
					{
						pEntity->sendErrorCode(ERROR_MANOR_HEROEXP_LEVELUPOVER);
						return;
					}
					int iNeedGold = iLevelUpNeedSecond/60;
					int modGold = iLevelUpNeedSecond%60;
					if(modGold > 0)
					{
						iNeedGold += 1;
					}
					IScriptEngine *pScript = getComponent<IScriptEngine>(COMPNAME_ScriptEngine, IID_IScriptEngine);
					assert(pScript);

					EventArgs args;
					args.context.setInt("entity", m_hEntity);
					args.context.setInt("cost", iNeedGold);
					args.context.setString("name",unit.strName.c_str());
					args.context.setInt("resId", dbData.ResId);
					
					pScript->runFunction("manorHeroExpGoldAddSpeed",&args, "EventArgs");
					//写回调
					break;
				}
			}
		}
	}
}

void ManorSystem::manorHeroExpResGoldAddSpeed(int iCost,int iResID)
{
	IEntity *pEntity = getEntityFromHandle(m_hEntity);
	assert(pEntity);
	
	int iActorGold  = pEntity->getProperty(PROP_ACTOR_GOLD,0);
	if(iActorGold < iCost )
	{
		pEntity->sendErrorCode(ERROR_NEED_GOLD);
		return;
	}
	//口扣钱
	pEntity->changeProperty(PROP_ACTOR_GOLD, 0-iCost, GSProto::en_Reason_ManorHeroExpResAddSpeed);
	
	ManorFactory * pManorFactory = getComponent<ManorFactory>(COMPNAME_ManorFactory, IID_IManorFactory);
	assert(pManorFactory);
	IZoneTime *pZoneTime = getComponent<IZoneTime>(COMPNAME_ZoneTime, IID_IZoneTime);
	assert(pZoneTime);
	
	//给修为矿升级
	for(size_t i = 0; i < m_manorSystemData.heroExpResVec.size(); ++i)
	{
		ServerEngine::ManorResData& dbData = m_manorSystemData.heroExpResVec[i];
		if(dbData.ResId == iResID )
		{
			if(dbData.bResIsBuild) dbData.bResIsBuild = false;
			ManorResUnit unit;
			bool res = pManorFactory->getActorHeroExpResByIdAndLevel(unit,dbData.ResId,dbData.iResLevel);
			assert(res);
			
			if(dbData.iResLevel >= 1)
			{
				m_manorSystemData.iProductHeroExpBeforLevelUp += calcProduct(dbData.beginProductSecond,unit);
				m_manorSystemData.iProductHeroExpBeforLevelUp -= m_manorSystemData.iBeLootHeroExp;
				m_manorSystemData.iBeLootHeroExp = 0;
			}
			
			dbData.iResLevel += 1;
			if(dbData.iResLevel != 1)
			{
				dbData.beginProductSecond = pZoneTime->GetCurSecond();
			}
			break;
		}
	}
	//更新客户端
	pushManorInfo2Client();
	
}

void ManorSystem::manorSilverResGoldAddSpeed(int  iCost,int iResID)
{
	IEntity *pEntity = getEntityFromHandle(m_hEntity);
	assert(pEntity);
	
	ManorFactory * pManorFactory = getComponent<ManorFactory>(COMPNAME_ManorFactory, IID_IManorFactory);
	assert(pManorFactory);
	IZoneTime *pZoneTime = getComponent<IZoneTime>(COMPNAME_ZoneTime, IID_IZoneTime);
	assert(pZoneTime);
	int iActorGold  = pEntity->getProperty(PROP_ACTOR_GOLD,0);
	if(iActorGold < iCost )
	{
		pEntity->sendErrorCode(ERROR_NEED_GOLD);
		return;
	}
	//口扣钱
	pEntity->changeProperty(PROP_ACTOR_GOLD, 0-iCost, GSProto::en_Reason_ManorSilverResAddSpeed);
	//升级
	for(size_t i = 0; i < m_manorSystemData.silverResVec.size(); ++i)
	{
		ServerEngine::ManorResData& dbData = m_manorSystemData.silverResVec[i];
		if(dbData.ResId == iResID )
		{
			if(dbData.bResIsBuild) dbData.bResIsBuild = false;
			ManorResUnit unit;
			bool res = pManorFactory->getActorSilverResByIdAndLevel(unit,dbData.ResId,dbData.iResLevel);
			assert(res);
			if(dbData.iResLevel >= 1)
			{
				m_manorSystemData.iProductSivlerBeforLevelUp += calcProduct(dbData.beginProductSecond,unit);
				m_manorSystemData.iProductSivlerBeforLevelUp -= m_manorSystemData.iBeLootSilver;
				m_manorSystemData.iBeLootSilver = 0;
			}
			
			dbData.iResLevel += 1;
			if(dbData.iResLevel != 1)
			{
				dbData.beginProductSecond = pZoneTime->GetCurSecond();
			}
			break;
		}
	}
	//更新客户端
	pushManorInfo2Client();
	
}

void ManorSystem::onResLevelUpCommon(const GSProto::CSMessage& msg)
{
	//有没有开放这个领地矿
	GSProto::CMD_MANOR_RES_COMMON_LEVELUP_CS csMsg;
	if( !csMsg.ParseFromString(msg.strmsgbody()))
	{
		return;
	}
	
	int iResId = csMsg.iresid();
	GSProto::ManorResType type = csMsg.irestype();
	//玩家开启这个矿没有 ?
	//在没有在升级
	ManorFactory * pManorFactory = getComponent<ManorFactory>(COMPNAME_ManorFactory, IID_IManorFactory);
	assert(pManorFactory);
	IZoneTime *pZoneTime = getComponent<IZoneTime>(COMPNAME_ZoneTime, IID_IZoneTime);
	assert(pZoneTime);

	IGlobalCfg *pGlobal = getComponent<IGlobalCfg>(COMPNAME_GlobalCfg, IID_IGlobalCfg);
	assert(pGlobal);

	IEntity *pEntity = getEntityFromHandle(m_hEntity);
	assert(pEntity);
	
	if(GSProto::en_ManorResType_Silver == type)
	{
		//正在升级的个数
		//cout<<"GSProto::en_ManorResType_Silver == type"<<endl;
		for(size_t i = 0; i < m_manorSystemData.silverResVec.size(); ++i)
		{
			ServerEngine::ManorResData& dbData = m_manorSystemData.silverResVec[i];
			if(iResId == dbData.ResId)
			{
				ManorResUnit unit;
				bool res = pManorFactory->getActorSilverResByIdAndLevel(unit,dbData.ResId,dbData.iResLevel);
				assert(res);

				if(dbData.iResLevel > 0)
				{
					//在升级不能继续升级
					if(dbData.bResIsBuild)
					{
						int iLevelUpNeedSecond = unit.iLevelUpSecond - (pZoneTime->GetCurSecond()- dbData.beginLevelUpSecond);
						if(iLevelUpNeedSecond > 0)
						{
							return;
						}
					}
				}

				//不能高于玩家等级3倍
				int iBase = pGlobal->getInt("领地矿等级级不能超过玩家等级倍",3);
				int iNeedActorLevel = (dbData.iResLevel + 1)*iBase;
				int iLevelUp = pEntity->getProperty(PROP_ENTITY_LEVEL,0);
				if( iNeedActorLevel >  iLevelUp)
				{
					GSProto::CMD_MANOR_RES_COMMON_LEVELUP_SC scMsg;
					scMsg.set_ineedactorlevel(iNeedActorLevel);
					pEntity->sendMessage(GSProto::CMD_MANOR_RES_COMMON_LEVELUP,  scMsg);
					return;
				}

	
				int iLevelNeedSilver = unit.iLevelUpCost;
				//最高级不能升级
				if(-1 == iLevelNeedSilver)
				{
					pEntity->sendErrorCode(ERROR_MANOR_RES_LEVELLIMIT);
					return;
				}
				int iActorHave = pEntity->getProperty( PROP_ACTOR_SILVER, 0);
				if(iActorHave <iLevelNeedSilver )
				{
					pEntity->sendErrorCode(ERROR_NEED_SILVER);
					return;
				}
				
				bool buildTeamEmpty = manorBuildTeamsCanBulid();
				if(!buildTeamEmpty)
				{
					pEntity->sendErrorCode(ERROR_MANOR_NOT_MORE_BUILDTEAMS);
					return;
				}
				
				//可升级 ，扣钱
				pEntity->changeProperty(PROP_ACTOR_SILVER, 0-iLevelNeedSilver,GSProto::en_Reason_ManorRes_LevelUp);
				//开始升级
				dbData.beginLevelUpSecond = pZoneTime->GetCurSecond();
				dbData.bResIsBuild = true;
				break;
			}
		}
	}
	else if(type == GSProto::en_ManorResType_HEROEXP)
	{
		//cout<<"GSProto::en_ManorResType_HEROEXP == type"<<endl;
		for(size_t i = 0; i < m_manorSystemData.heroExpResVec.size(); ++i)
		{
			ServerEngine::ManorResData& dbData = m_manorSystemData.heroExpResVec[i];
			if(iResId == dbData.ResId)
			{
				ManorResUnit unit;
				bool res = pManorFactory->getActorHeroExpResByIdAndLevel(unit,dbData.ResId,dbData.iResLevel);
				assert(res);
				
				//不能高于玩家等级3倍
				//在升级不能继续升级
				
				if(dbData.iResLevel > 0)
				{
					if(dbData.bResIsBuild)
					{
						int iLevelUpNeedSecond =unit.iLevelUpSecond-(pZoneTime->GetCurSecond() - dbData.beginLevelUpSecond);
						if(iLevelUpNeedSecond > 0)
						{
							pEntity->sendErrorCode( ERROR_MANOR_ISLEVELUP);
							return;
						}
					}
				}
						//不能高于玩家等级3倍
				int iBase = pGlobal->getInt("领地矿等级级不能超过玩家等级倍",3);
				int iNeedActorLevel = (dbData.iResLevel + 1)*iBase;
				int iLevelUp = pEntity->getProperty(PROP_ENTITY_LEVEL,0);
				if( iNeedActorLevel >  iLevelUp)
				{
					GSProto::CMD_MANOR_RES_COMMON_LEVELUP_SC scMsg;
					scMsg.set_ineedactorlevel(iNeedActorLevel);
					pEntity->sendMessage(GSProto::CMD_MANOR_RES_COMMON_LEVELUP,  scMsg);
					return;
				}
				
				int iLevelNeedSilver = unit.iLevelUpCost;
				//最高级不能升级
				if(-1 == iLevelNeedSilver)
				{
					pEntity->sendErrorCode(ERROR_MANOR_RES_LEVELLIMIT);
					return;
				}
				
				int iActorHave = pEntity->getProperty( PROP_ACTOR_SILVER, 0);
				if(iActorHave <iLevelNeedSilver )
				{
					pEntity->sendErrorCode(ERROR_NEED_SILVER);
					return;
				}
				
				
				bool buildTeamEmpty = manorBuildTeamsCanBulid();
				if(!buildTeamEmpty)
				{
					pEntity->sendErrorCode(ERROR_MANOR_NOT_MORE_BUILDTEAMS);
					return;
				}
				
				//可升级 ，扣钱
				pEntity->changeProperty(PROP_ACTOR_SILVER, 0-iLevelNeedSilver,GSProto::en_Reason_ManorRes_LevelUp);
				//开始升级
				dbData.beginLevelUpSecond = pZoneTime->GetCurSecond();
				dbData.bResIsBuild = true;
				break;
			}	
		}
	}
	pushManorInfo2Client();
}

void ManorSystem::onHarvestRes(const GSProto::CSMessage& msg)
{
	//收取次数是否已经用完
	IGlobalCfg *pGlobal = getComponent<IGlobalCfg>(COMPNAME_GlobalCfg, IID_IGlobalCfg);
	assert(pGlobal);
	IEntity *pEntity = getEntityFromHandle(m_hEntity);
	assert(pEntity);

	//IVIPFactory *pVipFactory = getComponent<>(const  string & strComponentName, MINIAPR :: IID rID)
	 // pGlobal->getInt("每日收取矿产次数",3);
	 
	IVIPFactory* pVipFactory = getComponent<IVIPFactory>(COMPNAME_VIPFactory,IID_IVIPFactory);
	assert(pVipFactory);
	int iHavestLimit = pVipFactory->getVipPropByHEntity(m_hEntity, VIP_PROP_MANOR_HARVEST_TIMES);
	
	int iActorHaveHavest = m_manorSystemData.iResHarvestTimes;
	if(iActorHaveHavest >= iHavestLimit)
	{
		pEntity->sendErrorCode(ERROR_MANOR_RES_HAVESTLIMIT);
		return;
	}

	EventArgs args ;
	args.context.setInt("entity", m_hEntity);
	args.context.setInt("remaind",iHavestLimit -iActorHaveHavest );

	IScriptEngine *pScrip = getComponent<IScriptEngine>(COMPNAME_ScriptEngine, IID_IScriptEngine);
	assert(pScrip);

	pScrip->runFunction("manorResHarvest",&args, "EventArgs");
	
	
}

void ManorSystem::manorResHarvestReturn()
{
	IGlobalCfg *pGlobal = getComponent<IGlobalCfg>(COMPNAME_GlobalCfg, IID_IGlobalCfg);
	assert(pGlobal);
	IEntity *pEntity = getEntityFromHandle(m_hEntity);
	assert(pEntity);
	IZoneTime *pZoneTime = getComponent<IZoneTime>( COMPNAME_ZoneTime, IID_IZoneTime);
	assert(pZoneTime);

	IVIPFactory* pVipFactory = getComponent<IVIPFactory>(COMPNAME_VIPFactory,IID_IVIPFactory);
	assert(pVipFactory);
	int iHavestLimit = pVipFactory->getVipPropByHEntity(m_hEntity, VIP_PROP_MANOR_HARVEST_TIMES);
	//int iHavestLimit = pGlobal->getInt("每日收取矿产次数",3);
	int iActorHaveHavest = m_manorSystemData.iResHarvestTimes;
	if(iActorHaveHavest >= iHavestLimit)
	{
		return;
	}
	
	//计算收益
	int iTotalSilver = 0;
	int iTotalHeroExp = 0;
	getTotalProduct(iTotalHeroExp,iTotalSilver);
	int iGetSilver = calcSilverPutOut() + m_manorSystemData.iProductSivlerBeforLevelUp;
	iGetSilver = std:: min(iTotalSilver,iGetSilver );
	iGetSilver = iGetSilver - m_manorSystemData.iBeLootSilver;
	
	int iGetHeroExp = calcHeroExpPutOut() + m_manorSystemData.iProductHeroExpBeforLevelUp;
	iGetHeroExp = std:: min(iGetHeroExp,iTotalHeroExp );
	iGetHeroExp = iGetHeroExp -m_manorSystemData.iBeLootHeroExp;
	//扣领取次数
	m_manorSystemData.iResHarvestTimes += 1;
	//m_manorSystemData.dwHarvestSecond = pZoneTime->GetCurSecond();
	
	for(size_t i = 0; i < m_manorSystemData.silverResVec.size(); ++i )
	{
		m_manorSystemData.silverResVec[i].beginProductSecond = pZoneTime->GetCurSecond();
	}

	for(size_t i = 0; i <m_manorSystemData.heroExpResVec.size(); ++i )
	{
		m_manorSystemData.heroExpResVec[i].beginProductSecond = pZoneTime->GetCurSecond();
	}
	
	/*if(!m_manorSystemData.bGuideOver)
	{
		
		iGetSilver = pGlobal->getInt("引导时铜币产出", 5000);
		iGetHeroExp = pGlobal->getInt("引导时修为产出", 5000);
		m_manorSystemData.bGuideOver = true;
	}
	*/
	
	//发放收益
	if(iGetSilver < 0) iGetSilver = 0;
	if(iGetHeroExp < 0) iGetHeroExp = 0;
	pEntity->changeProperty(PROP_ACTOR_SILVER, iGetSilver,GSProto::en_Reason_ManorRes_Harvest);
	pEntity->changeProperty(PROP_ENTITY_HEROEXP, iGetHeroExp,GSProto::en_Reason_ManorRes_Harvest);

	FDLOG("ManorSystem")<<"HarvestRes = | " << iGetSilver << " | "  << iGetHeroExp<<endl;

	//清理收益
	m_manorSystemData.iProductSivlerBeforLevelUp = 0;
	m_manorSystemData.iProductHeroExpBeforLevelUp = 0;
	m_manorSystemData.iBeLootSilver = 0;
	m_manorSystemData.iBeLootHeroExp = 0;
	//更新客户端
	pushManorInfo2Client(); 

	//领地收获次数
	 {
		EventArgs args;
		args.context.setInt("times",1);
		args.context.setInt("entity",m_hEntity);
		pEntity->getEventServer()->setEvent(EVENT_ENTITY_TASK_MANOR_HARVERST, args);
	}
}

void ManorSystem::getHeroExpTotalPutOut(int& heroExp)
{
	ManorFactory * pManorFactory = getComponent<ManorFactory>(COMPNAME_ManorFactory, IID_IManorFactory);
	assert(pManorFactory);

	for(size_t i = 0; i < m_manorSystemData.heroExpResVec.size(); ++i)
	{
		const ServerEngine::ManorResData& dbData = m_manorSystemData.heroExpResVec[i];
		ManorResUnit unit;
		bool res = pManorFactory->getActorHeroExpResByIdAndLevel(unit, dbData.ResId, dbData.iResLevel);
		assert(res);
		heroExp += unit.iResTotalProduct;
	}
}

void ManorSystem::getSilverTotalPutOut(int& silver)
{
	ManorFactory * pManorFactory = getComponent<ManorFactory>(COMPNAME_ManorFactory, IID_IManorFactory);
	assert(pManorFactory);
	
	for(size_t i = 0; i < m_manorSystemData.silverResVec.size(); ++i)
	{
		const ServerEngine::ManorResData& dbData = m_manorSystemData.silverResVec[i];
		ManorResUnit unit;
		bool res = pManorFactory->getActorSilverResByIdAndLevel(unit, dbData.ResId, dbData.iResLevel);
		assert(res);
		silver += unit.iResTotalProduct;
	}
}

void ManorSystem::getTotalProduct(int& heroExp,  int& silver)
{
	getHeroExpTotalPutOut(heroExp);
	getSilverTotalPutOut(silver);
}

void ManorSystem::sendWuHunDian2Client()
{
	CheckWuHunDianAndTieJiangPuBuild();
	GSProto::CMD_MANOR_WUHUNDIAN_QUERY_SC scMsg;
	IEntity *pEntity = getEntityFromHandle(m_hEntity);
	assert(pEntity);
	IZoneTime *pZone = getComponent<IZoneTime>(COMPNAME_ZoneTime, IID_IZoneTime);
	assert(pZone);
	ManorFactory * pManorFactory = getComponent<ManorFactory>(COMPNAME_ManorFactory, IID_IManorFactory);
	assert(pManorFactory);
	IGlobalCfg * pGlobal = getComponent<IGlobalCfg>(COMPNAME_GlobalCfg, IID_IGlobalCfg);
	assert(pGlobal);
	ServerEngine::ManorWuHunAndItemData& dbData = m_manorSystemData.wuHunDianData;
	HeroSoulOrItem wuHunDian;
	bool res = pManorFactory->getActorWuHunDianByLevel(wuHunDian,dbData.level);
	assert(res);
	
	scMsg.set_bisbulid(dbData.bResIsBuild);
	if(dbData.bResIsBuild)
	{
		int iLevelUpSecond =  wuHunDian.iLevelUpSecond - (pZone->GetCurSecond() - dbData.beginLevelUpSecond);
		assert(iLevelUpSecond > 0);
		scMsg.set_ibulidremainsecond(iLevelUpSecond);
	}
	scMsg.set_ilevelid(dbData.level);
	scMsg.set_bisproduct( dbData.bIsProduct);
	if(dbData.bIsProduct)
	{
		int iProductSecond =  wuHunDian.iProductSecond - ( pZone->GetCurSecond() - dbData.beginProductSecond);
		if(iProductSecond > 0)
		{
			scMsg.set_iproductreaminsecond(iProductSecond);
		}
	}
	else
	{
		int iFreeRefreshTimes = pGlobal->getInt("武魂殿免费刷新次数", 3);
		int iActorRefreshTimes = dbData.iHaveRefreshTimes;
		if(iFreeRefreshTimes <= iActorRefreshTimes )
		{
			scMsg.set_iremianrefreshtimes(0);
			//int iRefreshCost = pGlobal->getInt("武魂殿刷新花费铜币",1000);
			int iRefreshCost = pManorFactory->getRefreshTieJiangPuOrWuHunDianCost(iActorRefreshTimes+1);
			scMsg.set_irefreshcost(iRefreshCost);
		}
		else
		{
			scMsg.set_iremianrefreshtimes(iFreeRefreshTimes - iActorRefreshTimes);
		}
		scMsg.set_itotalremainrefreshtimes( iFreeRefreshTimes);
			
	}

	//分生产中和非生产来处理
	if(dbData.bIsProduct)
	{
		size_t iSize = dbData.wuHunAndItemList.size();
		if((size_t)wuHunDian.iOpenGeZiCount > iSize)
		{
			vector<ManorItem> itemList;
			pManorFactory->getWuHunByLevel(itemList,  wuHunDian.iOpenGeZiCount - iSize, dbData.level);
			for(size_t j = 0 ; j <  itemList.size(); ++j)
			{
				ServerEngine::ManorWuHunAndItemUnit unit;
				ManorItem& item = itemList[j];
				unit.id = item.iId;
				unit.iCount = item.iCount;
				unit.iBeLootCount = 0;
				unit.bWuXieKeji = false;
				dbData.wuHunAndItemList.push_back(unit);
			}
		}
		 iSize = dbData.wuHunAndItemList.size();
		for(size_t i = 0; i < iSize; ++i)
		{
			fillWuHunDianGeZiDetail( *(scMsg.add_szmanorherosoulbox()), dbData.wuHunAndItemList[i], i);
		}

		
	}
	else
	{
		//cache在开始生产的时候清理，等级提升的时候清理澹?	
		
		size_t iSize = dbData.befroreProductCache.size();
		if( (iSize  == 0) || ((size_t)wuHunDian.iOpenGeZiCount > iSize))
		{
			dbData.befroreProductCache.clear();
			vector<ManorItem> itemList;
			pManorFactory->getWuHunByLevel(itemList,  wuHunDian.iOpenGeZiCount, dbData.level);
			for(size_t j = 0 ; j <  itemList.size(); ++j)
			{
				ServerEngine::ManorWuHunAndItemUnit unit;
				ManorItem& item = itemList[j];
				unit.id = item.iId;
				unit.iCount = item.iCount;
				unit.iBeLootCount = 0;
				unit.bWuXieKeji = false;
				dbData.befroreProductCache.push_back(unit);
			}
		}
	
	 	iSize = dbData.befroreProductCache.size();
		for(size_t i = 0; i < iSize; ++i)
		{
			fillWuHunDianGeZiDetail( *(scMsg.add_szmanorherosoulbox()),  dbData.befroreProductCache[i], i);
		}
	}
	pEntity->sendMessage(GSProto::CMD_MANOR_WUHUNDIAN_QUERY, scMsg );
}

void ManorSystem::CheckWuHunDianAndTieJiangPuBuild()
{
	IZoneTime *pZone = getComponent<IZoneTime>(COMPNAME_ZoneTime, IID_IZoneTime);
	assert(pZone);
	ManorFactory* pManorFactory = getComponent<ManorFactory>(COMPNAME_ManorFactory, IID_IManorFactory);
	assert(pManorFactory);
	IGlobalCfg* pGlobal = getComponent<IGlobalCfg>(COMPNAME_GlobalCfg, IID_IGlobalCfg);
	assert(pGlobal);
	ServerEngine::ManorWuHunAndItemData& dbWuHunDianData = m_manorSystemData.wuHunDianData;
	ServerEngine::ManorWuHunAndItemData& dbTieJiangPuData = m_manorSystemData.tieJiangPuData;
	
	HeroSoulOrItem wuHunDian;
	bool res = pManorFactory->getActorWuHunDianByLevel(wuHunDian, dbWuHunDianData.level);
	assert(res);
	
	HeroSoulOrItem tieJiangPu;
	bool res2 = pManorFactory->getActorTieJiangPuByLevel(tieJiangPu, dbTieJiangPuData.level);
	assert(res2);
	
	if( dbWuHunDianData.bResIsBuild)
	{
		int iLevelUpSecond =  wuHunDian.iLevelUpSecond - (pZone->GetCurSecond() - dbWuHunDianData.beginLevelUpSecond);
		if( iLevelUpSecond <= 0)
		{
			if(wuHunDian.iLevelUpCost != -1)
			{
				dbWuHunDianData.level++ ;
				dbWuHunDianData.bResIsBuild = false ;
				dbWuHunDianData.befroreProductCache.clear();
				{
					EventArgs args ;
					args.context.setInt("nowlevel",dbWuHunDianData.level);
					IEntity *pEntity = getEntityFromHandle(m_hEntity);
					pEntity->getEventServer()->setEvent(EVENT_ENTITY_TASK_HEROSOUL_LEVEL , args);
				}
			}
			
		}
	}

	if(dbTieJiangPuData.bResIsBuild)
	{
		int iLevelUpSecond =  tieJiangPu.iLevelUpSecond - (pZone->GetCurSecond() - dbTieJiangPuData.beginLevelUpSecond);
		if( iLevelUpSecond <= 0)
		{
			if(tieJiangPu.iLevelUpCost != -1)
			{
				dbTieJiangPuData.level++ ;
				dbTieJiangPuData.bResIsBuild = false ;
				dbTieJiangPuData.befroreProductCache.clear();
				{
					EventArgs args ;
					args.context.setInt("nowlevel",dbTieJiangPuData.level);
					IEntity *pEntity = getEntityFromHandle(m_hEntity);
					pEntity->getEventServer()->setEvent(EVENT_ENTITY_TASK_TIEJIANGPU_LEVEL , args);
				}
			}
		}
	
	}

}

void ManorSystem::fillWuHunDianGeZiDetail(GSProto::ManorHeroSoulBox& box, const ServerEngine::ManorWuHunAndItemUnit& unit, int index)
{
	box.set_bisopen(true);
	GSProto::MonerHeroSoulDetail* detail = box.mutable_detail();
	detail->set_iherosoulid( unit.id);
	detail->set_bheroiswuxiekeji( unit.bWuXieKeji);
	detail->set_iherosoulcount(unit.iCount );
	detail->set_iherohavesoullootcount(unit.iBeLootCount);
	box.set_iboxindex(index);
}

void ManorSystem::onQueryWuHunDian(const GSProto::CSMessage& msg)
{
	sendWuHunDian2Client();
}

void ManorSystem::onWuHunDianLevelUp(const GSProto::CSMessage& msg)
{
	//最高级不能升级
	IEntity *pEntity = getEntityFromHandle(m_hEntity);
	assert(pEntity);
	
	IZoneTime *pZone = getComponent<IZoneTime>(COMPNAME_ZoneTime, IID_IZoneTime);
	assert(pZone);
	ManorFactory * pManorFactory = getComponent<ManorFactory>(COMPNAME_ManorFactory, IID_IManorFactory);
	assert(pManorFactory);
	ServerEngine::ManorWuHunAndItemData& dbData = m_manorSystemData.wuHunDianData;
	int iNeedActorLevel = (dbData.level + 1)*10;
	int iLevelUp = pEntity->getProperty(PROP_ENTITY_LEVEL,0);
	if( iNeedActorLevel >  iLevelUp)
	{
		GSProto::CMD_MANOR_WUHUNDIAN_LEVELUP_SC scMsg;
		scMsg.set_ineedactorlevel(iNeedActorLevel);
		pEntity->sendMessage(GSProto::CMD_MANOR_WUHUNDIAN_LEVELUP,  scMsg);
		return;
	}

	HeroSoulOrItem wuHunDian;
	bool res = pManorFactory->getActorWuHunDianByLevel(wuHunDian,dbData.level);
	assert(res);
	if(wuHunDian.iLevelUpCost == -1)
	{
		pEntity->sendErrorCode(ERROR_MANOR_WUHUNDIAN_LEVELIMMIT);
		return;
	}
	
	//正在升级不能升级
	if(dbData.bResIsBuild)
	{
		int iLevelUpSecond =  wuHunDian.iLevelUpSecond - (pZone->GetCurSecond() - dbData.beginLevelUpSecond);
		if(iLevelUpSecond <= 0)
		{
			dbData.bResIsBuild = false;
		}
		else
		{
			return;
		}
	}
	//判断钱足不足
	int iNeedSilver = wuHunDian.iLevelUpCost;
	int iActorHave = pEntity->getProperty(PROP_ACTOR_SILVER,0);
	if(iNeedSilver > iActorHave)
	{
		pEntity->sendErrorCode(ERROR_NEED_SILVER);
		return;
	}
	bool buildTeamEmpty = manorBuildTeamsCanBulid();
	if(!buildTeamEmpty)
	{
		pEntity->sendErrorCode(ERROR_MANOR_NOT_MORE_BUILDTEAMS);
		return;
	}
				
	//扣钱
	pEntity->changeProperty( PROP_ACTOR_SILVER, 0-iNeedSilver,GSProto::en_Reason_Manor_WuHunDian_LEVELUP_COST);
	//开始升级
	dbData.bResIsBuild = true;
	dbData.beginLevelUpSecond = pZone->GetCurSecond();
	sendWuHunDian2Client();
}

void ManorSystem::onWuHunDianBeginProduct(const GSProto::CSMessage& msg)
{
	//是否没有生产
	if(m_manorSystemData.wuHunDianData.bIsProduct)
	{
		return;
	}
	IZoneTime *pZoneTime = getComponent<IZoneTime>(COMPNAME_ZoneTime, IID_IZoneTime);
	assert(pZoneTime);
	m_manorSystemData.wuHunDianData.bIsProduct = true;
	m_manorSystemData.wuHunDianData.beginProductSecond = pZoneTime->GetCurSecond();
	m_manorSystemData.wuHunDianData.wuHunAndItemList.clear();
	m_manorSystemData.wuHunDianData.wuHunAndItemList = m_manorSystemData.wuHunDianData.befroreProductCache;
	m_manorSystemData.wuHunDianData.befroreProductCache.clear();
	//更新客户端
	sendWuHunDian2Client();

	//领地武魂殿祭奠次数
	 {
		EventArgs args;
		args.context.setInt("times",1);
		args.context.setInt("entity",m_hEntity);
		IEntity *pEntity = getEntityFromHandle(m_hEntity);
		assert(pEntity);
		pEntity->getEventServer()->setEvent(EVENT_ENTITY_TASK_MANOR_WuHunJiLian, args);
	}
}

void ManorSystem::onWuHunDianSetHeroWuxiekeji(const GSProto::CSMessage& msg)
{
	GSProto::CMD_MANOR_WUHUNDIAN_WUXIEKEJIE_CS csMsg;
	if(! csMsg.ParseFromString(msg.strmsgbody()))
	{
		return;
	}
	int index = csMsg.iboxindex();
	ServerEngine::ManorWuHunAndItemData& dbData = m_manorSystemData.wuHunDianData;
	if(!dbData.bIsProduct)
	{
		return;
	}
	if( dbData.wuHunAndItemList.size() <= (size_t)index )
	{
		return;
	}
	ServerEngine::ManorWuHunAndItemUnit& unit = dbData.wuHunAndItemList[index];
	if(unit.bWuXieKeji )
	{
		return;
	}
	//
	IEntity *pEntity = getEntityFromHandle(m_hEntity);
	assert(pEntity);
	IGlobalCfg *pGlobal = getComponent<IGlobalCfg>(COMPNAME_GlobalCfg, IID_IGlobalCfg);
	assert(pGlobal);
	int iNeedGold = pGlobal->getInt("给武魂设置无懈可击花费金币",20);
	int iActorHave = pEntity->getProperty(PROP_ACTOR_GOLD,0);
	if(iActorHave < iNeedGold)
	{
		pEntity->sendErrorCode(ERROR_NEED_GOLD);
		return;
	}
	//扣钱?
	pEntity->changeProperty( PROP_ACTOR_GOLD, 0-iNeedGold, GSProto::en_Reason_Manor_WuHunDian_WUXIEKEJI_Cost);
	
	//设置无懈可击
	unit.bWuXieKeji = true;
	//更新客户端	
	sendWuHunDian2Client();
}

void ManorSystem::onWuHunDianRefresh(const GSProto::CSMessage& msg)
{
	//生产中不能刷新
	ServerEngine::ManorWuHunAndItemData& dbData = m_manorSystemData.wuHunDianData;
	if(dbData.bIsProduct)
	{
		return;
	}
	IZoneTime *pZone = getComponent<IZoneTime>(COMPNAME_ZoneTime, IID_IZoneTime);
	assert(pZone);
	ManorFactory * pManorFactory = getComponent<ManorFactory>(COMPNAME_ManorFactory, IID_IManorFactory);
	assert(pManorFactory);
	IGlobalCfg * pGlobal = getComponent<IGlobalCfg>(COMPNAME_GlobalCfg, IID_IGlobalCfg);
	assert(pGlobal);
	
	HeroSoulOrItem wuHunDian;
	bool res = pManorFactory->getActorWuHunDianByLevel(wuHunDian,dbData.level);
	assert(res);
	
	if(dbData.bResIsBuild)
	{
		int iLevelUpSecond =  wuHunDian.iLevelUpSecond - (pZone->GetCurSecond() - dbData.beginLevelUpSecond);
		if(iLevelUpSecond <= 0)
		{
			dbData.bResIsBuild = false;
			dbData.level += 1;
		}
	}
	
	IEntity *pEntity = getEntityFromHandle(m_hEntity);
	assert(pEntity);
	//免费刷新次数是否已经用完
	int iFreeRefreshTimes = pGlobal->getInt("武魂殿免费刷新次数", 3);
	int iActorRefreshTimes = dbData.iHaveRefreshTimes;
	if(iFreeRefreshTimes <= iActorRefreshTimes )
	{
		//int iRefreshCost = pGlobal->getInt("武魂殿刷新花费铜币",1000);
		int iRefreshCost = pManorFactory->getRefreshTieJiangPuOrWuHunDianCost(iActorRefreshTimes+1);
		int iActorHave = pEntity->getProperty(PROP_ACTOR_SILVER,0);
		if(iActorHave < iRefreshCost)
		{
			pEntity->sendErrorCode(ERROR_NEED_SILVER);
			return;
		}
		// 扣钱
		pEntity->changeProperty(PROP_ACTOR_SILVER, 0-iRefreshCost, GSProto::en_Reason_Manor_WuHunDian_Refresh);
	}
	//扣除刷新次数
	dbData.iHaveRefreshTimes++;
	//刷新出新武魂
	m_manorSystemData.wuHunDianData.befroreProductCache.clear();
	//更新客户端
	sendWuHunDian2Client();
}

void ManorSystem::sendTieJiangPu2Client()
{
	CheckWuHunDianAndTieJiangPuBuild();
	GSProto::CMD_MANOR_TIEJIANGPU_QUERY_SC scMsg;
	IEntity *pEntity = getEntityFromHandle(m_hEntity);
	assert(pEntity);
	
	IZoneTime *pZone = getComponent<IZoneTime>(COMPNAME_ZoneTime, IID_IZoneTime);
	assert(pZone);
	ManorFactory * pManorFactory = getComponent<ManorFactory>(COMPNAME_ManorFactory, IID_IManorFactory);
	assert(pManorFactory);
	IGlobalCfg * pGlobal = getComponent<IGlobalCfg>(COMPNAME_GlobalCfg, IID_IGlobalCfg);
	assert(pGlobal);
	
	ServerEngine::ManorWuHunAndItemData& dbData = m_manorSystemData.tieJiangPuData;
	HeroSoulOrItem tieJiangPu;
	bool res = pManorFactory->getActorTieJiangPuByLevel(tieJiangPu, dbData.level);
	assert(res);
	
	if(dbData.bResIsBuild)
	{
		int iLevelUpSecond =  tieJiangPu.iLevelUpSecond - (pZone->GetCurSecond() - dbData.beginLevelUpSecond);
		assert(iLevelUpSecond > 0);
		scMsg.set_ibulidremainsecond(iLevelUpSecond);
	}
	scMsg.set_bisbulid(dbData.bResIsBuild);
	scMsg.set_ilevelid(dbData.level);
	
	scMsg.set_bisproduct( dbData.bIsProduct);
	if(dbData.bIsProduct)
	{
		int iProductSecond =  tieJiangPu.iProductSecond - ( pZone->GetCurSecond() - dbData.beginProductSecond);
		if(iProductSecond > 0)
		{
			scMsg.set_iproductreaminsecond(iProductSecond);
		}
		
	}
	else
	{
		int iFreeRefreshTimes = pGlobal->getInt("铁匠铺免费刷新次数", 3);
		int iActorRefreshTimes = dbData.iHaveRefreshTimes;
		if(iFreeRefreshTimes <= iActorRefreshTimes )
		{
			//int iRefreshCost = pGlobal->getInt("铁匠铺刷新花费铜币", 1000);
			int iRefreshCost = pManorFactory->getRefreshTieJiangPuOrWuHunDianCost(iActorRefreshTimes+1);
			scMsg.set_irefreshcost(iRefreshCost);
		}
		else
		{
			scMsg.set_iremianrefreshtimes(iFreeRefreshTimes - iActorRefreshTimes);
			scMsg.set_itotalremainrefreshtimes( iFreeRefreshTimes);
		}
	}
	
	//分配
	//分生产 和非生产来处理
	if(dbData.bIsProduct)
	{
		size_t iSize = dbData.wuHunAndItemList.size();
		if((size_t)tieJiangPu.iOpenGeZiCount > iSize)
		{
			vector<ManorItem> itemList;
			pManorFactory->getTieJiangPuByLevel(itemList,  tieJiangPu.iOpenGeZiCount - iSize, dbData.level);
			for(size_t j = 0 ; j <  itemList.size(); ++j)
			{
				ServerEngine::ManorWuHunAndItemUnit unit;
				ManorItem& item = itemList[j];
				unit.id = item.iId;
				unit.iCount = item.iCount;
				unit.iBeLootCount = 0;
				unit.bWuXieKeji = false;
				dbData.wuHunAndItemList.push_back(unit);
			}
			
		}
		
		iSize = dbData.wuHunAndItemList.size();
		
		for(size_t i = 0; i < iSize; ++i)
		{
			fillTiejiangPuGeZiDetail( *(scMsg.add_szmanortiejiangpuitembox()), dbData.wuHunAndItemList[i],i);
		}
		
		
	}
	else
	{
		//cache在开始生产的时候清理，等级提升的时候清理澹?	
		size_t iSize = dbData.befroreProductCache.size();
		if( (dbData.befroreProductCache.size()  == 0) || ( (size_t)tieJiangPu.iOpenGeZiCount > iSize))
		{
			dbData.befroreProductCache.clear();
			vector<ManorItem> itemList;
			pManorFactory->getTieJiangPuByLevel(itemList, tieJiangPu.iOpenGeZiCount, dbData.level);
			for(size_t j = 0 ; j<  itemList.size(); ++j)
			{
				ServerEngine::ManorWuHunAndItemUnit unit;
				ManorItem& item = itemList[j];
				unit.id = item.iId;
				unit.iCount = item.iCount;
				unit.iBeLootCount = 0;
				unit.bWuXieKeji = false;
				dbData.befroreProductCache.push_back(unit);
			}
		}
		 iSize = dbData.befroreProductCache.size();
		for(size_t i = 0; i < iSize; ++i)
		{
			fillTiejiangPuGeZiDetail( *(scMsg.add_szmanortiejiangpuitembox()), dbData.befroreProductCache[i], i);
		}
	}
	pEntity->sendMessage(GSProto::CMD_MANOR_TIEJIANGPU_QUERY, scMsg );
}

void ManorSystem::fillTiejiangPuGeZiDetail( GSProto::ManorTieJiangPuItemBox& box, const ServerEngine::ManorWuHunAndItemUnit& unit, int index)
{
	box.set_bisopen(true);
	GSProto::ManorTieJiangPuItemDetail& detail = *(box.mutable_itemdetail());
	detail.set_iitemid( unit.id);
	detail.set_bitemiswuxiekeji( unit.bWuXieKeji);
	detail.set_iitemcount(unit.iCount );
	detail.set_iitemhavelootcount(unit.iBeLootCount);
	box.set_iboxindex(index);
}

void ManorSystem::onQueryTieJiangPu(const GSProto::CSMessage& msg)
{
	sendTieJiangPu2Client();
}

void ManorSystem::onTieJiangPuLevelUp(const GSProto::CSMessage& msg)
{
	//最高级不能升级
	IEntity *pEntity = getEntityFromHandle(m_hEntity);
	assert(pEntity);

	IZoneTime *pZone = getComponent<IZoneTime>(COMPNAME_ZoneTime, IID_IZoneTime);
	assert(pZone);
	ManorFactory * pManorFactory = getComponent<ManorFactory>(COMPNAME_ManorFactory, IID_IManorFactory);
	assert(pManorFactory);
	ServerEngine::ManorWuHunAndItemData& dbData = m_manorSystemData.tieJiangPuData;

	//玩家等级除以10
	int iNeedActorLevel = (dbData.level + 1)*10;
	int iLevelUp = pEntity->getProperty(PROP_ENTITY_LEVEL,0);
	if( iNeedActorLevel >  iLevelUp)
	{
		GSProto::CMD_MANOR_TIEJIANGPU_LEVELUP_SC scMsg;
		scMsg.set_ineedactorlevel(iNeedActorLevel);
		pEntity->sendMessage(GSProto::CMD_MANOR_TIEJIANGPU_LEVELUP,  scMsg);
		return;
	}
		
	HeroSoulOrItem TieJiangPu;
	bool res = pManorFactory->getActorTieJiangPuByLevel(TieJiangPu, dbData.level);
	assert(res);
	if(TieJiangPu.iLevelUpCost == -1)
	{
		pEntity->sendErrorCode(ERROR_MANOR_TIEJIANGPU_LEVELLIMIT);
		return;
	}
	
	//正在升级不能升级
	if(dbData.bResIsBuild)
	{
			return;
	}
	//判断钱足不足
	int iNeedSilver = TieJiangPu.iLevelUpCost;
	int iActorHave = pEntity->getProperty(PROP_ACTOR_SILVER,0);
	if(iNeedSilver > iActorHave)
	{
		pEntity->sendErrorCode(ERROR_NEED_SILVER);
		return;
	}
	bool buildTeamEmpty = manorBuildTeamsCanBulid();
	if(!buildTeamEmpty)
	{
		pEntity->sendErrorCode(ERROR_MANOR_NOT_MORE_BUILDTEAMS);
		return;
	}
				
	//扣钱
	pEntity->changeProperty( PROP_ACTOR_SILVER, 0-iNeedSilver, GSProto::en_Reason_Manor_TieJiangPu_LEVELUP_COST);
	//开始升级
	dbData.bResIsBuild = true;
	dbData.beginLevelUpSecond = pZone->GetCurSecond();
	sendTieJiangPu2Client();
}

void ManorSystem::onTieJiangPuRefresh(const GSProto::CSMessage& msg)
{
	//生产中不能刷新
	ServerEngine::ManorWuHunAndItemData& dbData = m_manorSystemData.tieJiangPuData;
	if(dbData.bIsProduct)
	{
		return;
	}
	IZoneTime *pZone = getComponent<IZoneTime>(COMPNAME_ZoneTime, IID_IZoneTime);
	assert(pZone);
	ManorFactory * pManorFactory = getComponent<ManorFactory>(COMPNAME_ManorFactory, IID_IManorFactory);
	assert(pManorFactory);
	IGlobalCfg * pGlobal = getComponent<IGlobalCfg>(COMPNAME_GlobalCfg, IID_IGlobalCfg);
	assert(pGlobal);
	HeroSoulOrItem tieJiangPu;
	bool res = pManorFactory->getActorTieJiangPuByLevel(tieJiangPu,dbData.level);
	assert(res);
	IEntity *pEntity = getEntityFromHandle(m_hEntity);
	assert(pEntity);
	if(dbData.bResIsBuild)
	{
		int iLevelUpSecond =  tieJiangPu.iLevelUpSecond - (pZone->GetCurSecond() - dbData.beginLevelUpSecond);
		if(iLevelUpSecond <= 0)
		{
			dbData.bResIsBuild = false;
			dbData.level += 1;
		}
	}
	//免费刷新次数是否已经用完
	int iFreeRefreshTimes = pGlobal->getInt("铁匠铺免费刷新次数", 3);
	int iActorRefreshTimes = dbData.iHaveRefreshTimes;
	if(iFreeRefreshTimes <= iActorRefreshTimes )
	{
		//int iRefreshCost = pGlobal->getInt("铁匠铺刷新花费铜币",1000);
		int iRefreshCost = pManorFactory->getRefreshTieJiangPuOrWuHunDianCost(iActorRefreshTimes+1);
		int iActorHave = pEntity->getProperty(PROP_ACTOR_SILVER,0);
		if(iActorHave < iRefreshCost)
		{
			pEntity->sendErrorCode(ERROR_NEED_SILVER);
			return;
		}
		// 扣钱
		pEntity->changeProperty(PROP_ACTOR_SILVER, 0-iRefreshCost, GSProto::en_Reason_Manor_TieJiangPu_Refresh);
	}
	
	//扣除刷新次数
	dbData.iHaveRefreshTimes++;
	//刷新出新武魂
	m_manorSystemData.tieJiangPuData.befroreProductCache.clear();
	//更新客户端
	sendTieJiangPu2Client();
}

void ManorSystem::onTieJiangPuBeginProduct(const GSProto::CSMessage& msg)
{
	//是否没有生产
	if(m_manorSystemData.tieJiangPuData.bIsProduct)
	{
		return;
	}

	IZoneTime *pZoneTime = getComponent<IZoneTime>(COMPNAME_ZoneTime, IID_IZoneTime);
	assert(pZoneTime);
	m_manorSystemData.tieJiangPuData.bIsProduct = true;
	m_manorSystemData.tieJiangPuData.beginProductSecond = pZoneTime->GetCurSecond();
	m_manorSystemData.tieJiangPuData.wuHunAndItemList.clear();
	m_manorSystemData.tieJiangPuData.wuHunAndItemList = m_manorSystemData.tieJiangPuData.befroreProductCache;
	m_manorSystemData.tieJiangPuData.befroreProductCache.clear();
	
	//更新客户端
	sendTieJiangPu2Client();

	//领地武魂殿祭奠次数
	 {
		EventArgs args;
		args.context.setInt("times",1);
		args.context.setInt("entity",m_hEntity);
		IEntity *pEntity = getEntityFromHandle(m_hEntity);
		assert(pEntity);
		pEntity->getEventServer()->setEvent(EVENT_ENTITY_TASK_MANOR_ITEMDaZao, args);
	}
}

void ManorSystem::onTieJiangPuSetItemWuxiekeji(const GSProto::CSMessage& msg)
{
	GSProto::CMD_MANOR_TIEJIANGPU_WUXIEKEJI_CS csMsg;
	if(! csMsg.ParseFromString(msg.strmsgbody()))
	{
		return;
	}
	int index = csMsg.iboxindex();
	ServerEngine::ManorWuHunAndItemData& dbData = m_manorSystemData.tieJiangPuData;
	if(!dbData.bIsProduct)
	{
		return;
	}
	if( dbData.wuHunAndItemList.size() <= (size_t)index )
	{
		return;
	}
	ServerEngine::ManorWuHunAndItemUnit& unit = dbData.wuHunAndItemList[index];
	if(unit.bWuXieKeji )
	{
		return;
	}
	//
	IEntity *pEntity = getEntityFromHandle(m_hEntity);
	assert(pEntity);
	IGlobalCfg *pGlobal = getComponent<IGlobalCfg>(COMPNAME_GlobalCfg, IID_IGlobalCfg);
	assert(pGlobal);
	int iNeedGold = pGlobal->getInt("给武魂设置无懈可击花费金币",20);
	int iActorHave = pEntity->getProperty(PROP_ACTOR_GOLD,0);
	if(iActorHave < iNeedGold)
	{
		pEntity->sendErrorCode(ERROR_NEED_GOLD);
		return;
	}
	//扣钱?	
	pEntity->changeProperty( PROP_ACTOR_GOLD, 0-iNeedGold, GSProto::en_Reason_Manor_TieJiangPu_WUXIEKEJI_Cost);
	
	//设置无懈可击
	unit.bWuXieKeji = true;
	//更新客户端	
	sendTieJiangPu2Client();
}

void ManorSystem::onQueryManorLog(const GSProto::CSMessage& msg)
{
	GSProto::CMD_MANOR_QUERY_LOG_SC scMsg;
	for(size_t i = 0; i < m_manorSystemData.manorLogVec.size(); ++i)
	{
		GSProto::ManorLootLog& log = *(scMsg.add_szmanorlootlog());
		const string& msgStr = m_manorSystemData.manorLogVec[i];
		if(!log.ParseFromString(msgStr))
		{
			assert(0);
		}
	}
	m_manorSystemData.bHaveNewLog = false;
	IEntity *pEntity = getEntityFromHandle(m_hEntity);
	assert(pEntity);
	pEntity->sendMessage(GSProto::CMD_MANOR_QUERY_LOG, scMsg);
}

struct WillLootActorCallBack
{
	WillLootActorCallBack(HEntity hEntity, bool bRevenge)
		:m_QueryHEntity(hEntity),
		m_bIsRevenge(bRevenge)
	{
		
	}

	virtual void operator()(int iRet, HEntity hTarget, ServerEngine::PIRole& roleInfo)
	{
		if( iRet != ServerEngine::en_RoleRet_OK)
		{
			FDLOG("ManorSystem")<<"WillLootActorCallBack|"<< iRet <<endl;
			return;
		}
		
		IEntity *pEntity = getEntityFromHandle(m_QueryHEntity);
		if(!pEntity) return;
		ManorSystem* pQueryActorSyst = static_cast<ManorSystem*>(pEntity->querySubsystem(IID_IManorSystem));
		assert(pQueryActorSyst);
		IZoneTime *pZone = getComponent<IZoneTime>(COMPNAME_ZoneTime, IID_IZoneTime);
		assert(pZone);
		IGlobalCfg * pGlobal = getComponent<IGlobalCfg>( COMPNAME_GlobalCfg,  IID_IGlobalCfg);
		assert(pGlobal);
		
		IJZEntityFactory* pEntityFactory = getComponent<IJZEntityFactory>(COMPNAME_EntityFactory, IID_IJZEntityFactory);
		assert(pEntityFactory);
	
		if(0 != hTarget)
		{
			IEntity *pWillLootActor = getEntityFromHandle(hTarget);
			assert( pWillLootActor);
			if(hTarget == m_QueryHEntity )
			{
					return;
			}
			ManorSystem *pSystem = static_cast<ManorSystem*>(pWillLootActor->querySubsystem(IID_IManorSystem));
			assert(pSystem);
			ServerEngine::ManorSystemData systemData;
			pSystem->getActorManorSysData(systemData);
			if(systemData.bIsBeLoot)
			{
				//int remaindSecond =  pZone->GetCurSecond() - systemData.iLostBeLootSecond;
				//int iSecond = pGlobal->getInt("领地被掠夺了的保护时间",7200);

				int remaindSecond = pQueryActorSyst->getRemaindProtectSecond(systemData);
				if(remaindSecond >0 )
				{
					//在掠夺前已经被掠夺
					//提示客户端重新找
					if(m_bIsRevenge)
					{
						
							pEntity->sendErrorCode( ERROR_MANOR_IN_PROTECT);
					}
					else
					{
						pEntity->sendErrorCode( ERROR_NO_CANLOOTACTOR);
					}
					
					return;
				}
				else
				{
					pSystem->sendWillBeLootManorInfo(m_QueryHEntity);
				}
				
			}
			else
			{
				pSystem->sendWillBeLootManorInfo(m_QueryHEntity);
			}
			//这个他了下发信息
			
		}
		else
		{
			ServerEngine::RoleSaveData roleSaveData;
			ServerEngine::JceToObj(roleInfo.roleData, roleSaveData);
			map<taf::Int32, std::string>::iterator iter = roleSaveData.subsystemData.find(IID_IManorSystem);
			assert(iter != roleSaveData.subsystemData.end());
			ServerEngine::ManorSystemData systemData;
			ServerEngine::JceToObj(iter->second, systemData);
			
			if(systemData.bIsBeLoot)
			{
				//int remaindSecond =  pZone->GetCurSecond() - systemData.iLostBeLootSecond;
				//int iSecond = pGlobal->getInt("领地被掠夺了的保护时间",7200);
				//if(remaindSecond < iSecond )
				int remaindSecond = pQueryActorSyst->getRemaindProtectSecond(systemData);
				if(remaindSecond >0 )
				{
					//在掠夺前已经被掠夺
					//提示客户端重新找
					if(m_bIsRevenge)
					{
						pEntity->sendErrorCode( ERROR_MANOR_IN_PROTECT);
					}
					else
					{
						pEntity->sendErrorCode( ERROR_NO_CANLOOTACTOR);
					}
					return;
				}
				else
				{
					ServerEngine::ActorCreateContext tmpCreateCtx;
					tmpCreateCtx.strAccount = roleInfo.strAccount;
					ServerEngine::JceToObj(roleInfo.roleData, tmpCreateCtx.data);
				
					string strTmpData = ServerEngine::JceToStr(tmpCreateCtx);
					IEntity* pTargetGhost = pEntityFactory->getEntityFactory()->createEntity("Ghost", strTmpData);
					assert(pTargetGhost);
					
					ManorSystem *pSystem = static_cast<ManorSystem*>(pTargetGhost->querySubsystem(IID_IManorSystem));
					assert(pSystem);
					pSystem->sendWillBeLootManorInfo(m_QueryHEntity);
				
					delete pTargetGhost;
				}
				
			}
			else
			{
				ServerEngine::ActorCreateContext tmpCreateCtx;
				tmpCreateCtx.strAccount = roleInfo.strAccount;
				ServerEngine::JceToObj(roleInfo.roleData, tmpCreateCtx.data);
				
				string strTmpData = ServerEngine::JceToStr(tmpCreateCtx);
				IEntity* pTargetGhost = pEntityFactory->getEntityFactory()->createEntity("Ghost", strTmpData);
				assert(pTargetGhost);

				ManorSystem *pSystem = static_cast<ManorSystem*>(pTargetGhost->querySubsystem(IID_IManorSystem));
				assert(pSystem);
				pSystem->sendWillBeLootManorInfo(m_QueryHEntity);
				
				delete pTargetGhost;
			}
			//这个他了下发信息这里建一个gost
			
		}
	}
private:
	HEntity m_QueryHEntity;
	bool m_bIsRevenge;
};

void  ManorSystem::getActorManorInfo(GSProto::CMD_MANOR_REQUEST_LOOT_SC & scMsg, HEntity targetHEntity)
{
	IEntity *targetActor = getEntityFromHandle( m_hEntity);
	assert(targetActor);
	
	GSProto::BeLootActorInfo& actorInfo = *(scMsg.mutable_belootactorinfo());
	string strName = targetActor->getProperty(PROP_ENTITY_NAME, "");
	int iActorLevel = targetActor->getProperty(PROP_ENTITY_LEVEL,0);
	int iFightValue = targetActor->getProperty(PROP_ENTITY_FIGHTVALUE,0);
	int iHeadId = targetActor->getProperty(PROP_ENTITY_ACTOR_HEAD, 0);
	
	actorInfo.set_stractorname(strName);
	actorInfo.set_iactorlevel(iActorLevel);
	actorInfo.set_ifightvalue(iFightValue);
	actorInfo.set_iheadid(iHeadId);
	
	//铜矿的信息
	int iSilver = calcSilver(false,targetHEntity);
	//修为矿信息
	int iHeroExp = calcHeroExp(false,targetHEntity);
	//铁匠铺
	//下行给客户端
	scMsg.set_icanlootsilver( iSilver);
	scMsg.set_icanlootheroexp( iHeroExp);
	//scMsg.set_iheroid( iHeroId);
	//刷新消耗
	IGlobalCfg * pGlobal = getComponent<IGlobalCfg>(COMPNAME_GlobalCfg, IID_IGlobalCfg);
	assert(pGlobal);
	int iLootNeedSilver = searchCost(targetHEntity);//pGlobal->getInt("查找掠夺对象消耗铜币",2000);
	scMsg.set_irefreshcost( iLootNeedSilver);
	//掠夺消耗
	int iCostVigor = pGlobal->getInt("去掠夺消耗精力", 2);
	scMsg.set_ilootcostvigor( iCostVigor);
	//阵型数据
	getActorFormation(scMsg.mutable_szenemyheroformation());
	//pkRole
	GSProto::PBPkRole& pbPkRole = *(scMsg.mutable_enemy());
	pbPkRole.set_straccount( targetActor->getProperty(PROP_ACTOR_ACCOUNT,""));
	pbPkRole.set_iworldid(targetActor->getProperty(PROP_ACTOR_WORLD, 0));

	//增加矿和武魂殿的基本信息
	for(size_t i = 0; i < m_manorSystemData.silverResVec.size(); ++i)
	{
		ServerEngine::ManorResData& dbData = m_manorSystemData.silverResVec[i];
		GSProto::OtherManorResDetail& detail = *(scMsg.add_silverres());
		detail.set_iresid( dbData.ResId);
		detail.set_ireslevel( dbData.iResLevel);
	}

	for(size_t i = 0; i < m_manorSystemData.heroExpResVec.size();++i)
	{
		ServerEngine::ManorResData& dbData = m_manorSystemData.heroExpResVec[i];
		GSProto::OtherManorResDetail& detail = *(scMsg.add_heroexpres());
		detail.set_iresid( dbData.ResId);
		detail.set_ireslevel( dbData.iResLevel);
	}

	ServerEngine::ManorWuHunAndItemData &TieDbData = m_manorSystemData.tieJiangPuData;
	GSProto::OtherManorResDetail& Tiedetail = *(scMsg.mutable_tiejiangypu());
	Tiedetail.set_iresid( -1);
	Tiedetail.set_ireslevel( TieDbData.level);

	
	ServerEngine::ManorWuHunAndItemData &wuDbData = m_manorSystemData.wuHunDianData;
	GSProto::OtherManorResDetail& wudetail = *(scMsg.mutable_wuhundian());
	wudetail.set_iresid( -1);
	wudetail.set_ireslevel( wuDbData.level);
}

void ManorSystem::sendWillBeLootManorInfo(HEntity targetHEntity)
{
	IEntity *targetActor = getEntityFromHandle( targetHEntity);
	assert(targetActor);
	GSProto::CMD_MANOR_REQUEST_LOOT_SC scMsg;
	/*ManorSystem* pManorSystem = static_cast<ManorSystem*>(targetActor->querySubsystem(IID_IManorSystem));
	assert(pManorSystem);
	pManorSystem->*/
	getActorManorInfo(scMsg,targetHEntity );
	
	//扣钱
	//IGlobalCfg * pGlobal = getComponent<IGlobalCfg>(COMPNAME_GlobalCfg, IID_IGlobalCfg);
	//assert(pGlobal);
	int iLootNeedSilver = searchCost(targetHEntity);//pGlobal->getInt("去掠夺花费",2000);
	int iActorHave = targetActor->getProperty(PROP_ACTOR_SILVER, 0);
	if(iActorHave < iLootNeedSilver)
	{
		targetActor->sendErrorCode(ERROR_NEED_SILVER);
		return;
	}
	targetActor->changeProperty( PROP_ACTOR_SILVER, 0 - iLootNeedSilver,GSProto::en_Reason_Manor_Search);
	
	targetActor->sendMessage(GSProto::CMD_MANOR_REQUEST_LOOT, scMsg);
	
	
}

void ManorSystem::getActorFormation(google::protobuf::RepeatedPtrField< ::GSProto::ManorEnemyHeroFormation >* manorEnemyFormation)
{
	IEntity *pEntity = getEntityFromHandle(m_hEntity);
	assert(pEntity);
	IFormationSystem* pFormationSystem = static_cast<IFormationSystem*>( pEntity->querySubsystem(IID_IFormationSystem));
	assert(pFormationSystem);
	vector<HEntity> hEntityVec;
	pFormationSystem->getEntityList(hEntityVec);
	
	for(size_t i = 0; i < hEntityVec.size(); ++i)
	{
		HEntity hTmpEntity = hEntityVec[i];
		IEntity* pTmpHero = getEntityFromHandle(hTmpEntity);
		if(!pTmpHero) continue;
		int iPos = pFormationSystem->getEntityPos(hTmpEntity);
		GSProto::ManorEnemyHeroFormation& formation = *( manorEnemyFormation->Add());
		formation.set_ipos( iPos);
		formation.set_ibaseid( pTmpHero->getProperty(PROP_ENTITY_BASEID,0));
		formation.set_ilevel( pTmpHero->getProperty(PROP_ENTITY_LEVEL, 0));
		formation.set_ilevelstep(  pTmpHero->getProperty(PROP_ENTITY_LEVELSTEP, 0));
		formation.set_iquality(  pTmpHero->getProperty(PROP_ENTITY_QUALITY, 0));
	}
}

int ManorSystem::calcTieJiangPuCanGetItemId()
{
	IRandom *pRand = getComponent<IRandom>(COMPNAME_Random, IID_IMiniAprRandom);
	assert(pRand);
	IGlobalCfg *pGlobal = getComponent<IGlobalCfg>(COMPNAME_GlobalCfg, IID_IGlobalCfg);
	assert(pGlobal);
	IZoneTime *pZoneTime = getComponent<IZoneTime>( COMPNAME_ZoneTime, IID_IZoneTime);
	assert(pZoneTime);
	ManorFactory *pManorFact = getComponent<ManorFactory>( COMPNAME_ManorFactory, IID_IManorFactory);
	assert(pManorFact);
	
	int iLootChance = pGlobal->getInt("掠夺获取物品机会",50);
	ServerEngine::ManorWuHunAndItemData &data = m_manorSystemData.tieJiangPuData;
	int iChance = pRand->random()%100;
	if(iChance > iLootChance)
	{
		return -1;
	}
	//是否在保护状态
	int iSecond = pGlobal->getInt("铁匠铺保护时间",7200);
	if(!data.bIsProduct)
	{
		return -1;
	}
	int productTime = pZoneTime->GetCurSecond() - data.beginProductSecond;
	if(iSecond > productTime)
	{
		return -1;
	}

	HeroSoulOrItem item;
	pManorFact->getActorTieJiangPuByLevel(item, data.level);
	if(productTime > item.iProductSecond/2)
	{
		return -1;
	}
	//找一个来掠夺
	vector<ServerEngine::ManorWuHunAndItemUnit> unitVec;
	for(size_t i = 0; i < data.wuHunAndItemList.size(); ++i)
	{
		ServerEngine::ManorWuHunAndItemUnit& unit = data.wuHunAndItemList[i];
		int iRemaind = unit.iCount ;//- unit.iBeLootCount;
		if(unit.bWuXieKeji || iRemaind == 0)
		{
			continue;
		}
		unitVec.push_back(unit);
	}
	
	if(unitVec.size() <= 1)
	{
		return -1;
	}
	
	int randIndex = pRand->random()%unitVec.size();
	int iItemId = unitVec[randIndex].id;
	for(size_t i = 0; i < data.wuHunAndItemList.size(); ++i)
	{
		ServerEngine::ManorWuHunAndItemUnit& unit = data.wuHunAndItemList[i];
		if(iItemId == unit.id)
		{
			unit.iCount -=1;
			break;
		}
	}
	return iItemId;
}

int ManorSystem::calcWuHunDianCanGetIHeroId()
{
	IRandom *pRand = getComponent<IRandom>(COMPNAME_Random, IID_IMiniAprRandom);
	assert(pRand);
	
	IGlobalCfg *pGlobal = getComponent<IGlobalCfg>(COMPNAME_GlobalCfg, IID_IGlobalCfg);
	assert(pGlobal);
	
	IZoneTime *pZoneTime = getComponent<IZoneTime>( COMPNAME_ZoneTime, IID_IZoneTime);
	assert(pZoneTime);
	
	ManorFactory *pManorFact = getComponent<ManorFactory>( COMPNAME_ManorFactory, IID_IManorFactory);
	assert(pManorFact);
	
	int iLootChance = pGlobal->getInt("掠夺获取物品机会",50);
	ServerEngine::ManorWuHunAndItemData &data = m_manorSystemData.wuHunDianData;
	int iChance = pRand->random()%100;
	if(iChance > iLootChance)
	{
		return -1;
	}
	//是否在保护状态
	int iSecond = pGlobal->getInt("武魂殿保护时间",7200);
	if(!data.bIsProduct)
	{
		return -1;
	}

	int productTime = pZoneTime->GetCurSecond() - data.beginProductSecond;
	if(iSecond > productTime)
	{
		return -1;
	}

	HeroSoulOrItem item;
	pManorFact->getActorTieJiangPuByLevel(item, data.level);
	if(productTime > item.iProductSecond/2)
	{
		return -1;
	}
	
	//找一个来掠夺
	vector<ServerEngine::ManorWuHunAndItemUnit> unitVec;
	for(size_t i = 0; i < data.wuHunAndItemList.size(); ++i)
	{
		ServerEngine::ManorWuHunAndItemUnit& unit = data.wuHunAndItemList[i];
		int iRemaind = unit.iCount ;//- unit.iBeLootCount;
		if(unit.bWuXieKeji || iRemaind == 0)
		{
			continue;
		}
		unitVec.push_back(unit);
	}
	
	if(unitVec.size() <= 1 )
	{
		return -1;
	}
	
	int randIndex = pRand->random()%unitVec.size();
	int iHeroId = unitVec[randIndex].id;;
	for(size_t i = 0; i < data.wuHunAndItemList.size(); ++i)
	{
		ServerEngine::ManorWuHunAndItemUnit& unit = data.wuHunAndItemList[i];
		if(iHeroId == unit.id)
		{
			unit.iCount -= 1;
			break;
		}
	}
	
	return iHeroId;
}

int ManorSystem::calcSilver(bool bHaveLoot, HEntity hTargetEntity, int iFightStar/*= 0*/)
{
	//铜币奖励=对方领地当前产出的铜币*50%*（1-（我方等级-对方等级）*0.025）*（1-（我方战力-对方战力）/对方战力）*（0.4+战斗评价星数*0.3）
	//这里调用是另一个玩家的这个系统，所有target是调用者，也是这里的我方
	IEntity *pTargetEntity = getEntityFromHandle(m_hEntity);
	assert(pTargetEntity);
	IEntity *pMyEntity = getEntityFromHandle(hTargetEntity);
	assert(pMyEntity);
	IZoneTime *pZoneTime = getComponent<IZoneTime>(COMPNAME_ZoneTime, IID_IZoneTime);
	assert(pZoneTime);
	int iCurSecond = pZoneTime->GetCurSecond();

	int iMyLevel = pMyEntity->getProperty(PROP_ENTITY_LEVEL, 0);
	int iTargetLevel = pTargetEntity->getProperty(PROP_ENTITY_LEVEL, 0);
	
	IGlobalCfg *pGolobal = getComponent<IGlobalCfg>(COMPNAME_GlobalCfg, IID_IGlobalCfg);
	assert(pGolobal);
	int iSilverPersent = pGolobal->getInt("铜矿掠夺折扣", 5000);
	int iLevelPersent = pGolobal->getInt("铜矿掠夺等级差系数", 120);
	assert(iLevelPersent >0);
	int iStartPersentBase = pGolobal->getInt("铜矿掠夺战斗评星基数", 4000);
	int iStartModulus = pGolobal->getInt("铜矿掠夺战斗评星系数", 3000);
	int iSilverCount = 0;
	bool bTargetMachine = (pTargetEntity->getProperty(PROP_ENTITY_ISMACHINE, 0) == 1);
	if(bTargetMachine)
	{
		iSilverCount = m_manorSystemData.iProductSivlerBeforLevelUp -  m_manorSystemData.iBeLootSilver;
		if(iSilverCount < 0)  iSilverCount = 0;
	}
	else
	{	
		int iTotalSIlver = 0;
		getSilverTotalPutOut(iTotalSIlver);
	 	iSilverCount =  calcSilverPutOut() +  m_manorSystemData.iProductSivlerBeforLevelUp ;
		iSilverCount = std:: min(iTotalSIlver, iSilverCount);
		//iSilverCount = iSilverCount -  m_manorSystemData.iBeLootSilver;
		assert(iSilverCount >= 0);
	}
	
	double iRes = iSilverCount * (double)iSilverPersent/10000;
	double levelBase = (iMyLevel -iTargetLevel ) /(double) iLevelPersent;
	iRes =  iRes * (1- levelBase);
	//iRes = iRes*(1 -(iMyFightValue-iTargetFightValue)/iTargetFightValue);
	if(bHaveLoot)
	{
		iRes = iRes* ((double)iStartPersentBase/10000 + iFightStar* (double)iStartModulus/10000);
		//被掠夺了那么把钱扣了
		int iRemaindSilver =  iSilverCount - iRes;
		if(bTargetMachine)
		{
			int iMachinePersent = pGolobal->getInt("领地机器人资源恢复万分比", 2500);
			double machinePersent = iRemaindSilver/m_manorSystemData.iProductSivlerBeforLevelUp;
			if(machinePersent < (double)iMachinePersent/10000)
			{
				 m_manorSystemData.iBeLootSilver = 0;
			}
		}
		else
		{
			m_manorSystemData.iProductSivlerBeforLevelUp  = iRemaindSilver;
			for(size_t i = 0; i < m_manorSystemData.silverResVec.size(); ++i)
			{
				ServerEngine::ManorResData& resDBData = m_manorSystemData.silverResVec[i];
				if(resDBData.iResLevel == 0)  continue;
				resDBData.beginProductSecond = iCurSecond;
			}
		}
		
	}
	return iRes;
}

//两个函数是可以写在一起，万一以后两个的数值要不一样呢?
int ManorSystem::calcHeroExp(bool bHaveLoot, HEntity hTargetEntity, int iFightStar/*= 0*/)                                                  
{
	//修为奖励=对方领地当前产出的修为*50%*（1-（我方等级-对方等级）*0.025）*（1-（我方战力-对方战力）/对方战力）*（0.4+战斗评价星数*0.3）
	//这里调用是另一个玩家的这个系统，所有target是调用者，也是这里的我方
	IEntity *pTargetEntity = getEntityFromHandle(m_hEntity);
	assert(pTargetEntity);
	IEntity *pMyEntity = getEntityFromHandle(hTargetEntity);
	assert(pMyEntity);

	int iMyLevel = pMyEntity->getProperty(PROP_ENTITY_LEVEL, 0);
	int iTargetLevel = pTargetEntity->getProperty(PROP_ENTITY_LEVEL, 0);

	IZoneTime *pZoneTime = getComponent<IZoneTime>(COMPNAME_ZoneTime, IID_IZoneTime);
	assert(pZoneTime);
	int iCurSecond = pZoneTime->GetCurSecond();
	
	IGlobalCfg *pGolobal = getComponent<IGlobalCfg>(COMPNAME_GlobalCfg, IID_IGlobalCfg);
	assert(pGolobal);
	
	int iSilverPersent = pGolobal->getInt("铜矿掠夺折扣", 5000);
	int iLevelPersent = pGolobal->getInt("铜矿掠夺等级差系数", 120);
	assert(iLevelPersent > 0);
	int iStartPersentBase = pGolobal->getInt("铜矿掠夺战斗评星基数", 4000);
	int iStartModulus = pGolobal->getInt("铜矿掠夺战斗评星系数", 3000);
	
	int iheroExpCount = 0;
	bool bTargetMachine = pTargetEntity->getProperty(PROP_ENTITY_ISMACHINE, 0) == 1;
	if(bTargetMachine)
	{
		iheroExpCount = m_manorSystemData.iProductHeroExpBeforLevelUp - m_manorSystemData.iBeLootHeroExp;
		if(iheroExpCount < 0)  iheroExpCount = 0;
	}
	else
	{
		int iTotalHeroExp = 0;
		getHeroExpTotalPutOut(iTotalHeroExp);
	 	iheroExpCount = calcHeroExpPutOut() + m_manorSystemData.iProductHeroExpBeforLevelUp;
		iheroExpCount = std:: min(iheroExpCount, iTotalHeroExp);
		//iheroExpCount = iheroExpCount - m_manorSystemData.iBeLootHeroExp;
		assert(iheroExpCount >= 0); 
	}

	
	double iRes = iheroExpCount * (double)iSilverPersent/10000;
	double levelBase = (iMyLevel -iTargetLevel ) / (double)iLevelPersent;
	iRes =  iRes * (1- levelBase);
	if(bHaveLoot)
	{
		iRes = iRes* ((double)iStartPersentBase/10000 + iFightStar* (double)iStartModulus/10000);
		//被掠夺了那么把钱扣了
		int iRemaindHeroExp =  iheroExpCount - iRes;
		if(bTargetMachine)
		{
			m_manorSystemData.iBeLootHeroExp += iRes;
			iRemaindHeroExp = m_manorSystemData.iProductHeroExpBeforLevelUp - m_manorSystemData.iBeLootHeroExp;
			int iMachinePersent = pGolobal->getInt("领地机器人资源恢复万分比", 2500);
			double machinePersent = iRemaindHeroExp/m_manorSystemData.iProductHeroExpBeforLevelUp;
			if(machinePersent < (double)iMachinePersent/10000)
			{
				 m_manorSystemData.iBeLootHeroExp = 0;
			}
		}
		else
		{
			m_manorSystemData.iProductHeroExpBeforLevelUp  = iRemaindHeroExp;
			for(size_t i = 0; i < m_manorSystemData.heroExpResVec.size(); ++i)
			{
				ServerEngine::ManorResData& resDBData = m_manorSystemData.heroExpResVec[i];
				if(resDBData.iResLevel == 0) continue;
				resDBData.beginProductSecond = iCurSecond;
			}
		}
	}
	return iRes;
}

void ManorSystem::getActorManorSysData(ServerEngine::ManorSystemData &monorSystemData)
{
	monorSystemData = m_manorSystemData;
}

void ManorSystem::onReqRevenge(const GSProto::CSMessage& msg)
{
	GSProto::CMD_MANOR_REVENGE_CS csMsg;
	if(!csMsg.ParseFromString( msg.strmsgbody()))
	{
		return ;
	}

	IEntity *pEntity = getEntityFromHandle(m_hEntity);
	assert(pEntity);
	IGlobalCfg *pGlobal = getComponent<IGlobalCfg>(COMPNAME_GlobalCfg, IID_IGlobalCfg);
	assert(pGlobal);
	IZoneTime *pZoneTime = getComponent<IZoneTime>(COMPNAME_ZoneTime, IID_IZoneTime);
	assert(pZoneTime);
	int iCurSecond = pZoneTime->GetCurSecond();
	int iCurHour = pZoneTime->GetHourInDay(iCurSecond);
	int iCanLootBegin = pGlobal->getInt("每天开始掠夺时间", 8);
	int iCanLootEnd = pGlobal->getInt("每天结束掠夺时间", 23);
	if(iCurHour < iCanLootBegin ||  iCurHour>= iCanLootEnd )
	{
		pEntity->sendErrorCode(ERROR_MANOR_LOOT_NOT_THETIME);
		return;
	}
	
	
	GSProto::PBPkRole pbPKRole = csMsg.enemy();
	ServerEngine::PKRole targetKey;
	targetKey.strAccount = pbPKRole.straccount();
	targetKey.rolePos = 0;
	targetKey.worldID = pbPKRole.iworldid();
	
	IModifyDelegate* pModifyDelegate = getComponent<IModifyDelegate>(COMPNAME_ModifyDelegate, IID_IModifyDelegate);
	assert(pModifyDelegate);
	pModifyDelegate->submitDelegateTask(targetKey, WillLootActorCallBack(m_hEntity, true));
}

void ManorSystem::onRequestLoot(const GSProto::CSMessage& msg)
{
	
	if(gmFlag)
	{
		gmFlag = false;
		IModifyDelegate* pModifyDelegate = getComponent<IModifyDelegate>(COMPNAME_ModifyDelegate, IID_IModifyDelegate);
		assert(pModifyDelegate);
		pModifyDelegate->submitDelegateTask(gmRoleKey, WillLootActorCallBack(m_hEntity, true));
		return;
	}
	//8~23才能掠夺todo
	IEntity *pEntity = getEntityFromHandle(m_hEntity);
	assert(pEntity);
	IGlobalCfg *pGlobal = getComponent<IGlobalCfg>(COMPNAME_GlobalCfg, IID_IGlobalCfg);
	assert(pGlobal);
	
		//等级开放
	int iActorLevel = pEntity->getProperty(PROP_ENTITY_LEVEL,1);
	int iNeedLevel = pGlobal->getInt("领地掠夺功能开启等级",10 );
	if(iActorLevel < iNeedLevel )
	{
		pEntity->sendErrorCode(ERROR_MANOR_TOLOOT_OPNELEVEL);
		return;
	}

	IZoneTime *pZoneTime = getComponent<IZoneTime>(COMPNAME_ZoneTime, IID_IZoneTime);
	assert(pZoneTime);
	int iCurSecond = pZoneTime->GetCurSecond();
	int iCurHour = pZoneTime->GetHourInDay(iCurSecond);
	int iCanLootBegin = pGlobal->getInt("每天开始掠夺时间", 8);
	int iCanLootEnd = pGlobal->getInt("每天结束掠夺时间", 23);
	if(iCurHour < iCanLootBegin ||  iCurHour>= iCanLootEnd )
	{
		pEntity->sendErrorCode(ERROR_MANOR_LOOT_NOT_THETIME);
		return;
	}
	
	//给玩家扣点钱
	int iLootNeedSilver = searchCost(m_hEntity);//pGlobal->getInt("去掠夺花费",2000);
	int iActorHave = pEntity->getProperty(PROP_ACTOR_SILVER, 0);
	if(iActorHave < iLootNeedSilver)
	{
		pEntity->sendErrorCode(ERROR_NEED_SILVER);
		return;
	}
	//去领地找个人
	IArenaFactory* pArenaFact = getComponent<IArenaFactory>(COMPNAME_ArenaFactory, IID_IArenaFactory);
	assert(pArenaFact);
	ServerEngine::ArenaSavePlayer player;
	bool res = pArenaFact->getManorWillLootActor(m_hEntity,  player);
	if(!res) return;
	IModifyDelegate* pModifyDelegate = getComponent<IModifyDelegate>( COMPNAME_ModifyDelegate,  IID_IModifyDelegate);
	assert(pModifyDelegate);
	pModifyDelegate->submitDelegateTask(player.roleKey, WillLootActorCallBack(m_hEntity,false));
}

void  ManorSystem::GMLootFight(ServerEngine::PKRole pkRole) 
{
	gmFlag = true;
	gmRoleKey = pkRole;
	pushManorInfo2Client();
	IModifyDelegate* pModifyDelegate = getComponent<IModifyDelegate>(COMPNAME_ModifyDelegate, IID_IModifyDelegate);
	assert(pModifyDelegate);
	pModifyDelegate->submitDelegateTask(pkRole, WillLootActorCallBack(m_hEntity, true));
}


void ManorSystem::onPublishReward(const GSProto::CSMessage& msg)
{
	
}

void ManorSystem::onWuHunDianBulidAddSpeed(const GSProto::CSMessage& msg)
{
	IGlobalCfg *global = getComponent<IGlobalCfg>(COMPNAME_GlobalCfg, IID_IGlobalCfg);
	assert(global);
	IZoneTime *pZoneTime = getComponent<IZoneTime>(COMPNAME_ZoneTime, IID_IZoneTime);
	assert(pZoneTime);
	ManorFactory * pManorFactory = getComponent<ManorFactory>(COMPNAME_ManorFactory, IID_IManorFactory);
	assert(pManorFactory);

	ServerEngine::ManorWuHunAndItemData& dbData = m_manorSystemData.wuHunDianData;
	if(!dbData.bResIsBuild)
	{
		return;
	}
	//计算话费

	HeroSoulOrItem	HeroSoul;
	pManorFactory->getActorWuHunDianByLevel(HeroSoul, dbData.level);

	int iRemaindSecond = HeroSoul.iLevelUpSecond - (pZoneTime->GetCurSecond() - dbData.beginLevelUpSecond);
	int iNeedGold = iRemaindSecond/60;
	int modGold = iRemaindSecond%60;
	if(modGold > 0)
	{
		iNeedGold += 1;
	}
	
	IScriptEngine *pScript = getComponent<IScriptEngine>(COMPNAME_ScriptEngine, IID_IScriptEngine);
	assert(pScript);
	
	EventArgs args;
	args.context.setInt("entity", m_hEntity);
	args.context.setInt("cost", iNeedGold);

	pScript->runFunction("manorWuHunDianGoldAddLevelUpSpeed",&args, "EventArgs");
}

void ManorSystem::onTieJiangPuBulidAddSpeed(const GSProto::CSMessage& msg)
{
	IGlobalCfg *global = getComponent<IGlobalCfg>(COMPNAME_GlobalCfg, IID_IGlobalCfg);
	assert(global);
	IZoneTime *pZoneTime = getComponent<IZoneTime>(COMPNAME_ZoneTime, IID_IZoneTime);
	assert(pZoneTime);
	ManorFactory * pManorFactory = getComponent<ManorFactory>(COMPNAME_ManorFactory, IID_IManorFactory);
	assert(pManorFactory);

	ServerEngine::ManorWuHunAndItemData& dbData = m_manorSystemData.tieJiangPuData;
	if(!dbData.bResIsBuild)
	{
		return;
	}
	//计算话费

	HeroSoulOrItem	item;
	pManorFactory->getActorTieJiangPuByLevel(item, dbData.level);

	int iRemaindSecond = item.iLevelUpSecond - (pZoneTime->GetCurSecond() - dbData.beginLevelUpSecond);
	int iNeedGold = iRemaindSecond/60;
	int modGold = iRemaindSecond%60;
	if(modGold > 0)
	{
		iNeedGold += 1;
	}
	
	IScriptEngine *pScript = getComponent<IScriptEngine>(COMPNAME_ScriptEngine, IID_IScriptEngine);
	assert(pScript);
	
	EventArgs args;
	args.context.setInt("entity", m_hEntity);
	args.context.setInt("cost", iNeedGold);

	pScript->runFunction("manorTieJiangPuGoldAddLevelUpSpeed",&args, "EventArgs");
}

void ManorSystem::manorGoldAddWuHunDianLevelUpSpeed(int iCost)
{
	IEntity *pEntity = getEntityFromHandle(m_hEntity);
	assert(pEntity);

	int iNeedGold = iCost;
	int iActorHaveGold = pEntity->getProperty(PROP_ACTOR_GOLD,0);
	if(iCost > iActorHaveGold)
	{
		pEntity->sendErrorCode(ERROR_NEED_GOLD);
		return;
	}
	//扣钱
	pEntity->changeProperty(PROP_ACTOR_GOLD,0 -iNeedGold, GSProto::en_Reason_Manor_WuHunDian_LEVELUP_GOLDADDSPEED);
	
	//升级
	m_manorSystemData.wuHunDianData.level++;
	m_manorSystemData.wuHunDianData.bResIsBuild = false;
	m_manorSystemData.wuHunDianData.beginLevelUpSecond = 0;
	
	sendWuHunDian2Client();
}

void ManorSystem::manorGoldAddTieJiangPuLevelUpSpeed(int iCost)
{
	IEntity *pEntity = getEntityFromHandle(m_hEntity);
	assert(pEntity);

	int iNeedGold = iCost;
	int iActorHaveGold = pEntity->getProperty(PROP_ACTOR_GOLD,0);
	if(iCost > iActorHaveGold)
	{
		pEntity->sendErrorCode(ERROR_NEED_GOLD);
		return;
	}
	
	//扣钱
	pEntity->changeProperty(PROP_ACTOR_GOLD,0 -iNeedGold, GSProto::en_Reason_Manor_TieJiangPu_LEVELUP_GOLDADDSPEED);
	
	//升级
	m_manorSystemData.tieJiangPuData.level++;
	m_manorSystemData.tieJiangPuData.bResIsBuild = false;
	m_manorSystemData.tieJiangPuData.beginLevelUpSecond = 0;

	sendTieJiangPu2Client();
}

struct PVPFightDelegate
{
	PVPFightDelegate(ServerEngine::PKRole targetPKRole,HEntity attackHEntity )
		:m_TargetPKRole( targetPKRole),
		m_attackHEntity( attackHEntity)
	{
	}

	void operator()(taf::Int32 iRet, ServerEngine::BattleData& battleInfo)
	{
		if(en_FightResult_OK != iRet)
		{
			return;
		}
		IEntity* pEntity = getEntityFromHandle(m_attackHEntity);
		assert(pEntity);
		ManorSystem* pManorSys = static_cast<ManorSystem*>(pEntity->querySubsystem(IID_IManorSystem));
		assert(pManorSys);
		pManorSys->processLootFight(m_TargetPKRole, battleInfo);

		FDLOG("ManorSystem")<<"PVPFightDelegate|"<<iRet<<endl;
	}
private:
	ServerEngine::PKRole m_TargetPKRole;
	HEntity m_attackHEntity;
};

struct AfterFightGetActorDataDelegate
{
	AfterFightGetActorDataDelegate(HEntity hEntity,const ServerEngine::PKRole& target, const ServerEngine::BattleData& data)
		:m_hEntity(hEntity),
		m_pkRole(target),
		m_battleData(data)
	{
	}

	virtual void operator()(int iRet, HEntity hTarget, ServerEngine::PIRole& roleInfo)
	{
		if(iRet != ServerEngine::en_RoleRet_OK)
		{
			return;
		}
		IEntity *pEntity = getEntityFromHandle(m_hEntity);
		assert(pEntity);
		ManorSystem *pManor = static_cast<ManorSystem*>(pEntity->querySubsystem(IID_IManorSystem));
		assert(pManor);
		if(m_battleData.bAttackerWin)
		{
			pManor->processFightWin(hTarget, roleInfo, m_pkRole, m_battleData);
		}
		else
		{
			pManor->processFightFailed(hTarget, roleInfo, m_pkRole, m_battleData);
		}
	}
private:
	HEntity m_hEntity;
	ServerEngine::PKRole m_pkRole;
	ServerEngine::BattleData m_battleData;
};

void ManorSystem::processFightFailed(HEntity hTarget,
									ServerEngine::PIRole& roleInfo,
									const ServerEngine::PKRole& pkRole, 
									const ServerEngine::BattleData& battleData)
{
	IGlobalCfg* pGlobal = getComponent<IGlobalCfg>(COMPNAME_GlobalCfg, IID_IGlobalCfg);
	assert(pGlobal);

	IEntity *pEntity = getEntityFromHandle(m_hEntity);
	assert(pEntity);
	
	GSProto::ManorLootLog log;
	GSProto::PBPkRole& pbPkRole = *(log.mutable_attacker());
	pbPkRole.set_straccount( pEntity->getProperty(PROP_ACTOR_ACCOUNT,""));
	pbPkRole.set_iworldid(pEntity->getProperty(PROP_ACTOR_WORLD, 0));
	log.set_battackerwin(false);
	log.set_strattackername(pEntity->getProperty(PROP_ENTITY_NAME, ""));
	
	if(0 != hTarget )
	{
		IEntity *pTarget = getEntityFromHandle(hTarget);
		assert(pTarget);
		ManorSystem* pManor = static_cast<ManorSystem*>(pTarget->querySubsystem( IID_IManorSystem));
		assert(pManor);
		pManor ->AddManorLootLog(log);
	}
	else
	{
		ServerEngine::RoleSaveData data;
		ServerEngine::JceToObj( roleInfo.roleData, data);
		map<taf::Int32, std::string>::iterator iter = data.subsystemData.find(IID_IManorSystem);
		assert(iter != data.subsystemData.end());
		ServerEngine::ManorSystemData manorSystemData;
		ServerEngine::JceToObj(iter->second, manorSystemData);
		string logStr;
		log.SerializeToString(&logStr);
		int iLogCount = pGlobal->getInt("新领地日志上限",20);
		manorSystemData.manorLogVec.insert(manorSystemData.manorLogVec.begin(),logStr);
		if(manorSystemData.manorLogVec.size() > (size_t)iLogCount)
		{
			manorSystemData.manorLogVec.resize((size_t)iLogCount);
		}
		//回写数据
		manorSystemData.bIsBattle = false;
		manorSystemData.bHaveNewLog = true;
		iter->second = ServerEngine::JceToStr(manorSystemData);
		roleInfo.roleData = ServerEngine::JceToStr(data);
	}
	
	GSProto::Cmd_Sc_CommFightResult scCommFightResult;
	scCommFightResult.set_iissuccess( false);
	int iSceneID = pGlobal->getInt("新领地掠夺场景", 1);
	scCommFightResult.set_isceneid(iSceneID);
	
	GSProto::SCMessage pkg;
	HelpMakeScMsg(pkg, GSProto::CMD_COMM_FIGHTRESULT, scCommFightResult);

	IFightSystem* pFightSys = static_cast<IFightSystem*>(pEntity->querySubsystem(IID_IFightSystem));
	assert(pFightSys);
	
	pFightSys->sendAllBattleMsg(battleData, pkg);
	
	//scCommFightResult.set_istar(battleInfo.iStar);

	//pEntity->sendMessage(GSProto::CMD_COMM_FIGHTRESULT,scCommFightResult);
}

void ManorSystem::processFightWin(HEntity hTarget,
									ServerEngine::PIRole& roleInfo,
									const ServerEngine::PKRole& pkRole, 
									const ServerEngine::BattleData& battleData)
{
	CloseAttCommUP close(m_hEntity);
	IGlobalCfg* pGlobal = getComponent<IGlobalCfg>(COMPNAME_GlobalCfg, IID_IGlobalCfg);
	assert(pGlobal);
	IJZEntityFactory* pEntityFactory = getComponent<IJZEntityFactory>(COMPNAME_EntityFactory, IID_IJZEntityFactory);
	assert(pEntityFactory);
		
	IEntity *pEntity = getEntityFromHandle(m_hEntity);
	assert(pEntity);

	GSProto::ManorLootLog log;
	GSProto::PBPkRole &pbPkRole = *(log.mutable_attacker());
	string strAcccount = pEntity->getProperty(PROP_ACTOR_ACCOUNT,"");
	pbPkRole.set_straccount( strAcccount);
	pbPkRole.set_iworldid(pEntity->getProperty(PROP_ACTOR_WORLD, 0));
	log.set_battackerwin(true);
	log.set_strattackername(pEntity->getProperty(PROP_ENTITY_NAME, ""));
	
	int iSilver = 0;
	int iHeroExp =0;
	int iItemId = 0;
	int iHeroId = 0;

	HEntity target =  hTarget;
	if(0 == target)
	{
		ServerEngine::ActorCreateContext tmpCreateCtx;
		tmpCreateCtx.strAccount = roleInfo.strAccount;
		ServerEngine::JceToObj(roleInfo.roleData, tmpCreateCtx.data);
		string strTmpData = ServerEngine::JceToStr(tmpCreateCtx);
		IEntity* pTargetGhost = pEntityFactory->getEntityFactory()->createEntity("Ghost", strTmpData);
		target = pTargetGhost->getHandle();
		assert(pTargetGhost);
	}
	
	IEntity* pTarget = getEntityFromHandle(target);
	assert(pTarget);
	ManorSystem* pManor = static_cast<ManorSystem*>(pTarget->querySubsystem( IID_IManorSystem));
	assert(pManor);
	iSilver = pManor->calcSilver(true,m_hEntity, battleData.iStar);
	iHeroExp = pManor->calcHeroExp(true,m_hEntity, battleData.iStar);
	assert(iSilver >= 0);
	assert(iHeroExp >= 0);
	GSProto::FightAwardResult& resoult= *(log.mutable_award());
	GSProto::PropItem &propitem1 = *(resoult.add_szawardproplist());
	propitem1.set_ilifeattid(GSProto::en_LifeAtt_Silver);
	propitem1.set_ivalue( iSilver);
	
	if(iItemId !=-1)
	{
		GSProto::PropItem &propitem2 = *(resoult.add_szawardproplist());
		propitem2.set_ilifeattid(GSProto::en_LifeAtt_HeroExp);
		propitem2.set_ivalue( iHeroExp);
		iItemId = pManor->calcTieJiangPuCanGetItemId();
		GSProto::FightAwardItem& item = *(resoult.add_szawarditemlist());
		item.set_iitemid(iItemId);
		item.set_icount(1);
	} 

	
	iHeroId = pManor->calcWuHunDianCanGetIHeroId();
	int iitemId = -1;
	if(iHeroId != -1)
	{
		ITable* pTable = getCompomentObjectManager()->findTable(TABLENAME_Hero);
		assert(pTable);
		int iRecord = pTable->findRecord(iHeroId);
		assert(iRecord >= 0);
		iitemId = pTable->getInt(iRecord, "英雄魂魄物品ID");
		GSProto::FightAwardItem& item2 = *(resoult.add_szawarditemlist());
		item2.set_iitemid(iitemId);
		item2.set_icount(1);
	}
	
	pManor->AddManorLootLog(log);
	

	//获得的东西加在玩家身上
	pEntity->changeProperty( PROP_ACTOR_SILVER, iSilver, GSProto::en_Reason_Manor_Loot);
	pEntity->changeProperty( PROP_ENTITY_HEROEXP, iHeroExp, GSProto::en_Reason_Manor_Loot);

	string strBeLootAccount = pTarget->getProperty(PROP_ACTOR_ACCOUNT,"");
		
	FDLOG("ManorSystem")<<strBeLootAccount <<"|Be Loot| " << iSilver <<"|" << iHeroExp <<endl;
	FDLOG("ManorSystem")<< strAcccount<<"|Loot| " << iSilver <<"|" << iHeroExp <<endl;

	if(0 == hTarget)
	{
		SaveOneSubsystemData(pTarget, roleInfo, IID_IManorSystem);
		delete pTarget;
	}
	
	IItemSystem *pItemSystem = static_cast<IItemSystem*>(pEntity->querySubsystem(IID_IItemSystem));
	assert(pItemSystem);
	if(iitemId != -1) 
	{	
		pItemSystem->addItem(iitemId, 1,GSProto::en_Reason_Manor_Loot);
	}
	if(iItemId != -1)
	{
		pItemSystem->addItem(iItemId, 1, GSProto::en_Reason_Manor_Loot);
	}
		
	
	//增加收货
	GSProto::Cmd_Sc_CommFightResult scCommFightResult;
	int iSceneID = pGlobal->getInt("新领地掠夺场景", 1);
	scCommFightResult.set_isceneid(iSceneID);
	scCommFightResult.set_iissuccess( true);
	scCommFightResult.set_istar(battleData.iStar);
	GSProto::FightAwardResult& resoult2 = *(scCommFightResult.mutable_awardresult());
	resoult2 = resoult;
	
	GSProto::SCMessage pkg;
	HelpMakeScMsg(pkg, GSProto::CMD_COMM_FIGHTRESULT, scCommFightResult);

	IFightSystem* pFightSys = static_cast<IFightSystem*>(pEntity->querySubsystem(IID_IFightSystem));
	assert(pFightSys);
	pFightSys->sendAllBattleMsg(battleData, pkg);
	
	//pEntity->sendMessage(GSProto::CMD_COMM_FIGHTRESULT,scCommFightResult);
}

void ManorSystem::AddManorLootLog(GSProto::ManorLootLog& log)
{
	IGlobalCfg * pGlobal = getComponent<IGlobalCfg>(COMPNAME_GlobalCfg, IID_IGlobalCfg);
	assert( pGlobal);
	string logStr;
	log.SerializeToString(&logStr);
	int iLogCount = pGlobal->getInt("新领地日志上限",20);
	m_manorSystemData.manorLogVec.insert(m_manorSystemData.manorLogVec.begin(), logStr);
	if(m_manorSystemData.manorLogVec.size() > (size_t)iLogCount)
	{
		m_manorSystemData.manorLogVec.resize((size_t)iLogCount);
	}
	
	m_manorSystemData.bIsBeLoot = true;
	m_manorSystemData.bIsBattle = false;
	IZoneTime *pZoneTime = getComponent<IZoneTime>(COMPNAME_ZoneTime, IID_IZoneTime);
	assert(pZoneTime);
	m_manorSystemData.iLostBeLootSecond = pZoneTime->GetCurSecond();
	//跟新竞技场数据
	IArenaFactory *pArenaFac = getComponent<IArenaFactory>(COMPNAME_ArenaFactory, IID_IArenaFactory);
	assert(pArenaFac);
	pArenaFac->updateActorBeManorLootInfo(m_hEntity);
	//有新日志
	m_manorSystemData.bHaveNewLog = true;
}
	
void ManorSystem::processLootFight(const ServerEngine::PKRole& targetKey, const ServerEngine::BattleData& data)
{
	IEntity* pEntity = getEntityFromHandle(m_hEntity);
	assert(pEntity);
	IFightSystem* pFightSystem = static_cast<IFightSystem*>(pEntity->querySubsystem(IID_IFightSystem));
	assert(pFightSystem);

	//给对方留日志
	IModifyDelegate* pModifyDelegate = getComponent<IModifyDelegate>( COMPNAME_ModifyDelegate,  IID_IModifyDelegate);
	assert(pModifyDelegate);
	pModifyDelegate->submitDelegateTask(targetKey, AfterFightGetActorDataDelegate(m_hEntity,targetKey,data));
	//胜利了计算玩家获得，这里玩家实际得到的跟玩家看到的不同
	//我方获得钱币
	//FDLOG("ManorSystem")<<"processLootFight|"<<endl;
}

void ManorSystem::actorBeforeBattleDataCallBack(HEntity hTarget, ServerEngine::PIRole& roleInfo)
{
	IEntity *pEntity = getEntityFromHandle(m_hEntity);
	assert(pEntity);
	IGlobalCfg * pGlobal = getComponent<IGlobalCfg>(COMPNAME_GlobalCfg, IID_IGlobalCfg);
	assert( pGlobal);
	IZoneTime* pZone = getComponent<IZoneTime>(COMPNAME_ZoneTime, IID_IZoneTime);
	assert(pZone);
	
	ServerEngine::PKRole targetKey;
	if(0 != hTarget)
	{
		IEntity *pTargetEntity = getEntityFromHandle(hTarget);
		assert(pTargetEntity);
		ManorSystem *pManorSys = static_cast<ManorSystem*>(pTargetEntity->querySubsystem( IID_IManorSystem));
		assert(pManorSys);
		bool res = pManorSys->getActorIsInLoot();
		if(res )
		{
			pEntity->sendErrorCode(ERROR_MANOR_LOOT_OTHERBEFORYOU);
			return;
		}
		//修改数据
		pManorSys->actorBeLootChangeData();

		targetKey.strAccount = pTargetEntity->getProperty(PROP_ACTOR_ACCOUNT,""); 
		targetKey.rolePos = 0;
		targetKey.worldID = pTargetEntity->getProperty(PROP_ACTOR_WORLD,0);
		
		//去快乐的战斗吧
	}
	else
	{
		//处理离线数据了
		ServerEngine::RoleSaveData data;
		ServerEngine::JceToObj( roleInfo.roleData, data);
		map<taf::Int32, std::string>::iterator iter = data.subsystemData.find(IID_IManorSystem);
		assert(iter != data.subsystemData.end());
		ServerEngine::ManorSystemData manorSystemData;
		ServerEngine::JceToObj(iter->second, manorSystemData);
		if(manorSystemData.bIsBeLoot)
		{
			//int remaindSecond =  pZone->GetCurSecond() - manorSystemData.iLostBeLootSecond;
			//int iSecond = pGlobal->getInt("领地被掠夺了的保护时间",7200);
			int remaindSecond = getRemaindProtectSecond(manorSystemData);
			if(remaindSecond > 0 )
			{
				pEntity->sendErrorCode(ERROR_MANOR_LOOT_OTHERBEFORYOU);
				return;
			}

		}
		//修改数据
		manorSystemData.bIsBattle = true;
		//回写数据
		iter->second = ServerEngine::JceToStr(manorSystemData);
		roleInfo.roleData = ServerEngine::JceToStr(data);
		//去愉快的战斗吧
		targetKey.strAccount = roleInfo.strAccount;
		targetKey.rolePos = 0;
		targetKey.worldID = roleInfo.worldID;
		
	}
	
	int iNeedVigor = pGlobal->getInt("去掠夺消耗精力", 2);
	int ActorHave = pEntity->getProperty(PROP_ENTITY_ACTOR_VIGOR,0);
	if(ActorHave < iNeedVigor)
	{
		pEntity->sendErrorCode(ERROR_MANOR_LOOT_VIGOR);
		return;
	}
	pEntity->changeProperty(PROP_ENTITY_ACTOR_VIGOR, 0-iNeedVigor, GSProto::en_Reason_Manor_Actor_Loot_Other);
	IFightSystem* pFightSys = static_cast<IFightSystem*>(pEntity->querySubsystem(IID_IFightSystem));
	assert(pFightSys);
	
	ServerEngine::FightContext ctx;
	ctx.iSceneID = pGlobal->getInt("新领地掠夺场景", 1);
	ctx.iFightType = GSProto::en_FightType_ManorSystem;
	
	pFightSys->AsyncPVPFight(PVPFightDelegate(targetKey,m_hEntity), targetKey, ctx);

	//任务
	{
		EventArgs args;
		args.context.setInt("times",1);
		args.context.setInt("entity",m_hEntity);
		pEntity->getEventServer()->setEvent(EVENT_ENTITY_TASKFINISH_LOOT, args);
		
	}


	FDLOG("ManorSystem")<<"actorBeforeBattleDataCallBack|"<<endl;
	
}

bool ManorSystem::getActorIsInLoot()
{
	IGlobalCfg * pGlobal = getComponent<IGlobalCfg>(COMPNAME_GlobalCfg, IID_IGlobalCfg);
	assert( pGlobal);
	IZoneTime *pZone = getComponent<IZoneTime>(COMPNAME_ZoneTime, IID_IZoneTime);
	assert(pZone);

	if(m_manorSystemData.bIsBattle)
	{
		return false;
	}
	
	if(m_manorSystemData.bIsBeLoot)
	{
		//int remaindSecond =  pZone->GetCurSecond() - m_manorSystemData.iLostBeLootSecond;
		//int iSecond = pGlobal->getInt("领地被掠夺了的保护时间",7200);

		int remaindSecond = getRemaindProtectSecond(m_manorSystemData);
		if(remaindSecond > 0 )
		{
			m_manorSystemData.bIsBattle = true;
			return true ;
		}
	}
	return false;
}

void ManorSystem::actorBeLootChangeData()
{
	IZoneTime *pZone = getComponent<IZoneTime>(COMPNAME_ZoneTime, IID_IZoneTime);
	assert(pZone);
	m_manorSystemData.bIsBeLoot = true;
	m_manorSystemData.iLostBeLootSecond = pZone->GetCurSecond();
}
struct ActorDataCallBack
{
	ActorDataCallBack(HEntity hEntity)
		:m_QueryHEntity(hEntity)
	{
	
	}
	
	virtual void operator()(int iRet, HEntity hTarget, ServerEngine::PIRole& roleInfo)
	{
		if( iRet != ServerEngine::en_RoleRet_OK)
		{
			return;
		}

		IEntity *pEntity = getEntityFromHandle( m_QueryHEntity);
		if(!pEntity)
		{
			return;
		}
		
		ManorSystem* pManor = static_cast<ManorSystem*>(pEntity->querySubsystem(IID_IManorSystem));
		assert(pManor);
		pManor->actorBeforeBattleDataCallBack(hTarget, roleInfo);
	}
	
private:
	HEntity m_QueryHEntity;
	bool m_attackWin;
};

void ManorSystem::onRequestLootBattle(const GSProto::CSMessage& msg)
{
	GSProto::CMD_MANOR_REQUEST_LOOT_BATTLE_CS csMsg;
	if( !csMsg.ParseFromString( msg.strmsgbody()) )
	{
		return;
	}
	
	const GSProto::PBPkRole& pbPkRole = csMsg.enemy();
	IEntity * pEntity = getEntityFromHandle(m_hEntity);
	assert(pEntity);
	IGlobalCfg * pGlobal = getComponent<IGlobalCfg>(COMPNAME_GlobalCfg, IID_IGlobalCfg);
	assert( pGlobal);
	int iNeedVigor = pGlobal->getInt("去掠夺消耗精力", 2);
	int ActorHave = pEntity->getProperty(PROP_ENTITY_ACTOR_VIGOR,0);
	if(ActorHave < iNeedVigor)
	{
		pEntity->sendErrorCode(ERROR_MANOR_LOOT_VIGOR);
		return;
	}
	ServerEngine::PKRole targetKey;
	targetKey.strAccount = pbPkRole.straccount();
	targetKey.rolePos = 0;
	targetKey.worldID = pbPkRole.iworldid();
	//检查一下这个玩家是不是已经也被打了
	
	IModifyDelegate* pModifyDelegate = getComponent<IModifyDelegate>( COMPNAME_ModifyDelegate,  IID_IModifyDelegate);
	assert(pModifyDelegate);
	pModifyDelegate->submitDelegateTask(targetKey, ActorDataCallBack(m_hEntity));
}

void ManorSystem::resetHarvest()
{
	IZoneTime *pZoneTime = getComponent<IZoneTime>(COMPNAME_ZoneTime, IID_IZoneTime);
	assert(pZoneTime);
	
	m_manorSystemData.iResHarvestTimes = 0;
	m_manorSystemData.dwHarvestSecond = pZoneTime->GetCurSecond();
	m_manorSystemData.wuHunDianData.iHaveRefreshTimes = 0;
	m_manorSystemData.tieJiangPuData.iHaveRefreshTimes = 0;
}

void ManorSystem::checkReset()
{
	IZoneTime *pZoneTime = getComponent<IZoneTime>(COMPNAME_ZoneTime, IID_IZoneTime);
	assert(pZoneTime);
	if(!pZoneTime->IsInSameDay(m_manorSystemData.dwHarvestSecond, pZoneTime->GetCurSecond()))
	{
		m_manorSystemData.iResHarvestTimes = 0;
		m_manorSystemData.dwHarvestSecond = pZoneTime->GetCurSecond();
		m_manorSystemData.wuHunDianData.iHaveRefreshTimes = 0;
		m_manorSystemData.tieJiangPuData.iHaveRefreshTimes = 0;
	}
}

void ManorSystem::getActorSilverRes(vector<ServerEngine::ManorResData>& silverRes)
{
	silverRes = m_manorSystemData.silverResVec;
}

void ManorSystem::getActorHeroExpRes(vector<ServerEngine::ManorResData>& heroExpRes)
{
	heroExpRes = m_manorSystemData.heroExpResVec;
}

void ManorSystem::packSaveData(string& data)
{
	data = ServerEngine::JceToStr(m_manorSystemData);
}

void ManorSystem::manorNoCanLootActorReturn()
{
	//8~23才能掠夺todo
	IEntity *pEntity = getEntityFromHandle(m_hEntity);
	assert(pEntity);
	IGlobalCfg *pGlobal = getComponent<IGlobalCfg>(COMPNAME_GlobalCfg, IID_IGlobalCfg);
	assert(pGlobal);
	IZoneTime *pZoneTime = getComponent<IZoneTime>(COMPNAME_ZoneTime, IID_IZoneTime);
	assert(pZoneTime);
	int iCurSecond = pZoneTime->GetCurSecond();
	int iCurHour = pZoneTime->GetHourInDay(iCurSecond);
	int iCanLootBegin = pGlobal->getInt("每天开始掠夺时间", 8);
	int iCanLootEnd = pGlobal->getInt("每天结束掠夺时间", 23);
	if(iCurHour < iCanLootBegin ||  iCurHour>iCanLootEnd )
	{
		pEntity->sendErrorCode(ERROR_MANOR_LOOT_NOT_THETIME);
		return;
	}
	//去领地找个人
	IArenaFactory* pArenaFact = getComponent<IArenaFactory>(COMPNAME_ArenaFactory, IID_IArenaFactory);
	assert(pArenaFact);
	ServerEngine::ArenaSavePlayer player;
	bool res = pArenaFact->getManorWillLootActor(m_hEntity,  player);
	if(!res)
	{
		return;
	}
	IModifyDelegate* pModifyDelegate = getComponent<IModifyDelegate>( COMPNAME_ModifyDelegate,  IID_IModifyDelegate);
	assert(pModifyDelegate);
	pModifyDelegate->submitDelegateTask(player.roleKey, WillLootActorCallBack(m_hEntity, false));
}

int ManorSystem::getTieJiangPuLevel() 
{
	return m_manorSystemData.tieJiangPuData.level;
}

int ManorSystem::getWuHunDianLevel()
{
	return m_manorSystemData.wuHunDianData.level;
}


