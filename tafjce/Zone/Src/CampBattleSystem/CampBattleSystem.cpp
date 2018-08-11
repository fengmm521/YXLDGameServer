#include "CampBattleSystemPch.h"
#include "CampBattleSystem.h"

extern "C" IObject* createCampBattleSystem()
{
	return new CampBattleSystem;
}

CampBattleSystem::CampBattleSystem():
	m_hEntity(0)
{
}

CampBattleSystem::~CampBattleSystem()
{
	IEntity* pEntity  = getEntityFromHandle(m_hEntity);
	if(pEntity)
	{
		pEntity->getEventServer()->unsubscribeEvent(EVENT_ENTITY_PRE_RELEASE, this, &CampBattleSystem::saveCampBattle);
		pEntity->getEventServer()->unsubscribeEvent(EVENT_ENTITY_BEGIN_FIGHT,this,&CampBattleSystem::beginCampBattle);
	}
}

Uint32 CampBattleSystem::getSubsystemID() const
{
	return IID_ICampBattleSystem;
}

Uint32 CampBattleSystem::getMasterHandle()
{
	return m_hEntity;
}

bool CampBattleSystem::create(IEntity* pEntity, const std::string& strData)
{
	m_hEntity = pEntity->getHandle();	
	
	if(pEntity)
	{
		pEntity->getEventServer()->subscribeEvent(EVENT_ENTITY_PRE_RELEASE, this, &CampBattleSystem::saveCampBattle);

		pEntity->getEventServer()->subscribeEvent(EVENT_ENTITY_BEGIN_FIGHT,this,&CampBattleSystem::beginCampBattle);
	}
	
	return true;
}

void CampBattleSystem::beginCampBattle(EventArgs& args)
{
	const ServerEngine::FightContext* pFightCtx = (const ServerEngine::FightContext*)args.context.getInt64("clientParam", 0);
	assert(pFightCtx);

	if( pFightCtx->iFightType !=  GSProto::en_FightType_CampBattleFactory)
	{
		return;
	}
	

	ServerEngine::CampBattleFightCtx campBattleCtx;
	assert(pFightCtx->context.find("CampBattleCtx") != pFightCtx->context.end() );
	ServerEngine::JceToObj(pFightCtx->context.at("CampBattleCtx"), campBattleCtx);

	const vector<HEntity>* pMemberList = (const vector<HEntity>*)args.context.getInt64("memberlist", 0);
	assert(pMemberList);

	for(size_t i = 0; i < pMemberList->size(); i++)
	{
		HEntity hTmp = (*pMemberList)[i];
		IEntity* pTmp = getEntityFromHandle(hTmp);
		if(!pTmp) continue;

		if(i < GSProto::MAX_BATTLE_MEMBER_SIZE/2)
		{
			if(campBattleCtx.attCtx.bFixHP && (campBattleCtx.attCtx.fixHPMap.find(i) != campBattleCtx.attCtx.fixHPMap.end() ) )
			{
				int iHP = campBattleCtx.attCtx.fixHPMap.at(i);
				pTmp->setProperty(PROP_ENTITY_HP, iHP);
				FDLOG("CampBattleFactory")<<"|attCtx actor hp ======  ||"<< iHP <<"|"<<endl;
				pTmp->changeProperty( PROP_ENTITY_ATTAPERCENT,campBattleCtx.attCtx.attAddPercent, 0);
				pTmp->changeProperty( PROP_ENTITY_ARMORAVALUE,campBattleCtx.attCtx.werckAddPercent, 0);
			}
		}
		else
		{
			int tagertIndex = i - GSProto::MAX_BATTLE_MEMBER_SIZE/2 ;
			if(campBattleCtx.targetCtx.bFixHP && (campBattleCtx.targetCtx.fixHPMap.find(tagertIndex) != campBattleCtx.targetCtx.fixHPMap.end() ) )
			{
				int iHP = campBattleCtx.targetCtx.fixHPMap.at(tagertIndex );
				pTmp->setProperty(PROP_ENTITY_HP, iHP);
				FDLOG("CampBattleFactory")<<"|targetCtx actor hp ======  |"<< iHP <<"|"<<endl;
				pTmp->changeProperty( PROP_ENTITY_ATTAPERCENT,campBattleCtx.targetCtx.attAddPercent, 0);
				pTmp->changeProperty( PROP_ENTITY_ARMORAVALUE,campBattleCtx.targetCtx.werckAddPercent, 0);
			}


		}
	}
}

