#ifndef __EFFECT_SYSTEM_H__
#define __EFFECT_SYSTEM_H__

class EffectSystem:public ObjectBase<IEffectSystem>
{
public:

	// Constructor/
	EffectSystem();
	~EffectSystem();

	// IEffectSystem Interface
	bool addEffect(EffectContext& effect);
	bool addEffect(int iEffectID, const EffectContext& context);

	// IEntitySubSystem Interface
	virtual Uint32 getSubsystemID() const;
	virtual Uint32 getMasterHandle();
	virtual bool create(IEntity* pEntity, const std::string& strData);
	virtual bool createComplete();
	virtual const std::vector<Uint32>& getSupportMessage();
	virtual void onMessage(QxMessage* pMessage){}
	virtual void packSaveData(string& data){}

	void addPropEnhance(int iSkillID, int iPropID, int iValue);
	void subPropEnhance(int iSkillID, int iPropID, int iValue);
	int getPropEnhance(int iSkillID, int iPropID);

	void addPropPercentEnhance(int iSkillID, int iPropID, int iValue);
	void subPropPercentEnhance(int iSkillID, int iPropID, int iValue);
	int getPropPercentEnhance(int iSkillID, int iPropID);

	void addBuffChance(int iSkillID, int iBuffID, int iValue);
	void subBuffChance(int iSkillID, int iBuffID, int iValue);
	int getBuffEnhance(int iSkillID, int iBuffID);

private:

	map<Int64, int> m_propEnhanceMap;
	map<Int64, int> m_propEnhancePercentMap;
	map<Int64, int> m_buffChanceEnhanceMap;
 
	HEntity m_hMaster;
};

#endif
