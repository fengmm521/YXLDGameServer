#include "FightServerPch.h"
#include "FightFactory.h"
#include "FightSystem.h"
#include "ErrorCodeDef.h"
#include "IDungeonSystem.h"
#include "IClimbTowerSystem.h"


extern "C" IObject* createFightSystem()
{
	return new FightSystem;
}



FightSystem::FightSystem():m_dwLastFightRetTime(0), m_bPVEFight(false)
{
}

FightSystem::~FightSystem()
{
}

Uint32 FightSystem::getSubsystemID() const
{
	return IID_IFightSystem;
}

Uint32 FightSystem::getMasterHandle()
{
	return m_hEntity;
}

bool FightSystem::create(IEntity* pEntity, const std::string& strData)
{
	assert(pEntity);
	m_hEntity = pEntity->getHandle();

	return true;
}

bool FightSystem::createComplete()
{
	return true;
}

const std::vector<Uint32>& FightSystem::getSupportMessage()
{
	static std::vector<Uint32> resultList;
	if(resultList.size() == 0)
	{
		resultList.push_back(GSProto::CMD_FIGHT_USEACTORSKILL);
		resultList.push_back(GSProto::CMD_FIGHT_PLAYFINISH);
		resultList.push_back(GSProto::CMD_FIGHT_QUIT);
	}

	return resultList;
}

void FightSystem::onMessage(QxMessage* pMessage)
{
	assert(pMessage->dwMsgLen == sizeof(GSProto::CSMessage) );
	const GSProto::CSMessage& msg = *(const GSProto::CSMessage*)(pMessage->pMsgDataBuff);

	switch(msg.icmd() )
	{
		case GSProto::CMD_FIGHT_USEACTORSKILL:
			onReqUseActorSkill(msg);
			break;

		case GSProto::CMD_FIGHT_PLAYFINISH:
			onReqFightPlayFinish(msg);
			break;

		case GSProto::CMD_FIGHT_QUIT:
			onReqFightQuit(msg);
			break;
	}
}

template<typename T>
bool PVEAndPVPFightCache<T>::canSkip(int iFightType)
{
	IEntity* pEntity = getEntityFromHandle(m_hEntity);
	if(!pEntity) return false;

	FightSystem* pFightSys = static_cast<FightSystem*>(pEntity->querySubsystem(IID_IFightSystem) );
	if(!pFightSys) return false;

	if( (iFightType == GSProto::en_FightType_Scene) || (iFightType == GSProto::en_FightType_EquipScene) )
	{
		IDungeonSystem* pDungeonSys = static_cast<IDungeonSystem*>(pEntity->querySubsystem(IID_IDungeonSystem));
		assert(pDungeonSys);

		return pDungeonSys->canSkipCurScene();
	}
	else if(iFightType == GSProto::en_FightType_ClimbTower)
	{
		IClimbTowerSystem* pClimbTowerSys = static_cast<IClimbTowerSystem*>(pEntity->querySubsystem(IID_IClimbTowerSystem));
		assert(pClimbTowerSys);

		return pClimbTowerSys->canSkip();
	}
	else if(iFightType == GSProto::en_FightType_DreamLand)
	{
		return false;
	}
	
	else if(!pFightSys->m_bPVEFight)
	{
		return true;
	}

	return false;
}

