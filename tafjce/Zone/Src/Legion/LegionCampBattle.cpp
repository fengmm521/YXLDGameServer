#include "LegionPch.h"
#include "LegionCampBattle.h"
#include "LegionFactory.h"
#include "IJZEntityFactory.h"
#include "LegionImp.h"
#include "IModifyDelegate.h"
#include "IMailSystem.h"


LegionCampBattle::LegionCampBattle(LegionFactory* pLegionFactory):m_pLegionFactory(pLegionFactory)
{	
}

LegionCampBattle::~LegionCampBattle()
{
	ITimerComponent* pTimeAxis = getComponent<ITimerComponent>(COMPNAME_TimeAxis, IID_ITimerComponent);
	if(pTimeAxis)
	{
	  //	pTimeAxis->killTimer(m_hFightHandle);
	}
}

bool LegionCampBattle::initFromData(const ServerEngine::LegionCity& hisData, const ServerEngine::LegionCity& curData)
{
	m_curData = curData;

	m_iMaxLegionHave = 0;
	vector<ServerEngine::LegionBattle> legionBattleList = m_curData.legionBattleList;
	for(size_t i = 0; i < legionBattleList.size(); ++i)
	{
		BattleImpData battleImpData;
		battleImpData.legionBattleData = legionBattleList[i];

		m_BattleList.push_back(battleImpData);
	}
	m_legion2BattleId = m_curData.m_legion2BattleId;

	return true;
}

void LegionCampBattle::initCityCfgData(const LegionCityCfg& cityCfg)
{
	m_CityCfg = cityCfg;
}

void LegionCampBattle::packSaveData(ServerEngine::GlobalLegionCityData& globalCityData)
{

	vector<ServerEngine::LegionBattle> legionBattleList;
	for(size_t i = 0; i < m_BattleList.size(); ++i)
	{
		const BattleImpData& battleImpData = m_BattleList[i];
		ServerEngine::LegionBattle  legionBattle = battleImpData.legionBattleData;
		
		legionBattleList.push_back(legionBattle);
	}
	m_curData.legionBattleList = legionBattleList;
	m_curData.m_legion2BattleId = m_legion2BattleId;

	globalCityData.curLegionCityData[m_curData.iCityID] = m_curData;

}

string LegionCampBattle::strOwerName()
{
	return m_curData.strOwnerLegion;
}

string LegionCampBattle::getOwnerLegion()
{
	return m_curData.strOwnerLegion;
}

int LegionCampBattle::iGetCampBattleId()
{
	return m_curData.iCityID;
}


int LegionCampBattle::getCurReportLegionCount()
{
	return m_curData.reportList.size();
}

bool LegionCampBattle::reportCampBattle(HEntity hActor,const string& strLegionName)
{
	
	IEntity *pActor = getEntityFromHandle(hActor);
	if(!pActor) return false;
	
	//不能重复宣战
	if(m_curData.reportList.find(strLegionName) != m_curData.reportList.end() )
	{	
		pActor->sendErrorCode(ERROR_RE_REPORT);
		return false;	
	}
	//不能宣战自己
	if(m_curData.strOwnerLegion == strLegionName)	
	{
		pActor->sendErrorCode(ERROR_REPORT_SELF);
		return false;	
	}	
	
	IGlobalCfg* pGlobalCfg = getComponent<IGlobalCfg>(COMPNAME_GlobalCfg, IID_IGlobalCfg);	
	assert(pGlobalCfg);		
	int iLimitCount = pGlobalCfg->getInt("领地军团宣战数量", 3);
	if(m_curData.reportList.size() >= (size_t)iLimitCount)	
	{		
		pActor->sendErrorCode(ERROR_REPORT_FULL);		
		return false;	
	}
	
	string strActorName = pActor->getProperty(PROP_ENTITY_NAME, "");

	GSLegionImp* pLegion = static_cast<GSLegionImp*>(m_pLegionFactory->findLegion(strLegionName));
	if(!pLegion) return false;

		//军团人数
	int iMemberCount = pGlobalCfg->getInt("领地军团宣战人数", 3);
	if(pLegion->getMemberCount() < iMemberCount)
	{
		pActor->sendErrorCode( ERROR_LEGION_MEMBER_LIMITE);
		return false;
	}
	
	//扣钱
	int iLegionHaveMoney = pLegion->iGetLegionMoney();	
	int iNeddMoney = m_CityCfg.iReportCost;
	if(iLegionHaveMoney <iNeddMoney )
	{
		pActor->sendErrorCode( ERROR_LEGION_REPROT_NOMONEY);
		return false;
	}
	
	vector<string> memNameList;
	pLegion->getMemeberNameList(memNameList);

	ServerEngine::LegionReport reportUnit;
	reportUnit.strLegionName = strLegionName;

	map<std::string, ServerEngine::LegionMemberItem> memberList;
	pLegion->getMemberVSLegionMemDetail(memberList);
			
	map<std::string, ServerEngine::LegionMemberItem>::iterator it = memberList.begin();
	
	for(; it!= memberList.end() ; ++it)
	{
		
		ServerEngine::LegionMemberItem& memItem = it->second;

		ServerEngine::CityBattleItem item;
		
		item.strName 	= memItem.strName;
		item.roleKey 	= memItem.roleKey;

		vector<int> vec(3,0);
		item.starList = vec;	
		reportUnit.reportMemberList.insert(make_pair(item.strName, item));
	}

	int iHoleCityCount = m_pLegionFactory->getOwnCityCount(strLegionName);
	//reportUnit.iHoldCityCount = iHoleCityCount;

	m_iMaxLegionHave = max(m_iMaxLegionHave , iHoleCityCount);
	
	m_curData.reportList.insert(make_pair(strLegionName,reportUnit )) ;
	
	return true;
	
}

void LegionCampBattle::onTimer(int nEventId)
{
	
}


void LegionCampBattle::sendCampBattleInfo(HEntity hActor)
{
	IEntity *pEntity = getEntityFromHandle(hActor);
	if(!pEntity) return;
	
	GSProto::CMD_QUERY_CAMPBATTLE_INFO_SC scMsg;
	
	const ServerEngine::LegionReport& ownerLegion = m_curData.owerLogion;
	
	GSProto::LegionUnit& guardUnit = *(scMsg.mutable_guardlegion());
	guardUnit.set_strname(ownerLegion.strLegionName);
	guardUnit.set_iaddpercent(calAddPercent(m_pLegionFactory->getOwnCityCount(ownerLegion.strLegionName)));
	guardUnit.set_ihavecampcount(m_pLegionFactory->getOwnCityCount(ownerLegion.strLegionName));
	
	if(!m_curData.bNpcHold)
	{
		GSLegionImp* pLegion = static_cast<GSLegionImp*>(m_pLegionFactory->findLegion(ownerLegion.strLegionName));
		if(!pLegion) return;
		guardUnit.set_ilegionicon( pLegion->getLegionIconId());
	}
	else
	{
		guardUnit.set_ilegionicon( m_CityCfg.iLegionIcon);
	}

	map<std::string, ServerEngine::LegionReport>::iterator it =  m_curData.reportList.begin();
	for(; it != m_curData.reportList.end(); ++it)
	{
		const ServerEngine::LegionReport& attReprotLegion = it->second;

		GSProto::LegionUnit& addReportLegion = *(scMsg.add_szattacklist());

		addReportLegion.set_strname(attReprotLegion.strLegionName);
		addReportLegion.set_iaddpercent(calAddPercent(m_pLegionFactory->getOwnCityCount(attReprotLegion.strLegionName)));
		addReportLegion.set_ihavecampcount(m_pLegionFactory->getOwnCityCount(attReprotLegion.strLegionName));

		GSLegionImp* pReportLegion = static_cast<GSLegionImp*>(m_pLegionFactory->findLegion(attReprotLegion.strLegionName));
		assert(pReportLegion);

		addReportLegion.set_ilegionicon( pReportLegion->getLegionIconId());
	}

	scMsg.set_icangetsilver( m_CityCfg.iSilver);
	scMsg.set_ireportcost( m_CityCfg.iReportCost);
	scMsg.set_iid(m_CityCfg.iCityID);

	pEntity->sendMessage(GSProto::CMD_QUERY_CAMPBATTLE_INFO, scMsg);
}

