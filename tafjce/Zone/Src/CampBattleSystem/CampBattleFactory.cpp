#include "CampBattleSystemPch.h"
#include "CampBattleFactory.h"

extern "C" IComponent* createCampBattleFactory(Int32)
{
	return new CampBattleFactory;
}


CampBattleFactory::CampBattleFactory():
	m_state(en_CampBattle_BeforeStart)
{

	m_allBattlUnit.clear();	
	
	m_RankListA.clear();				
	m_matchingDequeA.clear();         	
   	m_waitBattleMapA.clear(); 			
	m_iFightValueA  = 0;                           		
	
	m_RankListB.clear();				
	m_matchingDequeB.clear();         	
 	m_waitBattleMapB.clear(); 		  	
 	m_iFightValueB = 0 ;                              	

   	m_StartSeconds = 0;					
	m_EndSeconds = 0;						
	m_SignUpTimeSeconds = 0;
}

CampBattleFactory::~CampBattleFactory()
{
	ITimerComponent* timer = getComponent<ITimerComponent>(COMPNAME_TimeAxis,IID_ITimerComponent);
	assert(timer);
	timer->killTimer(m_hCheckTimerHandle);
	
	m_RankListA.clear();				
	m_matchingDequeA.clear();         	
    m_waitBattleMapA.clear(); 			
	m_iFightValueA  = 0;  
	m_RankListB.clear();				
	m_matchingDequeB.clear();         	
 	m_waitBattleMapB.clear(); 	

	m_allBattlUnit.clear();
}

//GM 
void CampBattleFactory::startCampBattle(string startTime,string preTime, string endTime)
{

	//关闭定时器
	ITimerComponent* timer = getComponent<ITimerComponent>(COMPNAME_TimeAxis,IID_ITimerComponent);
	assert(timer);
	timer->killTimer( m_hCheckWaitVecTimerHandle);
	timer->killTimer( m_hBroadCastTimerHandle);
	timer->killTimer( m_hBroadCastReportTimerHandle);

	vector<int> StartTimeVec  = TC_Common::sepstr<int>(startTime, ":");
	vector<int> EndTimeVec = TC_Common::sepstr<int>(endTime, ":");
	vector<int> preTimeVec = TC_Common::sepstr<int>(preTime, ":");
	
	m_StartSeconds = StartTimeVec[0]*3600 + StartTimeVec[1]*60;
	m_EndSeconds = EndTimeVec[0]*3600 + EndTimeVec[1]*60;
	m_SignUpTimeSeconds = preTimeVec[0]*3600 + preTimeVec[1]*60;


	
	m_hCheckTimerHandle = timer->setTimer(this,1,5*1000,"CampBattleFactory");

	m_allBattlUnit.clear();
	m_RankListA.clear();
	m_RankListB.clear();

	IZoneTime* pZoneTime = getComponent<IZoneTime>(COMPNAME_ZoneTime,IID_IZoneTime);
	assert(pZoneTime);
	unsigned int curDayBeginSecond =  ONE_DAY_SECONDS -pZoneTime->GetCurDayLeftSeconds();
	if(curDayBeginSecond >= m_EndSeconds)
	{
		m_state = en_CampBattle_InEnd;
	}
	else if((curDayBeginSecond >= m_StartSeconds + m_SignUpTimeSeconds) && ( curDayBeginSecond <= m_EndSeconds) )
	{
		m_state = en_CampBattle_InBattle;
	}
	else
	{
		m_state = en_CampBattle_BeforeStart;
	}
	

	
}


bool CampBattleFactory::initlize(const PropertySet& propSet)
{
	//获取开始和结束时间
	IGlobalCfg* pGlobalconf = getComponent<IGlobalCfg>(COMPNAME_GlobalCfg,IID_IGlobalCfg);
	assert(pGlobalconf);
	string StartTimeStr = pGlobalconf->getString("阵营战开启时间","20:30"); //提前一分钟报名参加
	vector<int> StartTimeVec  = TC_Common::sepstr<int>(StartTimeStr, ":");
	string EndTimeStr =  pGlobalconf->getString("阵营战结束时间","21:00"); //提前一分钟报名参加
	vector<int> EndTimeVec = TC_Common::sepstr<int>(EndTimeStr, ":");
	m_StartSeconds = StartTimeVec[0]*3600+ StartTimeVec[1]*60;
	m_EndSeconds = EndTimeVec[0]*3600+ EndTimeVec[1]*60;
	
	string PerTimeStr =  pGlobalconf->getString("阵营战报名时间","00:01");
	vector<int> perTimeStrVec = TC_Common::sepstr<int>(PerTimeStr, ":");
	m_SignUpTimeSeconds =  perTimeStrVec[0]*3600 + perTimeStrVec[1]*60;

	//获取定时器
	ITimerComponent* timer = getComponent<ITimerComponent>(COMPNAME_TimeAxis,IID_ITimerComponent);
	assert(timer);
	
	m_hCheckTimerHandle = timer->setTimer(this,1,5*1000,"CampBattleFactory");
	

	IZoneTime* pZoneTime = getComponent<IZoneTime>(COMPNAME_ZoneTime,IID_IZoneTime);
	assert(pZoneTime);
	unsigned int curDayBeginSecond =  ONE_DAY_SECONDS -pZoneTime->GetCurDayLeftSeconds();
	if(curDayBeginSecond >= m_EndSeconds)
	{
		m_state = en_CampBattle_InEnd;
	}
	else if((curDayBeginSecond >= m_StartSeconds + m_SignUpTimeSeconds) && ( curDayBeginSecond <= m_EndSeconds) )
	{
		m_state = en_CampBattle_InBattle;
	}
	else
	{
		m_state = en_CampBattle_BeforeStart;
	}
	
	return true;
}

void CampBattleFactory::onTimer(int nEventId)
{
	if(1 == nEventId)
	{
		checkCampBattle();	
	}
	else if(2 == nEventId)
	{
		pollingWaitVec();
	}
	else if(3 == nEventId)
	{
		broadCastRankList();
	}
	else if(4 == nEventId)
	{
		broadCastBattleReport();
	}
	else if(5 == nEventId)
	{
		checkJumpBattleVec();
	}
	else if( 6 == nEventId)
	{
		startMatchBattle();
	}
}

void CampBattleFactory::checkJumpBattleVec()
{
	IUserStateManager* pUserManager = getComponent<IUserStateManager>(COMPNAME_LoginManager,IID_IUserStateManager);
	assert(pUserManager);	
	
	std::set<string>::iterator iter = m_JumpBattleActorNameSet.begin();
	for( ;iter != m_JumpBattleActorNameSet.end();)
	{
		HEntity hEntity = pUserManager->getActorByName(*iter);
		IEntity* pEntity = getEntityFromHandle(hEntity);
		if(pEntity)
		{
		  if(SignUp(hEntity))
			{
				m_JumpBattleActorNameSet.erase(iter++);
			}
			else
			{
				++iter;
			}
		}
		else
		{
			m_JumpBattleActorNameSet.erase(iter++);
		}
	}
}