template<typename T>
void PVEAndPVPFightCache<T>::operator()(taf::Int32 iRet, ServerEngine::BattleData& data)
{
	IEntity* pEntity = getEntityFromHandle(m_hEntity);
	if(!pEntity) return;

	// 重设PVE 的CD时间
	pEntity->setProperty(PROP_LAST_PVETIME, 0);

	FightSystem* pFightSys = static_cast<FightSystem*>(pEntity->querySubsystem(IID_IFightSystem) );
	if(!pFightSys) return;

	// 这里重置下，表示战斗返回了
	pFightSys->m_dwLastFightRetTime = 0;
	if(iRet != en_FightResult_OK)
	{
		m_cb(iRet, data);
		return;
	}

	FightFactory* pFightFactory = static_cast<FightFactory*>(getComponent<IFightFactory>(COMPNAME_FightFactory, IID_IFightFactory));
	assert(pFightFactory);

	// 如果战斗不能使用药物，直接调用callback
	if(!pFightFactory->canUseFightUseActorSkill(pFightSys->m_fightCtx.iFightType) )
	{
		m_cb(iRet, data);
		return;
	}
	
	// 成功了,先发送给客户端
	int iSendIndex = -1;
	if(m_bUseItem)
	{
		GSProto::CMD_FIGHT_USEACTORSKILL_SC scMsg;
		scMsg.set_busesuccess(pFightSys->m_fightCtx.useItemActionList == data.useItemList);
		
		pEntity->sendMessage(GSProto::CMD_FIGHT_USEACTORSKILL, scMsg);
		assert(pFightSys->m_fightCtx.useItemActionList.size() > 0);
		
		iSendIndex = pFightSys->m_fightCtx.useItemActionList.back().iActionIndex;
	}

	// 保存战报
	pFightSys->m_battleData = data;
	pFightSys->m_fightCtx.randValueList = data.randValueList;
	
	pFightSys->m_fightCtx.useItemActionList = data.useItemList;

	stringstream ss;
	for(size_t i = 0; i < data.randValueList.size(); i++)
	{
		ss<<data.randValueList[i]<<"|";
	}
	ss<<endl;

	FDLOG("FightRand")<<ss.str();
	
	// 发送给客户端, 注意，这里可能是Continue回来的数据，需要从上次的最后一个Action开始发送
	if(-1 == iSendIndex)
	{
		bool bCanSkip = canSkip(pFightSys->m_fightCtx.iFightType);
		pFightSys->sendAllBattleMsgWithSkipFlag(data, bCanSkip);
	}
	else if(m_bUseItem)
	{
		pFightSys->sendAllBattleMsgFromIndex(data, iSendIndex);
	}
}


template<typename T>
void PVEAndPVPFightCache<T>::operator()(taf::Int32 iRet, const ServerEngine::BattleData& data)
{
	(*this)(iRet, (ServerEngine::BattleData&)data);
}

void FightSystem::onReqFightPlayFinish(const GSProto::CSMessage& msg)
{
	// 是否存在Cache的战斗，如果没有，返回
	FightFactory* pFightFactory = static_cast<FightFactory*>(getComponent<IFightFactory>("FightFactory", IID_IFightFactory) );
	assert(pFightFactory);
	
	if(!pFightFactory->canUseFightUseActorSkill(m_fightCtx.iFightType))
	{
		return;
	}

	// 清理
	m_fightCtx = ServerEngine::FightContext();

	// 注意，这里就只下发结果了
	if(m_bPVEFight)
	{
		m_cb(en_FightResult_OK, m_battleData);
	}
	else
	{
		m_pvpCb(en_FightResult_OK, m_battleData);
	}
}

void FightSystem::onReqFightQuit(const GSProto::CSMessage& msg)
{	
	// 是否存在Cache的战斗，如果没有，返回
	FightFactory* pFightFactory = static_cast<FightFactory*>(getComponent<IFightFactory>("FightFactory", IID_IFightFactory) );
	assert(pFightFactory);
	
	if(!pFightFactory->canUseFightUseActorSkill(m_fightCtx.iFightType))
	{
		return;
	}
	
	// 设置为战斗失败
	ServerEngine::BattleData emptyBattle;
	emptyBattle.bAttackerWin = false;
	emptyBattle.bQuit = 1;

	if(m_bPVEFight)
	{
		m_cb(en_FightResult_OK, emptyBattle);
	}
	else
	{
		m_pvpCb(en_FightResult_OK, emptyBattle);
	}

	if(emptyBattle.bQuitSuccess)
	{
		m_fightCtx = ServerEngine::FightContext();
	}
}


