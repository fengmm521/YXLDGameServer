#include "FriendSystemPch.h"
#include "FriendSystem.h"

extern "C" IObject* createFriendSystem()
{
	return new FriendSystem;
}

FriendSystem::FriendSystem()
{
	
}

FriendSystem::~FriendSystem()
{
	ITimerComponent* timeComponent = getComponent<ITimerComponent>(COMPNAME_TimeAxis,IID_ITimerComponent);
	assert(timeComponent);
	timeComponent->killTimer( m_CheckNoticeTimerHandle);
}

Uint32 FriendSystem::getSubsystemID() const
{
	return IID_IFriendSystem;
}

Uint32 FriendSystem::getMasterHandle()
{
	return m_hEntity;
}

bool FriendSystem::create(IEntity* pEntity, const std::string& strData)
{
	m_hEntity = pEntity->getHandle();
	if(strData.size()>0)
	{
		ServerEngine::JceToObj(strData, m_FriendData);
		ResetData(m_FriendData);
	}
	else
	{
		m_FriendData.iGetPhyStrengthTimes = 0;
		m_FriendData.iGivePhyStrengthTimes = 0;

		IZoneTime* pZoneTime = getComponent<IZoneTime>(COMPNAME_ZoneTime,IID_IZoneTime);
		assert(pZoneTime);
		m_FriendData.dwLastChgTime = pZoneTime->GetCurSecond();
	}
	
	ITimerComponent* timeComponent = getComponent<ITimerComponent>(COMPNAME_TimeAxis,IID_ITimerComponent);
	assert(timeComponent);
	m_CheckNoticeTimerHandle = timeComponent->setTimer(this,1,60*1000,"FriendSystem");

	checkNotice();
	
	return true;
}

void FriendSystem::ResetData(ServerEngine::FriendSystemData& data)
{
	IZoneTime* pZoneTime = getComponent<IZoneTime>(COMPNAME_ZoneTime,IID_IZoneTime);
	assert(pZoneTime);
	
	if(!pZoneTime->IsInSameDay( data.dwLastChgTime,pZoneTime->GetCurSecond() ) )
	{
		data.iGetPhyStrengthTimes = 0;
		data.iGivePhyStrengthTimes = 0;
		data.dwLastChgTime = pZoneTime->GetCurSecond();
		data.m_haveGetActorMap.clear();
		data.m_haveGiveActorMap.clear();
	}
}

void FriendSystem::resetFriendNode(ServerEngine::FriendNode& node)
{
	IZoneTime* pZoneTime = getComponent<IZoneTime>(COMPNAME_ZoneTime,IID_IZoneTime);
	assert(pZoneTime);
	
	if(! pZoneTime->IsInSameDay( node.dwLastChgTime,pZoneTime->GetCurSecond() ) )
	{
		node.iGetType = GSProto::en_PhyStrengthType_None;
		node.bHaveGive = false;
		node.dwLastChgTime = pZoneTime->GetCurSecond();
	}
}


bool FriendSystem::createComplete()
{
	return true;
}

const std::vector<Uint32>& FriendSystem::getSupportMessage()
{
	static std::vector<Uint32> msgVec;
	if(0 == msgVec.size())
	{
		msgVec.push_back( GSProto::CMD_FRIENDSYSTEM_QUERY);
		msgVec.push_back( GSProto::CMD_FRIENDSYSTEM_DELETE_FRIEND);
		msgVec.push_back( GSProto::CMD_FRIENDSYSTEM_SEARCH_FRIEND);
		msgVec.push_back( GSProto::CMD_FRIENDSYSTEM_ADDORDELET_REQUESTFRIEND);
		msgVec.push_back( GSProto::CMD_FRIENDSYSTEM_QUERY_FRIENDLIST);
		msgVec.push_back( GSProto::CMD_FRIENDSYSTEM_QUERY_REQUESTLIST);
		msgVec.push_back( GSProto::CMD_FRIENDSYSTEM_GIVE_PHYSTRENGTH);
		msgVec.push_back( GSProto::CMD_FRIENDSYSTEM_GET_PHYSTENGTH);
		msgVec.push_back( GSProto::CMD_FRIENDSYSTEM_REQUEST);
		//查看玩家信息
		msgVec.push_back( GSProto::CMD_USENAME_FINED_ACTOR_INFO);
		msgVec.push_back( GSProto::CMD_USEPBPKROLE_FINED_ACTOR_INFO);

		msgVec.push_back( GSProto::CMD_FRIENDSYSTEM_ONEKEY_GET_STRENGTH);
	}
	return msgVec;
}

void FriendSystem::onMessage(QxMessage* pMessage)
{
	assert(pMessage->dwMsgLen == sizeof(GSProto::CSMessage));
	const GSProto::CSMessage& msg = *(GSProto::CSMessage*)(pMessage->pMsgDataBuff);
	int icmd = msg.icmd();
	
	if( (icmd != GSProto::CMD_USENAME_FINED_ACTOR_INFO) && (icmd != GSProto::CMD_USEPBPKROLE_FINED_ACTOR_INFO ))
	{
		bool bRes = canComein();
		if(!bRes)
		{
			IEntity *pEntity = getEntityFromHandle(m_hEntity);
			pEntity->sendErrorCode(ERROR_FRIEND_NOOPEN);
			return;
		}
	}
	
	switch(icmd)
	{
		case GSProto::CMD_FRIENDSYSTEM_QUERY:
			{
				onQueryFriendSystemInfo(msg);
			}break;
		
		case GSProto::CMD_FRIENDSYSTEM_DELETE_FRIEND:
			{
				onDeleteFriend(msg);
			}break;
		case GSProto::CMD_FRIENDSYSTEM_SEARCH_FRIEND:
			{
				onSearchFriend(msg);
			}break;
		
		case GSProto::CMD_FRIENDSYSTEM_ADDORDELET_REQUESTFRIEND:
			{
				onAgreeOrDeleteRequest(msg);
			}break;
		
		case GSProto::CMD_FRIENDSYSTEM_QUERY_FRIENDLIST:
			{
				onQueryFriendList(msg);	
			}break;
		
		case GSProto::CMD_FRIENDSYSTEM_QUERY_REQUESTLIST:
			{
				onQueryRequestList(msg);	
			}break;
		
		case GSProto::CMD_FRIENDSYSTEM_GIVE_PHYSTRENGTH:
			{
				onGivePhyStrength(msg);
			}break;
		
		case GSProto::CMD_FRIENDSYSTEM_GET_PHYSTENGTH:
			{
				onGetPhyStrength(msg);
			}break;

		case GSProto::CMD_FRIENDSYSTEM_REQUEST:
			{
				onRequestFriend(msg);
			}break;
		case GSProto::CMD_USENAME_FINED_ACTOR_INFO:
			{
				onUseNameFindActorInfo(msg);
			}break;
		case GSProto::CMD_USEPBPKROLE_FINED_ACTOR_INFO:
			{
				onUsePBPkRoleFindActorInfo(msg);
			}break;
		case GSProto::CMD_FRIENDSYSTEM_ONEKEY_GET_STRENGTH:
			{
				onOneKeyGetStrength(msg);
			}break;
	}
}

void  FriendSystem::onOneKeyGetStrength(const GSProto::CSMessage& msg)
{
	IEntity* pEntity = getEntityFromHandle(m_hEntity);
	assert(pEntity);
	IVIPFactory* factory = getComponent<IVIPFactory>(COMPNAME_VIPFactory,IID_IVIPFactory);
	assert(factory);
	int phyStrengthGetLimitTimes = factory->getVipPropByHEntity(m_hEntity,VIP_PROP_GETFRIEND_PHYSTRENGTH_TIMES);
	
	//达到了领取上限，不能继续领取
	if( phyStrengthGetLimitTimes <= m_FriendData.iGetPhyStrengthTimes)
	{
		pEntity->sendErrorCode(ERROR_OVER_GETLIMITTIMES);
		return;
	}

	//体力值加
	IGlobalCfg* pGlobal = getComponent<IGlobalCfg>(COMPNAME_GlobalCfg,IID_IGlobalCfg);
	assert(pGlobal);
	int iGetCount =  pGlobal->getInt("好友领取体力值",2);
	int iTotal = 0;
	ResetData( m_FriendData);
	std::map<string, ServerEngine::FriendNode>::iterator iter = m_FriendData.m_FriendMap.begin();
	for(; iter != m_FriendData.m_FriendMap.end(); ++iter)
	{
		resetFriendNode(iter->second);
		if(iter->second.iGetType != GSProto::en_PhyStrengthType_Have)
		{
			continue;
		}
		std::map<string,bool>::iterator iterHaveGet = m_FriendData.m_haveGetActorMap.find(iter->second.strFriendName);
		if(iterHaveGet != m_FriendData.m_haveGetActorMap.end())
		{
			iter->second.iGetType = GSProto::en_PhyStrengthType_Have;
			continue;
		}
		m_FriendData.iGetPhyStrengthTimes ++;
		if(phyStrengthGetLimitTimes <= m_FriendData.iGetPhyStrengthTimes)
		{
			break;
		}
		iTotal += 1;
		iter->second.iGetType = GSProto::en_PhyStrengthType_HaveGet;
		m_FriendData.m_haveGetActorMap[iter->second.strFriendName] = true;
	}

	iGetCount = iGetCount* iTotal;
	pEntity->changeProperty( PROP_ENTITY_PHYSTRENGTH,iGetCount,GSProto::en_Reason_FriendGivePhyStrength);

	GSProto::CMD_FRIENDSYSTEM_GET_PHYSTENGTH_SC s2cMsg;
	fillFriendPhyStrengthInfo(*s2cMsg.mutable_detail());
	pEntity->sendMessage(GSProto::CMD_FRIENDSYSTEM_GET_PHYSTENGTH,s2cMsg);
	checkNotice();
}