void CampBattleFactory::broadCastBattleReport()
{

	if(m_LogScMsg.szbattlereport_size() == 0 ) return;
	
	IUserStateManager* pUserStateMgr = getComponent<IUserStateManager>(COMPNAME_LoginManager,IID_IUserStateManager);
	assert(pUserStateMgr);
	GSProto::SCMessage outMsg;
	HelpMakeScMsg( outMsg,GSProto::CMD_CAMPBATTLE_BATTLE_LOG,m_LogScMsg);
	
	//FDLOG("CampBattleFactory")<<"CMD_CAMPBATTLE_BATTLE_LOG"<<m_LogScMsg.DebugString()<<endl;

	for(set<string>::iterator iter = m_allBroadCastNameSet.begin(); iter != m_allBroadCastNameSet.end(); ++iter)
	{
		HEntity lHEntity = pUserStateMgr->getActorByName(*iter);
		IEntity* pEntity = getEntityFromHandle(lHEntity);
		if(pEntity == 0) continue;
		
		pEntity->sendMessage(outMsg);
	}
	
	m_LogScMsg.Clear();
}

void CampBattleFactory::broadCastRankList()
{
	IUserStateManager* pUserStateMgr = getComponent<IUserStateManager>(COMPNAME_LoginManager,IID_IUserStateManager);
	assert(pUserStateMgr);

	if((m_RankListA.size() == 0)|| ( m_RankListB.size()==0) )
	{
		return;
	}
	GSProto::CMD_CAMPBATTLE_REFREASH_RANKLIST_SC scMsg;
	//左边 列表
	for(size_t i = 0; i< m_RankListA.size();++i)
	{
		BattleUnit unit = m_RankListA[i];
		
		GSProto::RankNodeInfo& nodeInfo = *scMsg.add_szleftranklist();
		nodeInfo.set_imainheroid( unit.iMainHeroId);
		nodeInfo.set_iactorhonor( unit.iHonor);
		nodeInfo.set_stractorname( unit.sActorName);
	}

	//右边 列表
	for(size_t i = 0; i< m_RankListB.size();++i)
	{
		BattleUnit unit = m_RankListB[i];
		
		GSProto::RankNodeInfo& nodeInfo = *scMsg.add_szrightranklist();
		nodeInfo.set_imainheroid( unit.iMainHeroId);
		nodeInfo.set_iactorhonor( unit.iHonor);
		nodeInfo.set_stractorname( unit.sActorName);
	}

	GSProto::SCMessage outPkg;
	HelpMakeScMsg( outPkg,GSProto::CMD_CAMPBATTLE_REFREASH_RANKLIST,scMsg);
	
	//FDLOG("CampBattleFactory")<<"CMD_CAMPBATTLE_REFREASH_RANKLIST|"<<scMsg.DebugString()<<endl;
	
	for(CampMap::iterator iter = m_allBattlUnit.begin(); iter != m_allBattlUnit.end(); ++iter)
	{
		HEntity lHEntity = pUserStateMgr->getActorByName(iter->first);
		IEntity* pEntity = getEntityFromHandle(lHEntity);
		if(pEntity == 0) continue;
		pEntity->sendMessage(outPkg);
	}
}

void CampBattleFactory::pollingWaitVec()
{
	
	for(CampMap::iterator iter = m_waitBattleMapA.begin(); iter != m_waitBattleMapA.end(); ++iter)
	{
		if(iter->second.iBattleCD  == 0)
			continue;
		if(iter->second.iBattleCD >0)
		{
			iter->second.iBattleCD -= 1;
			
			if(iter->second.iBattleCD  == 0)
			{
				CampMap::iterator iter2 = m_allBattlUnit.find(iter->second.sActorName);
				iter2->second.enActorState = en_CampBattleActorState_InWaitOver;
				iter2->second.iBattleCD = 0;
				FDLOG("CampBattleFactory")<<iter->second.sActorName<<"| AAAAA | CD == 0 |en_CampBattleActorState_InWaitOver"<<endl;
			}
		}
	}

	for(CampMap::iterator iter = m_waitBattleMapB.begin(); iter != m_waitBattleMapB.end(); ++iter)
	{
		if(iter->second.iBattleCD  == 0)
			continue;
		if(iter->second.iBattleCD >0)
		{
			iter->second.iBattleCD -= 1;
			if(iter->second.iBattleCD  == 0)
			{
				CampMap::iterator iter2 = m_allBattlUnit.find(iter->second.sActorName);
				iter2->second.enActorState = en_CampBattleActorState_InWaitOver;
				iter2->second.iBattleCD = 0;
				FDLOG("CampBattleFactory")<<iter->second.sActorName<<"| BBBBBB | CD == 0 |en_CampBattleActorState_InWaitOver"<<endl;
			}
		}
	}
}

