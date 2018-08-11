#include "DreamLandSystemPch.h"
#include "DreamLandSystem.h"


extern "C" IObject* createDreamLandSystem()
{
	return new DreamLandSystem;
}
extern int LifeAtt2Prop(int iLifeAtt);

DreamLandSystem::DreamLandSystem():m_FightFlag(false)
{
	m_FormationList.resize(GSProto::MAX_BATTLE_MEMBER_SIZE/2, 0);
	m_guardNamesVec.clear();
}

DreamLandSystem::~DreamLandSystem()
{
	IEntity* pEntity  = getEntityFromHandle(m_hEntity);
	if(pEntity)
	{
		pEntity->getEventServer()->unsubscribeEvent(EVENT_ENTITY_BEGIN_FIGHT,this,&DreamLandSystem::beginDreamLandSysBattle);
	}
}

Uint32 DreamLandSystem::getSubsystemID() const
{
	return IID_IDreamLandSystem;
}

Uint32 DreamLandSystem::getMasterHandle()
{
	return m_hEntity;
}

void DreamLandSystem::CMClearData()
{
	m_DreamLandData.actorHpMap.clear();
	m_DreamLandData.FormationMap.clear();
	m_DreamLandData.iHaveUseResetTime = 0;
	m_DreamLandData.sectionProcessVec.clear(); 
	m_guardNamesVec.clear();
	m_DreamLandData.unLockSectionId = -1;
	m_DreamLandData.iItemCount = 0;
	m_DreamLandData.sectionList.clear();
	m_FormationList.clear();
	m_DreamLandData.ihaveUseTimes = 0;

	//填充数据
	IDreamLandFactory* pDreamLandFactory = getComponent<IDreamLandFactory>(COMPNAME_DreamLandFactory,IID_IDreamLandFactory);
	assert(pDreamLandFactory);
	
	DreamLandSection firstSection;
	bool res = pDreamLandFactory->getFristSection(firstSection);
	assert(res);
	//开放的章节
	m_DreamLandData.sectionList.push_back( firstSection.iSectionId );

	//存入第一个关卡记录
	ServerEngine::DreamLandSceneRecord record;
	record.iSceneId = firstSection.sceneIdVec[0];
	record.sceneState = en_State_OPen;
	record.iHaveGetBox = 0;
	record.targethpMap.clear();
	m_DreamLandData.sectionProcessVec.push_back(record);

	//药瓶初始化个数
	IGlobalCfg* pGlobalCfg = getComponent<IGlobalCfg>(COMPNAME_GlobalCfg, IID_IGlobalCfg);
	assert(pGlobalCfg);

	int iInitItemCount = pGlobalCfg->getInt("战斗初始药物数量", 1);
	assert(iInitItemCount != -1);

	m_DreamLandData.iItemCount = iInitItemCount;

	m_DreamLandData.iHaveUseResetTime = 0;
	
	//未锁定关卡
 	m_DreamLandData.unLockSectionId = -1;
	
}

bool DreamLandSystem::create(IEntity* pEntity, const std::string& strData)
{
	m_hEntity = pEntity->getHandle();	
	pEntity->getEventServer()->subscribeEvent(EVENT_ENTITY_BEGIN_FIGHT,this,&DreamLandSystem::beginDreamLandSysBattle);
	
	if(strData.size() > 0)
	{
		ServerEngine::JceToObj(strData,m_DreamLandData);
		checkReset();
		//加载阵型数据
		loadHeroFormation();

		if( m_DreamLandData.unLockSectionId != -1)
		{
			map<int, vector<ServerEngine::ArenaSavePlayer> >::iterator iter = m_DreamLandData.sectionGuardPlayerMap.find(m_DreamLandData.unLockSectionId);
			if(iter !=  m_DreamLandData.sectionGuardPlayerMap.end())
			{
				m_guardNamesVec = iter->second;
			}
		}
		return true;
	}

	//填充数据
	IDreamLandFactory* pDreamLandFactory = getComponent<IDreamLandFactory>(COMPNAME_DreamLandFactory,IID_IDreamLandFactory);
	assert(pDreamLandFactory);
	
	DreamLandSection firstSection;
	bool res = pDreamLandFactory->getFristSection(firstSection);
	assert(res);
	//开放的章节
	m_DreamLandData.sectionList.push_back( firstSection.iSectionId );

	//存入第一个关卡记录
	ServerEngine::DreamLandSceneRecord record;
	record.iSceneId = firstSection.sceneIdVec[0];
	record.sceneState = en_State_OPen;
	record.iHaveGetBox = 0;
	record.targethpMap.clear();
	m_DreamLandData.sectionProcessVec.push_back(record);
	
	//药瓶初始化个数
	IGlobalCfg* pGlobalCfg = getComponent<IGlobalCfg>(COMPNAME_GlobalCfg, IID_IGlobalCfg);
	assert(pGlobalCfg);

	int iInitItemCount = pGlobalCfg->getInt("战斗初始药物数量", 1);
	assert(iInitItemCount != -1);

	m_DreamLandData.iItemCount = iInitItemCount;

	m_DreamLandData.iHaveUseResetTime = 1;

	IZoneTime* pZomeTime = getComponent<IZoneTime>(COMPNAME_ZoneTime, IID_IZoneTime);
	assert(pZomeTime);
	m_DreamLandData.dwTimeChg = pZomeTime->GetCurSecond();
	
	//未锁定关卡
	m_DreamLandData.unLockSectionId = -1;
	
	return true;
}
void DreamLandSystem::checkReset()
{
	
	IZoneTime* pZomeTime = getComponent<IZoneTime>(COMPNAME_ZoneTime, IID_IZoneTime);
	assert(pZomeTime);
	
	if(!pZomeTime->IsInSameDay(m_DreamLandData.dwTimeChg, pZomeTime->GetCurSecond() ) )
	{
		m_DreamLandData.iHaveUseResetTime = 0;
		m_DreamLandData.dwTimeChg = pZomeTime->GetCurSecond();
		//m_DreamLandData.deadHeroList.clear();
	}

}

void DreamLandSystem::beginDreamLandSysBattle(EventArgs& args)
{
	const ServerEngine::FightContext* pFightCtx = (const ServerEngine::FightContext*)args.context.getInt64("clientParam", 0);
	assert(pFightCtx);

	if( pFightCtx->iFightType !=  GSProto::en_FightType_DreamLand)
	{
		return;
	}

	HEntity hEntity = (HEntity)args.context.getInt64("attHEnttiy", 0);
	if(hEntity != m_hEntity) return;
	
	
	ServerEngine::DreamLandFightCtx dreamLandCtx;
	assert(pFightCtx->context.find("DreamLandFightCtx") != pFightCtx->context.end() );
	ServerEngine::JceToObj(pFightCtx->context.at("DreamLandFightCtx"), dreamLandCtx);

	const vector<HEntity>* pMemberList = (const vector<HEntity>*)args.context.getInt64("memberlist", 0);
	assert(pMemberList);
	

	for(size_t i = 0; i < pMemberList->size(); i++)
	{
		HEntity hTmp = (*pMemberList)[i];
		IEntity* pTmp = getEntityFromHandle(hTmp);
		if(!pTmp) continue;

		if(i < GSProto::MAX_BATTLE_MEMBER_SIZE/2)
		{
			if(dreamLandCtx.attCtx.bFixHP && (dreamLandCtx.attCtx.fixHPMap.find(i) != dreamLandCtx.attCtx.fixHPMap.end() ) )
			{
				int iHp = pTmp->getProperty(PROP_ENTITY_HP,0);
				int iPercent = dreamLandCtx.attCtx.fixHPMap.at(i);
				iHp = iHp* ((double)iPercent/10000.0);
				pTmp->setProperty(PROP_ENTITY_HP, iHp);
				
			}
		}
		else
		{
			int tagertIndex = i - GSProto::MAX_BATTLE_MEMBER_SIZE/2 ;
			int iClassID = pTmp->getProperty(PROP_ENTITY_CLASS, 0);
			map<int, int>::iterator iter = dreamLandCtx.properyMap.begin();
			for( ; iter != dreamLandCtx.properyMap.end(); ++iter)
			{
				int prop = LifeAtt2Prop(iter->first);
		        	assert(prop != -1);

				if( prop == PROP_ENTITY_HP)
				{
					if(iClassID != GSProto::en_class_Monster)
					{
						int propCount = pTmp->getProperty(PROP_ENTITY_MAXHPAPERCENT,0);
						propCount += iter->second;
						pTmp->setProperty(PROP_ENTITY_MAXHPAPERCENT,propCount);
					}
					else
					{
						int iMaxHp = dreamLandCtx.iMonsterHp*(1+ iter->second /10000.0);
						pTmp->setProperty(PROP_ENTITY_MAXHP,iMaxHp);
					}
					
				}
				else if( prop == PROP_ENTITY_ATT)
				{
					if(iClassID != GSProto::en_class_Monster)
					{
						int propCount = pTmp->getProperty(PROP_ENTITY_ATTAPERCENT,0);
						propCount += iter->second;
						pTmp->setProperty(PROP_ENTITY_ATTAPERCENT,propCount);
					}
					else
					{
							int	iMaxAtt =  dreamLandCtx.iMonsterAttValue*(1+ iter->second /10000.0);
							pTmp->setProperty(PROP_ENTITY_ATT,iMaxAtt);
					}
				}
				else
				{
					assert(0);
				}
			}

					
			if(dreamLandCtx.targetCtx.bFixHP && (dreamLandCtx.targetCtx.fixHPMap.find(tagertIndex) != dreamLandCtx.targetCtx.fixHPMap.end() ) )
			{
				float iHPPercent = dreamLandCtx.targetCtx.fixHPMap.at(tagertIndex )/10000.0;
				int iHp = pTmp->getProperty(PROP_ENTITY_HP,0);
				iHp =  iHp*iHPPercent;
				pTmp->setProperty(PROP_ENTITY_HP, iHp);
			}
		}
	}
}