int LegionCampBattle::calAddPercent(int iHaveHoldCityCount)
{
	int iPercent = 100*20*(m_iMaxLegionHave - iHaveHoldCityCount );
	return iPercent;
}

void LegionCampBattle::fillScJoinInfo(HEntity hActor, GSProto::CMD_LEGION_CAMPBATTLE_JOIN_SC& scMsg, bool bIsGuard /*= false*/,string strVsLegionName )
{

	scMsg.set_bisguard(bIsGuard);
	if(bIsGuard)
	{
	 if(strVsLegionName == "")
	 {
		for(size_t i = 0; i < m_BattleList.size(); ++i)
		{
			_fillLegionVsDetail(i, *(scMsg.add_szvslegionlist()) );
		}
	 }
	 else
	 {
	 	map<string, int>::iterator it = m_legion2BattleId.find(strVsLegionName);
		if( it == m_legion2BattleId.end()) return;
		int iBattleID = it->second;

		_fillLegionVsDetail(iBattleID, *(scMsg.mutable_selfvsinfo()));
		_fillLegionVsMember(iBattleID,false,*(scMsg.mutable_szmemberlist()));

		int iRemaindSecond = 0;
		m_pLegionFactory->getCampBattleStatus(iRemaindSecond);
		scMsg.set_iremaindsecond(iRemaindSecond);
	 }
	 
	}
	else
	{
		//玩家是攻击方，那么直接push对手

		//找到军团名字
		IEntity *pEntity = getEntityFromHandle(hActor);
		if(!pEntity) return;

		string strActorName = pEntity->getProperty(PROP_ENTITY_NAME, "");

		GSLegionImp* pLegion = static_cast<GSLegionImp*>(m_pLegionFactory->getActorLegion(strActorName));
		if(!pLegion) return;
		string ActorLegionName = pLegion->getLegionName();
		
		map<string, int>::iterator it = m_legion2BattleId.find(ActorLegionName);
		if( it == m_legion2BattleId.end()) return;
		int iBattleID = it->second;
		
		_fillLegionVsDetail(iBattleID, *(scMsg.mutable_selfvsinfo()));
		_fillLegionVsMember(iBattleID, true, *(scMsg.mutable_szmemberlist()));

		int iRemaindSecond = 0;
		m_pLegionFactory->getCampBattleStatus(iRemaindSecond);
		scMsg.set_iremaindsecond(iRemaindSecond);
	}
}
void LegionCampBattle::_fillLegionVsMember(int iBattleId, 
										   bool bSendGuard,
	::google::protobuf::RepeatedPtrField< ::GSProto::VSLegionMemDetail >& szmemberlist)
{

	const BattleImpData &battleImpData = m_BattleList[iBattleId];
	const ServerEngine::LegionBattle& legionBattleData = battleImpData.legionBattleData;
	
	if(bSendGuard)
	{
		const ServerEngine::CityBattleLegionData sendMemberData = legionBattleData.guardLegion;
		if(m_curData.bNpcHold)
		{
			map<std::string, ServerEngine::CityBattleItem>::const_iterator it = sendMemberData.memberList.begin();
			
			for(; it!= sendMemberData.memberList.end() ; ++it)
			{

				const ServerEngine::CityBattleItem& item = it->second;
				
				GSProto::VSLegionMemDetail& detail = *(szmemberlist.Add());
				
				detail.set_iheadid(item.iHeadIcon);
				detail.set_ilevel(item.iLevel);
				detail.set_bisnpc(true);
				detail.set_strname(item.strName);
				detail.set_bicanfight(item.starList[0]);
				detail.set_biicanfight(item.starList[1]);
				detail.set_biiicanfight(item.starList[2]);
				detail.set_ifightvalue(item.iFightValue);
			}
		}
		else
		{
			//根据军团名字
			//去军团系统装载玩家数据

			GSLegionImp* pLegion = static_cast<GSLegionImp*>(m_pLegionFactory->findLegion(m_curData.strOwnerLegion));
			if(!pLegion) return;
			
			map<std::string, ServerEngine::LegionMemberItem> memberList;
			pLegion->getMemberVSLegionMemDetail(memberList);

			map<std::string, ServerEngine::CityBattleItem>::const_iterator it = sendMemberData.memberList.begin();
			for(; it!= sendMemberData.memberList.end() ; ++it)
			{
				map<std::string, ServerEngine::LegionMemberItem>::iterator findIter = memberList.find(it->first);
				assert(findIter != memberList.end() );

				ServerEngine::LegionMemberItem& legionItem = findIter->second;
				const ServerEngine::CityBattleItem& battleItem = it->second;

				GSProto::VSLegionMemDetail& detail = *(szmemberlist.Add());
				detail.set_iheadid(legionItem.iHeadIcon);
				detail.set_ilevel(legionItem.iLevel);
				detail.set_bisnpc(false);
				detail.set_strname(legionItem.strName);
				detail.set_bicanfight(battleItem.starList[0]);
				detail.set_biicanfight(battleItem.starList[1]);
				detail.set_biiicanfight(battleItem.starList[2]);
				detail.set_ifightvalue(legionItem.iFightValue);
			}
			
		}
	}
	else
	{	
		const ServerEngine::CityBattleLegionData sendMemberData = legionBattleData.attLegion;

		GSLegionImp* pLegion = static_cast<GSLegionImp*>(m_pLegionFactory->findLegion(legionBattleData.attLegion.strLegionName));
		if(!pLegion) return;
		
		map<std::string, ServerEngine::LegionMemberItem> memberList;
		pLegion->getMemberVSLegionMemDetail(memberList);

		map<std::string, ServerEngine::CityBattleItem>::const_iterator it = sendMemberData.memberList.begin();
		for(; it!= sendMemberData.memberList.end() ; ++it)
		{
			map<std::string, ServerEngine::LegionMemberItem>::iterator findIter = memberList.find(it->first);
			assert(findIter != memberList.end() );

			ServerEngine::LegionMemberItem& legionItem = findIter->second;
			const ServerEngine::CityBattleItem& battleItem = it->second;

			GSProto::VSLegionMemDetail& detail = *(szmemberlist.Add());
			detail.set_iheadid(legionItem.iHeadIcon);
			detail.set_ilevel(legionItem.iLevel);
			detail.set_bisnpc(false);
			detail.set_strname(legionItem.strName);
			detail.set_bicanfight(battleItem.starList[0]);
			detail.set_biicanfight(battleItem.starList[1]);
			detail.set_biiicanfight(battleItem.starList[2]);
			detail.set_ifightvalue(legionItem.iFightValue);
		}
			
		
	}
}