void CampBattleFactory::checkCampBattle()
{
	IZoneTime* pZoneTime = getComponent<IZoneTime>(COMPNAME_ZoneTime,IID_IZoneTime);
	assert(pZoneTime);

	unsigned int curDayBeginSecond =  ONE_DAY_SECONDS -pZoneTime->GetCurDayLeftSeconds(); //pZoneTime->GetDayBeginSecond(pZoneTime->GetCurSecond() );
	switch(m_state)
	{
		case en_CampBattle_BeforeStart:
			{
				if( (curDayBeginSecond >= m_StartSeconds) && (curDayBeginSecond <=  m_StartSeconds + m_SignUpTimeSeconds) )
				{
					m_state = en_CampBattle_StartSignUp;
					//ITimerComponent* timeComponent = getComponent<ITimerComponent>(COMPNAME_TimeAxis,IID_ITimerComponent);
					//assert(timeComponent);
					//timeComponent->killTimer(m_hCheckTimerHandle);
					//m_hCheckTimerHandle = timeComponent->setTimer(this,1,1000,"CampBattleFactory");
					checkCampBattle();
				}
				
			}break;
		
		case en_CampBattle_StartSignUp:
			{
				if((curDayBeginSecond >= m_StartSeconds + m_SignUpTimeSeconds) && ( curDayBeginSecond <= m_EndSeconds) )
				{
					//将准备参战的玩家分阵营
					partPrePareList();

					ITimerComponent* timeComponent = getComponent<ITimerComponent>(COMPNAME_TimeAxis,IID_ITimerComponent);
					assert(timeComponent);
					
					IGlobalCfg* globalConf = getComponent<IGlobalCfg>(COMPNAME_GlobalCfg,IID_IGlobalCfg);
					int seconds = globalConf->getInt("阵营战匹配一场战斗的时间",20);
					m_hMatchBattleTimerHandle = timeComponent->setTimer(this,6,seconds*1000,"CampBattleFactory");
	
					int sigleAttackSecond = globalConf->getInt("阵营战战斗一个攻击时间",1);
					m_hCheckWaitVecTimerHandle = timeComponent->setTimer(this,2,sigleAttackSecond*1000,"CampBattleFactory");

					int refreshListSeconds = globalConf->getInt("阵营战刷新排行列表的时间",5);
					m_hBroadCastTimerHandle = timeComponent->setTimer(this,3,refreshListSeconds*1000,"CampBattleFactory");

					int ReportSeconds =  globalConf->getInt("阵营战广播战报间隔时间",6);
					m_hBroadCastReportTimerHandle =  timeComponent->setTimer(this,4,ReportSeconds*1000,"CampBattleFactory");


				       m_JumpBattleTimerHandle = timeComponent->setTimer(this,5,1*1000,"CampBattleFactory");
					   
					 timeComponent->killTimer(m_hCheckTimerHandle);
					m_hCheckTimerHandle = timeComponent->setTimer(this,1,1000,"CampBattleFactory");

					//广播战斗开始 
					GSProto::CMD_CAMPBATTLE_REFREASH_TIME_SC msg;
					GSProto::RemaindTime& time = *msg.mutable_remaindtime();
					time.set_bisstarted(true);
					time.set_iremaindsecond(m_EndSeconds- curDayBeginSecond);

					GSProto::SCMessage outPkg;
					HelpMakeScMsg( outPkg,GSProto::CMD_CAMPBATTLE_REFREASH_TIME,msg);

					IUserStateManager* pUserStateMgr = getComponent<IUserStateManager>(COMPNAME_LoginManager,IID_IUserStateManager);
					assert(pUserStateMgr);

					for(set<string>::iterator iter = m_allBroadCastNameSet.begin(); iter != m_allBroadCastNameSet.end(); ++iter)
					{
						HEntity lHEntity = pUserStateMgr->getActorByName( *iter );
						IEntity* pEntity = getEntityFromHandle(lHEntity);
						if(pEntity == 0) continue;
						pEntity->sendMessage(outPkg);
					}

					m_state = en_CampBattle_InBattle;

					//开场 匹配一次
					startMatchBattle();
				}
				
			}break;

		
		case en_CampBattle_InBattle:
			{
				
				if(curDayBeginSecond >= m_EndSeconds)
				{
					m_state = en_CampBattle_InEnd;
				}
				
			}break;
		
		case en_CampBattle_InEnd:
			{
				ITimerComponent* timeComponent = getComponent<ITimerComponent>(COMPNAME_TimeAxis,IID_ITimerComponent);
				assert(timeComponent);
				timeComponent->killTimer(m_hCheckTimerHandle);
				m_hCheckTimerHandle = timeComponent->setTimer(this,1,5*1000,"CampBattleFactory");
				m_state = en_CampBattle_BeforeStart; 

				//关闭定时器
				ITimerComponent* timer = getComponent<ITimerComponent>(COMPNAME_TimeAxis,IID_ITimerComponent);
				assert(timer);
				timer->killTimer( m_hCheckWaitVecTimerHandle);
				timer->killTimer( m_hBroadCastTimerHandle);
				timer->killTimer( m_hBroadCastReportTimerHandle);
					
				timeComponent->killTimer( m_hMatchBattleTimerHandle);

				//通知玩家阵营战结束
				
				IUserStateManager* pUserStateMgr = getComponent<IUserStateManager>(COMPNAME_LoginManager,IID_IUserStateManager);
				assert(pUserStateMgr);
				for(set<string>::iterator iter = m_allBroadCastNameSet.begin(); iter != m_allBroadCastNameSet.end(); ++iter)
				{
					
					HEntity lHEntity = pUserStateMgr->getActorByName(*iter);
					IEntity* pEntity = getEntityFromHandle(lHEntity);
					if(pEntity == 0) continue;
					CampMap::iterator iter2 = m_allBattlUnit.find(*iter);
					if( iter2 != m_allBattlUnit.end() )
					{
						GSProto::CMD_CMAPBATTLE_SYSTEM_CLOSE_SC scMsg;
						scMsg.set_itopdoublehitcount(iter2->second.iTopSuccession);
						scMsg.set_iwintimes(iter2->second.iWinTimes);
						scMsg.set_ifailedtimes(iter2->second.iFailedTimes);
						scMsg.set_irewardhonor(iter2->second.iHonor);
						scMsg.set_irewardsilver( iter2->second.iSilver);
						pEntity->sendMessage( GSProto::CMD_CMAPBATTLE_SYSTEM_CLOSE, scMsg);
					}
					else
					{
						GSProto::CMD_CMAPBATTLE_SYSTEM_CLOSE_SC scMsg;
						scMsg.set_itopdoublehitcount( 0);
						scMsg.set_iwintimes( 0);
						scMsg.set_ifailedtimes( 0);
						scMsg.set_irewardhonor( 0);
						scMsg.set_irewardsilver( 0);
						pEntity->sendMessage( GSProto::CMD_CMAPBATTLE_SYSTEM_CLOSE, scMsg);
					}
				}
	

				//数据清理
				m_RankListA.clear();
				m_RankListB.clear();
				m_matchingDequeA.clear();
				m_matchingDequeB.clear();
				m_waitBattleMapA.clear();
				m_waitBattleMapB.clear();
				m_allBattlUnit.clear();
			}break;
	}
	
}

bool CampBattleFactory::SignUp(HEntity actorEntity)
{

	IEntity *pEntity = getEntityFromHandle(actorEntity);
	if(!pEntity) return false;
	string strActorName = pEntity->getProperty(PROP_ENTITY_NAME,"");
	
	IFormationSystem* pFormationSystem = static_cast<IFormationSystem*>(pEntity->querySubsystem( IID_IFormationSystem));
	assert(pFormationSystem);

	// 已经开展将新玩家放入队列 
	if(m_state == en_CampBattle_StartSignUp)	
	{
	
		BattleUnit unit;
		unit.enActorState= en_CampBattleActorState_SignUp;
		unit.sActorName = strActorName;
		
		int fightValue = pEntity->getProperty(PROP_ENTITY_FIGHTVALUE,1);
		unit.iFightValue = fightValue;
		unit.iMainHeroId = pFormationSystem->getVisibleHeroID();

		m_allBattlUnit[unit.sActorName] = unit;
		return true;
	}
	else if( m_state == en_CampBattle_InBattle )
	{

		CampMap::iterator iter = m_allBattlUnit.find(strActorName);
		if(iter == m_allBattlUnit.end())
		{
			BattleUnit unit;
			unit.enActorState = en_CampBattleActorState_SignUp;
			unit.sActorName = strActorName;
			
			int fightValue = pEntity->getProperty(PROP_ENTITY_FIGHTVALUE,1);
			unit.iFightValue = fightValue;
			unit.iMainHeroId = pFormationSystem->getVisibleHeroID();

			bool res= addCamp(unit);
			if(res)
			{
				m_allBattlUnit[unit.sActorName] = unit;
			}
			return res;
		}
		else
		{
			if(iter->second.iCamp == CampTypeA )
			{
				if(iter->second.enActorState == en_CampBattleActorState_InWaitOver)
				{
					CampMap::iterator iter2 = m_waitBattleMapA.find(strActorName);
					assert(iter2 != m_waitBattleMapA.end());
					m_waitBattleMapA.erase(iter2);
					
					iter->second.enActorState = en_CampBattleActorState_InMatching;
					m_matchingDequeA.push_back(iter->second.sActorName);
					return true;
				}
				else if(iter->second.enActorState == en_CampBattleActorState_SignOut)
				{
					iter->second.enActorState = en_CampBattleActorState_InMatching;
					m_matchingDequeA.push_back(iter->second.sActorName);
					iter->second.iSuccessionTimes = 0;
					iter->second.hpMap.clear();
					
					return true;
				}
				else
				{
					if(m_JumpBattleActorNameSet.find(strActorName) == m_JumpBattleActorNameSet.end())
					{
						m_JumpBattleActorNameSet.insert(strActorName);
						return true;
					}
					
					return false;
				}
			}
			else if(iter->second.iCamp == CampTypeB )
			{
				if(iter->second.enActorState == en_CampBattleActorState_InWaitOver)
				{
					CampMap::iterator iter2 = m_waitBattleMapB.find(strActorName);
					assert(iter2 != m_waitBattleMapB.end());
					m_waitBattleMapB.erase(iter2);
					
					iter->second.enActorState = en_CampBattleActorState_InMatching;
					m_matchingDequeB.push_back(iter->second.sActorName);
					
					return true;
				}
				else if(iter->second.enActorState == en_CampBattleActorState_SignOut)
				{
					iter->second.enActorState = en_CampBattleActorState_InMatching;
					iter->second.iSuccessionTimes = 0;
					iter->second.hpMap.clear();
					m_matchingDequeB.push_back(iter->second.sActorName);
					
					return true;
				}
				else
				{
					if(m_JumpBattleActorNameSet.find(strActorName) == m_JumpBattleActorNameSet.end())
					{
						m_JumpBattleActorNameSet.insert(strActorName);
						return true;
					}
					
					return false;
				}
			}
		}
	}
	return false;
}