void DreamLandSystem::loadHeroFormation()
{
	m_FormationList.clear();
	m_FormationList.resize(GSProto::MAX_BATTLE_MEMBER_SIZE/2, 0);
	IEntity *pEntity = getEntityFromHandle(m_hEntity);
	assert(pEntity);
	IHeroSystem* pHeroSystem = static_cast<IHeroSystem*>(pEntity->querySubsystem(IID_IHeroSystem));
	assert(pHeroSystem);

	map<int, string>::iterator iter = m_DreamLandData.FormationMap.begin();
	for( ; iter != m_DreamLandData.FormationMap.end(); ++iter)
	{
		string str = iter->second;
		HEntity hTmpHero = pHeroSystem->getHeroFromUUID( str );
		if(0 != hTmpHero)
		{
			m_FormationList[iter->first] = hTmpHero;
		}
	}
}

bool DreamLandSystem::createComplete()
{
	return true;
}

const std::vector<Uint32>& DreamLandSystem::getSupportMessage()
{
	static std::vector<Uint32> msgList;

	if( msgList.size() == 0)
	{
		msgList.push_back( GSProto::CMD_DREAMLAND_OPEN_REQSECTON );
		msgList.push_back( GSProto::CMD_DREAMLAND_RESET );
		msgList.push_back( GSProto::CMD_DREAMLAND_QUERY_SCENEINFO );
		msgList.push_back( GSProto::CMD_DREAMLAND_QUERY_SCENEFORMATION );
		msgList.push_back( GSProto::CMD_DREAMLAND_REQUEST_OPENBOX );
		msgList.push_back( GSProto::CMD_DREAMLAND_GETACTOR_FORMATION_INFO );
		msgList.push_back( GSProto::CMD_DREAMLAND_FORMATION_CHG);
		msgList.push_back( GSProto::CMD_DREAMLAND_REQUEST_FIGHT);
		msgList.push_back( GSProto::CMD_DREAMLAND_QUERY_BOX );
		/*msgList.push_back( GSProto::CMD_DREAMLAND_QUERY_HAVEWINSCENE);*/
	}

	return msgList;
}

void DreamLandSystem::onMessage(QxMessage* pMessage)
{
	assert(sizeof(GSProto::CSMessage) == pMessage->dwMsgLen);
	checkReset();
	const GSProto::CSMessage &msg = *(const GSProto::CSMessage*)(pMessage->pMsgDataBuff);
	switch( msg.icmd())
	{
		case GSProto::CMD_DREAMLAND_OPEN_REQSECTON:
			{
				onReqOpenReqSectionInfo(msg);
			}break;
			
		case GSProto::CMD_DREAMLAND_RESET:
			{
				onReqResetDreamLand(msg);
			}break;
		
		case GSProto::CMD_DREAMLAND_QUERY_SCENEINFO:
			{
				onQuerySectionSceneInfo(msg);
			}break;
		
		case GSProto::CMD_DREAMLAND_QUERY_SCENEFORMATION:
			{
				onQuerySceneFormation(msg);
			}break;
		
		case GSProto::CMD_DREAMLAND_REQUEST_OPENBOX:
			{
				onRequstOPenBox(msg);
			}break;
		
		case GSProto::CMD_DREAMLAND_GETACTOR_FORMATION_INFO:
			{
				onGetActorFormation(msg);
			}break;
		
		case GSProto::CMD_DREAMLAND_FORMATION_CHG:
			{
				onChgActorFormation(msg);
			}break;
		
		case GSProto::CMD_DREAMLAND_REQUEST_FIGHT:
			{
				onRequstFight(msg);
			}break;

		case GSProto::CMD_DREAMLAND_QUERY_BOX:
			{
				onQueryBox(msg);
			}break;
		/*
		case GSProto::CMD_DREAMLAND_QUERY_HAVEWINSCENE:
			{
				onRequestHaveWinSceneInfo(msg);
			}break;*/
	}
}

/*
void DreamLandSystem::onRequestHaveWinSceneInfo(const GSProto::CSMessage& msg)
{
	GSProto::CMD_DREAMLAND_QUERY_HAVEWINSCENE_CS c2sMsg;	
	if( !c2sMsg.ParseFromString(msg.strmsgbody()))
	{
		return;
	}
	int iSectionId = c2sMsg.isectionid();
	int  iSceneId = c2sMsg.isceneid();

	if(iSectionId != m_DreamLandData.unLockSectionId || m_DreamLandData.unLockSectionId == -1)
	{
		return;
	}

	int  index = getSceneRecord(iSceneId);
	if( -1 == index)
	{
		return;
	}
	
	ServerEngine::DreamLandSceneRecord& record = m_DreamLandData.sectionProcessVec[index];

	if( record.sceneState != en_State_Finish )
	{
		return;
	}


	GSProto::CMD_DREAMLAND_QUERY_HAVEWINSCENE_SC s2cMsg;

	ServerEngine::DreamLandGuardData &data = record.Guarddata;
	
	s2cMsg.set_stractorname( data.actorName);

	for(size_t i =0; i < data.formation.size(); ++i)
	{
		ServerEngine::DreamLandGuardFormation &forma = data.formation[i];

		GSProto::DreamLandFormationItem &item = *(s2cMsg.add_szformationitem());

		item.set_ibaseid( forma.iBaseId);
		item.set_ipos( forma.iPos );
		item.set_ilevel( forma.iLevel);
		item.set_ilevelstep( forma.iStepLevel);
		item.set_ihp(0);
		item.set_imaxhp(0);
		item.set_bisgodanimal(forma.iIsGodAniaml == 1 );
	}
	
	IEntity* pEntity = getEntityFromHandle(m_hEntity);
	
	pEntity->sendMessage(GSProto::CMD_DREAMLAND_QUERY_HAVEWINSCENE, s2cMsg);
	
}*/

void DreamLandSystem::onReqOpenReqSectionInfo(const GSProto::CSMessage& msg)
{
	sendSectionInfo();
}

void DreamLandSystem::GMAddDreamLandResetTimes()
{
	m_DreamLandData.iHaveUseResetTime = 0;
}


void DreamLandSystem::sendSectionInfo()
{
	GSProto::CMD_DREAMLAND_OPEN_REQSECTON_SC c2sMsg;
	
	c2sMsg.set_iopencount(m_DreamLandData.sectionList.size());
	int iunLock = m_DreamLandData.unLockSectionId;
	
	c2sMsg.set_unlocksectionid( iunLock );

	//可重置次数
	IGlobalCfg* pGlobalcfg = getComponent<IGlobalCfg>(COMPNAME_GlobalCfg,IID_IGlobalCfg);
	assert(pGlobalcfg);

	IVIPFactory* factory = getComponent<IVIPFactory>(COMPNAME_VIPFactory,IID_IVIPFactory);
	assert(factory);
	int configTimes = factory->getVipPropByHEntity(m_hEntity, VIP_PROP_DREAMLAND_RESET);
	int iRemainTimes = configTimes - m_DreamLandData.iHaveUseResetTime;
	assert(iRemainTimes >= 0);
	c2sMsg.set_iresettimes(iRemainTimes);

	//下发
	IEntity *pEntity = getEntityFromHandle(m_hEntity);
	assert(pEntity);
	pEntity->sendMessage(GSProto::CMD_DREAMLAND_OPEN_REQSECTON,c2sMsg );
}

void DreamLandSystem::clearActorFormation()
{
	m_FormationList.clear();
	m_FormationList.resize(GSProto::MAX_BATTLE_MEMBER_SIZE/2, 0);
	m_DreamLandData.ihaveUseTimes = 0;
	m_DreamLandData.deadHeroList.clear();
}

void DreamLandSystem::onReqResetDreamLand(const GSProto::CSMessage& msg)
{
	IEntity *pEntity = getEntityFromHandle(m_hEntity);
	assert(pEntity);
	
	IGlobalCfg* pGlobalcfg = getComponent<IGlobalCfg>(COMPNAME_GlobalCfg,IID_IGlobalCfg);
	assert(pGlobalcfg);
	IVIPFactory* factory = getComponent<IVIPFactory>(COMPNAME_VIPFactory,IID_IVIPFactory);
	assert(factory);
	int configTimes = factory->getVipPropByHEntity(m_hEntity, VIP_PROP_DREAMLAND_RESET);

	if(configTimes <= m_DreamLandData.iHaveUseResetTime)
	{	
		pEntity->sendErrorCode(ERROR_DREAMLAND_RESETTIEMS_LIMIT);
		return;
	}

	//先扣次数 后做事
	m_DreamLandData.iHaveUseResetTime++;
	m_DreamLandData.unLockSectionId = -1;
	
	m_DreamLandData.sectionProcessVec.clear(); 
	m_guardNamesVec.clear();
	m_DreamLandData.actorHpMap.clear();
	
	clearActorFormation();

	m_DreamLandData.sectionGuardPlayerMap.clear();

	//药瓶初始化个数

	int iInitItemCount = pGlobalcfg->getInt("战斗初始药物数量", 1);
	assert(iInitItemCount != -1);

	m_DreamLandData.iItemCount = iInitItemCount;
	
	//下发信息
	sendSectionInfo();
}

void DreamLandSystem::sendSceneInfo(int iSecetionID)
{
	//填充数据
	IDreamLandFactory* pDreamLandFactory = getComponent<IDreamLandFactory>(COMPNAME_DreamLandFactory,IID_IDreamLandFactory);
	assert(pDreamLandFactory);
	
	DreamLandSection section;
	bool res = pDreamLandFactory->querySectionInfo(iSecetionID,section);
	assert(res);

	//第一个关卡可打
	GSProto::CMD_DREAMLAND_QUERY_SCENEINFO_SC s2cMsg;
	GSProto::SceneDetail& detail = *( s2cMsg.add_szscenedetail());
	detail.set_isceneid(section.sceneIdVec[0]);
	detail.set_isceneindex(0);
	detail.set_iscenestate(en_State_OPen);
	detail.set_bhavegetreward(0);

	//下发关卡信息
	IEntity *pEntity = getEntityFromHandle(m_hEntity);
	pEntity->sendMessage(GSProto::CMD_DREAMLAND_QUERY_SCENEINFO, s2cMsg);
}