void LegionCampBattle::_fillLegionVsDetail(int iBattleId, GSProto::VSLegion& vsLegion)
{
	
	vsLegion.set_ibattleid(iBattleId);
	vsLegion.set_icampid(m_curData.iCityID);
	
	const BattleImpData &battleImpData = m_BattleList[iBattleId];
	const ServerEngine::LegionBattle& legionBattleData = battleImpData.legionBattleData;
	const ServerEngine::CityBattleLegionData& attData =  legionBattleData.attLegion;
	const ServerEngine::CityBattleLegionData& guardData =  legionBattleData.guardLegion;
	
	GSProto::VSUnit& guardUnit = *(vsLegion.mutable_guardunit());

	if(!m_curData.bNpcHold)
	{
		GSLegionImp* pLegion = static_cast<GSLegionImp*>(m_pLegionFactory->findLegion(guardData.strLegionName));
		if(!pLegion) return;
		guardUnit.set_ilegionicon( pLegion->getLegionIconId());
	}
	else
	{
		guardUnit.set_ilegionicon( m_CityCfg.iLegionIcon);
	}
	
	//guardUnit.set_ilegionicon( pGuardLegion->getLegionIconId());
	guardUnit.set_istarcount(guardData.iGetStarCount);
	guardUnit.set_strlegionname(guardData.strLegionName);


	GSLegionImp* pAttLegion = static_cast<GSLegionImp*>(m_pLegionFactory->findLegion(attData.strLegionName));
	if(!pAttLegion) return;
	GSProto::VSUnit& attUnit = *(vsLegion.mutable_attunit());
	attUnit.set_ilegionicon( pAttLegion->getLegionIconId());
	attUnit.set_istarcount(attData.iGetStarCount);
	attUnit.set_strlegionname(attData.strLegionName);
}

bool LegionCampBattle::joinCampBattle(HEntity hActor, const string& strLegionName)
{
	return true;
}

struct LegionCityPVPFight
{	
	LegionCityPVPFight(LegionCampBattle* pLegionCity, 
		int iBattleId,
		const string& strAttName, 
		const string& strGuardName,
		bool bIsAttGuard,
		int iStar,
		HEntity hEntity):
		m_pLegionCampBattle(pLegionCity),
		m_iBattleId(iBattleId),
		m_strAttName(strAttName),
		m_strGuardName(strGuardName),
		m_bIsAttGuard(bIsAttGuard),
		m_iStar(iStar),
		m_hEntity(hEntity)
		{
		}
		