struct FindAcotrInfoDelegate
{
	FindAcotrInfoDelegate(FriendSystem* system, std::string name,HEntity hEntity ,bool isUseName)
		:m_pFriendSystem(system),
		m_name(name),
		m_hEntity(hEntity),
		m_bIsUseName(isUseName)
		{
		}
	virtual void operator()(int iRet, HEntity hEntity, ServerEngine::PIRole& roleInfo)
		{
			if( iRet != ServerEngine::en_RoleRet_OK)
			{
				return;
			}

			if( 0 != hEntity)
			{
				IEntity* pActorEntity = getEntityFromHandle(hEntity);
				sendFindActorInfo(pActorEntity);
			}
			else
			{

				//离线创建 Gost
				IJZEntityFactory* pEntityFactory = getComponent<IJZEntityFactory>(COMPNAME_EntityFactory, IID_IJZEntityFactory);
				assert(pEntityFactory);

				ServerEngine::ActorCreateContext actorCtx;
				actorCtx.strAccount = roleInfo.strAccount;
				ServerEngine::JceToObj(roleInfo.roleData, actorCtx.data);

				string strGhostCreateInfo = ServerEngine::JceToStr(actorCtx);
				IEntity* pGhost = pEntityFactory->getEntityFactory()->createEntity("Ghost", strGhostCreateInfo);
				if(!pGhost) return ;

				sendFindActorInfo(pGhost);

				delete pGhost;

				/*IUserStateManager* pUserStateManager = getComponent<IUserStateManager>(COMPNAME_LoginManager,IID_IUserStateManager);
				assert(pUserStateManager);
				pUserStateManager->kickUser(roleInfo.strAccount);*/
			
			}
		}
	
	void packHeroBaseData(IEntity* pHero, GSProto::HeroBaseData& scHeroBase)
	{
		int iTmpHeroID = pHero->getProperty(PROP_ENTITY_BASEID, 0);
		int iLevel = pHero->getProperty(PROP_ENTITY_LEVEL, 0);
		int iLevelStep = pHero->getProperty(PROP_ENTITY_LEVELSTEP, 0);
		int iFightValue = pHero->getProperty(PROP_ENTITY_FIGHTVALUE, 0);
		int iMaxHP = pHero->getProperty(PROP_ENTITY_MAXHP, 0);
		int iAtt = pHero->getProperty(PROP_ENTITY_ATT, 0);
		int iquality = pHero->getProperty( PROP_ENTITY_QUALITY, 0);
		
		scHeroBase.set_dwobjectid(pHero->getHandle() );
		scHeroBase.set_iheroid(iTmpHeroID);
		scHeroBase.set_ilevel(iLevel);
		scHeroBase.set_ilevelstep(iLevelStep);
		scHeroBase.set_ifightvalue(iFightValue);
		scHeroBase.set_imaxhp(iMaxHP);
		scHeroBase.set_iatt(iAtt);
		scHeroBase.set_iquality(iquality);
	}

	void sendFindActorInfo(IEntity* pActorEntity)
	{
		GSProto::CMD_FINED_ACTOR_INFO_SC s2cMsg;
		GSProto::ActorInfoDetail& detail = *s2cMsg.mutable_detail();
		detail.set_stractorname(m_name);
		int level = pActorEntity->getProperty(PROP_ENTITY_LEVEL,0);
		detail.set_iactorlevel(level);
		int iHeadId = pActorEntity->getProperty( PROP_ENTITY_ACTOR_HEAD,0);
		detail.set_iheadid(iHeadId);
		//竞技场排名
		IArenaFactory* arenaFactory = getComponent<IArenaFactory>(COMPNAME_ArenaFactory,IID_IArenaFactory);
		assert(arenaFactory);
		int rank = arenaFactory->getArenaRank(m_name);
		detail.set_iareanrank(rank);

		//军团归属
		ILegionFactory* pLegionFactory = getComponent<ILegionFactory>(COMPNAME_LegionFactory,IID_ILegionFactory);
		assert(pLegionFactory);
		
		ILegion* pLengion = pLegionFactory->getActorLegion(m_name);
		string belongName = "";
		if(pLengion)
		{
			belongName = pLengion->getLegionName();
			detail.set_strbelonglegionname(belongName);
		}
		
		//总战斗力
		int totalFightValue = pActorEntity->getProperty(PROP_ENTITY_FIGHTVALUE,0);
		detail.set_itotalfightstrength(totalFightValue);
		//FDLOG("ArenaSystem::pushArenaInfo")<<m_name<<" | "<<totalFightValue<<endl;
		//英雄详情
		//1.获取出阵列表
		IFormationSystem* pFormationSystem = static_cast<IFormationSystem*>( pActorEntity->querySubsystem(IID_IFormationSystem));
		assert(pFormationSystem);
		vector<HEntity> hEntityVec;
		pFormationSystem->getEntityList(hEntityVec);
		for(size_t i = 0; i < hEntityVec.size(); ++i)
		{
			HEntity hTmpEntity = hEntityVec[i];
			IEntity* pTmpHero = getEntityFromHandle(hTmpEntity);
			if(!pTmpHero) continue;
			GSProto::HeroBaseData& scHeroBase = *detail.add_szherolist();
			packHeroBaseData(pTmpHero,scHeroBase);
			int ipos = pFormationSystem->getEntityPos(hTmpEntity);
			scHeroBase.set_ipos(ipos );
		}

		IEntity* pEntity = getEntityFromHandle(m_hEntity);
		assert(pEntity);
		if(m_bIsUseName)
		{
		  pEntity->sendMessage(GSProto::CMD_USENAME_FINED_ACTOR_INFO,s2cMsg);
		}
		else
		{
			pEntity->sendMessage(GSProto::CMD_USEPBPKROLE_FINED_ACTOR_INFO,s2cMsg);
		}
	}
	
private:
	FriendSystem* m_pFriendSystem;
	std::string m_name;
	HEntity m_hEntity;
	bool m_bIsUseName;
};

struct FindNameDesc
{
	FindNameDesc(FriendSystem* system, std::string name,HEntity hEntity)
		:m_pFriendSystem(system),
		m_name(name),
		m_hEntity(hEntity)
		{
		}
	virtual void operator()(taf::Int32 ret,  const ServerEngine::NameDesc& descInfo)
		{
			if( ret != ServerEngine::en_NameRet_OK)
			{
				return;
			}
	
			ServerEngine::PKRole rolekey;
			rolekey.worldID = descInfo.wWolrdID;
			rolekey.rolePos = descInfo.iRolePos;
			rolekey.strAccount = descInfo.sAccount;
			//拿玩家数据
			IModifyDelegate* pModifyDelegate = getComponent<IModifyDelegate>(COMPNAME_ModifyDelegate, IID_IModifyDelegate);
			assert(pModifyDelegate);

			pModifyDelegate->submitDelegateTask(rolekey,FindAcotrInfoDelegate(m_pFriendSystem,m_name,m_hEntity,true));
		}
private:
	FriendSystem* m_pFriendSystem;
	std::string m_name;
	HEntity m_hEntity;
};

void FriendSystem::onUseNameFindActorInfo(const GSProto::CSMessage& msg)
{
	GSProto::CMD_USENAME_FINED_ACTOR_INFO_CS c2sMsg;
	if(!c2sMsg.ParseFromString(msg.strmsgbody()))
	{
		return;
	}

	// 可能玩家在线(包括机器人)
	IUserStateManager* pUserStateMgr = getComponent<IUserStateManager>(COMPNAME_LoginManager, IID_IUserStateManager);
	assert(pUserStateMgr);

	HEntity hTarget = pUserStateMgr->getRichEntityByName(c2sMsg.stractorname() );
	if(0 != hTarget)
	{
		FindAcotrInfoDelegate tmp(this, c2sMsg.stractorname(), m_hEntity, true);

		IEntity* pTarget = getEntityFromHandle(hTarget);
		assert(pTarget);
		
		tmp.sendFindActorInfo(pTarget);
		return;
	}

	//名字服拿数据
	IJZMessageLayer* pMsgLayer = getComponent<IJZMessageLayer>(COMPNAME_MessageLayer, IID_IJZMessageLayer);
	assert(pMsgLayer);
	try
	{
		pMsgLayer->AsyncGetNameDesc(new NameDescCallback( FindNameDesc( this,c2sMsg.stractorname(),m_hEntity) ) ,c2sMsg.stractorname());
	}
	catch(...)
	{
		IEntity* pEntity = getEntityFromHandle(m_hEntity);
		assert(pEntity);
		pEntity->sendErrorCode(ERROR_SYSERROR);
	}
	
}

void FriendSystem::onUsePBPkRoleFindActorInfo(const GSProto::CSMessage& msg)
{
	GSProto::CMD_USEPBPKROLE_FINED_ACTOR_INFO_CS c2sMsg;
	if(!c2sMsg.ParseFromString(msg.strmsgbody()))
	{
		return;
	}
	
	//拿玩家数据
	IModifyDelegate* pModifyDelegate = getComponent<IModifyDelegate>(COMPNAME_ModifyDelegate, IID_IModifyDelegate);
	assert(pModifyDelegate);

	ServerEngine::PKRole rolekey;
	GSProto::PBPkRole pkRole = c2sMsg.rolekey();
	rolekey.worldID = pkRole.iworldid();
	rolekey.strAccount = pkRole.straccount();
	pModifyDelegate->submitDelegateTask(rolekey,FindAcotrInfoDelegate(this,c2sMsg.stractorname(),m_hEntity,false));
}