bool CampBattleFactory::addCamp(BattleUnit& unit)
{
	int actorCountA = m_matchingDequeA.size() + m_waitBattleMapA.size();
	int actorCountB = m_matchingDequeB.size() + m_waitBattleMapB.size(); 

	if(actorCountA > actorCountB)
	{
		unit.iCamp = CampTypeB;
		m_matchingDequeB.push_back(unit.sActorName);
		m_iFightValueB += unit.iFightValue;
	}
	else if(actorCountA < actorCountB)
	{
		unit.iCamp = CampTypeA;
		m_matchingDequeA.push_back(unit.sActorName);
		m_iFightValueA += unit.iFightValue;
	}
	else
	{
		int fightA = 0;
		int fightB = 0;
		if(actorCountA != 0)
		{
			fightA = m_iFightValueA/actorCountA;
			
		}
		if(actorCountB != 0)
		{
			fightB = m_iFightValueB/actorCountB;
		}

		int centerfight = (fightA+fightB)/2;

		if(fightA > fightB)
		{
			if(centerfight >= unit.iFightValue)
			{
				unit.iCamp = CampTypeA;
				m_matchingDequeA.push_back(unit.sActorName);
				m_iFightValueA += unit.iFightValue;
			}
			else
			{
				unit.iCamp = CampTypeB;
				m_matchingDequeB.push_back(unit.sActorName);
				m_iFightValueB += unit.iFightValue;
			}
		}
		else
		{
			if(centerfight >= unit.iFightValue )
			{
				unit.iCamp = CampTypeB;
				m_matchingDequeB.push_back(unit.sActorName);
				m_iFightValueB += unit.iFightValue;
			}
			else
			{
				unit.iCamp = CampTypeA;
				m_matchingDequeA.push_back(unit.sActorName);
				m_iFightValueA += unit.iFightValue;
			}
		}

	}
	
	return true;
}

bool CampBattleFactory::IsStartCampBattle()
{
	if(m_state == en_CampBattle_StartSignUp || m_state == en_CampBattle_InBattle )	
	{
		return true;
	}
	
	return false;
}

struct CampBattlePVPFight
{

	CampBattlePVPFight(CampBattleFactory* pCampBattleFactory, string ActorAName, string ActorBName):
		m_pCampBattleFactory(pCampBattleFactory), m_actorAName(ActorAName), m_actorBName(ActorBName){}

	void operator()(taf::Int32 iRet, ServerEngine::BattleData& battleInfo)
	{
		if(iRet != en_FightResult_OK)
		{
			FDLOG("CampBattleFactory")<<"CampBattleFactoryPVP|"<<iRet<<"|"<<m_actorAName << "|" << m_actorBName<<endl;
			//todo
			m_pCampBattleFactory->reMatching(m_actorAName,m_actorBName);
			return; 
		}

		m_pCampBattleFactory->processFightResult(m_actorAName, m_actorBName, battleInfo);
	}

private:

	CampBattleFactory* m_pCampBattleFactory;
	string m_actorAName;
	string m_actorBName;
};

void CampBattleFactory::reMatching(string actorA, string actorB)
{
	m_matchingDequeA.push_back(actorA);
	m_matchingDequeB.push_back(actorB);
}