void FightSystem::onReqUseActorSkill(const GSProto::CSMessage& msg)
{
	// 如果当前没有Cache的战斗，返回
	FightFactory* pFightFactory = static_cast<FightFactory*>(getComponent<IFightFactory>("FightFactory", IID_IFightFactory) );
	assert(pFightFactory);
	
	if(!pFightFactory->canUseFightUseActorSkill(m_fightCtx.iFightType))
	{
		return;
	}

	GSProto::CMD_FIGHT_USEACTORSKILL_CS req;
	if(!req.ParseFromString(msg.strmsgbody() ) )
	{
		return;
	}

	if( (0 != req.bpost() ) && (1 != req.bpost() ) )
	{
		FDLOG("UseItem")<<"Invalid"<<endl;
		return;
	}

	// 如果战斗结果未返回，不能吃药(这里设置成时间, 超时后可以继续)
	if(m_bPVEFight && (time(0) < (m_dwLastFightRetTime + 5) ) )
	{
		FDLOG("UseItem")<<"CD"<<endl;
		return;
	}

	ServerEngine::FightUseItem tmpUseCtx;
	tmpUseCtx.iActionIndex = req.iactionindex();
	tmpUseCtx.iResultsetIndex = req.iresultsetindex();
	tmpUseCtx.iActorSkillID = req.iactorskillid();
	
	//tmpUseCtx.iTargetPos = req.itargetpos();
	tmpUseCtx.iPost = req.bpost();

	FDLOG("UseItem")<<"try|"<<req.iactionindex()<<"|"<<req.iresultsetindex()<<"|"<<(int)req.bpost()<<endl;

	// 验证是否合法
	if(m_fightCtx.useItemActionList.size() > 0)
	{
		const ServerEngine::FightUseItem& lastUseItem = m_fightCtx.useItemActionList.back();

		int iLastUseV = lastUseItem.iActionIndex * 1000 + lastUseItem.iResultsetIndex * 10 + lastUseItem.iPost;
		int iTmpUseV = tmpUseCtx.iActionIndex * 1000 + tmpUseCtx.iResultsetIndex * 10 + tmpUseCtx.iPost;
		
		if(iTmpUseV <= iLastUseV)
		{
			FDLOG("UseItem")<<"Drop|"<<req.iactionindex()<<"|"<<req.iresultsetindex()<<"|"<<(int)req.bpost()
			<<"|"<<lastUseItem.iActionIndex<<"|"<<lastUseItem.iResultsetIndex<<"|"<<lastUseItem.iPost
			<<endl;
			return;
		}
	}

	FDLOG("UseItem")<<"send|"<<req.iactionindex()<<"|"<<req.iresultsetindex()<<"|"<<(int)req.bpost()<<endl;
	m_fightCtx.useItemActionList.push_back(tmpUseCtx);

	IEntity* pEntity = getEntityFromHandle(m_hEntity);
	assert(pEntity);

	// 准备Continue战斗吧
	if(m_bPVEFight)
	{
		if(m_iMonsterGrpID > 0)
		{
			pFightFactory->AsyncContinuePVEFight(PVEAndPVPFightCache<DelegatePVEFight>(m_hEntity, m_cb, true), m_fightRoleInfo, m_iMonsterGrpID, m_fightCtx);
		}
		else if(m_monsterGrpList.size() > 0)
		{
			pFightFactory->AsyncContinueMultiPVEFight(PVEAndPVPFightCache<DelegatePVEFight>(m_hEntity, m_cb, true), m_fightRoleInfo, m_monsterGrpList, m_fightCtx);
		}
	}
	else
	{
		pFightFactory->AsyncContinuePVPFight(PVEAndPVPFightCache<DelegatePVPFight>(m_hEntity, m_pvpCb, true), m_fightRoleInfo, m_pvpTargetRoleInfo, m_fightCtx);
	}
}


