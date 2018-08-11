#ifndef __SKILLRESULT_EFFECT_H__
#define __SKILLRESULT_EFFECT_H__

#include "SkillResult_Base.h"

class SkillResult_Effect:public SkillResult_Base
{
public:

	SkillResult_Effect(const EffectContext& context):m_staticContext(context){}
	~SkillResult_Effect(){}

	virtual bool doResult(HEntity hMaster, EffectContext& effectContext);
	virtual SkillResult_Base* clone() const;

private:
	EffectContext m_staticContext;
};


SkillResult_Base* SkillResult_Effect::clone() const
{
	SkillResult_Effect* pNewResult = new SkillResult_Effect(m_staticContext);
	
	return pNewResult;
}

bool SkillResult_Effect::doResult(HEntity hMaster, EffectContext& effectContext)
{
	effectContext += m_staticContext;
	IEntity* pMaster = getEntityFromHandle(hMaster);
	if(!pMaster)
	{
		return false;
	}

	IEffectSystem* pEffectSystem = static_cast<IEffectSystem*>(pMaster->querySubsystem(IID_IEffectSystem) );
	assert(pEffectSystem);

	bool bResult = pEffectSystem->addEffect(effectContext);
	
	return bResult;
}

#endif