void CampBattleFactory::processFightResult( string  actorAName, string actorBName, ServerEngine::BattleData& battleInfo)
{
	//从匹配列表移除，加入待战列表，添加回合CD，当回合数为0时，放入待战列表(在待战列表处理中处理)
	
	//下行战斗结果 给全部参加者

	//战斗胜利，发奖

	if(m_state != en_CampBattle_InBattle )
	{
		return;
	}

	IUserStateManager* pUserManager = getComponent<IUserStateManager>(COMPNAME_LoginManager,IID_IUserStateManager);
	assert(pUserManager);	
	HEntity hEntityA= pUserManager->getActorByName(actorAName);
	IEntity* pEntityA= getEntityFromHandle(hEntityA);
	if(!pEntityA)	
		return;
 
	HEntity hEntityB= pUserManager->getActorByName(actorBName);
	IEntity* pEntityB= getEntityFromHandle(hEntityB);
	if(!pEntityB)	
		return;
	
	IGlobalCfg* pGlobal = getComponent<IGlobalCfg>(COMPNAME_GlobalCfg,IID_IGlobalCfg);
	assert(pGlobal);

	CloseAttCommUP closeA(hEntityA);
	CloseAttCommUP closeB(hEntityB);

	//发奖,下行战报
	int winerRewardHonor = 0;
	int winerRewardSilver = 0;

	CampMap::iterator iterA = m_allBattlUnit.find(actorAName);
	CampMap::iterator iterB = m_allBattlUnit.find(actorBName);
	
	BattleUnit actorAUnit = iterA->second;
	BattleUnit actorBUnit = iterB->second;
	
	
	getWinerSilverAndHonor(actorAUnit,actorBUnit,winerRewardSilver,winerRewardHonor,battleInfo);
	
	int loserRewardHonor = 0;
	int loserRewardSilver = 0;
	getLoserSilverAndHonor(actorAUnit,actorBUnit,loserRewardSilver,loserRewardHonor,battleInfo);

	broadCastBattleInfo(actorAUnit,actorBUnit,battleInfo.bAttackerWin,winerRewardSilver,winerRewardHonor);
	
	if(battleInfo.bAttackerWin)
	{
		iterA->second.iHonor += winerRewardHonor;
		iterA->second.iSuccessionTimes += 1;
		if(iterA->second.iSuccessionTimes > iterA->second.iTopSuccession)
		{
			iterA->second.iTopSuccession = iterA->second.iSuccessionTimes; 
		}
		iterA->second.iWinTimes++;
		iterA->second.iSilver += winerRewardSilver;

		iterB->second.iHonor += loserRewardHonor;
		iterB->second.iSilver += loserRewardSilver;
		iterB->second.iSuccessionTimes = 0;
		iterB->second.iFailedTimes++;

		pEntityA->changeProperty(PROP_ACTOR_SILVER ,winerRewardSilver,GSProto::en_Reason_CampBattleGet);
		pEntityA->changeProperty(PROP_ENTITY_HONOR ,winerRewardHonor,GSProto::en_Reason_CampBattleGet);

		pEntityB->changeProperty(PROP_ACTOR_SILVER ,loserRewardSilver,GSProto::en_Reason_CampBattleGet);
		pEntityB->changeProperty(PROP_ENTITY_HONOR ,loserRewardHonor,GSProto::en_Reason_CampBattleGet);

		
	}
	else
	{

		iterB->second.iHonor += winerRewardHonor;
		iterB->second.iSuccessionTimes += 1;
		if(iterB->second.iSuccessionTimes > iterB->second.iTopSuccession)
		{
			iterB->second.iTopSuccession = iterB->second.iSuccessionTimes; 
		}
		iterB->second.iWinTimes++;
		iterB->second.iSilver += winerRewardSilver;

		iterA->second.iHonor += loserRewardHonor;
		iterA->second.iSilver += loserRewardSilver;
		iterA->second.iSuccessionTimes = 0;
		iterA->second.iFailedTimes++;
		pEntityB->changeProperty(PROP_ACTOR_SILVER ,winerRewardSilver,GSProto::en_Reason_CampBattleGet);
		pEntityB->changeProperty(PROP_ENTITY_HONOR ,winerRewardHonor,GSProto::en_Reason_CampBattleGet);

		pEntityA->changeProperty(PROP_ACTOR_SILVER ,loserRewardSilver,GSProto::en_Reason_CampBattleGet);
		pEntityA->changeProperty(PROP_ENTITY_HONOR ,loserRewardHonor,GSProto::en_Reason_CampBattleGet);
	}
	
	
	//存入待战中
	GSProto::FightDataAll fightDataAll;
	if(!fightDataAll.ParseFromString(battleInfo.strBattleBody) )
	{
		return;
	}
	
	for(int i = 0; i < fightDataAll.szfightobject().size(); i++)
	{
		const GSProto::FightObj& tmpFightObj = fightDataAll.szfightobject().Get(i);
		int iPos = tmpFightObj.ipos();
		if(iPos < GSProto::MAX_BATTLE_MEMBER_SIZE/2)
		{
			iterA->second.hpMap[iPos] = tmpFightObj.ifinhp();
			FDLOG("CampBattleFactory")<<"  A "<<"|iPos ====|"<< iPos <<"| hp ==== |"<<tmpFightObj.ifinhp()<<endl;
		}
		else if( (iPos >= GSProto::MAX_BATTLE_MEMBER_SIZE/2) && (iPos < GSProto::MAX_BATTLE_MEMBER_SIZE) )
		{
			iterB->second.hpMap[iPos - GSProto::MAX_BATTLE_MEMBER_SIZE/2] = tmpFightObj.ifinhp();
			FDLOG("CampBattleFactory")<<"  B "<<"|iPos ====|"<< iPos - GSProto::MAX_BATTLE_MEMBER_SIZE/2 <<"| hp ==== |"<<tmpFightObj.ifinhp()<<endl;
		}
	}

	if(battleInfo.bAttackerWin)
	{
		iterB->second.hpMap.clear();
	}
	else
	{
		iterA->second.hpMap.clear();
	}
	
	iterA->second.iBattleCD = fightDataAll.szfightaction().size();	//CD 加界面切换缓冲时间
	iterB->second.iBattleCD = fightDataAll.szfightaction().size();

	iterA->second.enActorState = en_CampBattleActorState_InWaiting;
	iterB->second.enActorState = en_CampBattleActorState_InWaiting;
	
	m_waitBattleMapA[iterA->second.sActorName] = iterA->second; 
	m_waitBattleMapB[iterB->second.sActorName] = iterB->second; 

	
	updateRankList(m_RankListA,iterA->second);
	updateRankList(m_RankListB,iterB->second);
	iterA->second.ibattleTimes++;
	iterB->second.ibattleTimes++;
	
	if(iterA->second.ibattleTimes == 1)
	{
		//完成阵营战一次
		EventArgs argsA;
		argsA.context.setInt("times",1);
		argsA.context.setInt("entity",hEntityA);
		pEntityA->getEventServer()->setEvent(EVENT_ENTITY_FINISHCONDITION, argsA);
	}

	if(iterB->second.ibattleTimes == 1)
	{
		EventArgs argsB;
		argsB.context.setInt("times",1);
		argsB.context.setInt("entity",hEntityB);
		pEntityB->getEventServer()->setEvent(EVENT_ENTITY_FINISHCONDITION, argsB);
	}
	
	FDLOG("CampBattleFactory")<<"  A |"<<iterA->second.sActorName<<"|Camp == |"<<iterA->second.iCamp<<endl;
	FDLOG("CampBattleFactory")<<"  B |"<<iterB->second.sActorName<<"|Camp == |"<<iterB->second.iCamp<<endl;
	startMatchBattle();
	

}

//暂时处理100个
void CampBattleFactory::updateRankList(vector<BattleUnit>& vec, BattleUnit& actor)
{
	std::vector<BattleUnit>::iterator iter = find(vec.begin(),vec.end(),actor);
	if(iter != vec.end())
	{
		(*iter).iHonor = actor.iHonor;
		sort(vec.begin(),vec.end(),compareWithHonor);
	}
	else
	{
		if(vec.size() == 4)
		{
			if(vec[3].iHonor < actor.iHonor)
			{
				vec.pop_back();
				vec.push_back(actor);
				sort(vec.begin(),vec.end(),compareWithHonor);
			}
		}
		
		if(vec.size()<4)
		{
			vec.push_back(actor);
			sort(vec.begin(),vec.end(),compareWithHonor);
		}
	}
}