void FightSystem::AsyncMultPVEFight(DelegatePVEFight cb, vector<int> monsterList, const ServerEngine::FightContext& ctx)
{
	m_fightCtx = ctx;
	m_iMonsterGrpID = 0;
	m_monsterGrpList = monsterList;
	m_bPVEFight = true;

	FightFactory* pFightFactory = static_cast<FightFactory*>(getComponent<IFightFactory>("FightFactory", IID_IFightFactory) );
	assert(pFightFactory);

	IGlobalCfg* pGlobalCfg = getComponent<IGlobalCfg>(COMPNAME_GlobalCfg, IID_IGlobalCfg);
	assert(pGlobalCfg);

	// 验证CD时间
	IEntity* pEntity = getEntityFromHandle(m_hEntity);
	assert(pEntity);
	
	Uint32 dwLastPVETime = (Uint32)pEntity->getProperty(PROP_LAST_PVETIME, 0);
	Uint32 dwCurTime = time(0);

	Uint32 dwPVECD = (Uint32)pGlobalCfg->getInt("PVECD", 2);
	if(dwCurTime < (dwLastPVETime + dwPVECD) )
	{
		pEntity->sendErrorCode(ERROR_PVE_CD);
		return;
	}

	ServerEngine::ActorCreateContext roleCreateCtx;

	GhostSaveGuard ghostpackGuard(m_hEntity);
	roleCreateCtx.strAccount = pEntity->getProperty(PROP_ACTOR_ACCOUNT, "");
	pEntity->packSaveData(roleCreateCtx.data);


	//九天幻境特殊处理
	if( ctx.iFightType == GSProto::en_FightType_DreamLand)
	{
		ServerEngine::DreamLandFightCtx dreamLandCtx;
		assert(ctx.context.find("DreamLandFightCtx") != ctx.context.end() );
		ServerEngine::JceToObj(ctx.context.at("DreamLandFightCtx"), dreamLandCtx);
		
		//阵型保存
		assert(dreamLandCtx.strFormationData.size() > 0);
		roleCreateCtx.data.subsystemData[IID_IFormationSystem] = dreamLandCtx.strFormationData;

		//替换英雄系统数据
		ServerEngine::HeroSystemData oriHeroSystemData;
		assert(roleCreateCtx.data.subsystemData[IID_IHeroSystem].size() > 0);
		ServerEngine::JceToObj(roleCreateCtx.data.subsystemData[IID_IHeroSystem], oriHeroSystemData);

		ServerEngine::HeroSystemData newHeroSystemData;
		assert(dreamLandCtx.strHerolistData.size() > 0);
		ServerEngine::JceToObj(dreamLandCtx.strHerolistData , newHeroSystemData);

		oriHeroSystemData.heroList.clear();
		oriHeroSystemData.heroList = newHeroSystemData.heroList;
		
		//回写数据
		roleCreateCtx.data.subsystemData[IID_IHeroSystem ] = ServerEngine::JceToStr(oriHeroSystemData);
		
	}
	
	// 优化下，避免不必要的Role数据Copy
	if(pFightFactory->canUseFightUseActorSkill(m_fightCtx.iFightType) )
	{	
		m_fightRoleInfo = roleCreateCtx;
	}

	ServerEngine::BattleData battleInfo;
	m_dwLastFightRetTime = time(0);	
	m_cb = cb;
	pFightFactory->AsyncMultiPVEFight(PVEAndPVPFightCache<DelegatePVEFight>(m_hEntity, cb, 0), roleCreateCtx, monsterList, ctx);

	pEntity->setProperty(PROP_LAST_PVETIME, (int)dwCurTime);
}

void FightSystem::AsyncPVEFight(DelegatePVEFight cb, int iMonsterGrpID, const ServerEngine::FightContext& ctx)
{
	m_fightCtx = ctx;
	m_iMonsterGrpID = iMonsterGrpID;
	m_monsterGrpList.clear();
	m_bPVEFight = true;

	FightFactory* pFightFactory = static_cast<FightFactory*>(getComponent<IFightFactory>("FightFactory", IID_IFightFactory) );
	assert(pFightFactory);

	IGlobalCfg* pGlobalCfg = getComponent<IGlobalCfg>(COMPNAME_GlobalCfg, IID_IGlobalCfg);
	assert(pGlobalCfg);

	// 验证CD时间
	IEntity* pEntity = getEntityFromHandle(m_hEntity);
	assert(pEntity);
	
	Uint32 dwLastPVETime = (Uint32)pEntity->getProperty(PROP_LAST_PVETIME, 0);
	Uint32 dwCurTime = time(0);

	Uint32 dwPVECD = (Uint32)pGlobalCfg->getInt("PVECD", 2);
	if(dwCurTime < (dwLastPVETime + dwPVECD) )
	{
		pEntity->sendErrorCode(ERROR_PVE_CD);
		return;
	}

	ServerEngine::ActorCreateContext roleCreateCtx;

	GhostSaveGuard ghostpackGuard(m_hEntity);
	roleCreateCtx.strAccount = pEntity->getProperty(PROP_ACTOR_ACCOUNT, "");
	pEntity->packSaveData(roleCreateCtx.data);

	// 优化下，避免不必要的Role数据Copy
	if(pFightFactory->canUseFightUseActorSkill(m_fightCtx.iFightType) )
	{
		m_fightRoleInfo = roleCreateCtx;
	}

	ServerEngine::BattleData battleInfo;
	m_dwLastFightRetTime = time(0);
	m_cb = cb;
	pFightFactory->AsyncPVEFight(PVEAndPVPFightCache<DelegatePVEFight>(m_hEntity, cb, 0), roleCreateCtx, iMonsterGrpID, ctx);

	pEntity->setProperty(PROP_LAST_PVETIME, (int)dwCurTime);
}