		void operator()(taf::Int32 iRet, ServerEngine::BattleData& battleInfo)	
			{		
				if(iRet != en_FightResult_OK)		
				{		
					FDLOG("LegionCity")<<"LegionCityPVP|"<<iRet<<"|"<<endl;
					return;
				}	
				
				if(!m_pLegionCampBattle) return;
				
				m_pLegionCampBattle->processFightResult(m_iBattleId,m_strAttName, m_strGuardName, m_bIsAttGuard, m_iStar, battleInfo,m_hEntity);
			}
		private:	
			LegionCampBattle* m_pLegionCampBattle;
			int m_iBattleId;
			string m_strAttName;	
			string m_strGuardName;
			bool m_bIsAttGuard;
			int m_iStar;
			HEntity m_hEntity;
			
};
void LegionCampBattle::processFightResult(int iBattleId,const string& strAttMemberName, 
												const string& strGuardMemberName, 
												bool bIsAttGuard,
												int iStar,
												const ServerEngine::BattleData& battleInfo,HEntity hEntity)
{

	assert(iBattleId>=0 && iBattleId < (int)m_BattleList.size());
	
	BattleImpData& battleData = m_BattleList[iBattleId];

	//去掉战斗状态
	if(bIsAttGuard)
	{
		battleData.deleteActorAttStatus(strGuardMemberName,iStar );
	
		
	}
	else
	{
		battleData.deleteActorAttStatus(strAttMemberName,iStar );
	}
	
	ServerEngine::LegionBattle & legionBattle = battleData.legionBattleData;
	ServerEngine::CityBattleLegionData& guardData = legionBattle.guardLegion;
	ServerEngine::CityBattleLegionData& attData = legionBattle.attLegion;

	map<std::string, ServerEngine::CityBattleItem>::iterator iter = guardData.memberList.find(strGuardMemberName);
	assert(iter != guardData.memberList.end());
	ServerEngine::CityBattleItem& gaurdMem =  iter->second;

	map<std::string, ServerEngine::CityBattleItem>::iterator attIter = attData.memberList.find(strAttMemberName);
	assert(iter != attData.memberList.end());
	ServerEngine::CityBattleItem& attMem = attIter->second;

	int iconId = 0;
	if(m_curData.bNpcHold)
	{
		iconId = m_CityCfg.iLegionIcon;
	}
	else
	{
		GSLegionImp* pGuardLegion = static_cast<GSLegionImp*>(m_pLegionFactory->findLegion(legionBattle.guardLegion.strLegionName));
		if(!pGuardLegion) return;
		iconId = pGuardLegion->getLegionIconId();
	}
	
	GSLegionImp* pAttLegion = static_cast<GSLegionImp*>(m_pLegionFactory->findLegion(attData.strLegionName));
	if(!pAttLegion) return;
			
	//夺取星星
	if(battleInfo.bAttackerWin)
	{
		
		if(bIsAttGuard)
		{
			//争夺方 攻击胜利
			attData.iGetStarCount += iStar;
			attData.dwGetStarTime = time(0);

			assert(gaurdMem.starList[iStar-1] == 0);
			gaurdMem.starList[iStar-1] = 1;

			GSProto::FightRecord winRecord;
			winRecord.set_strselfname(strAttMemberName );
			winRecord.set_strenemylegionname( guardData.strLegionName);
			winRecord.set_strenemyname( strGuardMemberName);
			winRecord.set_igetstarcount(iStar);
			string strWinLog;
			winRecord.SerializeToString( &strWinLog);


			GSProto::VSLegion vsLegon;
			
			GSProto::VSUnit& vsUnit = *(vsLegon.mutable_guardunit());
			vsUnit.set_ilegionicon( iconId);
			vsUnit.set_istarcount(guardData.iGetStarCount );
			vsUnit.set_strlegionname(guardData.strLegionName);
			
			GSProto::VSUnit& vsAttUnit = *(vsLegon.mutable_attunit());
			vsAttUnit.set_ilegionicon( iconId);
			vsAttUnit.set_istarcount(attData.iGetStarCount );
			vsAttUnit.set_strlegionname(attData.strLegionName);

			vsLegon.set_icampid(m_CityCfg.iCityID);

			string baseLog;
			vsLegon.SerializeToString( & baseLog);

			string strLoseLog="";
			m_pLegionFactory->addFightLogInBattle(m_CityCfg.iCityID, attData.strLegionName,strLoseLog , strWinLog ,baseLog , m_curData.owerLogion.strLegionName, false);
			
			
		}
		else
		{
			//防守方攻击胜利
			guardData.iGetStarCount += iStar;
			guardData.dwGetStarTime = time(0);

			assert(attMem.starList[iStar-1] == 0);
			attMem.starList[iStar-1] = 1;

			GSProto::FightRecord winRecord;
			winRecord.set_strselfname(strGuardMemberName  );
			winRecord.set_strenemylegionname(  attData.strLegionName);
			winRecord.set_strenemyname( strAttMemberName);
			winRecord.set_igetstarcount(iStar);
			string strWinLog;
			winRecord.SerializeToString( &strWinLog);


			GSProto::VSLegion vsLegon;
			
			GSProto::VSUnit& vsUnit = *(vsLegon.mutable_guardunit());
			vsUnit.set_ilegionicon( iconId);
			vsUnit.set_istarcount(guardData.iGetStarCount );
			vsUnit.set_strlegionname(guardData.strLegionName);
			
			GSProto::VSUnit& vsAttUnit = *(vsLegon.mutable_attunit());
			vsAttUnit.set_ilegionicon(iconId);
			vsAttUnit.set_istarcount(attData.iGetStarCount );
			vsAttUnit.set_strlegionname(attData.strLegionName);
			vsLegon.set_icampid(m_CityCfg.iCityID);
			
			string baseLog;
			vsLegon.SerializeToString( & baseLog);
			string strLoseLog = "";
			m_pLegionFactory->addFightLogInBattle(m_CityCfg.iCityID, attData.strLegionName, strWinLog ,strLoseLog, baseLog , m_curData.owerLogion.strLegionName,true);
		}
	}
	else
	{
		if(bIsAttGuard)
		{
			GSProto::FightRecord loseRecord;
			loseRecord.set_strselfname(strAttMemberName  );
			loseRecord.set_strenemylegionname( guardData.strLegionName);
			loseRecord.set_strenemyname( strGuardMemberName);
			loseRecord.set_igetstarcount( -1);
			string strLoseLog;
			loseRecord.SerializeToString( &strLoseLog);
			

			GSProto::VSLegion vsLegon;
			
			GSProto::VSUnit& vsUnit = *(vsLegon.mutable_guardunit());
			vsUnit.set_ilegionicon( iconId);
			vsUnit.set_istarcount(guardData.iGetStarCount );
			vsUnit.set_strlegionname(guardData.strLegionName);
			
			GSProto::VSUnit& vsAttUnit = *(vsLegon.mutable_attunit());
			vsAttUnit.set_ilegionicon( iconId);
			vsAttUnit.set_istarcount(attData.iGetStarCount );
			vsAttUnit.set_strlegionname(attData.strLegionName);
			vsLegon.set_icampid(m_CityCfg.iCityID);
			
			string baseLog;
			vsLegon.SerializeToString( & baseLog);

			string strWinLog = "";
			m_pLegionFactory->addFightLogInBattle(m_CityCfg.iCityID, attData.strLegionName,strWinLog,strLoseLog ,baseLog , m_curData.owerLogion.strLegionName,false);
		}
		else
		{
			GSProto::FightRecord loseRecord;
			loseRecord.set_strselfname( strGuardMemberName );
			loseRecord.set_strenemylegionname(  attData.strLegionName);
			loseRecord.set_strenemyname( strAttMemberName);
			loseRecord.set_igetstarcount( -1);
			string strLoseLog;
			loseRecord.SerializeToString( &strLoseLog);


			
			GSProto::VSLegion vsLegon;
			
			GSProto::VSUnit& vsUnit = *(vsLegon.mutable_guardunit());
			vsUnit.set_ilegionicon( iconId);
			vsUnit.set_istarcount(guardData.iGetStarCount );
			vsUnit.set_strlegionname(guardData.strLegionName);
			
			GSProto::VSUnit& vsAttUnit = *(vsLegon.mutable_attunit());
			vsAttUnit.set_ilegionicon(iconId);
			vsAttUnit.set_istarcount(attData.iGetStarCount );
			vsAttUnit.set_strlegionname(attData.strLegionName);
			vsLegon.set_icampid(m_CityCfg.iCityID);
			
			string baseLog;
			vsLegon.SerializeToString( & baseLog);
			
			string strWinLog = "";
			m_pLegionFactory->addFightLogInBattle(m_CityCfg.iCityID, attData.strLegionName,strLoseLog,strWinLog,baseLog , m_curData.owerLogion.strLegionName,true);
		}
	}

	int iTempStar = iStar;
	if(!battleInfo.bAttackerWin)
	{
		iTempStar = iTempStar -1;
		assert(iTempStar>=0);
	}

	IGlobalCfg* pGlobalCfg = getComponent<IGlobalCfg>(COMPNAME_GlobalCfg, IID_IGlobalCfg);
	assert(pGlobalCfg);

	
	
	vector<int> dropList;
	if(bIsAttGuard)
	{
		string strDropList = pGlobalCfg->getString("攻击方领地军团战掉落ID", "3009#3010#3011#3012");
		dropList = TC_Common::sepstr<int>(strDropList, "#");	
	}
	else
	{
		string strDropList = pGlobalCfg->getString("防守方领地军团战掉落ID", "3013#3014#3015#3016");
		dropList = TC_Common::sepstr<int>(strDropList, "#");	
	}
	
	int iDropId = dropList[iTempStar];

	GSProto::Cmd_Sc_CommFightResult scFightResoult;
	scFightResoult.set_isceneid( pGlobalCfg->getInt("军团领地战场景", 1) );
	scFightResoult.set_iissuccess(battleInfo.bAttackerWin?1:0);
	scFightResoult.set_istar(battleInfo.iStar);

	GSProto::FightAwardResult& awardResult = *(scFightResoult.mutable_awardresult() );
	
	IDropFactory* pDropFactory = getComponent<IDropFactory>(COMPNAME_DropFactory, IID_IDropFactory);
	assert(pDropFactory);

    //掉落奖励
  	pDropFactory->calcDrop(iDropId, awardResult);

	// 执行奖励
	IUserStateManager* pUserStateMgr = getComponent<IUserStateManager>(COMPNAME_LoginManager, IID_IUserStateManager);
	assert(pUserStateMgr);

	IEntity *pEntity = getEntityFromHandle(hEntity);
	if(!pEntity) return;


	if(bIsAttGuard)
	{
		
		map<string,bool>::iterator actorbattInter = battleData.actorHaveBattleMap.find(strAttMemberName);
		assert(actorbattInter ==  battleData.actorHaveBattleMap.end());
		battleData.actorHaveBattleMap.insert(make_pair(strAttMemberName,true));
	}
	else
	{
		map<string,bool>::iterator actorbattInter = battleData.actorHaveBattleMap.find(strGuardMemberName);
		assert(actorbattInter ==  battleData.actorHaveBattleMap.end());
		battleData.actorHaveBattleMap.insert(make_pair(strGuardMemberName,true));
	}
	

	pDropFactory->excuteDrop(hEntity , awardResult, GSProto::en_Reason_LEGION_BATTLE);


	GSProto::SCMessage pkg;
	HelpMakeScMsg(pkg, GSProto::CMD_COMM_FIGHTRESULT, scFightResoult);
	IFightSystem* pFightSys = static_cast<IFightSystem*>(pEntity->querySubsystem(IID_IFightSystem));
	assert(pFightSys);
	
	pFightSys->sendAllBattleMsg(battleInfo, pkg);
	
	
}

