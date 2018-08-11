#ifndef __DUNGEON_SYSTEM_H__
#define __DUNGEON_SYSTEM_H__

#include "ITimeRangeValue.h"

class SaoDangPointResume:public ITimeRangeTask
{
public:

	SaoDangPointResume(){};
	void init(HEntity hEntity){m_hEntity = hEntity;}
	virtual TimeRangeExecute getExecuteDelegate();
	virtual TimeRangeCheck getCheckDelegate();
	virtual int checkInteval(){return 10*1000;}

	bool checkResume(Uint32 dwTime);
	void doResume(Uint32 dwTime);

private:

	HEntity m_hEntity;
};



class DungeonSystem:public ObjectBase<IDungeonSystem>, public Detail::EventHandle
{
public:

	friend class DungeonFightCb;
	

	DungeonSystem();
	~DungeonSystem();

	// IEntitySubSystem Interface
	virtual Uint32 getSubsystemID() const;
	virtual Uint32 getMasterHandle();
	virtual bool create(IEntity* pEntity, const std::string& strData);
	virtual bool createComplete();
	virtual const std::vector<Uint32>& getSupportMessage();
	virtual void onMessage(QxMessage* pMessage);
	virtual void packSaveData(string& data);

	// IDungeonSystem Interface
	virtual bool getSceneStar(int iSceneID, int& iPassStar);
	virtual bool isSectionOpend(int iSectionID);
	virtual int getCurSectionID();
	virtual bool canSkipCurScene();
	void initDungeonSysData(const string& strData);

	virtual bool getSectionHavePass(int iSectionID);
	virtual bool getSeceneHavePass(int isceneID);

private:
	
	void onEventSendToClient(EventArgs& args);
	void onQuerySectionMsg(const GSProto::CSMessage& message);
	void onDungeonFightMsg(const GSProto::CSMessage& message);
	void onGetSectionAwardMsg(const GSProto::CSMessage& message);
	void onReqSaoDangMsg(const GSProto::CSMessage& message);
	void onReqQueryFpEquipList(const GSProto::CSMessage& message);
	void onReqExecFp(const GSProto::CSMessage& message);
	
	bool isSectionPerfectPass(int iSectionID);
	void sendSectionUpdate(int iSectionID);
	bool isSceneOpened(int iSceneID);
	void processFightResult(int iScenID, int iRet, const ServerEngine::BattleData& data);
	void processDungeonAward(int iScenID, GSProto::FightAwardResult& awardResult, bool bFirstPass, bool bFirstPerfectPass, int& extraExp);
	bool updateSceneStar(int iSceneID, int iStar);
	void processNewPassScene(int iSceneID, int iStar);
	void doAwardPerfect(const PerfectAwardCfg* pPerfectCfg);
	void doHeroExpAward(GSProto::Cmd_Sc_CommFightResult& scCommFightResult, int iAddHeroExp);
	void doFpGenerate(const DungeonScene* pDungeonScene);
	int randomFpEquip(const vector<ChanceCfg>& chanceList);
	int calcFpCostGold();
	
	void addSpecialGuideHero(GSProto::FightAwardResult& awardResult);
	int getSectionStar(int iSectionID);
	void fillScSection(GSProto::DungeonSection* pTmpSecction, int iSectionID);
	bool fillScSceneDetail(GSProto::DungeonSceneDetail* pTmpDetail, int iSceneID);
	int getSaoDangCD();
	bool checkSceneFightCond(int iSceneID, int& iCostPhyStrength, bool& bHasPassLimit);
	void sceneAwardAndEvent(int iSceneID, GSProto::Cmd_Sc_CommFightResult& scCommFightResult, bool bPerfectPassSection, bool bFirstPassScene, bool bFirstPerfectPass);

private:

	HEntity m_hEntity;
	ServerEngine::DungeonSystemData m_dungeonSysData;
	int m_iCurFightingScene; // 仅仅在战斗服回调前有效
	SaoDangPointResume m_saoDangPointResume;
	bool m_bCanFp;
	vector<HItem> m_fpEquipList;
	vector<int> m_canGetedPosList;

	bool b_ActorIsFight;
	int m_iLastFightSecond;
};


#endif