void DreamLandSystem::onQuerySectionSceneInfo(const GSProto::CSMessage& msg)
{
	
	IEntity *pEntity = getEntityFromHandle(m_hEntity);
	GSProto::CMD_DREAMLAND_QUERY_SCENEINFO_CS c2sMsg;
	if(!c2sMsg.ParseFromString(msg.strmsgbody()))
	{
		return;	
	}

	int iSectionId = c2sMsg.isectionid();
	
	vector<int>::iterator iter = std::find(m_DreamLandData.sectionList.begin(),m_DreamLandData.sectionList.end(),iSectionId);
	if( iter == m_DreamLandData.sectionList.end())
	{
		pEntity->sendErrorCode( ERROR_DREAMLAND_ISLOCK_SECTION);
		return;
	}

	//没有锁定的关卡
	if( -1 == m_DreamLandData.unLockSectionId)
	{
		sendSceneInfo(iSectionId);
	}
	else
	{
		//不是锁定的关卡那就是客户端错了
		if( iSectionId != m_DreamLandData.unLockSectionId )
		{
			pEntity->sendErrorCode( ERROR_DREAMLAND_ISLOCK_SECTION);
			return;
		}

		//下发开启的关卡的进度
		GSProto::CMD_DREAMLAND_QUERY_SCENEINFO_SC s2cMsg;
		for(size_t i = 0; i < m_DreamLandData.sectionProcessVec.size(); ++i)
		{
			
			ServerEngine::DreamLandSceneRecord &record = m_DreamLandData.sectionProcessVec[i];
		
			GSProto::SceneDetail& detail = *( s2cMsg.add_szscenedetail());
			
			detail.set_isceneid( record.iSceneId);
			detail.set_isceneindex(i);
			int state = record.sceneState;
			detail.set_iscenestate(state);
			detail.set_bhavegetreward(record.iHaveGetBox);

			if(record.iHaveGetBox == 0)
			{
				break;
			}
		}

		//下发
		pEntity->sendMessage(GSProto::CMD_DREAMLAND_QUERY_SCENEINFO, s2cMsg);
	}
	
}

struct ActorInfo
{
	ActorInfo(HEntity hEntity,int sectionId, int isceneId ,ServerEngine::PKRole roleKey,string name)
		:m_hEntity(hEntity),
		iSectionId(sectionId),
		iSceneId(isceneId),
		m_roleKey(roleKey),
		m_strName(name)
		{}
	void operator()(int ret, HEntity hEntity, ServerEngine::PIRole& roleData)
	{

		if( ret != ServerEngine::en_RoleRet_OK)
		{
			FDLOG("DreamLandSystem")<<"Get ActorInfo|"<<ret<<"|"<< m_strName<<endl;
			return;
		}

		IEntity *pEntity = getEntityFromHandle(m_hEntity);
		if(!pEntity)
		{
			return;
		}
		
		DreamLandSystem* pDreamLandSystem = static_cast<DreamLandSystem*>(pEntity->querySubsystem(IID_IDreamLandSystem));
		if(!pDreamLandSystem)
		{
			return;
		}
		pDreamLandSystem->saveSceneActorData( hEntity,iSectionId,iSceneId, roleData, m_roleKey);
	}
private:
	//DreamLandSystem* m_pDreamLandSystem;
	HEntity m_hEntity;
	int iSectionId;
	int iSceneId;
	ServerEngine::PKRole m_roleKey;
	string m_strName;
	
};

void DreamLandSystem::saveSceneActorData(HEntity hEntity,int iSectionID, int iSceneID, ServerEngine::PIRole &roleData, ServerEngine::PKRole roleKey)
{

	int  index = getSceneRecord(iSceneID);
	if( -1 == index || index >= (int)m_DreamLandData.sectionProcessVec.size())
	{
		return;
	}
	ServerEngine::DreamLandSceneRecord& record = m_DreamLandData.sectionProcessVec[index];

	if(record.iHaveGuard == 1)
	{
		return;
	}
	record.iHaveGuard = 1;
	
	if(0 != hEntity)
	{
		ServerEngine::ActorCreateContext roleCreateCtx;
		GhostSaveGuard ghostpackGuard(hEntity);
		IEntity *pEntity = getEntityFromHandle(hEntity);
		roleCreateCtx.strAccount = pEntity->getProperty(PROP_ACTOR_ACCOUNT, "");
		pEntity->packSaveData(roleCreateCtx.data);

		m_DreamLandData.sceneActor.actorContext = roleCreateCtx;
		m_DreamLandData.sceneActor.roleKey = roleKey;
		sendSceneActorInfo( hEntity,index);
	}
	else
	{
		IJZEntityFactory* pEntityFactory = getComponent<IJZEntityFactory>(COMPNAME_EntityFactory, IID_IJZEntityFactory);
		assert(pEntityFactory);

		ServerEngine::ActorCreateContext actorContext;
		actorContext.strAccount = roleData.strAccount;
		ServerEngine::JceToObj( roleData.roleData, actorContext.data);
		string actorDataStr = ServerEngine::JceToStr(actorContext);
		
		IEntity* pTarget = pEntityFactory->getEntityFactory()->createEntity("Ghost",actorDataStr);
		assert(pTarget);
		{
		
			ServerEngine::ActorCreateContext roleCreateCtx;
			GhostSaveGuard ghostpackGuard(pTarget->getHandle());
			
			roleCreateCtx.strAccount = pTarget->getProperty(PROP_ACTOR_ACCOUNT, "");
			pTarget->packSaveData(roleCreateCtx.data);

			m_DreamLandData.sceneActor.actorContext = roleCreateCtx;
			m_DreamLandData.sceneActor.roleKey = roleKey;

			sendSceneActorInfo( pTarget->getHandle(),index);
		}
	
		delete pTarget;
	}
	
	sendSceneGuardFormation(iSectionID,iSceneID);
}

void DreamLandSystem::sendSceneGuardFormation(int iSectionId, int iSceneId)
{
	int  index = getSceneRecord(iSceneId);
	if( -1 == index || index >= (int)m_DreamLandData.sectionProcessVec.size())
	{
		return;
	}
	
	ServerEngine::DreamLandSceneRecord& record = m_DreamLandData.sectionProcessVec[index];

	GSProto::CMD_DREAMLAND_QUERY_SCENEFORMATION_SC scMsg;

	ServerEngine::DreamLandGuardData &data = record.Guarddata;
	scMsg.set_stractorname( data.actorName);
	scMsg.set_itotalfightvaule( data.iTotalFightValue);
	scMsg.set_iactorhead(data.iActorHead);
	scMsg.set_iactorlevel(data.iActorLevel);

	ILegionFactory* pLegionFactory = getComponent<ILegionFactory>(COMPNAME_LegionFactory,IID_ILegionFactory);
	assert(pLegionFactory);
	
	ILegion* pLengion = pLegionFactory->getActorLegion(data.actorName);
	string belongName = "";
	if(pLengion)
	{
		belongName = pLengion->getLegionName();
		scMsg.set_strlegionname(belongName);
	}

		
	IDreamLandFactory* pDreamLandFactory = getComponent<IDreamLandFactory>(COMPNAME_DreamLandFactory, IID_IDreamLandFactory);
	assert(pDreamLandFactory);
	DreamLandScene scene;
	bool res = pDreamLandFactory->querySceneInfo(iSceneId,scene);
	assert(res);
	
	for(size_t i =0; i < data.formation.size(); ++i)
	{
		ServerEngine::DreamLandGuardFormation &forma = data.formation[i];

		GSProto::DreamLandFormationItem &item = *(scMsg.add_szformationitem());

		item.set_ibaseid( forma.iBaseId);
		
		item.set_ipos( forma.iPos );
		item.set_ilevel( forma.iLevel);
		item.set_ilevelstep( forma.iStepLevel);
		item.set_iquality( forma.iQuality);
		int iHp = 0;
		map<int, int>::iterator iter = scene.addFightmap.begin();
		for( ; iter != scene.addFightmap.end(); ++iter)
		{
			int prop = LifeAtt2Prop(iter->first);
	      	  	assert(prop != -1);
			if( prop == PROP_ENTITY_HP)
			{
				iHp = iter->second;
				assert(iHp > 0);
			}
		}

		map<int,int>::iterator iter2 = record.targethpMap.find(forma.iPos);
		if(iter2 != record.targethpMap.end())
		{
			item.set_ihp( (forma.ihp * ( 1 + (double)iHp / 10000.0)) * ((double)iter2->second/10000.0)  );	
		}
		else
		{
			item.set_ihp(forma.ihp * (1+ (double)iHp / 10000.0) );	
		}

		item.set_imaxhp( forma.iMaxHp + (forma.ihp * ((double)iHp / 10000.0)));
		item.set_bisgodanimal( forma.iIsGodAniaml == 1 );
	}
	
	IEntity* pEntity = getEntityFromHandle(m_hEntity);
	
	pEntity->sendMessage(GSProto::CMD_DREAMLAND_QUERY_SCENEFORMATION, scMsg);
}

