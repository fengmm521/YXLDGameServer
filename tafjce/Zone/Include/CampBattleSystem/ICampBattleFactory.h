#ifndef ICMAPBATTLEFACTORY_H_
#define ICMAPBATTLEFACTORY_H_


enum CampBattleActorState
{
	en_CampBattleActorState_SignUp = 1, 				//报名
	en_CampBattleActorState_InMatching = 2, 			//匹配中
	en_CampBattleActorState_InBattling = 3, 			//战斗中
	en_CampBattleActorState_InWaiting  = 4, 			//待战中
	en_CampBattleActorState_InWaitOver = 5,				//待战结束
	en_CampBattleActorState_SignOut = 6,				//退出
};

enum CampBattleFactoryState
{
	en_CampBattle_BeforeStart = 0, //开始之前
	en_CampBattle_StartSignUp = 1,		//游戏报名开始
	en_CampBattle_InBattle = 3,		//战斗中
	en_CampBattle_InEnd = 4,		//阵营战结束
};

enum CampType
{
	CampTypeNul = 0,
	CampTypeA = 1,
	CampTypeB = 2,
};

//战斗单元
struct BattleUnit
{
public:
	BattleUnit():sActorName(""),
				iHonor(0),
				iFightValue(0),
				enActorState(en_CampBattleActorState_SignOut),
				iSuccessionTimes(0),
				iBattleCD(0),
				iMainHeroId(0),
				iWinTimes(0),
				iFailedTimes(0),
				iTopSuccession(0),
				iSilver(0),
				iCamp(0),
				ibattleTimes(0)
	{
		hpMap.clear();
	}
	~BattleUnit()
	{
		hpMap.clear();
	}
	
	std::string sActorName;
	int iHonor;
	int iFightValue;
	CampBattleActorState enActorState;
	int iSuccessionTimes;     //连胜次数
	int iBattleCD;			//战斗轮数
	int iMainHeroId;		//主将ID

	int iWinTimes;			//胜利次数
	int iFailedTimes; 		//失败次数
	int iTopSuccession;	   //最高连杀
	int iSilver; 		 //铜币
	int iCamp;
	int ibattleTimes;
	
	map<int ,int> hpMap;
	

	inline bool operator==(const BattleUnit& unit)
	{
		return unit.sActorName == sActorName;
	}
	
	inline BattleUnit& operator=(const BattleUnit& unit)
	{
		sActorName = unit.sActorName;
		iHonor = unit.iHonor;
		iFightValue = unit.iFightValue;
		enActorState = unit.enActorState;
		iSuccessionTimes = unit.iSuccessionTimes;
		iBattleCD = unit.iBattleCD;
		iMainHeroId = unit.iMainHeroId;
		iWinTimes = unit.iWinTimes;
		iFailedTimes = unit.iFailedTimes;
		iTopSuccession = unit.iTopSuccession;
		iSilver = unit.iSilver;
		iCamp = unit.iCamp;
		
		hpMap = unit.hpMap;
		ibattleTimes = unit.ibattleTimes;
		return *this;
	}

	BattleUnit(const BattleUnit& unit)
	{
		sActorName = unit.sActorName;
		iHonor = unit.iHonor;
		iFightValue = unit.iFightValue;
		enActorState = unit.enActorState;
		iSuccessionTimes = unit.iSuccessionTimes;
		iBattleCD = unit.iBattleCD;
		iMainHeroId = unit.iMainHeroId;
		iWinTimes = unit.iWinTimes;
		iFailedTimes = unit.iFailedTimes;
		iTopSuccession = unit.iTopSuccession;
		iSilver = unit.iSilver;
		iCamp = unit.iCamp;
		hpMap = unit.hpMap;
		ibattleTimes = unit.ibattleTimes;
	}
};


inline bool compareWithHonor(const BattleUnit& u1,const BattleUnit& u2)
{
	if(u1.iHonor != u2.iHonor)
	{
		return u1.iHonor > u2.iHonor;
	}
	return u1.sActorName > u2.sActorName;	
}

inline bool compareWithFightValue(const BattleUnit& u1,const BattleUnit& u2)
{
	if(u1.iFightValue != u2.iFightValue)
	{
		return u1.iFightValue > u2.iFightValue;
	}
	return u1.sActorName > u2.sActorName;
}


class ICampBattleFactory:public IComponent
{
public:
	//阵营战是否已经开启
	virtual bool IsStartCampBattle() = 0;
	//报名参战
	virtual bool SignUp(HEntity actorEntity) = 0;
	//获取排名列表A
	virtual const std::vector<BattleUnit>& getCampARankList() = 0;
	//获取排名列表B
	virtual const std::vector<BattleUnit>& getCampBRankList() = 0;
	//获取剩余时间
	virtual  void getRemaindTimeAndState(CampBattleFactoryState& state,int &remaindSecond) = 0;
	//移除参战单元
	virtual bool removeBattleUnit( HEntity actorEntiy ) = 0;
	
	virtual void startCampBattle(string startTime,string preTime, string endTime)= 0;

	virtual bool getActorBattlUnit(BattleUnit& unit,IEntity* pEntity) = 0;

	virtual void addBroadCastUnit(string strName) = 0;
 	virtual void removeBroadCastUnit(string strName) = 0;
};


#endif