void LegionCampBattle::onCampMemberBattle(HEntity hActor, int iBattleId ,string strMemName, int iStar)
{
	//判断角色是否在领地争夺战中
	LegionFactory* pLegionFactory = static_cast<LegionFactory*>(getComponent<ILegionFactory>(COMPNAME_LegionFactory, IID_ILegionFactory) );
	assert(pLegionFactory);
	
	IEntity *pEntity = getEntityFromHandle(hActor);
	if(!pEntity) return;
	
	string strActorName = pEntity->getProperty( PROP_ENTITY_NAME,  "");
	GSLegionImp* pLegion = static_cast<GSLegionImp*>(pLegionFactory->getActorLegion(strActorName));
	if(!pLegion) return;
	string strActorLegionName = pLegion->getLegionName();

	if(iBattleId<0 || (unsigned int)iBattleId >= m_BattleList.size())
	{
		return;
	}
	
	if(iStar<=0 || iStar >3) return;

	BattleImpData& battleData = m_BattleList[iBattleId];
	const ServerEngine::LegionBattle & legionBattle = battleData.legionBattleData;

	map<string,bool>::iterator actorbattInter = battleData.actorHaveBattleMap.find(strActorName);
	if(actorbattInter !=  battleData.actorHaveBattleMap.end())
	{
		pEntity->sendErrorCode(ERROR_LEGION_HAVE_BATTLE);
		return;
	}

	ServerEngine::CityBattleItem target;

	string strGuardName = "";
	string strAttName = "";
	bool bIsAttGuard = false;

	ServerEngine::CityBattleLegionData  attLegionData;
	ServerEngine::CityBattleLegionData  targetLegionData;
	
	if(strActorLegionName == legionBattle.attLegion.strLegionName)
	{
		//member 必须是防守方的人
		const ServerEngine::CityBattleLegionData& guardData =  legionBattle.guardLegion;
	
		map<string, ServerEngine::CityBattleItem>::const_iterator findIt = guardData.memberList.find(strMemName);
		assert(findIt !=  guardData.memberList.end());
		
		
		//要攻击这个难度是否可以攻击
		const ServerEngine::CityBattleItem& battleItem = findIt->second;
		const vector<taf::Int32>& starList = battleItem.starList;
		if(starList[iStar-1]) 
		{
			pEntity->sendErrorCode(ERROR_ATT_STAR_LOST);
			return;
		}
			

		target = battleItem;

		strGuardName = strMemName;
		strAttName 	= strActorName;
		bIsAttGuard = true; 
			
	}
	else if(strActorLegionName == legionBattle.guardLegion.strLegionName)
	{
		//att必须是攻击方
		const ServerEngine::CityBattleLegionData& attLegionData =  legionBattle.attLegion;
		
		map<string, ServerEngine::CityBattleItem>::const_iterator findIt = attLegionData.memberList.find(strMemName);
		if(findIt == attLegionData.memberList.end()) return;
		//要攻击这个难度是否可以攻击
		const ServerEngine::CityBattleItem& battleItem = findIt->second;
		const vector<taf::Int32>& starList = battleItem.starList;
		if(starList[iStar-1]) 
		{
			pEntity->sendErrorCode(ERROR_ATT_STAR_LOST);
			return;
		}
		
		target = battleItem;
		strGuardName = strActorName;
		strAttName = strMemName;
		bIsAttGuard = false;
	}
	else 
	{
		return;
	}

	//验证通过，去战斗吧
	//一个难度只能有一个人在战斗
   if(!battleData.addActorInAttStatus(strMemName,iStar))
   	{
   		pEntity->sendErrorCode(ERROR_ATT_MEM_ISBUSY);
   		return;
   	}

	IFightSystem* pFightSys = static_cast<IFightSystem*>(pEntity->querySubsystem(IID_IFightSystem));
	if(!pFightSys) return ;

	ServerEngine::FightContext ctx;
	if(bIsAttGuard)
	{
		fillBattleCtx(legionBattle.attLegion,legionBattle.guardLegion,ctx, iStar);
	}
	else
	{
		fillBattleCtx(legionBattle.guardLegion,legionBattle.attLegion,ctx, iStar);
	}
	
	pFightSys->AsyncPVPFight(LegionCityPVPFight(this,iBattleId,strAttName,strGuardName,bIsAttGuard,iStar , hActor), target.roleKey, ctx);
	
}

void LegionCampBattle::fillBattleCtx(const ServerEngine::CityBattleLegionData& attLegionData,const ServerEngine::CityBattleLegionData& targetLegionData, ServerEngine::FightContext& ctx,int iStar)
{
	
	IGlobalCfg* pGlobalCfg = getComponent<IGlobalCfg>(COMPNAME_GlobalCfg, IID_IGlobalCfg);
	assert(pGlobalCfg);
	
	ctx.iSceneID = pGlobalCfg->getInt("军团领地战场景", 1);
	ctx.iFightType = GSProto::en_FightType_LegionCity;

	//填充战斗

	string starAddPercent = pGlobalCfg->getString("军团领地星阶加成","1000#2000#3000");
	vector<int> starAddPercentList = TC_Common::sepstr<int>(starAddPercent,"#");
	assert(starAddPercentList.size() == 3);
	assert(iStar <= (int)starAddPercentList.size());

	ServerEngine::LegionCityFightCtx cityFightCtx;

	cityFightCtx.attCtx.iAddPercent = calAddPercent( m_pLegionFactory->getOwnCityCount(attLegionData.strLegionName));
	cityFightCtx.targetCtx.iAddPercent =  calAddPercent( m_pLegionFactory->getOwnCityCount(targetLegionData.strLegionName) ) + starAddPercentList[iStar-1];

	ctx.context["LegionCityFightCtx"] = ServerEngine::JceToStr(cityFightCtx);
}

bool LegionCampBattle::bIsLegionAddCampBattle(string strLegionName)
{

   map<std::string, ServerEngine::LegionReport>::iterator it = m_curData.reportList.find(strLegionName);

   return it == m_curData.reportList.end();
}


bool BattleImpData::bIsActorInAttStatus(string strName, int iStar)
{
	map<string,map<int, ServerEngine::TimeResetValue> >::iterator it = m_InBattleMap.find(strName);
	if(it == m_InBattleMap.end())
	{
		return false;
	}

    map<int, ServerEngine::TimeResetValue>& attMap = it->second;

	map<int, ServerEngine::TimeResetValue>::iterator starItr = attMap.find(iStar);
	
	if(starItr != attMap.end())
	{	
		if(starItr->second.dwLastChgTime -time(0) > 5*60)
		{
			attMap.erase(starItr);
			return false;
		}
		
		return true;
	}
	return false;
	
}