void DreamLandSystem::sendSceneActorInfo(HEntity hEntity, int index)
{
	if(index >= (int)m_DreamLandData.sectionProcessVec.size())
	{
		return;
	}
	
	ServerEngine::DreamLandSceneRecord& record = m_DreamLandData.sectionProcessVec[index];

	IDreamLandFactory* pDreamLandFactory = getComponent<IDreamLandFactory>( COMPNAME_DreamLandFactory,IID_IDreamLandFactory);
	assert(pDreamLandFactory);
	DreamLandScene lScene;
	bool res = pDreamLandFactory->querySceneInfo(record.iSceneId,lScene );
	assert(res);

	bool isLastScene = (lScene.iSceneType == eSeceneType_Special);
	
	//总战斗力
	IEntity *pActorEntity = getEntityFromHandle(hEntity);
	assert(pActorEntity);
	int totalFightValue = pActorEntity->getProperty(PROP_ENTITY_FIGHTVALUE,0);
	
	//1.获取出阵列表
	IFormationSystem* pFormationSystem = static_cast<IFormationSystem*>( pActorEntity->querySubsystem(IID_IFormationSystem));
	assert(pFormationSystem);
	
	vector<HEntity> hEntityVec;
	pFormationSystem->getEntityList(hEntityVec);

	ServerEngine::DreamLandGuardData& guardData =  record.Guarddata;
	
	guardData.actorName = pActorEntity->getProperty(PROP_ENTITY_NAME,"");
	guardData.iTotalFightValue = totalFightValue;
	guardData.iActorHead = pActorEntity->getProperty(PROP_ENTITY_ACTOR_HEAD,0);
	guardData.iActorLevel = pActorEntity->getProperty(PROP_ENTITY_LEVEL, 1);
	int iMonsterHp = 0;
	int iMonsterAtt = 0;
	for(size_t i = 0; i < hEntityVec.size(); ++i)
	{
		HEntity hTmpEntity = hEntityVec[i];
		IEntity* pTmpHero = getEntityFromHandle(hTmpEntity);
		if(!pTmpHero) continue;

		int iPos = pFormationSystem->getEntityPos(hTmpEntity);

		
		int iHp = pTmpHero->getProperty(PROP_ENTITY_MAXHP, 0);
		iMonsterHp += iHp;
		iMonsterAtt += pTmpHero->getProperty(PROP_ENTITY_ATT, 0);

		if(!isLastScene)
		{
			ServerEngine::DreamLandGuardFormation formation;
			formation.iPos = iPos;
			formation.iBaseId =  pTmpHero->getProperty(PROP_ENTITY_BASEID, 0);
			formation.iStepLevel =  pTmpHero->getProperty(PROP_ENTITY_LEVELSTEP, 0);
			formation.iLevel =  pTmpHero->getProperty(PROP_ENTITY_LEVEL, 0);
			//int iMaxHP = pTmpHero->getProperty(PROP_ENTITY_MAXHP, 0);
	
			formation.ihp = iHp;
			formation.iMaxHp = iHp;
			formation.iIsGodAniaml = false;
			formation.iQuality = pTmpHero->getProperty(PROP_ENTITY_QUALITY, 0);

			guardData.formation.push_back(formation);
		}
	}
	
	//神兽
	IGodAnimalSystem *pGodSys = static_cast<IGodAnimalSystem*>(pActorEntity->querySubsystem(IID_IGodAnimalSystem));
	if(pGodSys)
	{
		HEntity godHEntity = pGodSys->getActiveGodAnimal();
		IEntity *pGodEntity = getEntityFromHandle(godHEntity);
		if( pGodEntity)
		{
			iMonsterHp += pGodEntity->getProperty(PROP_ENTITY_MAXHP, 0);
			iMonsterAtt += pGodEntity->getProperty(PROP_ENTITY_ATT, 0);
			
			if(!isLastScene)
			{
				ServerEngine::DreamLandGuardFormation formation;
				formation.iPos = -1;
				formation.iBaseId =  pGodEntity->getProperty(PROP_ENTITY_BASEID, 0);
				formation.iStepLevel =  pGodEntity->getProperty(PROP_ENTITY_LEVELSTEP, 0);
				formation.iLevel =  pGodEntity->getProperty(PROP_ENTITY_LEVEL, 0);
				formation.iIsGodAniaml = 1;
				
				guardData.formation.push_back(formation);
			}
		}
	}
	
	record.iMonsterHp = iMonsterHp;
	record.iMonsterAttValue = iMonsterAtt;

	if(isLastScene)
	{
		int iGrouppId = lScene.iMonsterGrupId;
		ITable* pMonsterGrpTb = getCompomentObjectManager()->findTable(TABLENAME_MonsterGroup);
		assert(pMonsterGrpTb);

		int iGrpRecord = pMonsterGrpTb->findRecord(iGrouppId);
		assert(iGrpRecord >= 0);

		int iVisibleMonsterId = pMonsterGrpTb->getInt(iGrpRecord,"显示怪",0);
		assert(iVisibleMonsterId);
		
		ITable* pMonsterTb = getCompomentObjectManager()->findTable(TABLENAME_Monster);
		assert(pMonsterTb);
		
		int iMonsterRecord = pMonsterTb->findRecord(iVisibleMonsterId);
		assert(iMonsterRecord >= 0);
		
		int iLevel = pMonsterTb->getInt(iMonsterRecord,"等级",0);

		int iStepLevel =  pMonsterTb->getInt(iMonsterRecord,"怪物等阶",0);

		int iMonsterPos = -1;
		for(int i = 0; i < 9; i++)
		{
			stringstream ss;
			ss<<(i+1)<<"号位成员";
			string strKey = ss.str();
			int iMosterID = pMonsterGrpTb->getInt(iGrpRecord, strKey);
			if(iMosterID > 0) 
			{
				iMonsterPos = i;
				break;
			}
		}

		assert(iMonsterPos != -1);

		ServerEngine::DreamLandGuardFormation formation;
//		record.iMonsterPos = iMonsterPos;
		formation.iPos = iMonsterPos;
		formation.iBaseId =  iVisibleMonsterId;
		formation.iStepLevel = iStepLevel;
		formation.iLevel =  iLevel;
		formation.iIsGodAniaml = 1;
		formation.iMaxHp = iMonsterHp;
		formation.ihp = iMonsterHp;
		formation.bNotMonster = false;
		guardData.formation.push_back(formation);
	}
}

//守关玩家阵型
void DreamLandSystem::onQuerySceneFormation(const GSProto::CSMessage& msg)
{
	IEntity *pEntity = getEntityFromHandle(m_hEntity);
	assert(pEntity);
	
	GSProto::CMD_DREAMLAND_QUERY_SCENEFORMATION_CS csMsg;
	if(!csMsg.ParseFromString(msg.strmsgbody()))
	{
		return;
	}

	int iSectionId = csMsg.isectionid();
	int iScene = csMsg.isceneid();

	if( -1 == m_DreamLandData.unLockSectionId)
	{
		m_DreamLandData.sectionProcessVec.clear(); 
		m_guardNamesVec.clear();
		IDreamLandFactory* pDreamLandFactory = getComponent<IDreamLandFactory>(COMPNAME_DreamLandFactory, IID_IDreamLandFactory);
		assert(pDreamLandFactory);
		DreamLandSection  section;
		bool res =  pDreamLandFactory->querySectionInfo(iSectionId,section);
		if( -1 == res)
		{
			return;
		}

		ServerEngine::DreamLandSceneRecord record;
		record.iSceneId = section.sceneIdVec[0];
		record.sceneState = en_State_OPen;
		record.iHaveGetBox = 0;
		record.iHaveGuard = 0;
		record.targethpMap.clear();
		m_DreamLandData.sectionProcessVec.push_back(record);
	}
	
	int  index = getSceneRecord(iScene);
	int processSize = m_DreamLandData.sectionProcessVec.size();
	if( -1 == index || index >=  processSize)
	{
		return;
	}
	
	ServerEngine::DreamLandSceneRecord& record = m_DreamLandData.sectionProcessVec[index];
	if(record.iHaveGuard == 1)
	{
		sendSceneGuardFormation(iSectionId,iScene);
		return;
	}
	else
	{
		IDreamLandFactory* pDreamLandFactory = getComponent<IDreamLandFactory>(COMPNAME_DreamLandFactory, IID_IDreamLandFactory);
		assert(pDreamLandFactory);
		size_t size = m_DreamLandData.sectionProcessVec.size();
		if(size == 1)
		{
			bool getPlayerRes = getArenaSavePlayer(iSectionId);
			if(!getPlayerRes)
			{
				pEntity->sendErrorCode(ERROR_DREAMLAND_GREGUARD_ERROR);
				return;
			}
		}
		assert( (size-1) < m_guardNamesVec.size());
		ServerEngine::ArenaSavePlayer player = m_guardNamesVec[size-1];
		
		IModifyDelegate* pModifyDelegate = getComponent<IModifyDelegate>(COMPNAME_ModifyDelegate, IID_IModifyDelegate);
		assert(pModifyDelegate);

		pModifyDelegate->submitDelegateTask(player.roleKey, ActorInfo( m_hEntity, iSectionId, iScene,player.roleKey,player.strName ) );
	}
}

bool DreamLandSystem::getArenaSavePlayer(int iSectionId)
{
	map< int, vector<ServerEngine::ArenaSavePlayer> >::iterator iter = m_DreamLandData.sectionGuardPlayerMap.find(iSectionId);
	if( iter != m_DreamLandData.sectionGuardPlayerMap.end())
	{
		m_guardNamesVec = iter->second;
		return true;
	}
	
	m_guardNamesVec.clear();
	IEntity *pEntity = getEntityFromHandle(m_hEntity);
	assert(pEntity);
	
	IDreamLandFactory* pDreamLandFactory = getComponent<IDreamLandFactory>(COMPNAME_DreamLandFactory, IID_IDreamLandFactory);
	assert(pDreamLandFactory);
	DreamLandSection  section;
	bool res =  pDreamLandFactory->querySectionInfo(iSectionId,section);
	if(!res)
	{
		return false;
	}

	vector<int> scenIdVec = section.sceneIdVec;
	IArenaFactory* pArenaFactory = getComponent<IArenaFactory>(COMPNAME_ArenaFactory,IID_IArenaFactory);
	assert(pArenaFactory);
	size_t index = 0;
	int loop = 0;
	bool isLast= false;
	int count = 0;
	while(m_guardNamesVec.size() != scenIdVec.size())
	{
		count++;
		if(count == 100)assert(0);
		assert(index < scenIdVec.size());
		DreamLandScene scene;
		bool res1 = pDreamLandFactory->querySceneInfo(scenIdVec[index],scene);
		if( !res1 ) return false;
		int iMax = scene.iFightMaxWeight;
		int iMin = scene.iFightMinWeight;
		ServerEngine::ArenaSavePlayer player;
		bool res = pArenaFactory->getDreamLandList(m_hEntity,player,iMin,iMax);
		if(res)
		{
			vector<ServerEngine::ArenaSavePlayer>::iterator iter = std::find(m_guardNamesVec.begin(),m_guardNamesVec.end(),player);
			if( iter ==  m_guardNamesVec.end())
			{
				m_guardNamesVec.push_back(player);
				if(m_guardNamesVec.size() == ( index +1))
				{
					if(isLast)
					{
						index--;
					}
					else
					{
						index++;
					}
					loop = 0;
				}
			}
			else
			{
				loop++;
				if( loop == 17)
				{
					if(isLast)
					{
						index--;
					}
					else
					{
						index++;
					}
					loop = 0;
				}
			}
		}
		else
		{
			if(index == scenIdVec.size()-1)
			{	
				isLast = true;
				
			}
			
			if(isLast)
			{
				index--;
			}
			else
			{
				index++;
			}
			loop = 0;
		}

	}
	assert( m_guardNamesVec.size() == scenIdVec.size());
	m_DreamLandData.sectionGuardPlayerMap.insert(make_pair(iSectionId,m_guardNamesVec));
	return true;
}