void CampBattleFactory::getLoserSilverAndHonor( BattleUnit actorA, BattleUnit actorB, int &silver, int &honor,ServerEngine::BattleData& battleInfo)
{
	IUserStateManager* pUserStateMgr = getComponent<IUserStateManager>(COMPNAME_LoginManager,IID_IUserStateManager);
	assert(pUserStateMgr);
	
	IGlobalCfg* pGlobal = getComponent<IGlobalCfg>(COMPNAME_GlobalCfg, IID_IGlobalCfg);	
	assert(pGlobal);	
	int iScene = pGlobal->getInt("阵营战", 1);
	
	int topHonor = pGlobal->getInt("荣誉值获取上限",5000);
	int failedSilver = pGlobal->getInt("阵营战失败方银币固定奖励",500);
	honor = pGlobal->getInt("阵营战失败方荣誉值固定奖励",5);
	if(!battleInfo.bAttackerWin)
	{
		actorB = actorA;
	}
	//败方得小奖
	//actorB.iHonor += honor;
	int iTempHonor = actorB.iHonor + honor;
	int iGetHonor = honor;
	if(topHonor <= iTempHonor )
	{
		iGetHonor = topHonor - actorB.iHonor;
	}
	else
	{
		actorB.iHonor = iTempHonor;
	}
	honor = iGetHonor;

	//关闭上浮
	HEntity lHEntity = pUserStateMgr->getActorByName(actorB.sActorName);
	IEntity* pEntity = getEntityFromHandle(lHEntity); 
	CloseAttCommUP close(lHEntity);
	
	silver = failedSilver;

	GSProto::Cmd_Sc_CommFightResult scCommFightResult;
	scCommFightResult.set_isceneid(iScene);
	scCommFightResult.set_iissuccess(0);
	scCommFightResult.set_istar(battleInfo.iStar);

	GSProto::FightAwardResult* pAwardResult = scCommFightResult.mutable_awardresult();
	
	{
		GSProto::PropItem* pNewItem = pAwardResult->mutable_szawardproplist()->Add();
		pNewItem->set_ilifeattid(GSProto::en_LifeAtt_Honor);
		pNewItem->set_ivalue(iGetHonor);
	}

	{
		GSProto::PropItem* pNewItem = pAwardResult->mutable_szawardproplist()->Add();
		pNewItem->set_ilifeattid(GSProto::en_LifeAtt_Silver);
		pNewItem->set_ivalue(silver);
	}
	
	
	if(!pEntity) return;
	
	GSProto::SCMessage pkg;
	HelpMakeScMsg(pkg, GSProto::CMD_COMM_FIGHTRESULT, scCommFightResult);
	IFightSystem* pFightSys = static_cast<IFightSystem*>(pEntity->querySubsystem(IID_IFightSystem));
	assert(pFightSys);
	
	pFightSys->sendAllBattleMsg(battleInfo, pkg);

}

void CampBattleFactory::getWinerSilverAndHonor( BattleUnit actorA, BattleUnit actorB, int &silver, int &honor,ServerEngine::BattleData& battleInfo)
{
	IUserStateManager* pUserStateMgr = getComponent<IUserStateManager>(COMPNAME_LoginManager,IID_IUserStateManager);
	assert(pUserStateMgr);
	IGlobalCfg* pGlobal = getComponent<IGlobalCfg>(COMPNAME_GlobalCfg,IID_IGlobalCfg);
	assert(pGlobal);
	int iScene = pGlobal->getInt("阵营战", 1);

	int upSilver = pGlobal->getInt("阵营战连胜银币增量",50);
	int baseSilver = pGlobal->getInt("阵营战银币奖励基数",1000);
	int upHonor = pGlobal->getInt("阵营战连胜荣誉增量",5);
	int baseHonor = pGlobal->getInt("阵营战荣誉奖励基数",10);
	int topHonor = pGlobal->getInt("荣誉值获取上限",5000);
	int SingleendWinHonor = pGlobal->getInt("终结对方连胜奖励增量",5);

	if(!battleInfo.bAttackerWin)
	{
		BattleUnit tempUnit = actorA;
		actorA = actorB;
		actorB = tempUnit;
	}
	actorA.iSuccessionTimes += 1;

	HEntity lHEntity = pUserStateMgr->getActorByName(actorA.sActorName);
	IEntity* pEntity = getEntityFromHandle(lHEntity);
	//关闭上浮
	CloseAttCommUP close(lHEntity);
	
	//胜方得大奖
	silver = (actorA.iSuccessionTimes-1)*upSilver + baseSilver;
	int totalHonor = (actorA.iSuccessionTimes-1)*upHonor + baseHonor;
//	honor = totalHonor;
	if(actorB.iSuccessionTimes>1)
	{
		int endWinHonor = actorB.iSuccessionTimes*SingleendWinHonor;
		totalHonor += endWinHonor;
	}
	
	if( actorA.iHonor + totalHonor >= topHonor)
	{
		totalHonor = topHonor - actorA.iHonor;
	}
	
	honor = totalHonor;
	
	GSProto::Cmd_Sc_CommFightResult scCommFightResult;
	scCommFightResult.set_isceneid(iScene);
	scCommFightResult.set_iissuccess(1);
	scCommFightResult.set_istar(battleInfo.iStar);

	GSProto::FightAwardResult* pAwardResult = scCommFightResult.mutable_awardresult();
	
	{
		GSProto::PropItem* pNewItem = pAwardResult->mutable_szawardproplist()->Add();
		pNewItem->set_ilifeattid(GSProto::en_LifeAtt_Honor);
		pNewItem->set_ivalue(honor);
	}

	{
		GSProto::PropItem* pNewItem = pAwardResult->mutable_szawardproplist()->Add();
		pNewItem->set_ilifeattid(GSProto::en_LifeAtt_Silver);
		pNewItem->set_ivalue(silver);
	}
	
	GSProto::SCMessage pkg;
	HelpMakeScMsg(pkg, GSProto::CMD_COMM_FIGHTRESULT, scCommFightResult);
	IFightSystem* pFightSys = static_cast<IFightSystem*>(pEntity->querySubsystem(IID_IFightSystem));
	assert(pFightSys);
	
	pFightSys->sendAllBattleMsg(battleInfo, pkg);

}

void CampBattleFactory::broadCastBattleInfo( BattleUnit actorA, BattleUnit actorB, bool bIsAWin,int silver,int honor)
{
	
	IGlobalCfg* pGlobal = getComponent<IGlobalCfg>(COMPNAME_GlobalCfg,IID_IGlobalCfg);
	assert(pGlobal);
	//int topHonor = pGlobal->getInt("荣誉值获取上限",5000);
	//int SingleendWinHonor = pGlobal->getInt("终结对方连胜奖励增量",5);

	int winerCamp = 1;
	if(!bIsAWin)
	{
		BattleUnit temp = actorA;
		actorA = actorB;
		actorB = temp;
		winerCamp = 2;
	}
	
	
	//无连胜
	if(0 == actorA.iSuccessionTimes)
	{
		GSProto::CampBattleReport& report = *m_LogScMsg.add_szbattlereport();
		report.set_strwinactorname( actorA.sActorName);
		report.set_strfailedactorname( actorB.sActorName);
		report.set_ibattleresoulttype(GSProto::en_CampBattleResoultType_Win);
		report.set_irewardsilver(silver);
		/*
		if( honor-topHonor == 0)
		{
			report.set_irewardhonor(0);
		}
		else
		{
			if(actorB.iSuccessionTimes>1)
			{
			 	int endWinHonor = actorB.iSuccessionTimes*SingleendWinHonor;
			 	honor -= endWinHonor;
			}
			report.set_irewardhonor(honor);
		}
		*/
		report.set_irewardhonor(honor);
		report.set_iwinercampid(winerCamp);
		
		//是否终结
		if(actorB.iSuccessionTimes>1)
		{
			GSProto::CampBattleReport& report2 = *m_LogScMsg.add_szbattlereport();
			report2.set_strwinactorname( actorA.sActorName);
			report2.set_strfailedactorname( actorB.sActorName);
			report2.set_ibattleresoulttype(GSProto::en_CampBattleResoultType_WinEnd);
			report2.set_icurdoublehittimes(actorB.iSuccessionTimes);
			report2.set_iwinercampid(winerCamp);
			//int endWinHonor = actorB.iSuccessionTimes*SingleendWinHonor;
			
			/*if(honor >= topHonor)
			{
				report.set_irewardhonor(0);
			}
			else
			{
				report.set_irewardhonor(endWinHonor);
			}*/
			report2.set_irewardhonor(honor);
		}
	}
	//连胜
	else
	{
		
		GSProto::CampBattleReport& report = *m_LogScMsg.add_szbattlereport();
		report.set_strwinactorname( actorA.sActorName);
		report.set_strfailedactorname( actorB.sActorName);
		report.set_ibattleresoulttype(GSProto::en_CampBattleResoultType_DoubleHit);
		//连杀次数并没有更新
		report.set_icurdoublehittimes(actorA.iSuccessionTimes+1);
		report.set_irewardsilver(silver);
		report.set_iwinercampid(winerCamp);

		/*if( honor-topHonor >= 0)
		{
			report.set_irewardhonor(0);
		}
		else
		{
			if(actorB.iSuccessionTimes>1)
			{
			 	int endWinHonor = actorB.iSuccessionTimes*SingleendWinHonor;
				 honor -= endWinHonor;
			}
			report.set_irewardhonor(honor);
		}*/

		report.set_irewardhonor(honor);
		//是否终结
		if(actorB.iSuccessionTimes > 1)
		{
			GSProto::CampBattleReport& report2 = *m_LogScMsg.add_szbattlereport();
			report2.set_strwinactorname( actorA.sActorName);
			report2.set_strfailedactorname( actorB.sActorName);
			report2.set_ibattleresoulttype(GSProto::en_CampBattleResoultType_WinEnd);
			report2.set_icurdoublehittimes(actorB.iSuccessionTimes);
			//int endWinHonor = actorB.iSuccessionTimes*SingleendWinHonor;
			report2.set_iwinercampid(winerCamp);
		/*	if(honor >= topHonor)
			{
				report.set_irewardhonor();
			}
			else
			{
				report.set_irewardhonor(endWinHonor);
			}*/
			report2.set_irewardhonor(honor);
		}
	}
	
}

