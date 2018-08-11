#ifndef __SKILLEFFECTCONDITION_BASE_H__
#define __SKILLEFFECTCONDITION_BASE_H__


class SkillEffect_CondEffect;
class Condition_Base:public Detail::EventHandle
{
public:

	Condition_Base(SkillEffect_CondEffect* pEffect):m_pCondEffect(pEffect){}
	virtual ~Condition_Base(){}
	virtual void registerEvent(HEntity hEntity, const EffectContext& ctx) = 0;
	virtual void unregisterEvent(HEntity hEntity) = 0;
	virtual bool parseConditon(vector<string>& paramList) = 0;

protected:

	SkillEffect_CondEffect* m_pCondEffect;
};



#endif