struct FriendDataDelegate
{
public:
	FriendDataDelegate(HEntity hEntity, int size,ServerEngine::FriendNode node,bool bIsFriend,int iCmd)
		:m_hEntity(hEntity),
		m_size(size),
		m_SEFriendNode(node),
		m_bIsFriend(bIsFriend),
		m_iCmd(iCmd)
		{
		}
	virtual void operator()(int iRet, HEntity hEntity, ServerEngine::PIRole& roleInfo)
		{

			IEntity *pEntity = getEntityFromHandle(m_hEntity);
			if(!pEntity)
			{
				return;
			}

			FriendSystem* pFriendSystem = static_cast<FriendSystem*>(pEntity->querySubsystem(IID_IFriendSystem));
			assert(pFriendSystem);
			//错误
			if( ServerEngine::en_RoleRet_OK != iRet)
			{
				SvrErrLog("FriendSystem Get Role ERROR |%s|",m_SEFriendNode.strFriendName.c_str());
				return;
			}
			if(GSProto::CMD_FRIENDSYSTEM_QUERY == m_iCmd)
			{
				pFriendSystem->fillFriendNodeInfo(hEntity,roleInfo,m_size,m_bIsFriend,m_SEFriendNode);
			}
			if(GSProto::CMD_FRIENDSYSTEM_QUERY_FRIENDLIST == m_iCmd)
			{
				pFriendSystem->fillFriendListMsg(hEntity,roleInfo,m_size,m_SEFriendNode);
			}
			if(GSProto::CMD_FRIENDSYSTEM_QUERY_REQUESTLIST == m_iCmd)
			{
				pFriendSystem->fillRequestListMsg(hEntity,roleInfo,m_size,m_SEFriendNode);
			}
		}
private:
	HEntity m_hEntity;
	int m_size;
	ServerEngine::FriendNode m_SEFriendNode;
	bool m_bIsFriend;
	int m_count;
	int m_iCmd;
};

void FriendSystem::fillFriendNodeInfo(HEntity hEntity, ServerEngine::PIRole& roleInfo,int size,bool bIsFriend,ServerEngine::FriendNode node)
{
	
	GSProto::FriendNodeInfo& msgFriendNode =  *m_msgFriendSystemQueryBody.add_szfriendlist();
	//在线
	if( 0 != hEntity)
	{
		IEntity* pEntity = getEntityFromHandle(hEntity);
		assert(pEntity);

		IFormationSystem* formationSystem = static_cast<IFormationSystem*>( pEntity->querySubsystem(IID_IFormationSystem) );
		assert(formationSystem);
		
		msgFriendNode.set_ivisibleheroid(formationSystem->getVisibleHeroID());
		msgFriendNode.set_strfriendname(node.strFriendName);
		int actorLevel = pEntity->getProperty(PROP_ENTITY_LEVEL,0);
		msgFriendNode.set_ifriendlevel(actorLevel);
		msgFriendNode.set_bisonline(true);
		if(bIsFriend)
		{
			if( m_FriendData.m_haveGetActorMap.find(node.strFriendName) != m_FriendData.m_haveGetActorMap.end())
			{
				node.iGetType = GSProto::en_PhyStrengthType_HaveGet;
			}
			
			if( m_FriendData.m_haveGiveActorMap.find(node.strFriendName) != m_FriendData.m_haveGiveActorMap.end())
			{
				node.bHaveGive = true;
			}
			msgFriendNode.set_igettype(node.iGetType);
			msgFriendNode.set_ihavegive(node.bHaveGive);
		}
	}
	//不在线
	else
	{
		ServerEngine::RoleSaveData tmpData;
		ServerEngine::JceToObj(roleInfo.roleData, tmpData);
		
		map<int ,int>::iterator iter =  tmpData.basePropData.roleIntPropset.find(PROP_ENTITY_LEVEL);
		assert(iter != tmpData.basePropData.roleIntPropset.end() );
		int value = iter->second;
		
		msgFriendNode.set_strfriendname(node.strFriendName);
		msgFriendNode.set_ifriendlevel(value);
		msgFriendNode.set_bisonline(false);
		
		ServerEngine::FormationSystemData lFormationData;
		if( tmpData.subsystemData[IID_IFormationSystem].size()>0)
		{
			ServerEngine::JceToObj(tmpData.subsystemData[IID_IFormationSystem], lFormationData);
			msgFriendNode.set_ivisibleheroid(lFormationData.iVisibleHeroId);
		}
		else
		{
			SvrErrLog("FriendSystem Get IID_IFormationSystem DATA ERROR |%s|",node.strFriendName.c_str());
			return;
		}
		
		if(bIsFriend)
		{
			if( m_FriendData.m_haveGetActorMap.find(node.strFriendName) != m_FriendData.m_haveGetActorMap.end())
			{
				node.iGetType = GSProto::en_PhyStrengthType_HaveGet;
			}
			
			if( m_FriendData.m_haveGiveActorMap.find(node.strFriendName) != m_FriendData.m_haveGiveActorMap.end())
			{
				node.bHaveGive = true;
			}
			
			msgFriendNode.set_igettype(node.iGetType);
			msgFriendNode.set_ihavegive(node.bHaveGive);
		}
	}
	
	if(m_msgFriendSystemQueryBody.szfriendlist_size() ==  size)
	{

		m_msgFriendSystemQueryBody.set_bishaverequest(!bIsFriend);
		if(bIsFriend)
		{
			fillFriendPhyStrengthInfo(*m_msgFriendSystemQueryBody.mutable_detail());
		}
		
		IEntity* pEntity = getEntityFromHandle(m_hEntity);
		assert(pEntity);
		pEntity->sendMessage(GSProto::CMD_FRIENDSYSTEM_QUERY,m_msgFriendSystemQueryBody);

		m_msgFriendSystemQueryBody.Clear();
	}

}

void FriendSystem::fillFriendListMsg(HEntity hEntity, ServerEngine::PIRole& roleInfo,int size,ServerEngine::FriendNode node)
{
	
	GSProto::FriendNodeInfo& msgFriendNode =  *m_msgQueryFriendListBody.add_szfriendlist();
	//在线
	if( 0 != hEntity)
	{
		IEntity* pEntity = getEntityFromHandle(hEntity);
		assert(pEntity);

		IFormationSystem* formationSystem = static_cast<IFormationSystem*>( pEntity->querySubsystem(IID_IFormationSystem) );
		assert(formationSystem);
		
		msgFriendNode.set_ivisibleheroid(formationSystem->getVisibleHeroID());
		msgFriendNode.set_strfriendname(node.strFriendName);
		int actorLevel = pEntity->getProperty(PROP_ENTITY_LEVEL,0);
		msgFriendNode.set_ifriendlevel(actorLevel);
		msgFriendNode.set_bisonline(true);
		if( m_FriendData.m_haveGetActorMap.find(node.strFriendName) != m_FriendData.m_haveGetActorMap.end())
		{
			node.iGetType = GSProto::en_PhyStrengthType_HaveGet;
		}
		
		if( m_FriendData.m_haveGiveActorMap.find(node.strFriendName) != m_FriendData.m_haveGiveActorMap.end())
		{
			node.bHaveGive = true;
		}
		msgFriendNode.set_igettype(node.iGetType);
		msgFriendNode.set_ihavegive(node.bHaveGive);
	}
	//不在线
	else
	{
		ServerEngine::RoleSaveData tmpData;
		ServerEngine::JceToObj(roleInfo.roleData, tmpData);
		
		map<int ,int>::iterator iter =	tmpData.basePropData.roleIntPropset.find(PROP_ENTITY_LEVEL);
		assert(iter != tmpData.basePropData.roleIntPropset.end() );
		int value = iter->second;
		
		msgFriendNode.set_strfriendname(node.strFriendName);
		msgFriendNode.set_ifriendlevel(value);
		msgFriendNode.set_bisonline(false);
		
		ServerEngine::FormationSystemData lFormationData;
		if( tmpData.subsystemData[IID_IFormationSystem].size()>0)
		{
			ServerEngine::JceToObj(tmpData.subsystemData[IID_IFormationSystem], lFormationData);
			msgFriendNode.set_ivisibleheroid(lFormationData.iVisibleHeroId);
		}
		else
		{
			SvrErrLog("FriendSystem Get IID_IFormationSystem DATA ERROR |%s|",node.strFriendName.c_str());
			
			IEntity* pEntity = getEntityFromHandle(m_hEntity);
			assert(pEntity);
			pEntity->sendErrorCode(ERROR_SYSERROR);
			return;
		}

		
		if( m_FriendData.m_haveGetActorMap.find(node.strFriendName) != m_FriendData.m_haveGetActorMap.end())
		{
			node.iGetType = GSProto::en_PhyStrengthType_HaveGet;
		}
		
		if( m_FriendData.m_haveGiveActorMap.find(node.strFriendName) != m_FriendData.m_haveGiveActorMap.end())
		{
			node.bHaveGive = true;
		}
		
		msgFriendNode.set_igettype(node.iGetType);
		msgFriendNode.set_ihavegive(node.bHaveGive);
	}
	
	if(m_msgQueryFriendListBody.szfriendlist_size() ==  size)
	{
		fillFriendPhyStrengthInfo(*m_msgQueryFriendListBody.mutable_detail());
		
		IEntity* pEntity = getEntityFromHandle(m_hEntity);
		assert(pEntity);
		pEntity->sendMessage(GSProto::CMD_FRIENDSYSTEM_QUERY_FRIENDLIST,m_msgQueryFriendListBody);

		m_msgQueryFriendListBody.Clear();
	}

}
	