void CampBattleFactory::startMatchBattle()
{
	//一方为空不匹配
	if(0 == m_matchingDequeA.size())
	{
		//todo
		return;
	}
	if(0 == m_matchingDequeB.size())
	{
		//todo 
		return;
	}

	IRandom* pRandom = getComponent<IRandom>("Random", IID_IMiniAprRandom);
	assert(pRandom);
	
	//匹配
	int indexA = pRandom->random()%m_matchingDequeA.size();
	int indexB = pRandom->random()%m_matchingDequeB.size();


	string actorAName = m_matchingDequeA[indexA];
	string actorBName = m_matchingDequeB[indexB];
	
	//1.玩家是否在线
	IUserStateManager* pUserStateMgr = getComponent<IUserStateManager>(COMPNAME_LoginManager,IID_IUserStateManager);
	assert(pUserStateMgr);
	
	HEntity lHEntityA = pUserStateMgr->getActorByName(actorAName);
	HEntity lHEntityB = pUserStateMgr->getActorByName(actorBName);

	IEntity* pEntityA = getEntityFromHandle(lHEntityA);
	if(!pEntityA)
	{
		m_matchingDequeA.erase(m_matchingDequeA.begin()+indexA);
		startMatchBattle();
		return;
	}
	IEntity* pEntityB = getEntityFromHandle(lHEntityB);
	if(!pEntityB)
	{
		
		m_matchingDequeB.erase(m_matchingDequeB.begin()+indexB);
		startMatchBattle();
		return;
	}

	ServerEngine::PKRole actorBPkRole;
	getActorCreateContext(actorBPkRole,pEntityB);

	ServerEngine::FightContext ctx;
	fillBattleUnitContext(m_matchingDequeA[indexA],m_matchingDequeB[indexB],ctx);
	

	IFightSystem* fightSystem = static_cast<IFightSystem*>( pEntityA->querySubsystem(IID_IFightSystem));
	assert(fightSystem);
	
	fightSystem->AsyncPVPFight( CampBattlePVPFight(this,m_matchingDequeA[indexA],m_matchingDequeB[indexB]),actorBPkRole,ctx);

	//移除匹配列表，待战斗完成，放入待战列表 在这之间不在匹配列表 也不在待战列表，?
	m_matchingDequeA.erase(m_matchingDequeA.begin()+indexA);
	m_matchingDequeB.erase(m_matchingDequeB.begin()+indexB);
	
}

void CampBattleFactory::fillBattleUnitContext(string actorAName, string actorBName,ServerEngine::FightContext& fightContext)
{

	CampMap::iterator iterA = m_allBattlUnit.find( actorAName );
	CampMap::iterator iterB = m_allBattlUnit.find( actorBName );

	
	IGlobalCfg* pGlobalCfg = getComponent<IGlobalCfg>(COMPNAME_GlobalCfg, IID_IGlobalCfg);	
	assert(pGlobalCfg);	
	int iScene = pGlobalCfg->getInt("阵营战场景", 1);
	fightContext.iFightType = GSProto::en_FightType_CampBattleFactory;
	fightContext.iSceneID = iScene;
	//todo 血量的的改变
	// 填充攻击者
	ServerEngine::CampBattleFightCtx campBattleCtx;
	
	if( iterA->second.iSuccessionTimes >= 1)
	{
		campBattleCtx.attCtx.bFixHP = true;
		campBattleCtx.attCtx.fixHPMap = iterA->second.hpMap;
	}
	else
	{
		campBattleCtx.attCtx.bFixHP = false;
	}

	// 填充目标
	if( iterB->second.iSuccessionTimes >= 1)
	{
		campBattleCtx.targetCtx.bFixHP = true;
		campBattleCtx.targetCtx.fixHPMap = iterB->second.hpMap;
	}
	else
	{
		campBattleCtx.targetCtx.bFixHP = false;
	}


	int attAddPercent =  pGlobalCfg->getInt("阵营战攻击加成万分比", 500);
	int werckAddPercent =  pGlobalCfg->getInt("阵营战护甲加值", 30);
		

	campBattleCtx.attCtx.attAddPercent = 0  - iterA->second.iSuccessionTimes*attAddPercent;
	campBattleCtx.attCtx.werckAddPercent = 0 - iterA->second.iSuccessionTimes*werckAddPercent;

	campBattleCtx.targetCtx.attAddPercent = 0  - iterB->second.iSuccessionTimes*attAddPercent;
	campBattleCtx.targetCtx.werckAddPercent = 0 - iterB->second.iSuccessionTimes*werckAddPercent;

	
	// 打包
	fightContext.context["CampBattleCtx"] = ServerEngine::JceToStr(campBattleCtx);

}

bool CampBattleFactory::getActorCreateContext(ServerEngine::PKRole& pkRole, IEntity* pEntity)
{
	assert(pEntity);
	pkRole.strAccount = pEntity->getProperty(PROP_ACTOR_ACCOUNT,"");
	pkRole.rolePos = pEntity->getProperty(PROP_ACTOR_ROLEPOS,0);
	pkRole.worldID = pEntity->getProperty(PROP_ACTOR_WORLD,0);
	return true;
}


void CampBattleFactory::matchSuccess()
{

}

