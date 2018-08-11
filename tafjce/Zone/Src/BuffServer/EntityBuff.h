#ifndef __ENTITY_BUFF_H__
#define __ENTITY_BUFF_H__

class BuffCondition_Base;
class EntityBuff:public ObjectBase<IEntityBuff>, public Detail::EventHandle
{
public:

	friend class BuffFactory;

	EntityBuff();
	EntityBuff(const EntityBuff& rhs);
	~EntityBuff();

	bool initlize(HEntity hGiver, HEntity hMaster, int iInitOverlap, const EffectContext& preContext);

	// IEntityBuff Interface
	virtual int getOverlapCount();
	virtual int getMaxOverlapCount();
	virtual Int32 getBuffID() const;
	virtual IEntityBuff* clone()const;
	virtual string getBuffUUID();
	virtual HEntity getMaster();
	virtual HEntity getGiver();
	virtual void setOverlapCount(int iNewOverlapCount);
	virtual int getBuffType() const;
	virtual int getBuffGroupID()const {return m_iBuffGroupID;}
	virtual void doSpecialEffect();
	virtual bool isShowClient() const;
	virtual void setSysReleaseFlag();

	void resetEffectRound();

public:

	bool isImMunityBuff(int iBuffID);

private:
	void beginEffect();
	void endEffect();
	void roundEffect();
	void onEventPreAttack(EventArgs& args);
	void onEventPostAttack(EventArgs& args);
	void onEventDead(EventArgs& args);
	void onBeginFight(EventArgs& args);
	void delSelf();
	void incEffectCount();

private:

	typedef std::vector<int> EffectList;
	typedef std::vector<BuffCondition_Base*> ConditionList;

	// config data
	int m_iBuffID;
	string m_strBuffName;
	int m_iBuffGroupID;
	int m_iBuffType;
	Uint32 m_dwLifeRound;		// 持续回合
	bool m_bPostAttackCount;  // 是否行动后计数
	int m_iMaxOverlapCount;
	EffectList m_initEffectList; // 开始执行，不能撤销
	EffectList m_normalEffectList;
	int m_iRoundInteval;	// 间隔生效回合
	int m_bShowClient;
	EffectList m_roundEffectList;

	int m_iSpecialEffectID;
	ConditionList m_specialConditionList;

	// dynamic data
	string m_strUUID;
	int m_iOverlapCount;
	HEntity m_hMaster;
	HEntity m_hGiver;
	int m_iLastRoundIndex; //上次生效回合
	int m_iEffectRoundCnt;  // 已经生效的回合数目
	EffectContext m_preContext;

	vector<HEntity> m_fightMemberList;
	bool m_bSysRelease;
};

#endif