void PVPGetRoleCb::callback_getRole(taf::Int32 ret,  const ServerEngine::PIRole& roleInfo)
{
	ServerEngine::BattleData emptyData;
	try
	{
		if(ServerEngine::en_RoleRet_OK == ret)
		{
			IEntity* pAttacker = getEntityFromHandle(m_hAttacker);
			if(!pAttacker)
			{
				m_cb(en_FightResult_SysError, emptyData);
				return;
			}

			FightSystem* pAttFightSystem = static_cast<FightSystem*>(pAttacker->querySubsystem(IID_IFightSystem));
			assert(pAttFightSystem);
			
			ServerEngine::ActorCreateContext targeterCtx;
			targeterCtx.strAccount = m_targetKey.strAccount;
			ServerEngine::JceToObj(roleInfo.roleData, targeterCtx.data);

			pAttFightSystem->_AsyncPVPFight(m_cb, targeterCtx, m_fightCtx);
		}
		else
		{
			m_cb(en_FightResult_SysError, emptyData);
			SvrErrLog("PVPGetRoleCb|GetRoleError|ret|%d|%s|%d", ret, m_targetKey.strAccount.c_str(), m_targetKey.rolePos);
		}
	}
	catch(...)
	{
		m_cb(en_FightResult_SysError, emptyData);
	}
}

void PVPGetRoleCb::callback_getRole_exception(taf::Int32 ret)
{
	ServerEngine::BattleData emptyData;
	m_cb(en_FightResult_SysError, emptyData);
	SvrErrLog("PVPGetRoleCb|GetRoleException|ret|%d|%s|%d", ret, m_targetKey.strAccount.c_str(), m_targetKey.rolePos);
}


void FightSystem::_AsyncPVPFight(DelegatePVPFight cb, const ServerEngine::ActorCreateContext targeterCtx, const ServerEngine::FightContext& ctx)
{	
	m_fightCtx = ctx;
	m_bPVEFight = false;

	ServerEngine::ActorCreateContext attackerCtx;

	IEntity* pAttacker = getEntityFromHandle(m_hEntity);
	assert(pAttacker);

	GhostSaveGuard ghostpackGuard(m_hEntity);
	attackerCtx.strAccount = pAttacker->getProperty(PROP_ACTOR_ACCOUNT, "");
	pAttacker->packSaveData(attackerCtx.data);

	//九天幻境特殊处理
	if( ctx.iFightType == GSProto::en_FightType_DreamLand)
	{
		ServerEngine::DreamLandFightCtx dreamLandCtx;
		assert(ctx.context.find("DreamLandFightCtx") != ctx.context.end() );
		ServerEngine::JceToObj(ctx.context.at("DreamLandFightCtx"), dreamLandCtx);
		
		//阵型保存
		assert(dreamLandCtx.strFormationData.size() > 0);
		attackerCtx.data.subsystemData[IID_IFormationSystem] = dreamLandCtx.strFormationData;

		//替换英雄系统数据
		ServerEngine::HeroSystemData oriHeroSystemData;
		assert(attackerCtx.data.subsystemData[IID_IHeroSystem].size() > 0);
		ServerEngine::JceToObj(attackerCtx.data.subsystemData[IID_IHeroSystem], oriHeroSystemData);

		ServerEngine::HeroSystemData newHeroSystemData;
		assert(dreamLandCtx.strHerolistData.size() > 0);
		ServerEngine::JceToObj(dreamLandCtx.strHerolistData , newHeroSystemData);

		oriHeroSystemData.heroList.clear();
		oriHeroSystemData.heroList = newHeroSystemData.heroList;
		
		//回写数据
		attackerCtx.data.subsystemData[IID_IHeroSystem ] = ServerEngine::JceToStr(oriHeroSystemData);
		
	}
	
	// 优化下，避免不必要的Role数据Copy
	FightFactory* pFightFactory = static_cast<FightFactory*>(getComponent<IFightFactory>("FightFactory", IID_IFightFactory) );
	assert(pFightFactory);
	
	if(pFightFactory->canUseFightUseActorSkill(m_fightCtx.iFightType) )
	{
		m_fightRoleInfo = attackerCtx;
		m_pvpTargetRoleInfo = targeterCtx;
	}

	m_dwLastFightRetTime = time(0);

	ServerEngine::BattleData battleInfo;
	m_pvpCb = cb;
	pFightFactory->AsyncPVPFight(PVEAndPVPFightCache<DelegatePVPFight>(m_hEntity, cb, 0), attackerCtx, targeterCtx, ctx);
}


