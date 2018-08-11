#ifndef __SKILL_H__
#define __SKILL_H__

#include "SkillResult/SkillResult_Base.h"
#include "SkillCondition/SkillCondition_Base.h"

class ISkillSystem;
class Skill:public ObjectBase<ISkill>, public Detail::EventHandle
{
public:

	friend class SkillFactory;
	friend class SkillSystem;
	friend class SkillLevelStepSort;

	Skill();
	~Skill();
	Skill(const Skill& rhs);

	// ISkill Interface
	virtual bool init(ISkillSystem* pSkillSystem);
	virtual bool canUseSkill(const vector<HEntity>& targetItem);
	virtual bool useSkill(const vector<HEntity>& memberList, const EffectContext& preContext);
	virtual int getSkillID() const{return m_nSkillID;};
	virtual int getSkillType() const{return m_nSkillType;};
	virtual int getSkillGroupID() const{return m_iSkillGroupID;};
	virtual int getSkillLevel() const{return m_nSkillLv;};
	virtual int getMaxLevel() const{return m_iMaxLvl;};
	virtual ISkill* clone() const;
	virtual void packData(int& iskillData);
	virtual bool isActive();
	virtual bool isBattleSkill() const{return m_iBatterAttack != 0;}
	virtual bool canBackAttack() const{return m_bCanBackAttack;}

	void onLevelStepChg(int iOldLevelStep, int iNewLevelStep);

private:

	void doCommonEffect(const vector<HEntity>* memberList, const EffectContext& preContext);
	void doSpecialResult(const vector<HEntity>* memberList, const EffectContext& preContext);
	void usePassiveSkill();
	void usePassiveSkillNeg();
	void registerFightEvent();
	void unRegisterFightEvent();
	void onEventBeginFight(EventArgs& args);
	void onEventBeginRound(EventArgs& args);
	
private:

	PROFILE_OBJ_COUNTER(Skill)

	// 配置数据
	int m_nSkillID;
	int m_iActiveLevelStep;
	int m_iActiveQuality;
	int m_nSkillType;
	int m_iBatterAttack; //是否连击

	int m_iSkillGroupID;
	int m_nSkillLv;
	int m_iMaxLvl;
	int m_iCDRound;
	bool m_bCanBackAttack; // 是否可以被反击
	int m_iSmallSkillChance;
	
	// 特殊条件
	vector<SkillCondition_Base*> m_conditionList;
	std::vector<int> m_effectList;
	std::vector<SkillResult_Base*> m_specialResultList;
	int m_iNeedAnger;
	int m_iConsumeAnger;
	
	
	// 动态数据
	int m_iLastUseRound;	// 上次使用技能的回合数
	int m_iCurrentRound;  // 当前回合数
	ISkillSystem*	m_pSkillSystem;
};


#endif
