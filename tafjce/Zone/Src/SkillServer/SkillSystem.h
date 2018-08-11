#ifndef __SKILLSYSTEM_H__
#define __SKILLSYSTEM_H__

#include "ISkillSystem.h"

class SkillSystem:public ObjectBase<ISkillSystem>, public Detail::EventHandle 
{
public:

	SkillSystem();
	~SkillSystem();

	// IEntitySubsystem Interface
	virtual Uint32 getSubsystemID() const;
	virtual Uint32 getMasterHandle();
	virtual bool create(IEntity* pEntity, const std::string& strData);
	virtual bool createComplete();
	virtual const std::vector<Uint32>& getSupportMessage();
	virtual void onMessage(QxMessage* pMessage);
	virtual void packSaveData(string& data);

	// ISkillSystem Interface
	virtual bool useSkill(const vector<HEntity>& memberList);
	virtual int getSelectedActiveSkill();
	virtual bool useTempSkill(int iSkillID, const vector<HEntity>& memberList, const EffectContext& preContext=EffectContext());
	virtual bool addTempSkill(int iSkillID);
	virtual void getTallentSkillList(vector<int>& skillList);
	virtual int getStageSkill();
	virtual void backAttack(HEntity hTarget);
	virtual vector<ISkill*> getSkillList(bool bOnlyActive);
	virtual ISkill* getSkill(int iSkillID);

	vector<int> getSkillListByType(int iSkillType, bool bOnlyActive);
	
   
	ISkill* selectSkill(const vector<HEntity>& memberList);
	
	void onEventLevelStepChg(EventArgs& args);
	
private:

 	bool initSkillList(const vector<int>& skillIDList);
	bool initHeroAndGoldAnimalSkill();
	bool initMonsterSkill();
	void onEventPreBeginFight(EventArgs& args);
	int _getTopSkillByType(int iSkillType, bool bActive);

	void onMemberUseSkill(EventArgs& args);
	
private:

	PROFILE_OBJ_COUNTER(SkillSystem);
	typedef std::vector<ISkill*> MapSkill;

	HEntity m_hEntity;
	MapSkill m_skillMap;

	MapSkill m_tempSkillMap;		// 临时技能(法宝、装备技能等使用)
	int m_iCurrentRound;
};


#endif