void FriendSystem::fillRequestListMsg(HEntity hEntity, ServerEngine::PIRole& roleInfo,int size,ServerEngine::FriendNode node)
{
	
	GSProto::FriendNodeInfo& msgFriendNode =  *m_msgQueryRequestBody.add_szfriendlist();
	//在线
	if( 0 != hEntity)
	{
		IEntity* pEntity = getEntityFromHandle(hEntity);
		assert(pEntity);

		IFormationSystem* formationSystem = static_cast<IFormationSystem*>( pEntity->querySubsystem(IID_IFormationSystem) );
		assert(formationSystem);
		
		msgFriendNode.set_ivisibleheroid(formationSystem->getVisibleHeroID());
		msgFriendNode.set_strfriendname(node.strFriendName);
		int actorLevel = pEntity->getProperty(PROP_ENTITY_LEVEL,0);
		msgFriendNode.set_ifriendlevel(actorLevel);
		msgFriendNode.set_bisonline(true);
	}
	//不在线
	else
	{
		ServerEngine::RoleSaveData tmpData;
		ServerEngine::JceToObj(roleInfo.roleData, tmpData);
		
		map<int ,int>::iterator iter =	tmpData.basePropData.roleIntPropset.find(PROP_ENTITY_LEVEL);
		assert(iter != tmpData.basePropData.roleIntPropset.end() );
		int value = iter->second;
		
		msgFriendNode.set_strfriendname(node.strFriendName);
		msgFriendNode.set_ifriendlevel(value);
		msgFriendNode.set_bisonline(false);
		
		ServerEngine::FormationSystemData lFormationData;
		if( tmpData.subsystemData[IID_IFormationSystem].size()>0)
		{
			ServerEngine::JceToObj(tmpData.subsystemData[IID_IFormationSystem], lFormationData);
			msgFriendNode.set_ivisibleheroid(lFormationData.iVisibleHeroId);
		}
		else
		{
			SvrErrLog("FriendSystem Get IID_IFormationSystem DATA ERROR |%s|",node.strFriendName.c_str());
			
			IEntity* pEntity = getEntityFromHandle(m_hEntity);
			assert(pEntity);
			pEntity->sendErrorCode(ERROR_SYSERROR);
			return;
		}
	}
	
	if(m_msgQueryRequestBody.szfriendlist_size() ==  size)
	{
		IEntity* pEntity = getEntityFromHandle(m_hEntity);
		assert(pEntity);
		pEntity->sendMessage(GSProto::CMD_FRIENDSYSTEM_QUERY_REQUESTLIST,m_msgQueryRequestBody);

		m_msgQueryRequestBody.Clear();
	}

}

void FriendSystem::fillFriendPhyStrengthInfo(GSProto::FriendPhyStrengthDetail& detail)
{
	//隔天重置
	ResetData( m_FriendData);
	//总次数待定
	//todo 
	IVIPFactory* factory = getComponent<IVIPFactory>(COMPNAME_VIPFactory,IID_IVIPFactory);
	assert(factory);
	int getTimesLimit = factory->getVipPropByHEntity(m_hEntity,VIP_PROP_GETFRIEND_PHYSTRENGTH_TIMES);
	int giveTimesLimit = factory->getVipPropByHEntity(m_hEntity,VIP_PROP_GIVEFRIEND_PHYSTRENGTH_TIEMS);
	
	detail.set_igettimes(m_FriendData.iGetPhyStrengthTimes);
	detail.set_igivetimes(m_FriendData.iGivePhyStrengthTimes);

	detail.set_itopgettimes(getTimesLimit);
	detail.set_itopgivetimes(giveTimesLimit);
	int countLimit = factory->getVipPropByHEntity(m_hEntity,VIP_PROP_FRIENDCOUNT_LIMIT);
	detail.set_ifrendcountlimit(countLimit);
}

void FriendSystem::onQueryFriendSystemInfo(const GSProto::CSMessage& msg)
{
   //获取数据
	IModifyDelegate* pModifyDelegate = getComponent<IModifyDelegate>(COMPNAME_ModifyDelegate, IID_IModifyDelegate);
	assert(pModifyDelegate);

	//有请求下发请求列表，没有请求发好友列表
	if(m_FriendData.m_RequestMap.size() > 0)
	{
		
		if(m_FriendData.m_RequestMap.size()>=30)
		{	
			m_FriendData.m_RequestMap.clear();	
		}
		std::map<std::string, ServerEngine::FriendNode>::iterator iter = m_FriendData.m_RequestMap.begin();
		for(; iter != m_FriendData.m_RequestMap.end(); ++iter)
		{
			pModifyDelegate->submitDelegateTask(iter->second.roleKey,FriendDataDelegate(m_hEntity,m_FriendData.m_RequestMap.size(),iter->second,false,GSProto::CMD_FRIENDSYSTEM_QUERY));
		}
	}
	else
	{
		if(m_FriendData.m_FriendMap.size() == 0)
		{
			GSProto::CMD_FRIENDSYSTEM_QUERY_SC msgBody;
			fillFriendPhyStrengthInfo(*msgBody.mutable_detail());
			IEntity* pEntity = getEntityFromHandle(m_hEntity);
			assert(pEntity);
			pEntity->sendMessage(GSProto::CMD_FRIENDSYSTEM_QUERY,msgBody);
			return;
		}
		
		std::map<std::string,ServerEngine::FriendNode>::iterator iter = m_FriendData.m_FriendMap.begin();
		for( ; iter !=  m_FriendData.m_FriendMap.end(); ++iter)
		{
			//检查是否需要重置数据
			resetFriendNode(iter->second);
			pModifyDelegate->submitDelegateTask(iter->second.roleKey,FriendDataDelegate(m_hEntity,m_FriendData.m_FriendMap.size(),iter->second,true,GSProto::CMD_FRIENDSYSTEM_QUERY));
		}
		
	}
	
}

struct DeleteFriend
{

	DeleteFriend(HEntity hEntity,string name)
		:m_hEntity(hEntity),
		m_name(name)
	{
	}

	virtual void operator()(int iRet, HEntity hEntity, ServerEngine::PIRole& roleInfo)
	{
		IEntity *pEntity = getEntityFromHandle(m_hEntity);
		if(!pEntity) return;
		
		FriendSystem* pDelegate = static_cast<FriendSystem*>(pEntity->querySubsystem(IID_IFriendSystem));
		if(!pDelegate) return;
		pDelegate->deleteFriend(iRet,hEntity, roleInfo,m_name);
	}
	
private:
	//;
	HEntity m_hEntity;
	std::string m_name;
};

void FriendSystem::deleteFriend(int iRet,HEntity hEntity, ServerEngine::PIRole& roleInfo,string strName)
{
	IEntity* pEntity = getEntityFromHandle(m_hEntity);
	assert(pEntity);
	
	if( ServerEngine::en_RoleRet_OK != iRet)
	{
		SvrErrLog("FriendSystem Get Role ERROR |%s|",strName.c_str());
		pEntity->sendErrorCode(ERROR_SYSERROR);
		return;
	}
	map<string,ServerEngine::FriendNode>::iterator iter = m_FriendData.m_FriendMap.find(strName);
	if( iter == m_FriendData.m_FriendMap.end())
	{
		return;
	}
	
	//对方在线
	if( 0 != hEntity)
	{
		IEntity *lTargetEntity = getEntityFromHandle(hEntity);
		assert(lTargetEntity);
		IFriendSystem* pFriendSystem = static_cast<IFriendSystem*>(lTargetEntity->querySubsystem(IID_IFriendSystem));
		assert(pFriendSystem);
		//从对方列表中删除然后删除自己的
		pFriendSystem->deleteFriend(pEntity->getProperty(PROP_ENTITY_NAME,""));
		m_FriendData.m_FriendMap.erase(iter);
		pEntity->sendMessage(GSProto::CMD_FRIENDSYSTEM_DELETE_FRIEND);
		
	}
	else
	{
		ServerEngine::RoleSaveData tmpData;
		ServerEngine::JceToObj(roleInfo.roleData, tmpData);

		ServerEngine::FriendSystemData lFriendData;
		ServerEngine::JceToObj(tmpData.subsystemData[IID_IFriendSystem],lFriendData);
		
		map<string,ServerEngine::FriendNode>::iterator iter2  = lFriendData.m_FriendMap.find( pEntity->getProperty(PROP_ENTITY_NAME,"") );
		if(iter2 == lFriendData.m_FriendMap.end())
		{
			return;
		}
		
		lFriendData.m_FriendMap.erase(iter2);
		//回写数据
		tmpData.subsystemData[IID_IFriendSystem] = ServerEngine::JceToStr(lFriendData);
		roleInfo.roleData = ServerEngine::JceToStr(tmpData);

		//在自己的列表中把对方删掉
		m_FriendData.m_FriendMap.erase(iter);
		
		//通知客户端
		pEntity->sendMessage(GSProto::CMD_FRIENDSYSTEM_DELETE_FRIEND);
	}
	
}

bool FriendSystem::deleteFriend(string name)
{
	map<string,ServerEngine::FriendNode>::iterator iter  = m_FriendData.m_FriendMap.find(name);
	if(iter != m_FriendData.m_FriendMap.end())
	{
		m_FriendData.m_FriendMap.erase(iter);
		return true;
	}
	return false;
}