bool BattleImpData::addActorInAttStatus(string strName, int iStar)
{
	if( bIsActorInAttStatus(strName, iStar) ) return false;
	
	ServerEngine::TimeResetValue staus;
	staus.dwLastChgTime = time(0);
	staus.iValue = 1;

	map<string, map<int, ServerEngine::TimeResetValue> >::iterator it = m_InBattleMap.find(strName);
	if(it == m_InBattleMap.end())
	{
		 map<int, ServerEngine::TimeResetValue> temp;
		 temp.insert(make_pair(iStar, staus));
		 
		 m_InBattleMap.insert(make_pair(strName, temp ));

		return true;
	}

	(it->second).insert(make_pair(iStar, staus));
	return true;
}

bool BattleImpData::deleteActorAttStatus(string strName, int iStar)
{
	map<string, map<int, ServerEngine::TimeResetValue> >::iterator it = m_InBattleMap.find(strName);
	if(it != m_InBattleMap.end())
	{
		
   		map<int, ServerEngine::TimeResetValue>& attMap = it->second;

		map<int, ServerEngine::TimeResetValue>::iterator starItr = attMap.find(iStar);
		
		if(starItr != it->second.end())
		{
			it->second.erase(starItr);
			return true;
		}
	}
	
	return true;
}

void LegionCampBattle::cleanDataBeforeReport()
{
	m_BattleList.clear();
	LegionFactory* pLegionFactory = static_cast<LegionFactory*>(getComponent<ILegionFactory>(COMPNAME_LegionFactory, IID_ILegionFactory) );
	assert(pLegionFactory);

	IGlobalCfg *pGlobal = getComponent<IGlobalCfg>(COMPNAME_GlobalCfg, IID_IGlobalCfg);
	assert(pGlobal);
	if(!m_curData.bNpcHold)
	{
		//检查军团人数
		
		int iMemCount = pLegionFactory->iGetLegionMemCount(m_curData.strOwnerLegion);
		int iNeedMemCount = pGlobal->getInt("军团领地战至少需要的成员",20);
		
		if(iMemCount >= iNeedMemCount)
		{
			//填充庄家
			updateCityOwner();
			
			return;
		}
			
	}
	
	//重新分配领地
	fillNPC();
	
}

void LegionCampBattle::fillNPC()
{

	//原来是军团占领的
	
	IArenaFactory *pArenaFactory = getComponent<IArenaFactory>(COMPNAME_ArenaFactory, IID_IArenaFactory);
	assert(pArenaFactory);
	vector<ServerEngine::ArenaSavePlayer> playerList;
	
	IGlobalCfg *pGlobal = getComponent<IGlobalCfg>(COMPNAME_GlobalCfg, IID_IGlobalCfg);
	assert(pGlobal);
	int iNeedMemCount = pGlobal->getInt("军团领地战至少需要的成员",20);
	
	bool bResoult = pArenaFactory->getActorForCityBattle(playerList, iNeedMemCount);
	assert(bResoult);
	assert(playerList.size() == (unsigned int)iNeedMemCount );
	
	ServerEngine::LegionReport& owerLogion = m_curData.owerLogion;
	owerLogion.strLegionName = m_CityCfg.strNpcLegionName;

	m_curData.strOwnerLegion = m_CityCfg.strNpcLegionName;
	//owerLogion.iHoldCityCount = 1;
	m_iMaxLegionHave = 1;

	owerLogion.reportMemberList.clear();
	
	vector<ServerEngine::ArenaSavePlayer>::iterator it = playerList.begin();
	for(;it != playerList.end(); ++it)
	{
		ServerEngine::ArenaSavePlayer & player =  *it;

		ServerEngine::CityBattleItem item;
		
		item.strName 	 = player.strName;
		item.roleKey 	 = player.roleKey;
		item.iLevel  	 = player.iLevel;
		item.iHeadIcon 	 = player.iShowHeroID;
		item.iFightValue = player.iFightValue;
		
		vector<int> vec(3,0);
		item.starList = vec;
		
		owerLogion.reportMemberList[item.strName] =  item;
	}

	m_curData.bNpcHold = true;
	
	m_curData.reportList.clear();
	//m_curData.fightRecordList.clear();
	m_curData.legionBattleList.clear();
}

void LegionCampBattle::updateCityOwner()
{
	LegionFactory* pLegionFactory = static_cast<LegionFactory*>(getComponent<ILegionFactory>(COMPNAME_LegionFactory, IID_ILegionFactory) );
	assert(pLegionFactory);
	
	ServerEngine::LegionReport& owerLogion = m_curData.owerLogion;


	owerLogion.reportMemberList.clear();
	
	GSLegionImp* pLegion = static_cast<GSLegionImp*>(m_pLegionFactory->findLegion(m_curData.strOwnerLegion));
	if(!pLegion) return ;

	map<std::string, ServerEngine::LegionMemberItem> memberList;
	pLegion->getMemberVSLegionMemDetail(memberList);
			
	map<std::string, ServerEngine::LegionMemberItem>::const_iterator it = memberList.begin();
	
	for(; it!= memberList.end() ; ++it)
	{
		const ServerEngine::LegionMemberItem& memItem = it->second;

		ServerEngine::CityBattleItem item;
		
		item.strName 	 = memItem.strName;
		item.roleKey     = memItem.roleKey;

		vector<int> vec(3,0);
		item.starList = vec;	
		
		owerLogion.reportMemberList.insert(make_pair(item.strName, item));
	}

	m_curData.reportList.clear();
	//m_curData.fightRecordList.clear();
	m_curData.legionBattleList.clear();
	m_curData.bNpcHold = false;
	
}


void  LegionCampBattle::pairBattle()
{

	m_legion2BattleId.clear();
	
	ServerEngine::CityBattleLegionData guardCityBattleData;
	guardCityBattleData.strLegionName = m_curData.strOwnerLegion;
	guardCityBattleData.iGetStarCount = 0;
	guardCityBattleData.memberList = m_curData.owerLogion.reportMemberList;
	
	map<std::string, ServerEngine::LegionReport>::iterator it =  m_curData.reportList.begin();

	int m_iMaxLegionHave = 0;
	if(!m_curData.bNpcHold) 
	{
		
		m_iMaxLegionHave = m_pLegionFactory->getOwnCityCount(m_curData.owerLogion.strLegionName);
	}
	
	
	for(; it != m_curData.reportList.end(); ++it)
	{
		const ServerEngine::LegionReport& attReprotLegion = it->second;

		ServerEngine::LegionBattle legionBattle;
		legionBattle.guardLegion = guardCityBattleData;
		ServerEngine::CityBattleLegionData& attLegion = legionBattle.attLegion;
		attLegion.strLegionName = it->first;
		attLegion.memberList = attReprotLegion.reportMemberList;
		attLegion.iGetStarCount = 0;
		int iActorHoldCityCount = m_pLegionFactory->getOwnCityCount(attReprotLegion.strLegionName);
		
		if(m_iMaxLegionHave<iActorHoldCityCount)  m_iMaxLegionHave =iActorHoldCityCount;

		BattleImpData battleImpData;
		battleImpData.legionBattleData = legionBattle;

		m_BattleList.push_back(battleImpData);

		m_legion2BattleId.insert(make_pair(attLegion.strLegionName,m_BattleList.size()-1));
	}
	
}