void CampBattleSystem::saveCampBattle(EventArgs& args)
{
	ICampBattleFactory* factory = getComponent<ICampBattleFactory>(COMPNAME_CampBattleFactory, IID_ICampBattleFactory);
	assert(factory);
	factory->removeBattleUnit(m_hEntity);
}

bool CampBattleSystem::createComplete()
{
	return true;
}

const std::vector<Uint32>& CampBattleSystem::getSupportMessage()
{
	static std::vector<Uint32> sMsgList;
	if(sMsgList.size() == 0)
	{
		sMsgList.push_back( GSProto::CMD_CAMPBATTLE_OPEN );
		sMsgList.push_back( GSProto::CMD_CAMPBATTLE_JOIN );
		sMsgList.push_back( GSProto::CMD_CAMPBATTLE_CLOSE );
		sMsgList.push_back( GSProto::CMD_CAMPBATTLE_JOIN_CANCLE);
	}
	return sMsgList;
}

void CampBattleSystem::onMessage(QxMessage* pMessage)
{
	assert(pMessage->dwMsgLen == sizeof(GSProto::CSMessage));
	
	bool res = checkOpen();
	if(!res)
	{
		IEntity *pEntity = getEntityFromHandle(m_hEntity);
		assert(pEntity);

		pEntity->sendErrorCode(ERROR_CAMP_NOOPEN);
		return ;
	}
	
	const GSProto::CSMessage& msg = *( GSProto::CSMessage*)(pMessage->pMsgDataBuff);
	switch( msg.icmd())
	{
		case GSProto::CMD_CAMPBATTLE_OPEN:
			{
				onOpenCampBattle(msg);
			}break;
		
		case GSProto::CMD_CAMPBATTLE_JOIN:
			{
				onJoinCampBattle(msg);
			}break;
		
		case GSProto::CMD_CAMPBATTLE_CLOSE:
			{
				onCloseCampBattle(msg);
			}break;
		case GSProto::CMD_CAMPBATTLE_JOIN_CANCLE:
			{
				onJoinCancel(msg);
			}break;
	}
}

bool CampBattleSystem::checkOpen()
{
	IGlobalCfg* pGlobalconf = getComponent<IGlobalCfg>(COMPNAME_GlobalCfg,IID_IGlobalCfg);
	assert(pGlobalconf);
	int openLevel  = pGlobalconf->getInt("阵营战开启等级",20);
	
	IEntity *pEntity = getEntityFromHandle(m_hEntity);
	assert(pEntity);
	int actorLevel = pEntity->getProperty(PROP_ENTITY_LEVEL,0);

	return actorLevel>= openLevel;
	
}


void CampBattleSystem::onJoinCancel(const GSProto::CSMessage& msg)
{
	ICampBattleFactory* factory = getComponent<ICampBattleFactory>(COMPNAME_CampBattleFactory, IID_ICampBattleFactory);
	assert(factory);
	bool res = factory->removeBattleUnit( m_hEntity );

	if(res)
	{
		IEntity *pEntity = getEntityFromHandle(m_hEntity);

		pEntity->sendMessage(GSProto::CMD_CAMPBATTLE_JOIN_CANCLE);
	}
	
}

void CampBattleSystem::packSaveData(string& data)
{

}

void CampBattleSystem::fillActorInfo( GSProto::CampBattlePlayerDetail& detail)
{
	ICampBattleFactory* factory = getComponent<ICampBattleFactory>(COMPNAME_CampBattleFactory, IID_ICampBattleFactory);
	assert(factory);

	BattleUnit unit;
	factory->getActorBattlUnit(unit,getEntityFromHandle(m_hEntity));
	
	detail.set_itopdoublehittimes(unit.iTopSuccession);
	detail.set_icurdoublehittimes( unit.iSuccessionTimes);
	detail.set_isumwintimes(unit.iWinTimes);
	detail.set_isumfailedtimes(	unit.iFailedTimes);
	detail.set_isumhonor(unit.iHonor);
	detail.set_isumsilvercount( unit.iSilver);
}

void CampBattleSystem::fillTimeInfo( GSProto::RemaindTime& remainTime)
{
	ICampBattleFactory* factory = getComponent<ICampBattleFactory>(COMPNAME_CampBattleFactory, IID_ICampBattleFactory);
	assert(factory);
	CampBattleFactoryState state = en_CampBattle_BeforeStart;
	int second = 0;
	factory->getRemaindTimeAndState(state,second);

	if(state == en_CampBattle_StartSignUp)
	{
		remainTime.set_bisstarted(false);
	}
	else
	{
		remainTime.set_bisstarted(true);
	}
	
	remainTime.set_iremaindsecond(second);
}