void DreamLandSystem::sendFormationToClient()
{
	IEntity* pEntity = getEntityFromHandle(m_hEntity);
	assert(pEntity);
	GSProto::CMD_DREAMLAND_GETACTOR_FORMATION_INFO_SC s2cMsg;

	IHeroSystem* pHeroSystem = static_cast<IHeroSystem*>(pEntity->querySubsystem(IID_IHeroSystem));
	assert(pHeroSystem);
	
	for(size_t i = 0; i < m_FormationList.size(); ++i)
	{
		HEntity hEntity= m_FormationList[i];
		IEntity* pTempEntity = getEntityFromHandle(hEntity);
		if(!pTempEntity) continue;
	
		int iPos = i;
		
		GSProto::DreamLandFormation &heroFormation = *s2cMsg.add_szdreamlandformation();
		heroFormation.set_dwobjcetid( m_FormationList[i]);
		heroFormation.set_ipos( iPos);
	}

	IGlobalCfg* pGlobalcfg = getComponent<IGlobalCfg>(COMPNAME_GlobalCfg,IID_IGlobalCfg);
	assert(pGlobalcfg);
	int configTimes = pGlobalcfg->getInt("幻境可战死英雄数",10);
	int remaindTimes = configTimes - m_DreamLandData.deadHeroList.size();
	remaindTimes = std::max(0,remaindTimes);
	  //assert(( (remaindTimes >= 0) && (remaindTimes <= configTimes) ) );

	vector<string>::iterator iter2 = m_DreamLandData.deadHeroList.begin(); 
	for(; iter2 != m_DreamLandData.deadHeroList.end(); ++iter2)
	{
		const string strUUID = *iter2;
		HEntity hEntity = pHeroSystem->getHeroFromUUID(strUUID);
		assert(0 != hEntity );
		GSProto::DreamLandHeroInfo& info = *(s2cMsg.add_szhavebattleheroinfo());
		info.set_dwobjcetid(hEntity);
		info.set_iblootpresent(0);
	}
	
	map<std::string, taf::Int32>::iterator iter3 = m_DreamLandData.actorHpMap.begin();
	for(; iter3 != m_DreamLandData.actorHpMap.end(); ++iter3)
	{
		const string strUUID = iter3->first;
		HEntity hEntity = pHeroSystem->getHeroFromUUID(strUUID);
		assert(0 != hEntity );
		GSProto::DreamLandHeroInfo& info = *(s2cMsg.add_szhavebattleheroinfo());
		info.set_dwobjcetid(hEntity);
		info.set_iblootpresent(iter3->second);
	}

	s2cMsg.set_remaindtime(remaindTimes );
	
	pEntity->sendMessage(GSProto::CMD_DREAMLAND_GETACTOR_FORMATION_INFO , s2cMsg);
}


void DreamLandSystem::onRequstOPenBox(const GSProto::CSMessage& msg)
{
	GSProto::CMD_DREAMLAND_REQUEST_OPENBOX_CS c2sMsg;	
	if( !c2sMsg.ParseFromString(msg.strmsgbody()))
	{
		return;
	}
	int iSectionId = c2sMsg.isectionid();
	int  iSceneId = c2sMsg.isceneid();

	if(iSectionId != m_DreamLandData.unLockSectionId || m_DreamLandData.unLockSectionId == -1)
	{
		return;
	}

	int  index = getSceneRecord(iSceneId);
	if( -1 == index || index >= (int)m_DreamLandData.sectionProcessVec.size())
	{
		return;
	}

	ServerEngine::DreamLandSceneRecord& record = m_DreamLandData.sectionProcessVec[index];

	if( record.iHaveGetBox == 1 || record.sceneState != en_State_Finish )
	{
		return;
	}
	
	IDreamLandFactory* pDreamLandFactory = getComponent<IDreamLandFactory>( COMPNAME_DreamLandFactory,IID_IDreamLandFactory);
	assert(pDreamLandFactory);
	DreamLandScene lScene;
	bool res = pDreamLandFactory->querySceneInfo(iSceneId,lScene );
	assert(res);

	DreamLandBox box;
	bool boxRes = pDreamLandFactory->queryDreamLandBoxInfo(lScene.iDreamLandBoxId,box);
	assert(boxRes);

	IDropFactory* pDropFactory = getComponent<IDropFactory>(COMPNAME_DropFactory, IID_IDropFactory);
	assert(pDropFactory);

	GSProto::CMD_DREAMLAND_REQUEST_OPENBOX_SC s2cMsg;
	GSProto::FightAwardResult& awardResult = *s2cMsg.mutable_awardresoult();

	for(size_t i = 0; i < box.m_RewardVec.size(); ++i)
	{
		pDropFactory->calcDrop( box.m_RewardVec[i], awardResult);
	}

	for(size_t i = 0; i < box.m_magicRewardVec.size(); ++i)
	{
		pDropFactory->calcDrop( box.m_magicRewardVec[i], awardResult);
	}
	
   //	AddHeroReturn heroReturn(m_hEntity);
	// 执行奖励
	pDropFactory->excuteDrop(m_hEntity, awardResult, GSProto::en_Reason_ClimTowerChallengeAward);
	//GSProto::DreamRewardHero& hero = *(s2cMsg.mutable_awardhero());
	//hero.set_icount(heroReturn.iSoulCount);
	//hero.set_iheroid( heroReturn.iHeroId);
	//hero.set_iherosoulid(heroReturn.iSoulId);
	
	//宝箱设为已经领奖
	record.iHaveGetBox = 1;

	IEntity* pEntity  = getEntityFromHandle(m_hEntity);
	assert(pEntity);
	pEntity->sendMessage(GSProto::CMD_DREAMLAND_REQUEST_OPENBOX, s2cMsg);

	//下发开启的关卡的进度
	GSProto::CMD_DREAMLAND_QUERY_SCENEINFO_SC scMsg;
	for(size_t i = 0; i < m_DreamLandData.sectionProcessVec.size(); ++i)
	{
		
		ServerEngine::DreamLandSceneRecord &record = m_DreamLandData.sectionProcessVec[i];
	
		GSProto::SceneDetail& detail = *( scMsg.add_szscenedetail());
		
		detail.set_isceneid( record.iSceneId);
		detail.set_isceneindex(i);
		int state = record.sceneState;
		detail.set_iscenestate(state);
		detail.set_bhavegetreward(record.iHaveGetBox);

		if(record.iHaveGetBox == 0)
		{
			break;
		}
	}
	pEntity->sendMessage(GSProto::CMD_DREAMLAND_QUERY_SCENEINFO, scMsg);
	
}

void DreamLandSystem::onQueryBox(const GSProto::CSMessage& msg)
{
	GSProto::CMD_DREAMLAND_QUERY_BOX_CS c2sMsg;
	if( ! c2sMsg.ParseFromString(msg.strmsgbody()))
	{
		return;
	}
	
	unsigned int iSceneId = c2sMsg.isceneid();
	
	IDreamLandFactory* pDreamLandFactory = getComponent<IDreamLandFactory>( COMPNAME_DreamLandFactory,IID_IDreamLandFactory);
	assert(pDreamLandFactory);
	DreamLandScene lScene;
	bool res = pDreamLandFactory->querySceneInfo(iSceneId,lScene );
	assert(res);

	DreamLandBox box;
	bool boxRes = pDreamLandFactory->queryDreamLandBoxInfo(lScene.iDreamLandBoxId,box);
	assert(boxRes);

	IDropFactory* pDropFactory = getComponent<IDropFactory>(COMPNAME_DropFactory, IID_IDropFactory);
	assert(pDropFactory);

	GSProto::CMD_DREAMLAND_QUERY_BOX_SC s2cMsg;
	GSProto::FightAwardResult& awardResult = *s2cMsg.mutable_awardresoult();

	for(size_t i = 0; i < box.m_RewardVec.size(); ++i)
	{
		pDropFactory->calcDrop( box.m_RewardVec[i], awardResult);
	}

	bool bHaveMagic = (box.m_magicRewardVec.size() > 0);
	s2cMsg.set_bhavemagic(bHaveMagic);

	//下发客户端
	IEntity* pEntity  = getEntityFromHandle(m_hEntity);
	assert(pEntity);
	pEntity->sendMessage(GSProto::CMD_DREAMLAND_QUERY_BOX, s2cMsg);
}


void DreamLandSystem::onGetActorFormation(const GSProto::CSMessage& msg)
{
	if( -1 == m_DreamLandData.unLockSectionId)
	{
		clearActorFormation();
	}
	sendFormationToClient();
}