void FriendSystem::onDeleteFriend(const GSProto::CSMessage& msg)
{
	GSProto::CMD_FRIENDSYSTEM_DELETE_FRIEND_CS c2smsg;
	if( !c2smsg.ParseFromString(msg.strmsgbody()) )
	{	
		return;
	}

	//是否合法
	map<string,ServerEngine::FriendNode>::iterator iter = m_FriendData.m_FriendMap.find(c2smsg.strname());
	if( iter == m_FriendData.m_FriendMap.end())
	{
		IEntity *pEntity = getEntityFromHandle(m_hEntity);
		pEntity->sendErrorCode(ERROR_FRIENDSYSTEM_NOT_FRIEND);
		return;
	}

	//获取数据
	IModifyDelegate* pModifyDelegate = getComponent<IModifyDelegate>(COMPNAME_ModifyDelegate, IID_IModifyDelegate);
	assert(pModifyDelegate);

	pModifyDelegate->submitDelegateTask(iter->second.roleKey,DeleteFriend(m_hEntity,iter->second.strFriendName));
	
}

void FriendSystem::onSearchFriend(const GSProto::CSMessage& msg)
{
	//推荐列表
	IUserStateManager* userManager = getComponent<IUserStateManager>(COMPNAME_LoginManager,IID_IUserStateManager);
	assert(userManager);
	std::vector<HEntity> hEntityVec;
	userManager->getRecommendList(hEntityVec);
	
	IGlobalCfg* pGlobal = getComponent<IGlobalCfg>( COMPNAME_GlobalCfg,IID_IGlobalCfg);
	assert(pGlobal);
	int recommondCount = pGlobal->getInt("好友推荐列表人数",8);

	IRandom* pRandom = getComponent<IRandom>("Random", IID_IMiniAprRandom);
	assert(pRandom);
	
	int i = 0;
	GSProto::CMD_FRIENDSYSTEM_SEARCH_FRIEND_SC s2cMsgBody;
	while( i < recommondCount)
	{	
		int count = hEntityVec.size();
		//没有玩家或者玩家数为 0
		if( count == 0 )
		{
			break;
		}
		int index = pRandom->random()%count;
		//去掉自己
		HEntity hEntity = hEntityVec[index];
		hEntityVec.erase( hEntityVec.begin() + index);
		if(hEntity == m_hEntity)
		{
			continue;	
		}

		//不在线的不要
		IEntity* pEntity = getEntityFromHandle(hEntity);
		if(0 == pEntity)
		{
			continue;
		}
		//已经是好友的不要
		std::string strName = pEntity->getProperty(PROP_ENTITY_NAME,"");
		if( m_FriendData.m_FriendMap.find(strName) != m_FriendData.m_FriendMap.end())
		{
			continue;
		}
		else
		{
			//已经请求的不要
			if( m_FriendData.m_RequestMap.find(strName) != m_FriendData.m_RequestMap.end())
			{
				continue;
			}
			else
			{

				int actorLevel = pEntity->getProperty(PROP_ENTITY_LEVEL,0);
				//未开启的好友不能加
				int openLevel  = pGlobal->getInt("好友系统开启等级",15);
				if(actorLevel < openLevel )
				{
					continue;
				}
				
				IFormationSystem* formationSystem = static_cast<IFormationSystem*>( pEntity->querySubsystem(IID_IFormationSystem) );
				assert(formationSystem);
				GSProto::FriendNodeInfo& node = *s2cMsgBody.add_szsearchresoultlist();
				node.set_ivisibleheroid(formationSystem->getVisibleHeroID());
				node.set_strfriendname(strName);
				node.set_ifriendlevel(actorLevel);
				node.set_bisonline(true);
				++i;
			}
		}
	}

	IEntity* pEntity = getEntityFromHandle(m_hEntity);
	assert(pEntity);
	pEntity->sendMessage( GSProto::CMD_FRIENDSYSTEM_SEARCH_FRIEND,s2cMsgBody);
}

struct AddFriend
{
	AddFriend(ServerEngine::FriendNode node,HEntity hEntity,ServerEngine::FriendNode myNode)
		:m_node(node),
		m_hEntity(hEntity),
		m_Mynode(myNode)
	{}
	virtual void operator()(int iRet, HEntity hEntity, ServerEngine::PIRole& roleInfo)
	{
		IEntity* pEntity = getEntityFromHandle(m_hEntity);
		assert(pEntity);
		
		if( ServerEngine::en_RoleRet_OK != iRet)
		{
			pEntity->sendErrorCode(ERROR_SYSERROR);
			return;
		}
		
		if(0 != hEntity)
		{
			IEntity* pFriendEntity = getEntityFromHandle(hEntity);
			IFriendSystem* pFriendSystem = static_cast<IFriendSystem*>( pFriendEntity->querySubsystem(IID_IFriendSystem));
			assert(pFriendSystem);
			bool res = pFriendSystem->addFriendToMap(m_node.strFriendName,m_node);
			IEntity* pMyEntity = getEntityFromHandle(m_hEntity);
			IFriendSystem* pMyFriendSystem = static_cast<IFriendSystem*>( pMyEntity->querySubsystem(IID_IFriendSystem));
			assert(pMyFriendSystem);
				
			if(!res)
			{
				pMyFriendSystem->deleteFriend(m_node.strFriendName);
				pMyEntity->sendErrorCode(ERROR_FRIEND_FRIEND_LIMIT);
				return;
			}

			pMyFriendSystem->addFriendToMap(m_Mynode.strFriendName, m_Mynode);
			
		}
		else
		{
			ServerEngine::RoleSaveData data; 
			ServerEngine::JceToObj(roleInfo.roleData ,data);

			ServerEngine::FriendSystemData temp;
			if(data.subsystemData[IID_IFriendSystem].size() == 0)
			{
				return;
			}
			ServerEngine::JceToObj(data.subsystemData[IID_IFriendSystem],temp);
			if(temp.m_FriendMap.find( m_node.strFriendName )!= temp.m_FriendMap.end())
			 {
				pEntity->sendErrorCode(ERROR_HAS_FRIEND);
				//从请求列表中删除 
				pEntity->sendMessage(GSProto::CMD_FRIENDSYSTEM_ADDORDELET_REQUESTFRIEND);
				
			 	return;
			 }

			int iFriendCount = temp.m_FriendMap.size();
			IVIPFactory* factory = getComponent<IVIPFactory>(COMPNAME_VIPFactory,IID_IVIPFactory);
			assert(factory);
			int iFriendLimit = factory->getVipPropByHEntity(m_hEntity, VIP_PROP_FRIENDCOUNT_LIMIT);
			assert(iFriendLimit > 0);
			if(iFriendCount >=iFriendLimit )
			{
				pEntity->sendErrorCode(ERROR_FRIEND_FRIEND_LIMIT);
				
				//从请求列表中删除 
				pEntity->sendMessage(GSProto::CMD_FRIENDSYSTEM_ADDORDELET_REQUESTFRIEND);

			 	return;
			}
			 
			IZoneTime* pZoneTime = getComponent<IZoneTime>(COMPNAME_ZoneTime,IID_IZoneTime);
			assert(pZoneTime);
			
			m_node.dwLastChgTime =  pZoneTime->GetCurSecond();
			m_node.iGetType = GSProto::en_PhyStrengthType_None;
			m_node.bHaveGive = false;
			
			temp.m_FriendMap[m_node.strFriendName] = m_node; 

			data.subsystemData[IID_IFriendSystem] = ServerEngine::JceToStr(temp);
			roleInfo.roleData = ServerEngine::JceToStr(data);

			IEntity* pMyEntity = getEntityFromHandle(m_hEntity);
			IFriendSystem* pMyFriendSystem = static_cast<IFriendSystem*>( pMyEntity->querySubsystem(IID_IFriendSystem));
			assert(pMyFriendSystem);
			pMyFriendSystem->addFriendToMap(m_Mynode.strFriendName, m_Mynode);
		}
		
		//从请求列表中删除 
		pEntity->sendMessage(GSProto::CMD_FRIENDSYSTEM_ADDORDELET_REQUESTFRIEND);
		IFriendSystem* pMyFriendSystem = static_cast<IFriendSystem*>( pEntity->querySubsystem(IID_IFriendSystem));
		assert(pMyFriendSystem);
		pMyFriendSystem->checkNotice();
	}

private:
	ServerEngine::FriendNode m_node; 
	HEntity m_hEntity;
	ServerEngine::FriendNode m_Mynode; 
};
bool FriendSystem::deleteRequest(string strName)
{
	std::map<string,ServerEngine::FriendNode>::iterator iter = m_FriendData.m_RequestMap.find(strName);

	if( iter != m_FriendData.m_RequestMap.end())
	{
		m_FriendData.m_RequestMap.erase(iter);
		return true;
	}
	return false;
}

bool FriendSystem::addFriendToMap(string name, ServerEngine::FriendNode node)
{
	if(m_FriendData.m_FriendMap.find(name) !=  m_FriendData.m_FriendMap.end())
	{
		return false;
	}

	IVIPFactory* factory = getComponent<IVIPFactory>(COMPNAME_VIPFactory,IID_IVIPFactory);
	assert(factory);
	unsigned int iFriendLimit = factory->getVipPropByHEntity(m_hEntity, VIP_PROP_FRIENDCOUNT_LIMIT);
	assert(iFriendLimit > 0);
	if(iFriendLimit <= m_FriendData.m_FriendMap.size())
	{
		return false;
	}
	
	IZoneTime* pZoneTime = getComponent<IZoneTime>(COMPNAME_ZoneTime,IID_IZoneTime);
	assert(pZoneTime);
	
	node.dwLastChgTime =  pZoneTime->GetCurSecond();
	node.iGetType = 0;
	node.bHaveGive = 0;
	m_FriendData.m_FriendMap[name] = node;
	return true;
}