void  LegionCampBattle::battleOver()
{
	IGlobalCfg *pGlobal = getComponent<IGlobalCfg>(COMPNAME_GlobalCfg, IID_IGlobalCfg);
	assert(pGlobal);
	int iNeedStarCount = pGlobal->getInt("军团领地战至少获得星星数",20);
	
	string strWinerLegion = m_curData.strOwnerLegion;
	int iMaxStar = 0;
	int imaxGetSecond = 0;
	if(m_curData.bNpcHold)
	{
		
		for(size_t i = 0; i < m_BattleList.size(); ++i)
		{
			const BattleImpData& battleData = m_BattleList[i];
			const ServerEngine::LegionBattle& legionBattle = battleData.legionBattleData;

			if(legionBattle.attLegion.iGetStarCount < iNeedStarCount )
			{
				continue;
			}
			
			if(legionBattle.attLegion.iGetStarCount > iMaxStar)
			{
				iMaxStar = legionBattle.attLegion.iGetStarCount;
				imaxGetSecond = legionBattle.attLegion.dwGetStarTime;
	
				strWinerLegion = legionBattle.attLegion.strLegionName;

				
			}
			else if(legionBattle.attLegion.iGetStarCount == iMaxStar)
			{
				if( (unsigned int)imaxGetSecond > legionBattle.attLegion.dwGetStarTime)
				{
					iMaxStar = legionBattle.attLegion.iGetStarCount;
					imaxGetSecond = legionBattle.attLegion.dwGetStarTime;
	
					strWinerLegion = legionBattle.attLegion.strLegionName;
				}
			}
		}

		
		//给打赢了军团但是没有第一的人发补偿
		for(size_t i = 0; i < m_BattleList.size(); ++i)
		{
			const BattleImpData& battleData = m_BattleList[i];
			const ServerEngine::LegionBattle& legionBattle = battleData.legionBattleData;

			if(legionBattle.attLegion.iGetStarCount < iNeedStarCount )
			{
				continue;
			}
			
			if(legionBattle.attLegion.strLegionName !=  strWinerLegion)	
			{
				//群发邮件

				sendMail(legionBattle.attLegion.memberList);
			}
		}
		
	}
	else
	{
		for(size_t i = 0; i < m_BattleList.size(); ++i)
		{
			const BattleImpData& battleData = m_BattleList[i];
			const ServerEngine::LegionBattle& legionBattle = battleData.legionBattleData;
			if(legionBattle.attLegion.iGetStarCount > iMaxStar)
			{
				iMaxStar = legionBattle.attLegion.iGetStarCount;
				imaxGetSecond = legionBattle.attLegion.dwGetStarTime;

				strWinerLegion = legionBattle.attLegion.strLegionName;
			}
			else if(legionBattle.attLegion.iGetStarCount == iMaxStar)
			{
				if( (unsigned int)imaxGetSecond > legionBattle.attLegion.dwGetStarTime)
				{
					iMaxStar = legionBattle.attLegion.iGetStarCount;
					imaxGetSecond = legionBattle.attLegion.dwGetStarTime;

					strWinerLegion = legionBattle.attLegion.strLegionName;
				}
			}

			if( legionBattle.guardLegion.iGetStarCount > iMaxStar)
			{
				iMaxStar = legionBattle.guardLegion.iGetStarCount;
				imaxGetSecond = legionBattle.guardLegion.dwGetStarTime;

				strWinerLegion = legionBattle.guardLegion.strLegionName;
			}
			else if(legionBattle.guardLegion.iGetStarCount == iMaxStar)
			{
				if((unsigned int)imaxGetSecond > legionBattle.guardLegion.dwGetStarTime)
				{
					iMaxStar = legionBattle.guardLegion.iGetStarCount;
					imaxGetSecond = legionBattle.guardLegion.dwGetStarTime;

					strWinerLegion = legionBattle.guardLegion.strLegionName;
				}
			}
		}

		
		//给打赢了军团但是没有第一的人发补偿
		//不是第一名，不是防守方，但是打赢了防守方

		if(strWinerLegion != m_curData.strOwnerLegion)
		{
			for(size_t i = 0; i < m_BattleList.size(); ++i)
			{
				const BattleImpData& battleData = m_BattleList[i];
				const ServerEngine::LegionBattle& legionBattle = battleData.legionBattleData;
			
				if(legionBattle.attLegion.strLegionName !=  strWinerLegion)	
				{
					//群发邮件

					if( legionBattle.attLegion.iGetStarCount > legionBattle.guardLegion.iGetStarCount)
					{
						sendMail(legionBattle.attLegion.memberList);
					}
					else if(legionBattle.attLegion.iGetStarCount == legionBattle.guardLegion.iGetStarCount)
					{
						if( legionBattle.attLegion.dwGetStarTime < legionBattle.guardLegion.dwGetStarTime)
						{
							sendMail(legionBattle.attLegion.memberList);
						}
					}
					
				}
			}
		}
	}
	
	//成功的不是防守方
	if(strWinerLegion != m_curData.strOwnerLegion)
	{
		
		for(size_t i = 0; i < m_BattleList.size(); ++i)
		{
			GSProto::LastFightLogItem item;
			item.set_icampid(m_CityCfg.iCityID);
		 	item.set_bisguard(false);
			const BattleImpData& battleData = m_BattleList[i];
			const ServerEngine::LegionBattle& legionBattle = battleData.legionBattleData;
			if(legionBattle.attLegion.strLegionName == strWinerLegion)
			{
				 addFightResoultLog(false,true,legionBattle.attLegion.strLegionName);
				 
				// m_curData.owerLogion.iHoldCityCount = legionBattle.attLegion.iHoldCityCount;
				 m_curData.owerLogion.strLegionName = legionBattle.attLegion.strLegionName;
				 m_curData.owerLogion.reportMemberList = legionBattle.attLegion.memberList;
				 m_curData.bNpcHold = false;

				 //攻击方军团占有数+1

			}
			else
			{
				 addFightResoultLog(false,false,legionBattle.attLegion.strLegionName);
			}
		}
		
		if(!m_curData.bNpcHold)
		{
			addFightResoultLog(true,false,m_curData.strOwnerLegion);
		}
	}
	else
	{
		for(size_t i = 0; i < m_BattleList.size(); ++i)
		{
			
			const BattleImpData& battleData = m_BattleList[i];
			const ServerEngine::LegionBattle& legionBattle = battleData.legionBattleData;

			addFightResoultLog(false,false,legionBattle.attLegion.strLegionName);
		}
		
		if(!m_curData.bNpcHold)
		{
			addFightResoultLog(true,true,m_curData.strOwnerLegion);
		}
	}
	
	m_curData.strOwnerLegion = strWinerLegion;

	m_iMaxLegionHave =  m_pLegionFactory->getOwnCityCount(m_curData.strOwnerLegion);
	m_curData.reportList.clear();
	
}

struct GetActorSendMailCallBack
{
	GetActorSendMailCallBack(string strUUID, ServerEngine::MailData  mailData, int iBoradId)
		:m_strUUID(strUUID),
		m_mailData(mailData),
		m_iBoradId(iBoradId)
	{
		
	}