//变阵
void DreamLandSystem::onChgActorFormation(const GSProto::CSMessage& msg)
{
	if(m_FightFlag) return;
	
	GSProto::CMD_DREAMLAND_FORMATION_CHG_CS c2sMsg;

	if(!c2sMsg.ParseFromString(msg.strmsgbody()))
	{
		return;
	}
	// 验证英雄是否合法
	vector<HEntity> csFormationList;
	csFormationList.resize(GSProto::MAX_BATTLE_MEMBER_SIZE/2,0);
	
	IEntity* pEntity = getEntityFromHandle(m_hEntity);
	assert(pEntity);
	IFormationSystem* pFormationSys = static_cast<IFormationSystem*>(pEntity->querySubsystem(IID_IFormationSystem));
	assert(pFormationSys);
	int iLimitCount = pFormationSys->getFormationHeroLimit();
	if(c2sMsg.szformationlist_size()  > iLimitCount)
	{
		pEntity->sendErrorCode(ERROR_FORMATION_LIMIT);
		return;
	}
	
	for(int i = 0; i < c2sMsg.szformationlist_size(); ++i)
	{
		const GSProto::DreamLandFormation& data = c2sMsg.szformationlist( i );
		IEntity* pHeroEntity = getEntityFromHandle(data.dwobjcetid());
		if( !pHeroEntity)return;
		HEntity hMaster = pHeroEntity->getProperty(PROP_ENTITY_MASTER, 0);
		if(m_hEntity != hMaster) return;
		int ipos = data.ipos();
		
		if( -1 > ipos && ipos > GSProto::MAX_BATTLE_MEMBER_SIZE/2 )
		{
			return;
		}
		
		if(ipos%3 == 2) return;
		unsigned int dwObjcetId = data.dwobjcetid();
		//验证唯一性
		for(size_t j = 0; j < csFormationList.size(); ++j)
		{
			if( dwObjcetId == csFormationList[ j ] )
			{
				return;
			}
		}
		string str = pHeroEntity->getProperty( PROP_ENTITY_UUID,"");
		vector<string>::iterator iter2 = std::find(m_DreamLandData.deadHeroList.begin(), m_DreamLandData.deadHeroList.end(), str );
		if( iter2 != m_DreamLandData.deadHeroList.end())
		{
			return;
		}

		csFormationList[ipos] = dwObjcetId;
	}
	int iNotZeroCount = (int)std::count_if(csFormationList.begin(), csFormationList.end(), bind2nd(not_equal_to<Uint32>(), 0));
	if(iNotZeroCount == 0)
	{
		pEntity->sendErrorCode(ERROR_FORMATION_CANNOTEMPTY);
		return;
	}
	m_FormationList = csFormationList;
	sendFormationToClient();
}

/*
void DreamLandSystem::takeHeroUpFormation(HEntity htargetHero,int iPos)
{
	IEntity* pEntity = getEntityFromHandle(m_hEntity);
	assert(pEntity);

	IEntity* pMoveEntity = getEntityFromHandle(htargetHero);
	if(!pMoveEntity) return;

	int iHeroID = pMoveEntity->getProperty(PROP_ENTITY_BASEID, 0);
	int iSameIDPos = getHeroPosFromBaseID(iHeroID);

	// 如果已经有相同的英雄，并且不会被替换下，则提示错误
	if( (iSameIDPos >=0) && (iSameIDPos != iPos) )
	{
		pEntity->sendErrorCode(ERROR_FORMATION_SAMEID);
		return;
	}
	
	string str = pMoveEntity->getProperty( PROP_ENTITY_UUID,"");
	vector<string>::iterator iter2 = std::find(m_DreamLandData.deadHeroList.begin(), m_DreamLandData.deadHeroList.end(), str );
	if( iter2 != m_DreamLandData.deadHeroList.end())
	{
		IEntity * pActor = getEntityFromHandle( m_hEntity);
		assert(pActor);
		pEntity->sendErrorCode(ERROR_DREAMLAND_HERO_HAVE_DEATH);
		return;
	}

	// 直接替换
	if(0 != m_FormationList[iPos])
	{
		m_FormationList[iPos] = htargetHero;
		sendFormationToClient();
		return;
	}
	
	// 新增，验证上阵人数限制
	IFormationSystem* pFormationSys = static_cast<IFormationSystem*>(pEntity->querySubsystem(IID_IFormationSystem));
	assert(pFormationSys);
	int iLimitCount = pFormationSys->getFormationHeroLimit();
	int iCurrentCount = (int)std::count_if(m_FormationList.begin(), m_FormationList.end(), bind2nd(not_equal_to<Uint32>(), 0));

	if(iCurrentCount >= iLimitCount)
	{
		pEntity->sendErrorCode(ERROR_FORMATION_LIMIT);
		return;
	}

	m_FormationList[iPos] = htargetHero;
	//m_DreamLandData.ihaveUseTimes++;
	sendFormationToClient();
}
*/
int DreamLandSystem::getHeroPosFromBaseID(int iBaseID)
{
	for(size_t i = 0; i < m_FormationList.size(); i++)
	{
		IEntity* pTmpEntity = getEntityFromHandle(m_FormationList[i]);
		if(!pTmpEntity) continue;

		int iTmpHeroID = pTmpEntity->getProperty(PROP_ENTITY_BASEID, 0);
		if(iTmpHeroID == iBaseID)
		{
			return (int)i;
		}
	}

	return -1;
}

/*
void DreamLandSystem::chagePos(HEntity hMoveEntity, int iSrcPos, int iTargetPos)
{
	if( -1 == iTargetPos )
	{
		m_FormationList[iSrcPos] = 0;
		//m_DreamLandData.ihaveUseTimes--;
	}
	else
	{
		HEntity hTmp = m_FormationList[iTargetPos];
		m_FormationList[iTargetPos] = hMoveEntity;
		m_FormationList[iSrcPos] = hTmp;
	}
	sendFormationToClient();
}
*/
class DreamLandSystemPVPFight
{
public:
	DreamLandSystemPVPFight(HEntity hEntity,string actorName,int sectionId, int sceneId)
		:m_hEntity(hEntity),
		m_actorName(actorName),
		m_SectionId(sectionId),
		m_SceneId(sceneId)
	{
	}
	void operator()(taf::Int32 iRet, ServerEngine::BattleData& battleInfo)
	{
		if(iRet != en_FightResult_OK)
		{
			FDLOG("DreamLandSystemBattle")<<"DreamLandSystemPVPFight|"<<iRet<<"|"<<m_actorName << "|"<<endl;
			return; 
		}

		IEntity* pEntity = getEntityFromHandle(m_hEntity);
		if(!pEntity) return;

		DreamLandSystem* pDreamLandSys = static_cast<DreamLandSystem*>(pEntity->querySubsystem(IID_IDreamLandSystem) );
		assert(pDreamLandSys);
		pDreamLandSys->processFightResult(m_SectionId, m_SceneId, battleInfo);
	}

private:
	HEntity m_hEntity;
	string m_actorName;
	int m_SectionId;
	int m_SceneId;
};

bool DreamLandSystem::getUUIDByiPos(int iPos, string& sUUID)
{
	HEntity hEntity = m_FormationList[iPos];
	if(0 == hEntity) return false;
	IEntity *pHeroEntity = getEntityFromHandle(hEntity);
	assert(pHeroEntity);
	sUUID = pHeroEntity->getProperty( PROP_ENTITY_UUID, "");
	return true;
}

void DreamLandSystem::processFightResult(int iSectionId, int iSceneId,const ServerEngine::BattleData& battleInfo)
{
	int  index = getSceneRecord(iSceneId);
	if( -1 == index || index >= (int)m_DreamLandData.sectionProcessVec.size() )
	{
		return;
	}

	ServerEngine::DreamLandSceneRecord& record = m_DreamLandData.sectionProcessVec[index];
	//record.bFirstFight = false;
	//to do
	//保存血量
	GSProto::FightDataAll fightDataAll;
	if(!fightDataAll.ParseFromString(battleInfo.strBattleBody) )
	{
		return;
	}
	
	//九天幻境没有掉落，但是还是要给英雄显示
	GSProto::Cmd_Sc_CommFightResult scCommFightResult;
	scCommFightResult.set_iissuccess( battleInfo.bAttackerWin ? 1 : 0);
	scCommFightResult.set_istar(battleInfo.iStar);
	scCommFightResult.set_iherogetexp(0);
	
	ITable* pHeroExpTb = getCompomentObjectManager()->findTable(TABLENAME_HeroLevelExp);
	assert(pHeroExpTb);
	
	m_DreamLandData.actorHpMap.clear();
	for(int i = 0; i < fightDataAll.szfightobject().size(); i++)
	{
		const GSProto::FightObj& tmpFightObj = fightDataAll.szfightobject().Get(i);
		int iPos = tmpFightObj.ipos();
		int iMaxHp = 0;
		if(tmpFightObj.iobjecttype() == GSProto::en_class_Hero)
		{
			const GSProto::FightHero& fightHero = tmpFightObj.hero();
			iMaxHp = fightHero.imaxhp();
		}else if(tmpFightObj.iobjecttype()  == GSProto::en_class_Monster)
		{
			const GSProto::FightMonster& fightMonstr= tmpFightObj.monster();
			iMaxHp = fightMonstr.imaxhp();
		}
		
		if(iPos < GSProto::MAX_BATTLE_MEMBER_SIZE/2)
		{
			
			string UUID = "";
			bool res = getUUIDByiPos(iPos,UUID);
			if(!res) continue;
			
			GSProto::HeroExpAwardDetail &heroExpAwardDetail = *(scCommFightResult.add_szheroaward());
			HEntity hHeroEntity = m_FormationList[iPos];
			IEntity *pHero = getEntityFromHandle(hHeroEntity);
			assert(pHero);
			int iTmpHeroLevel = pHero->getProperty(PROP_ENTITY_LEVEL, 0);
			int iTmpCurHeroExp = pHero->getProperty(PROP_ENTITY_HEROEXP, 0);
			int iTmpRecord = pHeroExpTb->findRecord(iTmpHeroLevel);
			assert(iTmpRecord >= 0);
			int iNeedHeroExp = pHeroExpTb->getInt(iTmpRecord, "修为");
			int iPercent = (int)( (double)iTmpCurHeroExp*100/(double)iNeedHeroExp);
			heroExpAwardDetail.set_iheroexppercent(iPercent);
			heroExpAwardDetail.set_blevelup(false);
			heroExpAwardDetail.set_dwheroobjectid(hHeroEntity );
			
			int curHp = tmpFightObj.ifinhp();
			if(curHp == 0)
			{
				m_FormationList[iPos] = 0;
			 	vector<string>::iterator iter = std::find( m_DreamLandData.deadHeroList.begin(),  m_DreamLandData.deadHeroList.end(),UUID );
				if(iter != m_DreamLandData.deadHeroList.end()) return;
				m_DreamLandData.deadHeroList.push_back(UUID);
			}
			else
			{	
				m_DreamLandData.actorHpMap[UUID] = ( ( ( double)curHp)/iMaxHp) * 10000;
			}
			
			
		}
		else if( (iPos >= GSProto::MAX_BATTLE_MEMBER_SIZE/2) && (iPos < GSProto::MAX_BATTLE_MEMBER_SIZE) )
		{
			record.targethpMap[iPos - GSProto::MAX_BATTLE_MEMBER_SIZE/2] = ((double)tmpFightObj.ifinhp()/iMaxHp)* 10000;
		}
	}
	
	IEntity *pEntity = getEntityFromHandle(m_hEntity);
	
	if(battleInfo.bAttackerWin)
	{
		record.sceneState = en_State_Finish;
		//to do
		//裁剪玩家数据保存
		
		IDreamLandFactory* pDreamLandFactory = getComponent<IDreamLandFactory>( COMPNAME_DreamLandFactory,IID_IDreamLandFactory);
		assert(pDreamLandFactory);
		DreamLandSection lSection;
		bool res = pDreamLandFactory->querySectionInfo(iSectionId,lSection );
		assert(res);
		
		//开启新关卡
		openNewScene(m_DreamLandData.unLockSectionId,iSceneId);	
		
		//任务- 通过一个关卡
		{
			EventArgs args;
			args.context.setInt("times",1);
			args.context.setInt("entity",m_hEntity);
			pEntity->getEventServer()->setEvent(EVENT_ENTITY_TASK_CRUSH_DREAMLAND_SCENE, args);
		}
	}

	//保存药瓶
	int iItemCount = fightDataAll.icuritemcount();
	m_DreamLandData.iItemCount = iItemCount;
	pEntity->sendMessage(GSProto::CMD_COMM_FIGHTRESULT,scCommFightResult);
	m_FightFlag = false;
}