void FriendSystem::onAgreeOrDeleteRequest(const GSProto::CSMessage& msg)
{
	GSProto::CMD_FRIENDSYSTEM_ADDORDELET_REQUESTFRIEND_CS c2sMsg;
	if( !c2sMsg.ParseFromString(msg.strmsgbody()))
	{
		return;
	}
	
	IZoneTime* pZoneTime = getComponent<IZoneTime>(COMPNAME_ZoneTime,IID_IZoneTime);
	assert(pZoneTime);
	
	IEntity* pEntity = getEntityFromHandle(m_hEntity);
	assert(pEntity);
	
	if(c2sMsg.bagree())
	{
		map<string, ServerEngine::FriendNode>::iterator iter = m_FriendData.m_RequestMap.find(c2sMsg.stname());
		if(iter == m_FriendData.m_RequestMap.end())
		{
			return;
		}
		
		if( m_FriendData.m_FriendMap.find(iter->second.strFriendName) != m_FriendData.m_FriendMap.end())
		{
			pEntity->sendErrorCode(ERROR_HAS_FRIEND);
			pEntity->sendMessage(GSProto::CMD_FRIENDSYSTEM_ADDORDELET_REQUESTFRIEND);
			//m_FriendData.m_RequestMap.erase(iter);
			return;
		}

		//好友人数达到上限
		int iFriendCount = m_FriendData.m_FriendMap.size();
		IVIPFactory* factory = getComponent<IVIPFactory>(COMPNAME_VIPFactory,IID_IVIPFactory);
		assert(factory);
		int iFriendLimit = factory->getVipPropByHEntity(m_hEntity, VIP_PROP_FRIENDCOUNT_LIMIT);
		assert(iFriendLimit > 0);
		if(iFriendLimit <= iFriendCount)
		{
			pEntity->sendErrorCode( ERROR_FRIENDSYSTEM_FRINEDLIMIT);
			return;
		}

		iter->second.dwLastChgTime =  pZoneTime->GetCurSecond();
		iter->second.iGetType = GSProto::en_PhyStrengthType_None;
		iter->second.bHaveGive = false;
		ServerEngine::FriendNode MyNode = iter->second;
		//m_FriendData.m_FriendMap[iter->second.strFriendName] = 
	
		///把自己加入对方的好友列表中
		IModifyDelegate* pModifyDelegate = getComponent<IModifyDelegate>(COMPNAME_ModifyDelegate, IID_IModifyDelegate);
		assert(pModifyDelegate);
		
		ServerEngine::FriendNode node;
		
		node.strFriendName =  pEntity->getProperty(PROP_ENTITY_NAME,"");
		ServerEngine::PKRole roleKey;
		roleKey.strAccount = pEntity->getProperty(PROP_ACTOR_ACCOUNT, "");
		roleKey.rolePos = pEntity->getProperty(PROP_ACTOR_ROLEPOS, 0);
		roleKey.worldID = pEntity->getProperty(PROP_ACTOR_WORLD, 0);

		node.roleKey = roleKey;
		
		pModifyDelegate->submitDelegateTask(iter->second.roleKey,AddFriend(node,m_hEntity,MyNode));
		
		deleteRequest(iter->second.strFriendName);
			
	}
	else
	{
		IEntity* pEntity = getEntityFromHandle(m_hEntity);
		assert(pEntity);
		map<string, ServerEngine::FriendNode>::iterator iter = m_FriendData.m_RequestMap.find(c2sMsg.stname());
		if(iter == m_FriendData.m_RequestMap.end())
		{
			
			return;
		}
		m_FriendData.m_RequestMap.erase(iter);
		
		pEntity->sendMessage(GSProto::CMD_FRIENDSYSTEM_ADDORDELET_REQUESTFRIEND);
		checkNotice();
	}

	
}

void FriendSystem::onQueryFriendList(const GSProto::CSMessage& msg)
{
	if(m_FriendData.m_FriendMap.size() == 0)
	{
		GSProto::CMD_FRIENDSYSTEM_QUERY_FRIENDLIST_SC msgBody;
		fillFriendPhyStrengthInfo(*msgBody.mutable_detail());
		IEntity* pEntity = getEntityFromHandle(m_hEntity);
		assert(pEntity);
		pEntity->sendMessage(GSProto::CMD_FRIENDSYSTEM_QUERY_FRIENDLIST,msgBody);
		return;
	}
	
	//获取数据
	IModifyDelegate* pModifyDelegate = getComponent<IModifyDelegate>(COMPNAME_ModifyDelegate, IID_IModifyDelegate);
	assert(pModifyDelegate);
	std::map<std::string,ServerEngine::FriendNode>::iterator iter = m_FriendData.m_FriendMap.begin();
	for( ; iter !=  m_FriendData.m_FriendMap.end(); ++iter)
	{
		//检查是否需要重置数据
		resetFriendNode(iter->second);
		pModifyDelegate->submitDelegateTask(iter->second.roleKey,FriendDataDelegate(m_hEntity,m_FriendData.m_FriendMap.size(),iter->second,true,GSProto::CMD_FRIENDSYSTEM_QUERY_FRIENDLIST));
	}
	
}

void FriendSystem::onQueryRequestList(const GSProto::CSMessage& msg)
{
	 //获取数据
	IModifyDelegate* pModifyDelegate = getComponent<IModifyDelegate>(COMPNAME_ModifyDelegate, IID_IModifyDelegate);
	assert(pModifyDelegate);
	
	if(m_FriendData.m_RequestMap.size() == 0)
	{
		GSProto::CMD_FRIENDSYSTEM_QUERY_REQUESTLIST_SC msgBody;
		//fillFriendPhyStrengthInfo(*msgBody.mutable_detail());
		IEntity* pEntity = getEntityFromHandle(m_hEntity);
		assert(pEntity);
		pEntity->sendMessage(GSProto::CMD_FRIENDSYSTEM_QUERY_REQUESTLIST,msgBody);
		return;
	}
	
	std::map<std::string, ServerEngine::FriendNode>::iterator iter = m_FriendData.m_RequestMap.begin();
	for(; iter != m_FriendData.m_RequestMap.end(); ++iter)
	{
		pModifyDelegate->submitDelegateTask(iter->second.roleKey,FriendDataDelegate(m_hEntity,m_FriendData.m_RequestMap.size(),iter->second,false,GSProto::CMD_FRIENDSYSTEM_QUERY_REQUESTLIST));
	}
}

struct GiveOrGetPhyDelegate
{
	GiveOrGetPhyDelegate(FriendSystem* system,string name):m_pSystem(system),m_name(name)
	{
	}

	virtual void operator()(int iRet, HEntity hEntity, ServerEngine::PIRole& roleInfo)
		{
			if( ServerEngine::en_RoleRet_OK != iRet)
			{
				return;
			}
			m_pSystem->onGetOrGivePhyStrength(hEntity,roleInfo,m_name);
		}
	
private:
	FriendSystem* m_pSystem;
	string m_name;
};

void FriendSystem::onGetOrGivePhyStrength(HEntity hEntity, ServerEngine::PIRole& roleInfo,string name)
{
	if(0 != hEntity)
	{
		IEntity* pEntity = getEntityFromHandle(hEntity);
		assert(pEntity);

		IFriendSystem* pFriendSystem = static_cast<IFriendSystem*>(pEntity->querySubsystem(IID_IFriendSystem));
		assert(pFriendSystem);
		
		bool res = pFriendSystem->GetOrGivePhyStrength(name);
		if(!res)
		{
			return;
		}
	}
	else
	{
		ServerEngine::RoleSaveData data; 
		ServerEngine::JceToObj(roleInfo.roleData ,data);
		
		ServerEngine::FriendSystemData temp;
		if(data.subsystemData[IID_IFriendSystem].size() == 0)
		{
			return;
		}
		ServerEngine::JceToObj(data.subsystemData[IID_IFriendSystem],temp);
		map<string,ServerEngine::FriendNode>::iterator iter = temp.m_FriendMap.find(name);
		if (iter == temp.m_FriendMap.end())
		{
			return;
		}
		resetFriendNode( iter->second);
		if( (iter->second.iGetType == GSProto::en_PhyStrengthType_HaveGet)||(iter->second.iGetType == GSProto::en_PhyStrengthType_Have) )
		{
			return;
		}
	 
		iter->second.iGetType = GSProto::en_PhyStrengthType_Have; 
		//数据回写
		data.subsystemData[IID_IFriendSystem] = ServerEngine::JceToStr(temp);
		roleInfo.roleData = ServerEngine::JceToStr(data);
	}
	
	IEntity* pEntity = getEntityFromHandle(m_hEntity);
	assert(pEntity);

	GSProto::CMD_FRIENDSYSTEM_GIVE_PHYSTRENGTH_SC s2cMsg;
	fillFriendPhyStrengthInfo(*s2cMsg.mutable_detail());
	
	pEntity->sendMessage(GSProto::CMD_FRIENDSYSTEM_GIVE_PHYSTRENGTH,s2cMsg);
	
}

bool FriendSystem::GetOrGivePhyStrength(string name)
{
	map<string,ServerEngine::FriendNode>::iterator iter = m_FriendData.m_FriendMap.find(name);
	if(iter ==  m_FriendData.m_FriendMap.end())
	{
		return false;
	}
	
	//检查是否已经过时间了
	resetFriendNode(iter->second);
	if(iter->second.iGetType != GSProto::en_PhyStrengthType_Have )
	{
		iter->second.iGetType = GSProto::en_PhyStrengthType_Have;
		return true;
	}
	return false;
}

void FriendSystem::onTimer(int nEventId)
{
	checkNotice();
}