void FightSystem::AsyncPVPFight(DelegatePVPFight cb, const ServerEngine::PKRole& target, const ServerEngine::FightContext& ctx)
{
	IMessageLayer* pMsgLayer = getComponent<IMessageLayer>(COMPNAME_MessageLayer, IID_IMessageLayer);
	assert(pMsgLayer);

	// 如果目标在线，采取在线数据
	try
	{
		IUserStateManager* pUserStateMgr = getComponent<IUserStateManager>(COMPNAME_LoginManager, IID_IUserStateManager);
		assert(pUserStateMgr);

		//const UserInfo* pTargetUserInfo = pUserStateMgr->getUserInfo(target.strAccount);
		HEntity hTarget = pUserStateMgr->getRichEntityByAccount(target.strAccount);
		if(getEntityFromHandle(hTarget) )
		{
			IEntity* pTarget = getEntityFromHandle(hTarget);
			assert(pTarget);

			ServerEngine::ActorCreateContext targeterCtx;
			targeterCtx.strAccount = target.strAccount;

			GhostSaveGuard ghostpackGuard(pTarget->getHandle() );
			pTarget->packSaveData(targeterCtx.data);
			
			_AsyncPVPFight(cb, targeterCtx, ctx);
			return;
		}

		// 否则，拉取离线数据
		ServerEngine::RolePrxCallbackPtr roleCallbackPrx = new PVPGetRoleCb(m_hEntity,target, ctx, cb);
		pMsgLayer->AsyncGetRole(target, roleCallbackPrx);
	}
	catch(...)
	{
		ServerEngine::BattleData emptyData;
		cb(en_FightResult_SysError, emptyData);
	}
}


void FightSystem::AsyncPVPFight(DelegatePVPFight cb, const ServerEngine::ActorCreateContext& targetCtx, const ServerEngine::FightContext& ctx)
{

	try
	{
		_AsyncPVPFight(cb, targetCtx, ctx);
	}
	catch(...)
	{
		ServerEngine::BattleData emptyData;
		cb(en_FightResult_SysError, emptyData);
	}
}


void FightSystem::sendBattleMemMsgFixSkip(const GSProto::FightDataAll& allData, bool bCanSkip)
{
	FightFactory* pFightFactory = static_cast<FightFactory*>(getComponent<IFightFactory>("FightFactory", IID_IFightFactory) );
	assert(pFightFactory);

	IGlobalCfg* pGlobalCfg = getComponent<IGlobalCfg>(COMPNAME_GlobalCfg, IID_IGlobalCfg);
	assert(pGlobalCfg);
	
	int iInitItemCount = allData.iinititemcount();

	GSProto::Cmd_Sc_FightMember scMember;
	
	*scMember.mutable_szfightobject() = allData.szfightobject();
	scMember.set_ifighttype(allData.ifighttype() );
	scMember.set_isceneid(allData.isceneid() );
	scMember.set_iisattackerwin(allData.iisattackerwin() );

	bool bCanUseItem = pFightFactory->canUseFightUseActorSkill(allData.ifighttype() );
	scMember.set_bcanuseactorskill(bCanUseItem);
	scMember.set_bfinalfight(allData.bonlyonefight() );

	if(allData.has_lefthead() && allData.has_righthead() )
	{
		*scMember.mutable_lefthead() = allData.lefthead();
		*scMember.mutable_righthead() = allData.righthead();
	}

	if(bCanUseItem)
	{
		scMember.set_iitemcount(iInitItemCount);
	}
	else
	{
		scMember.set_iitemcount(0);
	}
	scMember.set_bcanskipbattle(bCanSkip);

	IEntity* pMaster = getEntityFromHandle(m_hEntity);
	assert(pMaster);

	GSProto::SCMessage pkg;
	HelpMakeScMsg(pkg, GSProto::CMD_FIGHT_MEMBER, scMember);
	
	pMaster->sendMessage(pkg);
}