void CampBattleSystem::onOpenCampBattle(const GSProto::CSMessage& msg)
{
	ICampBattleFactory* factory = getComponent<ICampBattleFactory>(COMPNAME_CampBattleFactory, IID_ICampBattleFactory);
	assert(factory);
	
	//未到开启时间发送错误码
	bool bIsOpen = factory->IsStartCampBattle();
	if(!bIsOpen)
	{
		IEntity* pEntity = getEntityFromHandle(m_hEntity);
		assert(pEntity);

		pEntity->sendErrorCode(ERROR_CAMPBATTLE_NOTACTIVE);
		return;
	}
	
	//已经开启，下行阵营战详情
	GSProto::CMD_CAMPBATTLE_OPEN_SC scMsg;
	
	//左边 列表
	std::vector<BattleUnit> rankListA = factory->getCampARankList();
	for(size_t i = 0; i<rankListA.size();++i)
	{
		const BattleUnit& unit = rankListA[i];
		
		GSProto::RankNodeInfo* nodeInfo = scMsg.add_szleftranklist();
		nodeInfo->set_imainheroid( unit.iMainHeroId);
		nodeInfo->set_iactorhonor( unit.iHonor);
		nodeInfo->set_stractorname( unit.sActorName);
	}

	//右边 列表
	std::vector<BattleUnit> rankListB = factory->getCampBRankList();
	for(size_t i = 0; i<rankListB.size();++i)
	{
		const BattleUnit&  unit = rankListB[i];
		
		GSProto::RankNodeInfo* nodeInfo = scMsg.add_szrightranklist();
		nodeInfo->set_imainheroid( unit.iMainHeroId);
		nodeInfo->set_iactorhonor( unit.iHonor);
		nodeInfo->set_stractorname( unit.sActorName);
	}

	//玩家信息
	fillActorInfo(*scMsg.mutable_playerdetail());
	//时间信息
	fillTimeInfo(*scMsg.mutable_remaindtime());

	GSProto::SCMessage outPkg;
	HelpMakeScMsg( outPkg,GSProto::CMD_CAMPBATTLE_OPEN,scMsg);
	
	IEntity* pEntity = getEntityFromHandle(m_hEntity);
	assert(pEntity);

	pEntity->sendMessage(outPkg);

	//add broadCast
	string strName = pEntity->getProperty(PROP_ENTITY_NAME,"");
	factory->addBroadCastUnit(strName);
	
	//参战不能换队形
	IFormationSystem* formationSystem = static_cast<IFormationSystem*>(pEntity->querySubsystem(IID_IFormationSystem));
	assert(formationSystem);
	formationSystem->setChgFormationSwitchState(true);
	
}

void CampBattleSystem::onJoinCampBattle(const GSProto::CSMessage& msg)
{
	ICampBattleFactory* factory = getComponent<ICampBattleFactory>(COMPNAME_CampBattleFactory, IID_ICampBattleFactory);
	assert(factory);
	IEntity *pEntity = getEntityFromHandle(m_hEntity);
	
	bool resoult = factory->SignUp(m_hEntity);
	
	if(resoult)
	{
		pEntity->sendMessage(GSProto::CMD_CAMPBATTLE_JOIN);
	}
}

void CampBattleSystem::onCloseCampBattle(const GSProto::CSMessage& msg)
{
	ICampBattleFactory* factory = getComponent<ICampBattleFactory>(COMPNAME_CampBattleFactory, IID_ICampBattleFactory);
	assert(factory);

	IEntity* pEntity = getEntityFromHandle(m_hEntity);
	assert(pEntity);
	
	bool resoult = factory->removeBattleUnit( m_hEntity );
	if(!resoult)
	{
		pEntity->sendErrorCode(ERROR_CAMPBATTLE_SIGNOUT);
		return;
	}

	//add broadCast
	string strName = pEntity->getProperty(PROP_ENTITY_NAME,"");
	factory->removeBroadCastUnit(strName);
	
	pEntity->sendMessage(GSProto::CMD_CAMPBATTLE_CLOSE);
	//打开队形切换
	IFormationSystem* formationSystem = static_cast<IFormationSystem*>(pEntity->querySubsystem(IID_IFormationSystem));
	assert(formationSystem);
	formationSystem->setChgFormationSwitchState(false);
}