void FriendSystem::checkNotice()
{
	bool bNeedNotice = false;
	IVIPFactory* factory = getComponent<IVIPFactory>(COMPNAME_VIPFactory,IID_IVIPFactory);
	assert(factory);
	int phyStrengthGetLimitTimes = factory->getVipPropByHEntity(m_hEntity,VIP_PROP_GETFRIEND_PHYSTRENGTH_TIMES);
	//达到了领取上限，不能继续领取
	if( phyStrengthGetLimitTimes > m_FriendData.iGetPhyStrengthTimes)
	{
		bNeedNotice = true; 
		//return;
	}
	
	if(bNeedNotice)
	{
		bNeedNotice = false;
		map<std::string, ServerEngine::FriendNode>::iterator iter = m_FriendData.m_FriendMap.begin();
		for(;iter != m_FriendData.m_FriendMap.end(); ++iter )
		{
			resetFriendNode(iter->second);
			if(iter->second.iGetType == GSProto::en_PhyStrengthType_Have)
			{
				bNeedNotice = true; 
				break;
			}
		}
	}

    int iFriendLimit =  factory->getVipPropByHEntity(m_hEntity,VIP_PROP_FRIENDCOUNT_LIMIT);
    if( (m_FriendData.m_RequestMap.size() > 0)&& (iFriendLimit > (int)m_FriendData.m_FriendMap.size()))
    {
    		bNeedNotice = true; 
    }

	IEntity *pEntity = getEntityFromHandle(m_hEntity);
	assert(pEntity);

	pEntity->chgNotice( GSProto::en_NoticeGuid_Friend,bNeedNotice);
}

void FriendSystem::onGivePhyStrength(const GSProto::CSMessage& msg)
{
	GSProto::CMD_FRIENDSYSTEM_GIVE_PHYSTRENGTH_CS  c2sMsg;

	ResetData( m_FriendData);

	if( !c2sMsg.ParseFromString(msg.strmsgbody()))
	{
		return;
	}
	ResetData( m_FriendData);
	std::map<string, ServerEngine::FriendNode>::iterator iter = m_FriendData.m_FriendMap.find(c2sMsg.stname());
	if(iter == m_FriendData.m_FriendMap.end())
	{
		return;
	}
	
	resetFriendNode(iter->second);
	if(iter->second.bHaveGive)
	{
		return;
	}

	IEntity* pEntity = getEntityFromHandle(m_hEntity);
	assert(pEntity);

	std::map<string,bool>::iterator giveActorIter =  m_FriendData.m_haveGiveActorMap.find(c2sMsg.stname());
	if(giveActorIter != m_FriendData.m_haveGiveActorMap.end())
	{
		pEntity->sendErrorCode(ERROR_FRIENDSYSTEM_HAVE_GIVESTRENGTH);
		iter->second.bHaveGive = true;
		return;
	}
	
	IVIPFactory* factory = getComponent<IVIPFactory>(COMPNAME_VIPFactory,IID_IVIPFactory);
	assert(factory);
	int phyStrengthGiveLimit = factory->getVipPropByHEntity(m_hEntity,VIP_PROP_GIVEFRIEND_PHYSTRENGTH_TIEMS);
	
	if(phyStrengthGiveLimit <= m_FriendData.iGivePhyStrengthTimes)
	{
		pEntity->sendErrorCode(ERROR_OVER_GIVELIMIT);
		return;
	}
	//赠送次数加1
	m_FriendData.iGivePhyStrengthTimes += 1;

	iter->second.bHaveGive = true;

	m_FriendData.m_haveGiveActorMap[c2sMsg.stname()] = true;
	
	//给玩家数据中添加 好友送的体力值 
	
	
	//获取数据
	IModifyDelegate* pModifyDelegate = getComponent<IModifyDelegate>(COMPNAME_ModifyDelegate, IID_IModifyDelegate);
	assert(pModifyDelegate);
	string name = pEntity->getProperty(PROP_ENTITY_NAME ,"");
	pModifyDelegate->submitDelegateTask(iter->second.roleKey,GiveOrGetPhyDelegate(this,name));

	//任务系统赠送一次体力
	 {
		EventArgs args;
		args.context.setInt("times",1);
		args.context.setInt("entity",m_hEntity);
		pEntity->getEventServer()->setEvent(EVENT_ENTITY_TASK_GIVE_Strength, args);
	}

	
}

void FriendSystem::onGetPhyStrength(const GSProto::CSMessage& msg)
{
	IEntity* pEntity = getEntityFromHandle(m_hEntity);
	assert(pEntity);
	
	GSProto::CMD_FRIENDSYSTEM_GET_PHYSTENGTH_CS c2sMsg;
	if( !c2sMsg.ParseFromString(msg.strmsgbody()))
	{
		return;
	}

	ResetData( m_FriendData);

	std::map<string, ServerEngine::FriendNode>::iterator iter = m_FriendData.m_FriendMap.find(c2sMsg.stname());
	if(iter == m_FriendData.m_FriendMap.end())
	{
		return;
	}

	resetFriendNode(iter->second);
	if(iter->second.iGetType != GSProto::en_PhyStrengthType_Have)
	{
		return;
	}

	std::map<string,bool>::iterator iterHaveGet = m_FriendData.m_haveGetActorMap.find(c2sMsg.stname());
	if(iterHaveGet != m_FriendData.m_haveGetActorMap.end())
	{

		pEntity->sendErrorCode(ERROR_FRIENDSYSTEM_HAVE_GETSTRENGTH);
		iter->second.iGetType = GSProto::en_PhyStrengthType_Have;
		return;
	}

	
	IVIPFactory* factory = getComponent<IVIPFactory>(COMPNAME_VIPFactory,IID_IVIPFactory);
	assert(factory);
	int phyStrengthGetLimitTimes = factory->getVipPropByHEntity(m_hEntity,VIP_PROP_GETFRIEND_PHYSTRENGTH_TIMES);

	//达到了领取上限，不能继续领取
	if( phyStrengthGetLimitTimes <= m_FriendData.iGetPhyStrengthTimes)
	{
		pEntity->sendErrorCode(ERROR_OVER_GETLIMITTIMES);
		return;
	}
	
	//体力值加
	IGlobalCfg* pGlobal = getComponent<IGlobalCfg>(COMPNAME_GlobalCfg,IID_IGlobalCfg);
	assert(pGlobal);
	
	int iGetCount =  pGlobal->getInt("好友领取体力值",2);
	pEntity->changeProperty( PROP_ENTITY_PHYSTRENGTH,iGetCount,GSProto::en_Reason_FriendGivePhyStrength);

	iter->second.iGetType = GSProto::en_PhyStrengthType_HaveGet;
	
	m_FriendData.m_haveGetActorMap[c2sMsg.stname()] = true;
	
	m_FriendData.iGetPhyStrengthTimes +=1;

	GSProto::CMD_FRIENDSYSTEM_GET_PHYSTENGTH_SC s2cMsg;
	fillFriendPhyStrengthInfo(*s2cMsg.mutable_detail());
	
	pEntity->sendMessage(GSProto::CMD_FRIENDSYSTEM_GET_PHYSTENGTH,s2cMsg);
	
	checkNotice();
	
}

struct NameDescDelegate
{
	NameDescDelegate(HEntity  hEntity, string name)
		:m_hEntity(hEntity),
		m_name(name)
		{
		}
	virtual void operator()(taf::Int32 ret,  const ServerEngine::NameDesc& descInfo)
		{
			IEntity *pEntity = getEntityFromHandle(m_hEntity);
			if(!pEntity) return;
			FriendSystem* pFriendSystem = static_cast<FriendSystem*>(pEntity->querySubsystem(IID_IFriendSystem));
			if(!pFriendSystem) return;
			pFriendSystem->getRequestNameDesc(ret,descInfo);
		}
private:
	HEntity m_hEntity;
	//FriendSystem* m_pFriendSystem;
	string m_name;
};

struct RequestActorDeleget
{
	RequestActorDeleget(HEntity  hEntity)
		:m_hEntity(hEntity)
		{
		
		}
	virtual void operator()(int iRet, HEntity hEntity, ServerEngine::PIRole& roleInfo)
		{
			if( ServerEngine::en_RoleRet_OK != iRet)
			{
				return;
			}
			IEntity *pEntity = getEntityFromHandle(m_hEntity);
			if(!pEntity) return;
			FriendSystem* pFriendSystem = static_cast<FriendSystem*>(pEntity->querySubsystem(IID_IFriendSystem));
			if(!pFriendSystem) return;
			
			pFriendSystem->addRequestToList(hEntity,roleInfo);
		}
private:
	HEntity m_hEntity;
	//FriendSystem* m_pFriendSystem;
};