void FightSystem::sendBattleMemMsg(const GSProto::FightDataAll& allData)
{
	sendBattleMemMsgFixSkip(allData, true);
}

void FightSystem::sendBattleAction(const GSProto::FightDataAll& allData)
{
	IEntity* pMaster = getEntityFromHandle(m_hEntity);
	assert(pMaster);

	for(int i = 0; i < allData.szfightaction().size(); i++)
	{
		GSProto::Cmd_Sc_FightAction scAction;
		*scAction.mutable_singleaction() = allData.szfightaction().Get(i);

		GSProto::SCMessage pkg;
		HelpMakeScMsg(pkg, GSProto::CMD_FIGHT_ACTION, scAction);

		pMaster->sendMessage(pkg);
	}
}

void FightSystem::sendBattleFin()
{
	GSProto::SCMessage pkg;
	pkg.set_icmd(GSProto::CMD_FIGHT_FIN);

	IEntity* pMaster = getEntityFromHandle(m_hEntity);
	assert(pMaster);

	pMaster->sendMessage(pkg);
}

void FightSystem::sendAllBattleMsg(const ServerEngine::BattleData& battleData)
{
	GSProto::FightDataAll fightDataAll;
	if(!fightDataAll.ParseFromString(battleData.strBattleBody) )
	{
		return;
	}

	sendBattleMemMsg(fightDataAll);
	sendBattleAction(fightDataAll);

	// 发送一个无奖励的Result
	GSProto::Cmd_Sc_CommFightResult fightResoult;
	fightResoult.set_isceneid(fightDataAll.isceneid() );
	fightResoult.set_iissuccess(battleData.bAttackerWin);
	fightResoult.set_istar(battleData.iStar);
	
	IEntity* pEntity = getEntityFromHandle(m_hEntity);
	assert(pEntity);
	
	pEntity->sendMessage(GSProto::CMD_COMM_FIGHTRESULT, fightResoult);
	
	sendBattleFin();
}

void FightSystem::sendAllBattleMsgWithSkipFlag(const ServerEngine::BattleData& battleData, bool bSkipFlag)
{
	GSProto::FightDataAll fightDataAll;
	if(!fightDataAll.ParseFromString(battleData.strBattleBody) )
	{
		return;
	}

	sendBattleMemMsgFixSkip(fightDataAll, bSkipFlag);
	sendBattleAction(fightDataAll);
	sendBattleFin();
}


void FightSystem::sendAllBattleMsgFromIndex(const ServerEngine::BattleData& battleData, int iIndex)
{
	GSProto::FightDataAll fightDataAll;
	if(!fightDataAll.ParseFromString(battleData.strBattleBody) )
	{
		return;
	}

	IEntity* pEntity = getEntityFromHandle(m_hEntity);
	assert(pEntity);

	for(int i = 0; i < fightDataAll.szfightaction_size(); i++)
	{
		if(i< iIndex) continue;

		GSProto::Cmd_Sc_FightAction scAction;
		*scAction.mutable_singleaction() = fightDataAll.szfightaction().Get(i);

		pEntity->sendMessage(GSProto::CMD_FIGHT_ACTION, scAction);
	}

	sendBattleFin();
}


void FightSystem::sendAllBattleMsg(const ServerEngine::BattleData& battleData, GSProto::SCMessage& resultMsg)
{
	GSProto::FightDataAll fightDataAll;
	if(!fightDataAll.ParseFromString(battleData.strBattleBody) )
	{
		return;
	}

	sendBattleMemMsg(fightDataAll);
	sendBattleAction(fightDataAll);

	GSProto::Cmd_Sc_CommFightResult fightResoult;
	if(!fightResoult.ParseFromString(resultMsg.strmsgbody()))
	{
		return;
	}

	IEntity* pMaster = getEntityFromHandle(m_hEntity);
	assert(pMaster);
	pMaster->sendMessage(GSProto::CMD_COMM_FIGHTRESULT,fightResoult);
	
	sendBattleFin();
}
;