	virtual void operator()(int iRet, HEntity hTarget, ServerEngine::PIRole& roleInfo)
	{
		if( iRet != ServerEngine::en_RoleRet_OK)
		{
			FDLOG("LegionCampBattle")<<"sendMail|"<< iRet <<endl;
			return;
		}

		if(0 != hTarget)
		{
			IEntity *pEntity = getEntityFromHandle(hTarget);
			assert(pEntity);
			IMailSystem*pMailSys = static_cast<IMailSystem*>(pEntity->querySubsystem( IID_IMailSystem));
			assert(pMailSys);
			pMailSys->addMail(m_strUUID,m_mailData, "LegionCampBattle_Win");
		}
		else
		{
			ServerEngine::RoleSaveData roleSaveData;
			ServerEngine::JceToObj(roleInfo.roleData, roleSaveData);
			map<taf::Int32, std::string>::iterator iter = roleSaveData.subsystemData.find(IID_IMailSystem);
			assert(iter != roleSaveData.subsystemData.end());
			ServerEngine::MailSystemData systemData;
			ServerEngine::JceToObj(iter->second, systemData);
			//修改数据
			systemData.mailDataList.push_back( m_mailData );
			//回写
			iter->second = ServerEngine::JceToStr(systemData);
			roleInfo.roleData = ServerEngine::JceToStr(roleSaveData);
		}
		
	}
	private:
		string m_strUUID;
		ServerEngine::MailData  m_mailData;
		int m_iBoradId;
};



void LegionCampBattle::sendMail( const map<std::string, ServerEngine::CityBattleItem>& memList)
{
	int iBroadId = 1006;
		
	IModifyDelegate* pModifyDelegate = getComponent<IModifyDelegate>(COMPNAME_ModifyDelegate, IID_IModifyDelegate);
	assert(pModifyDelegate);
	string strUUID;
	ServerEngine::MailData mailData;
	
	fillMail(strUUID, mailData, iBroadId);

	map<std::string, ServerEngine::CityBattleItem>::const_iterator iter = memList.begin();
	for( ; iter != memList.end(); ++iter )
	{
		const ServerEngine::CityBattleItem& item = iter->second;
		pModifyDelegate->submitDelegateTask(item.roleKey, GetActorSendMailCallBack(strUUID, mailData,iBroadId));
	}
		
}


void LegionCampBattle::addFightResoultLog(bool bIsGuard, bool bIsWin, string strLegionName)
{
	GSProto::LastFightLogItem item;
	item.set_icampid(m_CityCfg.iCityID);
	item.set_bisguard(bIsGuard);
	item.set_biswin(bIsWin);
	string strLog;
	item.SerializeToString(&strLog);
	m_pLegionFactory->addFightLogInBattleOver(strLegionName,m_CityCfg.iCityID,strLog);
}

void LegionCampBattle::onQueryCurFightCondition(HEntity hActor)
{
	IEntity *pEntity = getEntityFromHandle(hActor);
	if(!pEntity) return;

	GSProto::CMD_QUERY_CUR_FIGHT_CONDITION_SC scMsg;

	for(size_t i = 0; i < m_BattleList.size(); ++i)
	{
		const BattleImpData& battleData = m_BattleList[i];
		const ServerEngine::LegionBattle& legionBattle = battleData.legionBattleData;

		GSProto::VSLegion & vsLegion = *(scMsg.add_szvslegionlist());
		vsLegion.set_ibattleid(i);
		vsLegion.set_icampid(m_CityCfg.iCityID);

		GSProto::VSUnit & vsattUnit = *(vsLegion.mutable_attunit());
		GSProto::VSUnit & vsguardUnit = *(vsLegion.mutable_guardunit());

		int iconId = 0;
		if(m_curData.bNpcHold)
		{
			iconId = m_CityCfg.iLegionIcon;
		}
		else
		{
			GSLegionImp* pGuardLegion = static_cast<GSLegionImp*>(m_pLegionFactory->findLegion(legionBattle.guardLegion.strLegionName));
			if(!pGuardLegion) return;
			iconId = pGuardLegion->getLegionIconId();
		}
		
		
		GSLegionImp* pAttLegion = static_cast<GSLegionImp*>(m_pLegionFactory->findLegion(legionBattle.attLegion.strLegionName));
		if(!pAttLegion) return;
	
		vsattUnit.set_ilegionicon(pAttLegion->getLegionIconId());
		vsattUnit.set_istarcount(legionBattle.attLegion.iGetStarCount);
		vsattUnit.set_strlegionname(legionBattle.attLegion.strLegionName);


		vsguardUnit.set_ilegionicon(iconId);
		vsguardUnit.set_istarcount(legionBattle.guardLegion.iGetStarCount);
		vsguardUnit.set_strlegionname(legionBattle.guardLegion.strLegionName);
		
			
	}

	pEntity->sendMessage(GSProto::CMD_QUERY_CUR_FIGHT_CONDITION, scMsg);
	
}


void LegionCampBattle::setLegionGuard(string strLegionName)
{
	m_curData.strOwnerLegion = strLegionName;
	m_curData.bNpcHold = false;
	m_curData.owerLogion.strLegionName = strLegionName;
	//m_curData.owerLogion.iHoldCityCount = 1;
}


void LegionCampBattle::fillMail(string& struuid, ServerEngine::MailData & mailData, int iBroadId)
{
	// 检测策划配置的公告
	ITable* pMailTb = getCompomentObjectManager()->findTable("Mail");
	assert(pMailTb);
	
	int iMailRecord = pMailTb->findRecord(iBroadId);
	assert(iMailRecord >= 0);
	
	uuid_t itemuuid;
	uuid_generate(itemuuid);
	char szUUIDString[1024] = {0};
	uuid_unparse_upper(itemuuid, szUUIDString);
	mailData.strUUID = szUUIDString;

	mailData.strTitle = pMailTb->getString(iMailRecord, "标题");
	mailData.strSender = pMailTb->getString(iMailRecord, "发送者");
	string strContent =  pMailTb->getString(iMailRecord, "内容");

	string strDropList = pMailTb->getString(iMailRecord, "奖励");
	vector<int> dropIDList = TC_Common::sepstr<int>(strDropList, "#");

	
	//string strTmpContent = TC_Common::replace(strContent, "%rank", TC_Common::tostr(iCanContribute));
	
	mailData.strContent = strContent;
	mailData.iState = GSProto::en_MailState_UnRead;
	mailData.dwSenderTime = time(0);
	mailData.iKeepDay = pMailTb->getInt(iMailRecord, "保留天数");
	mailData.iMailType = GSProto::enMailType_Com;

	IDropFactory* pDropFactory = getComponent<IDropFactory>(COMPNAME_DropFactory, IID_IDropFactory);
	assert(pDropFactory);
	
	GSProto::FightAwardResult awardResult;
	for(vector<int>::iterator it = dropIDList.begin(); it != dropIDList.end(); it++)
	{
		pDropFactory->calcDrop(*it, awardResult);
	}
		
	if(awardResult.IsInitialized() )
	{
		awardResult.SerializeToString(&mailData.strAwardInfo);
	}

	struuid = mailData.strUUID;

	//PLAYERLOG(pEntity)<<"AddMail| Legion _destroy|"<<struuid<<"|"<<awardResult.DebugString()<<endl;
}



