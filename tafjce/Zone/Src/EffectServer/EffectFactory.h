#ifndef __EFFECT_FACTORY_H__
#define __EFFECT_FACTORY_H__

#include "IEffectFactory.h"
#include "EffectDefine.h"

class EffectFactory:public ComponentBase<IEffectFactory, IID_IEffectFactory>
{
public:

	friend class EffectSystem;

	// Constructor
	EffectFactory();
	~EffectFactory();

	// IComponent Interface
	virtual bool initlize(const PropertySet& propSet);

	// IEffectFactiry Interface
	virtual bool parseEffect(const string& strEffectCmd, EffectContext& context);
	virtual bool doEffect(HEntity hEntity, int iEffectID, const EffectContext& effectContext);
	virtual bool isImMunityBuff(int iBuffID, int iEffectID, const EffectContext& effectContext);
	virtual bool isPropNotify(int iPropID, int& iShowPropID);
	
private:
	bool registerEffect(ISkillEffect* pSkillEffect);
	ISkillEffect* getEffect(Int32 nEffectType);
	bool loadEffect();
	bool loadNotifyProp();
	void preRegister();
	bool doSingleEffect(HEntity hEntity, const EffectContext& context);
	
private:

	typedef std::map<Int32, ISkillEffect*>	MapEffect;
	MapEffect	m_mapEffect;

	typedef std::map<string, ISkillEffect*> MapNameEffect;
	MapNameEffect m_mapNameEffect;

	typedef std::map<int, vector<EffectContext> > MapEffectContext;
	MapEffectContext	m_mapEffectContext;

	map<int, int> m_notifyPropList;
};


#endif
