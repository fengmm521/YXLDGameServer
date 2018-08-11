/********************************************************************
created:	2013-9-24
author:		Pelog (唐超)
summary:	战斗
*********************************************************************/
#pragma once

#include "FmSysUtil.h"
#include "FmGeometry.h"
#include "Util/FmStateTimer.h"
#include "FmEntity.h"
#include "NetWork/GameServer.pb.h"
#include "Skill/FmSkillResultData.h"

NS_FM_BEGIN
class Scene;
class Hero;

struct FightMgrIndex 
{
	int m_actionIndex;
	int m_resultIndex;
	bool m_isFirst;
	FightMgrIndex()
	{
		m_actionIndex = -1;
		m_resultIndex = -1;
		m_isFirst = true;
	}
};

struct PlayerInfo_ 
{
	int iHeadID;
	string strActorName;
	int iLevel;
	PlayerInfo_()
	{
		iHeadID = 1;
		iLevel = 1;
		strActorName = "";
	}
};
struct ObjectData_ 
{
	int iBaseID;
	int iLevel;
	int iLevelStep;
	int iQuality;
	int location;
	ObjectData_()
	{
		iBaseID = 1;
		iLevel = 1;
		iLevelStep = 1;
		iQuality = 1;
		location = -1;
	}
};
struct FightMember 
{
	ObjectData_ Objects[18];
	PlayerInfo_ leftPlayer;
	PlayerInfo_ rightPlayer;
	int iIsAttackerWin;
	FightMember()
	{
		leftPlayer = PlayerInfo_();
		rightPlayer = PlayerInfo_();
		iIsAttackerWin = 0;
	}
};
class FightMgr
{
private:
	StateTimer	m_MainState;			// 主状态
	FightMgrIndex m_index;
	FightMgrIndex m_lastIndex;
public:
	bool		m_isCg;
	bool		m_isCanItem;
	bool		m_isCanRevAcion;
	bool		m_isSkip;
	bool		m_canSkip;
	bool		m_bInitStateRuning;

	int			m_itemCount;
	Hero*		m_finalHero;

	int         m_fightType;

	SINGLETON_MODE(FightMgr);
	FightMgr();
	virtual ~FightMgr();

	// 状态
	StateTimer& GetMainState()	{ return m_MainState; }

	// 更新接口
	bool OnUpdate( uint delta );
	static void OnFightMember( int iCmd, GSProto::SCMessage& pkg );
	static void OnFightAction( int iCmd, GSProto::SCMessage& pkg );
	static void OnFightFin( int iCmd, GSProto::SCMessage& pkg );

	void InitFight();
	Hero* GetHeroByPos(int pos);
	FightMember getFightMember();
	GSProto::Cmd_Sc_FightMember m_fightMember;
	vector<GSProto::Cmd_Sc_FightAction> m_fightActionList;
	vector<GSProto::FightObj> m_replaceMem;
	bool	m_isRevAll;
	int m_replaceIndex;

	vector<GSProto::FightResulSet> m_fightResultList;

	SkillResultData m_curSkillResultData;
	stSkillData* m_skill;
	stSkillEffectData* m_skillEffect;
	Hero* m_actionHero;

	void DecodeOneResultSet();
	void EndOneResultSet();

	void Replay();
	void GetFightMgrIndex(FightMgrIndex& indexSt);

	void FightResume();
	void FinishFight();

	void setAllFightObjVisible(bool bVisible);
	void initAllHeroUILayer();
};

NS_FM_END