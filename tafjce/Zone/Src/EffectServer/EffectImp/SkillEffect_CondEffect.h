#ifndef __SKILLEFFECT_CONDEFFECT_H__
#define __SKILLEFFECT_CONDEFFECT_H__

#include "SkillEffect_Helper.h"
#include "IFightFactory.h"
#include "Condition_Base.h"


class Condition_HitResult;
class SkillEffect_CondEffect:public SkillEffect_Helper, public Detail::EventHandle
{
public:

	SkillEffect_CondEffect(){}

	virtual bool doSignleEffect(HEntity hEntity, const EffectContext& effectContext);
	virtual bool parseEffect(const std::string& strEffectCmd, EffectContext& context);
	virtual int getEffectType(){return en_SkillEffect_CondEffect;}
	virtual vector<string> getTypeString();
	void doTriggerEffect(HEntity hEntity, const EffectContext& preCtx);
	static string concatString(const vector<string>& paramList);

	void onEventPreRelease(EventArgs& args);

private:

	map<HEntity, EffectContext> m_mapContextMap;

	//Condition_Base* m_pTriggerCondition;
	//EffectContext m_triggerEffectCtx;
	//vector<int> m_effectIDList;
};



#endif

