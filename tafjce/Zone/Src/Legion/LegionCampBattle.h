#ifndef __LEGION_CAMPBATTLE_H__
#define __LEGION_CAMPBATTLE_H__

struct LegionBattleMember
{
	LegionBattleMember():bMonster(false), iMemberLevel(1), iMonsterGrpID(0),iMonsterGrpSeq(0){}

	bool bMonster;
	ServerEngine::PKRole roleKey;
	int iMemberLevel;
	string strName;
	string strLegionName;
	int iMonsterGrpID;
	int iMonsterGrpSeq;
	
		
	bool operator < (const LegionBattleMember& rhs) const
	{
		if(bMonster != rhs.bMonster)
		{
			return (int)bMonster > (int)rhs.bMonster;
		}

		if(bMonster)
		{
			return iMonsterGrpSeq > rhs.iMonsterGrpSeq;
		}
		else
		{
			return strName > rhs.strName;
		}
	}

	bool operator==(const LegionBattleMember& rhs)const
	{
		return (!(*this < rhs) ) && (!(rhs < *this)) ;
	}
};


struct LegionCityCfg
{
	LegionCityCfg():iCityID(0), iSilver(0), iLegionLevel(1){}

	int iCityID;
	string strCityName;
	int iSilver;
	string strNpcLegionName;
	string strLeaderName;
	int iLegionLevel;
	int iReportCost;
	int iLegionIcon;
};


struct  BattleImpData
{
	~BattleImpData()
	{
		m_InBattleMap.clear();
		actorHaveBattleMap.clear();
	}
	ServerEngine::LegionBattle legionBattleData;
	map<string, map<int, ServerEngine::TimeResetValue> > m_InBattleMap;

	map<string,bool>  actorHaveBattleMap;
 	bool bIsActorInAttStatus(string strName, int iStar);
	
	bool addActorInAttStatus(string strName, int iStar);
	
	bool deleteActorAttStatus(string strName, int iStar);
};

class LegionFactory;
class LegionCampBattle:public ITimerCallback
{
public:

//	friend class GetBattleMemCtx;
	LegionCampBattle()
	{
	}


	LegionCampBattle(LegionFactory* pLegionFactory);
	~LegionCampBattle();

	// ITimerCallback Interface
	virtual void onTimer(int nEventId);
	
	bool initFromData(const ServerEngine::LegionCity& hisData, const ServerEngine::LegionCity& curData);
	void packSaveData(ServerEngine::GlobalLegionCityData& globalCityData);

	string strOwerName();
	int iGetCampBattleId();
	int getCurReportLegionCount();

	bool reportCampBattle(HEntity hActor,const string& strLegionName);

	void sendCampBattleInfo(HEntity hActor);


	void fillScJoinInfo(HEntity hActor, GSProto::CMD_LEGION_CAMPBATTLE_JOIN_SC& scMsg, bool bIsGuard = false, string strVsLegionName="");

	bool joinCampBattle(HEntity hActor, const string& strLegionName);

	void onCampMemberBattle(HEntity hActor, int iBattleId ,string strMemName, int iStar);

	bool bIsLegionAddCampBattle(string strLegionName);

	void fillBattleCtx(const ServerEngine::CityBattleLegionData& attLegionData,const ServerEngine::CityBattleLegionData& targetLegionData, ServerEngine::FightContext& ctx, int iStar);
	void processFightResult(int iBattleId,const string& strAttMemberName, const string& strGuardMemberName, bool bIsAttGuard,int iStar,const ServerEngine::BattleData& battleInfo,HEntity hEntity);
	
	void cleanDataBeforeReport();

	void initCityCfgData(const LegionCityCfg& m_CityCfg);

	void fillNPC();
	void updateCityOwner();

	void pairBattle();
	void battleOver();

	int calAddPercent(int iHaveHoldCityCount);

	void _fillLegionVsDetail(int iBattleId, GSProto::VSLegion& vsLegion);
	void _fillLegionVsMember(int iBattleId,
							bool bSendGuard,
		::google::protobuf::RepeatedPtrField< ::GSProto::VSLegionMemDetail >& szmemberlist);

	string getOwnerLegion();

	void onQueryCurFightCondition(HEntity hActor);

	void addFightResoultLog(bool bIsGuard, bool bIsWin, string strLegionName);

	void setLegionGuard(string strLegionName);

	void fillMail(string& struuid, ServerEngine::MailData & mailData, int iBroadId);

	void sendMail( const map<std::string, ServerEngine::CityBattleItem>& memList);
private:
	LegionFactory* m_pLegionFactory;
	ServerEngine::LegionCity m_curData;
	vector<BattleImpData> m_BattleList;

	map<string, int> m_legion2BattleId;

	int m_iMaxLegionHave;

	LegionCityCfg m_CityCfg;
	
};




#endif