void FriendSystem::addRequestToList(HEntity hEntity, ServerEngine::PIRole& roleInfo)
{

	IGlobalCfg* pGlobal = getComponent<IGlobalCfg>( COMPNAME_GlobalCfg,IID_IGlobalCfg);
	assert(pGlobal);
	//在线
	if(0 != hEntity)
	{
		IEntity* pFriendEntity = getEntityFromHandle(hEntity);
		assert(pFriendEntity);
		
		IEntity* pMyEntity = getEntityFromHandle(m_hEntity);
		assert(pMyEntity);
		
		int actorLevel = pFriendEntity->getProperty(PROP_ENTITY_LEVEL,0);
		//未开启的好友不能加
		
		int openLevel  = pGlobal->getInt("好友系统开启等级",15);
		if(actorLevel < openLevel )
		{	
			pMyEntity->sendErrorCode(ERROR_FRIEND_FRIEND_LEVEL);
			return;
		}
		IFriendSystem* pFriendSystem = static_cast<IFriendSystem*>( pFriendEntity->querySubsystem(IID_IFriendSystem));
		assert(pFriendSystem);

		std::string strName = pMyEntity->getProperty(PROP_ENTITY_NAME,"");
		
		ServerEngine::PKRole roleKey;
		roleKey.strAccount = pMyEntity->getProperty(PROP_ACTOR_ACCOUNT, "");
		roleKey.rolePos = pMyEntity->getProperty(PROP_ACTOR_ROLEPOS, 0);
		roleKey.worldID = pMyEntity->getProperty(PROP_ACTOR_WORLD, 0);
		
		bool res = pFriendSystem->addRequstToRequestMap(strName,roleKey);
		if(!res)
		{
			IEntity* pEntity = getEntityFromHandle(m_hEntity);
			assert(pEntity);
			pEntity->sendMessage(GSProto::CMD_FRIENDSYSTEM_REQUEST);
			return;
		}
	}
	//不在线拿数据库修改
	else
	{
		IEntity* pMyEntity = getEntityFromHandle(m_hEntity);
		assert(pMyEntity);

		ServerEngine::RoleSaveData roleSaveData;
		ServerEngine::JceToObj(roleInfo.roleData,roleSaveData);

		map<int ,int>::iterator iter = roleSaveData.basePropData.roleIntPropset.find(PROP_ENTITY_LEVEL);
		assert(iter != roleSaveData.basePropData.roleIntPropset.end() );
		int actorLevel = iter->second;
		//未开启的好友不能加
		int openLevel  = pGlobal->getInt("好友系统开启等级",15);
		if(actorLevel < openLevel )
		{
			pMyEntity->sendErrorCode(ERROR_FRIEND_FRIEND_LEVEL);
			return;
		}

		string strName = pMyEntity->getProperty(PROP_ENTITY_NAME,"");
		
		ServerEngine::PKRole roleKey;
		roleKey.strAccount = pMyEntity->getProperty(PROP_ACTOR_ACCOUNT, "");
		roleKey.rolePos = pMyEntity->getProperty(PROP_ACTOR_ROLEPOS, 0);
		roleKey.worldID = pMyEntity->getProperty(PROP_ACTOR_WORLD, 0);
		
		ServerEngine::FriendSystemData temp;
		if(roleSaveData.subsystemData[IID_IFriendSystem].size() > 0)
		{
			ServerEngine::JceToObj(roleSaveData.subsystemData[IID_IFriendSystem],temp);
			if(temp.m_RequestMap.find(strName) != temp.m_RequestMap.end())
			{
				pMyEntity->sendErrorCode(ERROR_HAS_REQUESTLIST);
				return;
			}
		}
		else
		{
			temp.iGetPhyStrengthTimes = 0;
			temp.iGivePhyStrengthTimes = 0;
		}
		ServerEngine::FriendNode node;
		node.roleKey = roleKey;
		node.strFriendName= strName;
		temp.m_RequestMap[strName] = node;

		//回写数据
		roleSaveData.subsystemData[IID_IFriendSystem] = ServerEngine::JceToStr(temp);
		
		roleInfo.roleData = ServerEngine::JceToStr(roleSaveData);
	}

	//todo 发送添加成功提示
	IEntity* pEntity = getEntityFromHandle(m_hEntity);
	assert(pEntity);
	pEntity->sendMessage(GSProto::CMD_FRIENDSYSTEM_REQUEST);
	
}

bool FriendSystem::addRequstToRequestMap(string name,ServerEngine::PKRole role)
{
	if(m_FriendData.m_RequestMap.find(name) != m_FriendData.m_RequestMap.end())
	{
		return false;
	}

	if(m_FriendData.m_RequestMap.size() >=30)
	{
			m_FriendData.m_RequestMap.erase(m_FriendData.m_RequestMap.begin());
	}
	
	ServerEngine::FriendNode node;
	node.roleKey = role;
	node.strFriendName= name;
	m_FriendData.m_RequestMap[name] = node;

	return true;
}

void FriendSystem::getRequestNameDesc(int iRet,const ServerEngine::NameDesc& nameInfo)
{
	IEntity* pEntity = getEntityFromHandle(m_hEntity);
	assert(pEntity);

	//不存在这个人
	if(ServerEngine::en_NameRet_NameNotExist == iRet )
	{
		pEntity->sendErrorCode(ERROR_ACTORNAME_NOEXITED);
		return;
	}

	//存在这个玩家，加入玩家的请求列表
	//获取数据
	IModifyDelegate* pModifyDelegate = getComponent<IModifyDelegate>(COMPNAME_ModifyDelegate, IID_IModifyDelegate);
	assert(pModifyDelegate);

	ServerEngine::PKRole roleKey;
	roleKey.worldID = nameInfo.wWolrdID;
	roleKey.rolePos = nameInfo.iRolePos;
	roleKey.strAccount = nameInfo.sAccount;
	
	pModifyDelegate->submitDelegateTask(roleKey,RequestActorDeleget(m_hEntity));
}

void FriendSystem::onRequestFriend(const GSProto::CSMessage& msg)
{
	GSProto::CMD_FRIENDSYSTEM_REQUEST_CS c2sMsg;
	if(!c2sMsg.ParseFromString(msg.strmsgbody()))
	{
		return;
	}

	//自己朋友表中是否存在这个朋友
	IEntity* pEntity = getEntityFromHandle(m_hEntity);
	assert(pEntity);
	if(m_FriendData.m_FriendMap.find(c2sMsg.strname()) != m_FriendData.m_FriendMap.end())
	{
		pEntity->sendErrorCode(ERROR_HAS_FRIEND);
		return;
	}
	
	
	IJZMessageLayer* pMsgLayer = getComponent<IJZMessageLayer>(COMPNAME_MessageLayer, IID_IJZMessageLayer);
	assert(pMsgLayer);
	try
	{
		pMsgLayer->AsyncGetNameDesc(new NameDescCallback( NameDescDelegate( m_hEntity,c2sMsg.strname() ) ) ,c2sMsg.strname());
	}
	catch(...)
	{
		IEntity* pEntity = getEntityFromHandle(m_hEntity);
		assert(pEntity);
		pEntity->sendErrorCode(ERROR_SYSERROR);
	}
}

void FriendSystem::packSaveData(string& data)
{
	data = ServerEngine::JceToStr(m_FriendData);
}

bool FriendSystem::canComein()
{
	IGlobalCfg* pGlobalconf = getComponent<IGlobalCfg>(COMPNAME_GlobalCfg,IID_IGlobalCfg);
	assert(pGlobalconf);
	int openLevel  = pGlobalconf->getInt("好友系统开启等级",15);
	
	IEntity *pEntity = getEntityFromHandle(m_hEntity);
	assert(pEntity);
	int actorLevel = pEntity->getProperty(PROP_ENTITY_LEVEL,0);

	return actorLevel>= openLevel;
}

void FriendSystem::GMOnlineGetActorBaseInfo(string& strJson)
{
	IEntity* pEntity = getEntityFromHandle(m_hEntity);
	assert(pEntity);

	int iWorldId = pEntity->getProperty(PROP_ACTOR_WORLD,0);
	int iLevel = pEntity->getProperty(PROP_ENTITY_LEVEL,1);
	int iExp = pEntity->getProperty(PROP_ENTITY_EXP, 0);
	int iVipLevel = pEntity->getProperty(PROP_ACTOR_VIPLEVEL, 0);
	int iVipExp = pEntity->getProperty(PROP_ACTOR_VIPEXP, 0);
	int iGold = pEntity->getProperty(PROP_ACTOR_GOLD,0);
	int iSilver = pEntity->getProperty(PROP_ACTOR_SILVER,0);
	string strName = pEntity->getProperty(PROP_ENTITY_NAME,"");
	string strBelongLengionName = "No Lengion";

	//竞技场排名
	IArenaFactory* arenaFactory = getComponent<IArenaFactory>(COMPNAME_ArenaFactory,IID_IArenaFactory);
	assert(arenaFactory);
	int rank = arenaFactory->getArenaRank(strName);

	//军团归属
	ILegionFactory* pLegionFactory = getComponent<ILegionFactory>(COMPNAME_LegionFactory,IID_ILegionFactory);
	assert(pLegionFactory);

	ILegion* pLengion = pLegionFactory->getActorLegion(strName);
	if(pLengion)
	{
		strBelongLengionName = pLengion->getLegionName();
	}
	int iStrength = pEntity->getProperty(PROP_ENTITY_PHYSTRENGTH,0);
	int iTaozi = pEntity->getProperty(PROP_ENTITY_ACTOR_VIGOR, 0);

	//组成Json
	string strEndsign = "\",\"";
	string strBeginSign = "\":\"";
	strJson = "{\"cmd\":\"rolebase\",\"data\":{\"Serverid\":\""+TC_I2S(iWorldId)+strEndsign+"level"+strBeginSign
																+TC_I2S(iLevel)+strEndsign+"exp"+strBeginSign
																+TC_I2S(iExp)+strEndsign+"viplevel"+strBeginSign
																+TC_I2S(iVipLevel)+strEndsign+"vipexp"+strBeginSign
																+TC_I2S(iVipExp)+strEndsign+"gold"+strBeginSign
																+TC_I2S(iGold)+strEndsign+"silver"+strBeginSign
																+TC_I2S(iSilver)+strEndsign+"legionname"+strBeginSign
																+strBelongLengionName+strEndsign+"pvprank"+strBeginSign
																+TC_I2S(rank)+strEndsign+"strength"+strBeginSign
																+TC_I2S(iStrength)+strEndsign+"taozi"+strBeginSign
																+TC_I2S(iTaozi)+"\"}}";

	cout<<strJson.c_str()<<endl;
	
}