void CampBattleFactory::partPrePareList()
{	
	std::vector<BattleUnit> m_PrepareList;			//报名准备列表
	CampMap::iterator iter = m_allBattlUnit.begin();
	for(;iter != m_allBattlUnit.end(); ++iter)
	{
		m_PrepareList.push_back(iter->second);
	}
		
	//先排序
	sort(m_PrepareList.begin(),m_PrepareList.end(),compareWithFightValue);
	
	size_t size = m_PrepareList.size();
	if(size == 0)
	{
		return;
	}
	
	m_iFightValueA = 0;
	m_iFightValueB = 0;
	
	//首位组合
	size_t begin = 0;
	size_t end = size-1;

	while( begin <= end && begin < size)
	{
		if(begin == end || size == 2)
		{
			if(m_iFightValueA > m_iFightValueB)
			{
				CampMap::iterator iter = m_allBattlUnit.find(m_PrepareList[begin].sActorName);
				iter->second.enActorState = en_CampBattleActorState_InMatching;
				iter->second.iCamp = CampTypeB;
				
				m_iFightValueB += m_PrepareList[begin].iFightValue;
				m_matchingDequeB.push_back(m_PrepareList[begin].sActorName);
			}
			else 
			{
				CampMap::iterator iter = m_allBattlUnit.find(m_PrepareList[begin].sActorName);
				iter->second.enActorState = en_CampBattleActorState_InMatching;
				iter->second.iCamp = CampTypeA;
				
				m_iFightValueA += m_PrepareList[begin].iFightValue;
				m_matchingDequeA.push_back(m_PrepareList[begin].sActorName);	
			}
			begin++;
		}
		else
		{
			if(begin%2 == 0)
			{
				CampMap::iterator iter = m_allBattlUnit.find(m_PrepareList[begin].sActorName);
				iter->second.enActorState = en_CampBattleActorState_InMatching;
				iter->second.iCamp = CampTypeA;

				m_matchingDequeA.push_back(m_PrepareList[begin].sActorName);
				m_iFightValueA += m_PrepareList[begin].iFightValue;
				
				CampMap::iterator iterend = m_allBattlUnit.find(m_PrepareList[end].sActorName);
				iterend->second.enActorState = en_CampBattleActorState_InMatching;
				iterend->second.iCamp = CampTypeA;
				
				m_matchingDequeA.push_back(m_PrepareList[end].sActorName);
				m_iFightValueA += m_PrepareList[end].iFightValue;
			}
			else
			{
				CampMap::iterator iter = m_allBattlUnit.find(m_PrepareList[begin].sActorName);
				iter->second.enActorState = en_CampBattleActorState_InMatching;
				iter->second.iCamp = CampTypeB;

				CampMap::iterator iterEnd = m_allBattlUnit.find(m_PrepareList[end].sActorName);
				iterEnd->second.enActorState = en_CampBattleActorState_InMatching;
				iterEnd->second.iCamp = CampTypeB;
				
				m_matchingDequeB.push_back(m_PrepareList[begin].sActorName);
				m_matchingDequeB.push_back(m_PrepareList[end].sActorName);
				
				m_iFightValueB += m_PrepareList[begin].iFightValue;
				m_iFightValueB += m_PrepareList[end].iFightValue;
			}
			begin++;
			end--;
		}
	}
	//分块完成 清理数据
	m_PrepareList.clear();
}

const std::vector<BattleUnit>& CampBattleFactory::getCampARankList()
{
	return m_RankListA;
}

const std::vector<BattleUnit>& CampBattleFactory::getCampBRankList()
{
	return m_RankListB;
}

void CampBattleFactory::getRemaindTimeAndState(CampBattleFactoryState& state,int &remaindSecond)
{
	state = m_state;

	IZoneTime* pZoneTime = getComponent<IZoneTime>(COMPNAME_ZoneTime,IID_IZoneTime);
	assert(pZoneTime);

	unsigned int curDayBeginSecond = 24*3600-pZoneTime->GetCurDayLeftSeconds();;
	if(en_CampBattle_StartSignUp == state )
	{
		remaindSecond = m_StartSeconds + m_SignUpTimeSeconds - curDayBeginSecond;
	}
	else if(en_CampBattle_InBattle == state )
	{
		remaindSecond = m_EndSeconds - curDayBeginSecond;
	}
}

bool CampBattleFactory::removeBattleUnit(HEntity actorEntiy)
{

	if(en_CampBattle_InEnd == m_state)
	{
		return true;
	}
	
	//可能在报名阶段下线
	IEntity* pEntity = getEntityFromHandle(actorEntiy);
	if(!pEntity) return false;
	string strName = pEntity->getProperty(PROP_ENTITY_NAME,"");

	CampMap::iterator iter = m_allBattlUnit.find(strName);
   
	if(iter == m_allBattlUnit.end())
	{
		return true;
	}
	
	if(iter->second.enActorState == en_CampBattleActorState_SignOut) 
		return true;

	if(iter->second.enActorState == en_CampBattleActorState_InMatching)
	{
		if(iter->second.iCamp == CampTypeA)
		{
			vector<string>::iterator matchIter = find(m_matchingDequeA.begin(),m_matchingDequeA.end(),strName);
			if(matchIter != m_matchingDequeA.end() )
			{
				m_matchingDequeA.erase(matchIter);
			}
		}
		else
		{
			vector<string>::iterator matchIter = find(m_matchingDequeB.begin(),m_matchingDequeB.end(),strName);
			if(matchIter != m_matchingDequeB.end() )
			{
				m_matchingDequeB.erase(matchIter);
			}
		}
	}

	if(iter->second.enActorState == en_CampBattleActorState_InWaiting || iter->second.enActorState == en_CampBattleActorState_InWaitOver )
	{
		if(iter->second.iCamp == CampTypeA)
		{
			CampMap::iterator waitIter = m_waitBattleMapA.find(strName);
			if(waitIter != m_waitBattleMapA.end() )
			{
				m_waitBattleMapA.erase(waitIter);
			}
			else
			{
				m_JumpBattleActorNameSet.erase(strName);
			}
		}
		else
		{
			CampMap::iterator waitIter = m_waitBattleMapB.find(strName);
			if(waitIter != m_waitBattleMapB.end() )
			{
				m_waitBattleMapB.erase(waitIter);
			}
			else
			{
				m_JumpBattleActorNameSet.erase(strName);
			}
		}
	}

	iter->second.enActorState = en_CampBattleActorState_SignOut;
	iter->second.iSuccessionTimes = 0;
	iter->second.hpMap.clear();
	return true;
}

bool CampBattleFactory::getActorBattlUnit(BattleUnit& unit,IEntity* pEntity)
{
	string strName = pEntity->getProperty(PROP_ENTITY_NAME,"");
	CampMap::iterator iter = m_allBattlUnit.find(strName);
	if(iter == m_allBattlUnit.end())
	{
		return false;
	}
	unit = iter->second;
	return true;
}

void CampBattleFactory::addBroadCastUnit(string strName)
{
	m_allBroadCastNameSet.insert(strName);
}

void CampBattleFactory::removeBroadCastUnit(string strName)
{
	m_allBroadCastNameSet.erase(strName);
}

