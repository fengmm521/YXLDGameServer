#ifndef CAMPBATTLEFACTORY_H_
#define CAMPBATTLEFACTORY_H_

#include <set>

class CampBattleFactory:public ComponentBase<ICampBattleFactory,IID_ICampBattleFactory>	,public ITimerCallback 
{
public:
	CampBattleFactory();
	virtual ~CampBattleFactory();
public:
	virtual bool IsStartCampBattle();	//外部接口，阵营战是否已经开启，@return true-已经开启
	virtual bool SignUp(HEntity actorEntity);	//外部接口,参加阵营战
	virtual const std::vector<BattleUnit>& getCampARankList();
	virtual const std::vector<BattleUnit>& getCampBRankList();
	virtual void getRemaindTimeAndState(CampBattleFactoryState& state,int &remaindSecond);
	virtual bool removeBattleUnit( HEntity actorEntiy);
	virtual void startCampBattle(string startTime,string preTime, string endTime);
	
	virtual void addBroadCastUnit(string strName);
 	virtual void removeBroadCastUnit(string strName);

public:
	virtual bool initlize(const PropertySet& propSet);
	// ITimerCallback Interface
	virtual void onTimer(int nEventId);
public:
	void checkCampBattle();			//检测阵营战状态，根据状态开启功能
	void sendBattleData();			//下行战斗数据
	void startMatchBattle();             //匹配战斗
	void partPrePareList();
	void matchSuccess();			//匹配成功
	bool getActorCreateContext(ServerEngine::PKRole& pkRole, IEntity* pEntity);
	void processFightResult( string actorAName,string actorBName, ServerEngine::BattleData& battleInfo);
	void fillBattleUnitContext(string actorAName, string actorBName,ServerEngine::FightContext& fightContext);
	void pollingWaitVec();			//轮询待战列表

	void getWinerSilverAndHonor( BattleUnit actorA, BattleUnit actorB, int &silver, int &honor,ServerEngine::BattleData& battleInfo);
	void getLoserSilverAndHonor( BattleUnit actorA, BattleUnit actorB, int &silver, int &honor,ServerEngine::BattleData& battleInfo);

	void broadCastBattleInfo( BattleUnit actorA, BattleUnit actorB, bool bIsAWin,int silver,int honor);
	void updateRankList(vector<BattleUnit>& vec, BattleUnit& actor);
	void broadCastRankList();
	void broadCastBattleReport();
	void reMatching(string actorA, string actorB);			//重新匹配

	bool addCamp(BattleUnit& unit);
	virtual bool getActorBattlUnit(BattleUnit& unit,IEntity* pEntity) ;

	void checkJumpBattleVec();
	
private:
	
	typedef	std::map<std::string,BattleUnit> CampMap;

	CampMap m_allBattlUnit;
	set<string> m_allBroadCastNameSet;
	std::vector<string>  m_matchingDequeB;         	//匹配队列B
	std::vector<string>  m_matchingDequeA;         	//匹配队列A
	CampMap m_waitBattleMapA;			//待战列表A
	CampMap m_waitBattleMapB;		  	//待战列表B
	int m_iFightValueA;   				//A阵营战斗力
	int m_iFightValueB;                 //B阵营战斗力
	
	std::vector<BattleUnit> m_RankListA;				//A阵营排行列表
	std::vector<BattleUnit> m_RankListB;				//B阵营排行列表
	
	unsigned int m_StartSeconds;					//当天开始的秒数
	unsigned int m_EndSeconds;						//当天战斗结束时间
	unsigned int m_SignUpTimeSeconds;				//报名等待时
	
 	CampBattleFactoryState m_state;					//阵营战状态
	ITimerComponent::TimerHandle m_hCheckTimerHandle; //定时器，定时检查阵营战是否开启
	ITimerComponent::TimerHandle m_hCheckWaitVecTimerHandle; //定时器, 定时检查待战列表
	ITimerComponent::TimerHandle m_hBroadCastTimerHandle; //定时器, 定时定时广播排行列表
	ITimerComponent::TimerHandle m_hBroadCastReportTimerHandle; //定时器, 定时广播战报
	
	ITimerComponent::TimerHandle m_hMatchBattleTimerHandle; //定时器，定时检查阵营战是否开启
	set<string> m_JumpBattleActorNameSet;

	ITimerComponent::TimerHandle m_JumpBattleTimerHandle;
	GSProto::CMD_CAMPBATTLE_BATTLE_LOG_SC m_LogScMsg;
};


#endif
