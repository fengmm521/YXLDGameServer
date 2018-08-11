#ifndef __SKILLEFFECT_QIXING_H__
#define __SKILLEFFECT_QIXING_H__


#include "SkillEffect_Helper.h"

class SkillEffect_QiXing:public SkillEffect_Helper, public Detail::EventHandle
{
public:

	SkillEffect_QiXing(){}

	virtual bool doSignleEffect(HEntity hEntity, const EffectContext& effectContext);
	virtual bool parseEffect(const std::string& strEffectCmd, EffectContext& context);
	virtual int getEffectType(){return en_SkillEffect_QiXing;}
	virtual vector<string> getTypeString();

	void onEventTouchDead(EventArgs& args);
	void onEventDelBuff(EventArgs& args);
	void onEventPreRelease(EventArgs& args);

private:

	//int m_iBuffID;
	//vector<int> m_effectIDList;

	map<HEntity, EffectContext> m_mapContext;
};


bool SkillEffect_QiXing::doSignleEffect(HEntity hEntity, const EffectContext& effectContext)
{
	IEntity* pEntity = getEntityFromHandle(hEntity);
	assert(pEntity);

	bool isNegativeEffect = effectContext.getInt(PROP_EFFECT_NEGATIVE) == 1;

	if(isNegativeEffect)
	{
		pEntity->getEventServer()->unsubscribeEvent(EVENT_ENTITY_EFFECT_TOCHBEKILL, this, &SkillEffect_QiXing::onEventTouchDead);
		pEntity->getEventServer()->unsubscribeEvent(EVENT_ENTITY_RELEASEBUFF, this, &SkillEffect_QiXing::onEventDelBuff);
		pEntity->getEventServer()->unsubscribeEvent(EVENT_ENTITY_PRE_RELEASE, this, &SkillEffect_QiXing::onEventPreRelease);
	}
	else
	{
		pEntity->getEventServer()->subscribeEvent(EVENT_ENTITY_EFFECT_TOCHBEKILL, this, &SkillEffect_QiXing::onEventTouchDead);
		pEntity->getEventServer()->subscribeEvent(EVENT_ENTITY_RELEASEBUFF, this, &SkillEffect_QiXing::onEventDelBuff);
		pEntity->getEventServer()->subscribeEvent(EVENT_ENTITY_PRE_RELEASE, this, &SkillEffect_QiXing::onEventPreRelease);
	}

	m_mapContext[hEntity] = effectContext;

	return true;
}


bool SkillEffect_QiXing::parseEffect(const std::string& strEffectCmd, EffectContext& context)
{
	vector<string> paramList = TC_Common::sepstr<string>(strEffectCmd, Effect_MagicSep);
	assert(paramList.size() > 0);

	if( (paramList[0] == "七星灯") && (paramList.size() >= 3) )
	{
		context.setInt(PROP_EFFECT_TYPE, (Int32)getEffectType() );
	
		int iBuffID = AdvanceAtoi(paramList[1]);

		context.setInt(PROP_EFFECT_QIXING_BUFFID, iBuffID);
		
		paramList.erase(paramList.begin(), paramList.begin() + 2);
		
		/*string strRealEffect = SkillEffect_CondEffect::concatString(paramList);

		IEffectFactory* pEffectFactory = getComponent<IEffectFactory>(COMPNAME_EffectFactory, IID_IEffectFactory);
		assert(pEffectFactory);

		pEffectFactory->parseEffect(strRealEffect, m_effectCtx);*/

		vector<int>* pEffectList = new vector<int>();
		for(size_t i = 0; i < paramList.size(); i++)
		{
			pEffectList->push_back(AdvanceAtoi(paramList[i]));
		}

		context.setInt64(PROP_EFFECT_QIXING_EFFECTLIST, (Int64)pEffectList);

		return true;
	}

	return false;
}

vector<string> SkillEffect_QiXing::getTypeString()
{
	vector<string> resultList;

	resultList.push_back("七星灯");

	return resultList;
}


void SkillEffect_QiXing::onEventPreRelease(EventArgs& args)
{
	HEntity hEntity = args.context.getInt("entity");
	m_mapContext.erase(hEntity);
}


void SkillEffect_QiXing::onEventTouchDead(EventArgs& args)
{
	HEntity hEntity = args.context.getInt("entity");

	IEntity* pEntity = getEntityFromHandle(hEntity);
	assert(pEntity);

	IBuffSystem* pBuffSys = static_cast<IBuffSystem*>(pEntity->querySubsystem(IID_IBuffSystem));
	assert(pBuffSys);

	if(m_mapContext.find(hEntity) == m_mapContext.end() )
	{
		return;
	}

	EffectContext& effectCtx = m_mapContext[hEntity];

	int iBuffID = effectCtx.getInt(PROP_EFFECT_QIXING_BUFFID);

	// 如果身上已经有这个Buff,不处理
	if(pBuffSys->existBuff(iBuffID) )
	{
		return;
	}

	int iUsed = effectCtx.getInt(PROP_EFFECT_QIXING_USED);
	if(iUsed > 0)
	{
		return;
	}

	effectCtx.setInt(PROP_EFFECT_QIXING_USED, 1);

	// 设置个标记吧
	pEntity->setProperty(PROP_ENTITY_PREDEAD_USESKILLFLAG, 1);

	pBuffSys->addBuff(hEntity, iBuffID);
	pEntity->setProperty(PROP_ENTITY_PREDEAD_USESKILLFLAG, 0);
}

void SkillEffect_QiXing::onEventDelBuff(EventArgs& args)
{
	HEntity hEntity = args.context.getInt("entity");
	if(m_mapContext.find(hEntity) == m_mapContext.end() )
	{
		return;
	}

	const EffectContext& effectCtx = m_mapContext[hEntity];
	int iCfgBuffID = effectCtx.getInt(PROP_EFFECT_QIXING_BUFFID);

	int iBuffID = args.context.getInt("buffid");
	if(iBuffID != iCfgBuffID) return;

	
	IEntity* pEntity = getEntityFromHandle(hEntity);
	assert(pEntity);

	// 判断是否还活着
	int iHP = pEntity->getProperty(PROP_ENTITY_HP, 0);
	if(iHP <= 0) return;

	IEffectSystem* pEffectSys = static_cast<IEffectSystem*>(pEntity->querySubsystem(IID_IEffectSystem));
	assert(pEffectSys);

	EffectContext tmpContext;
	
	tmpContext.setInt(PROP_EFFECT_GIVER, (Int32)hEntity);

	IFightFactory* pFightFactory = getComponent<IFightFactory>(COMPNAME_FightFactory, IID_IFightFactory);
	assert(pFightFactory);

	

	const vector<HEntity>* pMemberList = pFightFactory->getMemberList();
	tmpContext.setInt64(PROP_EFFECT_FIGHTMEMBERLIST, (Int64)pMemberList);

	vector<int>* pEffectIDList = (vector<int>*)effectCtx.getInt64(PROP_EFFECT_QIXING_EFFECTLIST, 0);

	for(size_t i = 0; i < pEffectIDList->size(); i++)
	{
		pEffectSys->addEffect((*pEffectIDList)[i], tmpContext);
	}

	//pEffectSys->addEffect(m_effectCtx);
}

#endif