void DreamLandSystem::openNewScene(int iSectionId,int iSceneId)
{
	if(iSectionId != m_DreamLandData.unLockSectionId)
	{
		return;
	}
	//填充数据
	IDreamLandFactory* pDreamLandFactory = getComponent<IDreamLandFactory>(COMPNAME_DreamLandFactory,IID_IDreamLandFactory);
	assert(pDreamLandFactory);

	DreamLandScene nowSceneInfo;
	int res = pDreamLandFactory->querySceneInfo(iSceneId, nowSceneInfo);
	assert(res);
	if( nowSceneInfo.iSceneType == eSeceneType_Special )
	{
		int iNextSectionId = pDreamLandFactory->queryNextSectionId(iSectionId);
		if(iNextSectionId == -1)
		{
			//最后一关
			return;
		}

		m_DreamLandData.sectionList.push_back(iNextSectionId);
		return;
	}
	
	DreamLandScene nextSceneInfo;
	int res2 = pDreamLandFactory->queryNextSceneinfo(iSectionId,iSceneId,nextSceneInfo);
	if(res2 == -1)
	{
		return;
	}
	
	//存入关卡记录
	ServerEngine::DreamLandSceneRecord record;
	record.iSceneId = nextSceneInfo.iSceneId;
	record.sceneState = en_State_OPen;
	record.iHaveGetBox = 0;
	record.targethpMap.clear();
	m_DreamLandData.sectionProcessVec.push_back(record);
}

int  DreamLandSystem::getSceneRecord(int iSceneId)
{
	
	size_t size = m_DreamLandData.sectionProcessVec.size();
	for(size_t i = 0; i < size; ++i)
	{
		if(iSceneId == m_DreamLandData.sectionProcessVec[i].iSceneId )
		{
			return i;
		}
	}
	return -1;

}

void DreamLandSystem::GMRequsetFormation(int iSection, int iSceneID)
{
	IEntity *pEntity = getEntityFromHandle(m_hEntity);
	assert(pEntity);
	
	
	int iSectionId = iSection;
	int iScene = iSceneID;

		//填充数据
	IDreamLandFactory* pDreamLandFactory = getComponent<IDreamLandFactory>(COMPNAME_DreamLandFactory,IID_IDreamLandFactory);
	assert(pDreamLandFactory);
	//开放的章节
	m_DreamLandData.sectionList.clear();
	m_DreamLandData.sectionList.push_back(iSection );

	//存入第一个关卡记录
	m_DreamLandData.sectionProcessVec.clear(); 
	m_guardNamesVec.clear();
	ServerEngine::DreamLandSceneRecord record;
	record.iSceneId = iSceneID;
	record.sceneState = en_State_OPen;
	record.iHaveGetBox = 0;
	record.targethpMap.clear();
	m_DreamLandData.sectionProcessVec.push_back(record);
	
	/*if(-1 == m_DreamLandData.unLockSectionId || m_DreamLandData.unLockSectionId != iSectionId)
	{
		return;
	}
	
	int  index = getSceneRecord(iScene);
	if( -1 == index)
	{
		return;
	}
	ServerEngine::DreamLandSceneRecord& record = m_DreamLandData.sectionProcessVec[index];
	if(record.iHaveGuard == 1)
	{
		IJZEntityFactory* pEntityFactory = getComponent<IJZEntityFactory>(COMPNAME_EntityFactory, IID_IJZEntityFactory);
		assert(pEntityFactory);
		string strData = ServerEngine::JceToStr( m_DreamLandData.sceneActor.actorContext);
		IEntity* pTarget = pEntityFactory->getEntityFactory()->createEntity("Ghost",strData);
		assert(pTarget);
		
		sendSceneActorInfo( pTarget->getHandle(),index);
		
		delete pTarget;
		return;
	}
	else
	{*/
		//IDreamLandFactory* pDreamLandFactory = getComponent<IDreamLandFactory>(COMPNAME_DreamLandFactory, IID_IDreamLandFactory);
	//	assert(pDreamLandFactory);
		DreamLandScene scene;

		bool res = pDreamLandFactory->querySceneInfo(iScene,scene);
		if( !res ) return;
//		int iMax = scene.iFightMaxWeight;
	//	int iMin = scene.iFightMinWeight;

	
		bool getPlayerRes = getArenaSavePlayer(iSectionId);
		if(!getPlayerRes)
		{
			pEntity->sendErrorCode(ERROR_DREAMLAND_GREGUARD_ERROR);
			return;
		}
			ServerEngine::ArenaSavePlayer player = m_guardNamesVec[m_DreamLandData.sectionProcessVec.size()-1];
		IModifyDelegate* pModifyDelegate = getComponent<IModifyDelegate>(COMPNAME_ModifyDelegate, IID_IModifyDelegate);
		assert(pModifyDelegate);

		pModifyDelegate->submitDelegateTask(player.roleKey, ActorInfo( m_hEntity, iSectionId, iScene,player.roleKey,player.strName ) );
	//}
}

class DreamLandSystemMultPVEFight
{
public:
	DreamLandSystemMultPVEFight(HEntity hEntity,string actorName,int sectionId, int sceneId)
		:m_hEntity(hEntity),
		m_actorName(actorName),
		m_SectionId(sectionId),
		m_SceneId(sceneId)
	{
	}
	void operator()(int iRet, const ServerEngine::BattleData& battleInfo)
	{
		if(iRet != en_FightResult_OK)
		{
			FDLOG("DreamLandSystemBattle")<<"DreamLandSystemPVPFight|"<<iRet<<"|"<<m_actorName << "|"<<endl;
			return; 
		}

		IEntity* pEntity = getEntityFromHandle(m_hEntity);
		if(!pEntity) return;

		DreamLandSystem* pDreamLandSys = static_cast<DreamLandSystem*>(pEntity->querySubsystem(IID_IDreamLandSystem) );
		assert(pDreamLandSys);
		pDreamLandSys->processFightResult(m_SectionId, m_SceneId, battleInfo);
	}

private:
	HEntity m_hEntity;
	string m_actorName;
	int m_SectionId;
	int m_SceneId;
};

void DreamLandSystem::GMRequsetFight(int iSectionId, int iSceneId)
{
	IEntity* pEntity = getEntityFromHandle(m_hEntity);
	assert(pEntity);
	//验证合法性
	vector<int>::iterator iter = std::find( m_DreamLandData.sectionList.begin(), m_DreamLandData.sectionList.end(),iSectionId);
	if( iter ==  m_DreamLandData.sectionList.end())
	{
		return;
	}

	int index = getSceneRecord(iSceneId);
	if(index == -1 ||  (int)m_DreamLandData.sectionProcessVec.size() <= index)
	{
		return;
	}
	ServerEngine::DreamLandSceneRecord& record = m_DreamLandData.sectionProcessVec[index];
	
	if((record.sceneState != en_State_OPen) || (record.iHaveGuard == 0) )
	{
		return;
	}
	
	if(m_DreamLandData.unLockSectionId == -1)
	{
		m_DreamLandData.unLockSectionId = iSectionId;
	}
	
	
	//战斗
	ServerEngine::PKRole actorRole;
	getActorCreateContext(actorRole,pEntity);

	ServerEngine::FightContext ctx;
	fillBattleUnitContext(ctx ,record ,iSceneId );


	IDreamLandFactory* pDreamLandFactory = getComponent<IDreamLandFactory>(COMPNAME_DreamLandFactory, IID_IDreamLandFactory);
	assert(pDreamLandFactory);
	DreamLandScene scene;
	bool res = pDreamLandFactory->querySceneInfo(iSceneId,scene);
	assert(res);

	IFightSystem *pFightSys = static_cast<IFightSystem*>( pEntity->querySubsystem(IID_IFightSystem));
	assert(pFightSys);
	string strName = pEntity->getProperty(PROP_ENTITY_NAME,"");

	if(scene.iSceneType == eSeceneType_Special)
	{
		vector<int> memlist;
		memlist.push_back(scene.iMonsterGrupId);
		pFightSys->AsyncMultPVEFight(DreamLandSystemMultPVEFight(m_hEntity, strName, iSectionId, iSceneId ),memlist ,ctx);
	}
	else
	{
		pFightSys->AsyncPVPFight( DreamLandSystemPVPFight(m_hEntity, strName, iSectionId, iSceneId ), m_DreamLandData.sceneActor.actorContext, ctx);
	}

	m_FightFlag = true;
}

void DreamLandSystem::onRequstFight(const GSProto::CSMessage& msg)
{
	IEntity* pEntity = getEntityFromHandle(m_hEntity);
	assert(pEntity);

	//阵亡人数
	IGlobalCfg* pGlobalcfg = getComponent<IGlobalCfg>(COMPNAME_GlobalCfg,IID_IGlobalCfg);
	assert(pGlobalcfg);
	int configTimes = pGlobalcfg->getInt("幻境可战死英雄数",10);
	int remaindTimes = configTimes - m_DreamLandData.deadHeroList.size();
	//assert(remaindTimes >= 0);
	if(remaindTimes <= 0)
	{
		pEntity->sendErrorCode(ERROR_DREAMLAND_DEATH_HERO_LIMIT);
		return;
	}
	
	GSProto::CMD_DREAMLAND_REQUEST_FIGHT_CS c2sMsg;
	if(!c2sMsg.ParseFromString(msg.strmsgbody()))
	{
		return;
	}

	unsigned int iSectionId = c2sMsg.isectionid();
	unsigned int iSceneId = c2sMsg.isceneid();
	
	//验证合法性
	vector<int>::iterator iter = std::find( m_DreamLandData.sectionList.begin(), m_DreamLandData.sectionList.end(),iSectionId);
	if( iter ==  m_DreamLandData.sectionList.end())
	{
		return;
	}
	int  index = getSceneRecord(iSceneId);
	if(-1 == index || index >= (int)m_DreamLandData.sectionProcessVec.size() )return;

	if(index  >= 1 )
	{
		ServerEngine::DreamLandSceneRecord& recordLast = m_DreamLandData.sectionProcessVec[index-1];
		if(recordLast.iHaveGetBox != 1 && recordLast.sceneState != en_State_Finish)
		{
			return;
		}
	}

	ServerEngine::DreamLandSceneRecord& record = m_DreamLandData.sectionProcessVec[index];
	
	if( (record.sceneState != en_State_OPen) || (record.iHaveGuard == 0) )
	{
		return;
	}
	
	if(m_DreamLandData.unLockSectionId == -1)
	{
		m_DreamLandData.unLockSectionId = iSectionId;

		map< int, vector<ServerEngine::ArenaSavePlayer> >::iterator guardPlayerIter = m_DreamLandData.sectionGuardPlayerMap.begin();
		for( ; m_DreamLandData.sectionGuardPlayerMap.end() != guardPlayerIter; )
		{
			int sectionId = guardPlayerIter->first;
			if(  m_DreamLandData.unLockSectionId != sectionId )
			{
				m_DreamLandData.sectionGuardPlayerMap.erase(guardPlayerIter++);
			}
			else
			{
				guardPlayerIter++;
			}
		}
	}

	int iCurrentCount = (int)std::count_if(m_FormationList.begin(), m_FormationList.end(), bind2nd(not_equal_to<Uint32>(), 0));
	if( iCurrentCount == 0)
	{
		pEntity->sendErrorCode(ERROR_DREMALAND_NO_HERO);
		return;
	}

	/*if(iCurrentCount < 5)
	{
		pEntity->sendErrorCode(ERROR_DREAMLAND_NO_FIVE);
		return;
	}*/
	
	//战斗
	ServerEngine::PKRole actorRole;
	getActorCreateContext(actorRole,pEntity);

	ServerEngine::FightContext ctx;
	fillBattleUnitContext(ctx ,record , iSceneId);

	IDreamLandFactory* pDreamLandFactory = getComponent<IDreamLandFactory>(COMPNAME_DreamLandFactory, IID_IDreamLandFactory);
	assert(pDreamLandFactory);
	DreamLandScene scene;
	bool res = pDreamLandFactory->querySceneInfo(iSceneId,scene);
	assert(res);

	IFightSystem *pFightSys = static_cast<IFightSystem*>( pEntity->querySubsystem(IID_IFightSystem));
	assert(pFightSys);
	string strName = pEntity->getProperty(PROP_ENTITY_NAME,"");

	if(scene.iSceneType == eSeceneType_Special)
	{
		vector<int> memlist;
		memlist.push_back(scene.iMonsterGrupId);
		pFightSys->AsyncMultPVEFight(DreamLandSystemMultPVEFight(m_hEntity, strName, iSectionId, iSceneId ),memlist ,ctx);
	}
	else
	{
		pFightSys->AsyncPVPFight( DreamLandSystemPVPFight(m_hEntity, strName, iSectionId, iSceneId ), m_DreamLandData.sceneActor.actorContext, ctx);
	}
	m_FightFlag = true;
}

bool DreamLandSystem::getActorCreateContext(ServerEngine::PKRole& pkRole, IEntity* pEntity)
{

	pkRole.strAccount = pEntity->getProperty(PROP_ACTOR_ACCOUNT,"");
	pkRole.rolePos = pEntity->getProperty(PROP_ACTOR_ROLEPOS,0);
	pkRole.worldID = pEntity->getProperty(PROP_ACTOR_WORLD,0);
	return true;
}

void DreamLandSystem::fillBattleUnitContext(ServerEngine::FightContext& fightContext,ServerEngine::DreamLandSceneRecord& recordCtx,  int iSceneId)
{	
	IGlobalCfg* pGlobalCfg = getComponent<IGlobalCfg>(COMPNAME_GlobalCfg, IID_IGlobalCfg);	
	assert(pGlobalCfg);
	
	int iScene = pGlobalCfg->getInt("幻境战斗场景", 1);
	fightContext.iFightType = GSProto::en_FightType_DreamLand;
	fightContext.iSceneID = iScene;
		
	//todo 血量的的改变
	// 填充攻击者
	
	ServerEngine::DreamLandFightCtx dreamLandCtx;

	dreamLandCtx.attCtx.bFixHP = true;
	dreamLandCtx.targetCtx.bFixHP = true;


	for(size_t i = 0; i < m_FormationList.size(); ++i)
	{
		HEntity hTmpHEntity = m_FormationList[i];
		IEntity *pHeroEntity = getEntityFromHandle( hTmpHEntity);
		if( !pHeroEntity) continue;

		string uuId = pHeroEntity->getProperty(PROP_ENTITY_UUID, "");
		
		map<string,int>::iterator iter = m_DreamLandData.actorHpMap.find(uuId);
		
		if( iter != m_DreamLandData.actorHpMap.end() )
		{
			if(  iter->second == 0)	continue;
			dreamLandCtx.attCtx.fixHPMap[i] = iter->second;
		}
	}

	for(size_t i = 0; i < GSProto::MAX_BATTLE_MEMBER_SIZE/2 ; ++i)
	{
		map<int,int>::iterator iter = recordCtx.targethpMap.find(i);
		if( iter != recordCtx.targethpMap.end())
		{
			dreamLandCtx.targetCtx.fixHPMap[i] = iter->second;
		}
	}


	IDreamLandFactory* pDreamLandFactory = getComponent<IDreamLandFactory>(COMPNAME_DreamLandFactory, IID_IDreamLandFactory);
	assert(pDreamLandFactory);
	DreamLandScene scene;
	bool res = pDreamLandFactory->querySceneInfo(iSceneId,scene);
	assert(res);

	if(scene.iSceneType == eSeceneType_Special)
	{
		dreamLandCtx.properyMap = scene.addFightmap;
	}
	
	//怪物的血
	dreamLandCtx.iMonsterHp = recordCtx.iMonsterHp;
	dreamLandCtx.iMonsterAttValue = recordCtx.iMonsterAttValue;

	
	//传入药瓶
	 dreamLandCtx.iItemCount = m_DreamLandData.iItemCount;
   
	 packHeroListData(dreamLandCtx.strHerolistData);
	 packHeroFormationData(dreamLandCtx.strFormationData);

	// 打包
	fightContext.context["DreamLandFightCtx"] = ServerEngine::JceToStr(dreamLandCtx);

}

void DreamLandSystem::packHeroListData(string& data)
{

	ServerEngine::HeroSystemData tmpSaveData;
	for(size_t i = 0; i < m_FormationList.size(); i++)
	{
		IEntity* pHero = getEntityFromHandle(m_FormationList[i]);
		if(!pHero) continue;
		
		tmpSaveData.heroList.push_back(ServerEngine::RoleSaveData() );
		pHero->packSaveData( tmpSaveData.heroList.back() );
	}

	data = ServerEngine::JceToStr(tmpSaveData);
}	

void DreamLandSystem::packHeroFormationData(string& data)
{
	ServerEngine::FormationSystemData saveData;

	int iVisibelId = 0;
	for(size_t i = 0; i < m_FormationList.size(); i++)
	{
		HEntity hTmpEntity = m_FormationList[i];
		if(0 == hTmpEntity) continue;
		
		IEntity* pTmpHero = getEntityFromHandle(hTmpEntity);
		if(!pTmpHero) continue;

		if(iVisibelId == 0)
		{
			iVisibelId = pTmpHero->getProperty(PROP_ENTITY_BASEID, 0);
		}
		
		int iClassID = pTmpHero->getProperty(PROP_ENTITY_CLASS, 0);
		assert(GSProto::en_class_Hero == iClassID);
		
		string strUUID = pTmpHero->getProperty(PROP_ENTITY_UUID, "");
		
		ServerEngine::FormationItem tmpItem;
		tmpItem.strUUID = strUUID;
		saveData.formationList[i] = tmpItem;
	}
	saveData.iVisibleHeroId = iVisibelId;
	data = ServerEngine::JceToStr(saveData);
}


int DreamLandSystem::getIposByUUDID(string UUID)
{
	IEntity *pEntity = getEntityFromHandle(m_hEntity);
	IHeroSystem* pHeroSystem = static_cast<IHeroSystem*>(pEntity->querySubsystem(IID_IHeroSystem));
	assert(pHeroSystem);
	HEntity hTmpHero = pHeroSystem->getHeroFromUUID(UUID);
	if(0 == hTmpHero)
	{
		return -1;
	}
	vector<HEntity>::iterator iter = std::find(m_FormationList.begin(),m_FormationList.end(),hTmpHero);
	int iPos = std::distance(m_FormationList.begin(),iter);
	return iPos;
}


void DreamLandSystem::packSaveData(string& data)
{
	m_DreamLandData.FormationMap.clear();
	for(size_t i = 0; i < m_FormationList.size(); ++i)
	{
		HEntity hTmpEntity = m_FormationList[i];
		IEntity *pTmpHero = getEntityFromHandle(hTmpEntity);
		if(!pTmpHero) continue;
		
        string strUUID = pTmpHero->getProperty(PROP_ENTITY_UUID, "");
		
		m_DreamLandData.FormationMap[i] = strUUID;
	}
	
	data = ServerEngine::JceToStr( m_DreamLandData);
}
