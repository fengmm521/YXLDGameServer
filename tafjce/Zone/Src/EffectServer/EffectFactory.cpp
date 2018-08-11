#include "EffectServerPch.h"
#include "EffectFactory.h"
//#include "IAwardFactory.h"
//#include "IMailFactory.h"
#include "ErrorCodeDef.h"
//#include "IFormationSystem.h"
//#include "IContainerSystem.h"
//#include "ITeamFightSystem.h"
#include "util/tc_encoder.h"
#include "EffectImp/SkillEffect_Helper.h"
#include "EffectImp/SkillEffect_AddProperty.h"
#include "EffectImp/SkillEffect_Damage.h"
#include "EffectImp/SkillEffect_AddBuff.h"
#include "EffectImp/SkillEffect_RandDelBuff.h"
#include "EffectImp/SkillEffect_CondDead.h"
#include "EffectImp/SkillEffect_ReboundDamage.h"
#include "EffectImp/SkillEffect_ContinueKill.h"
#include "EffectImp/SkillEffect_Relive.h"
#include "EffectImp/SkillEffect_PropEnhance.h"
#include "EffectImp/SkillEffect_BuffEnhance.h"
#include "EffectImp/SkillEffect_ActorDamageHeal.h"
#include "EffectImp/SkillEffect_CondEffect.h"
#include "EffectImp/SkillEffect_ImMunityBuff.h"
#include "EffectImp/SkillEffect_TouchDeadEffect.h"

#include "EffectImp/SkillEffect_PropEnhanceByHP.h"
#include "EffectImp/SkillEffect_QiXing.h"
#include "EffectImp/SkillEffect_ZiBao.h"

//#include "EffectImp/SkillEffect_CallAssistant.h"

//#include "EffectImp/SkillEffect_AddProperty.h"
/*#include "EffectImp/SkillEffect_Damage.h"
#include "EffectImp/SkillEffect_OpenDungeonPage.h"
#include "EffectImp/SkillEffect_TriggerFinParkour.h"
#include "EffectImp/SkillEffect_TriggerGather.h"
#include "EffectImp/SkillEffect_TriggerKill.h"
#include "EffectImp/SkillEffect_Heal.h"
#include "EffectImp/SkillEffect_Batter.h"
#include "EffectImp/SkillEffect_LearnSkill.h"
#include "EffectImp/SkillEffect_AddPet.h"
#include "EffectImp/SkillEffect_DelBuff.h"
#include "EffectImp/SkillEffect_ImMunityBuff.h"
#include "EffectImp/SkillEffect_LearnFormation.h"
#include "EffectImp/SkillEffect_OpenChest.h"
#include "EffectImp/SkillEffect_AddExp.h"
#include "EffectImp/SkillEffect_SwitchTeamFightScene.h"
#include "EffectImp/SkillEffect_RicherBuffMsg.h"
#include "EffectImp/SkillEffect_ActiveMagicGhost.h"
#include "EffectImp/SkillEffect_CombineItem.h"
#include "EffectImp/SkillEffect_PetSoulCombine.h"*/


extern "C" IComponent* createEffectFactory(Int32)
{
	return new EffectFactory;
}


EffectFactory::EffectFactory()
{
}

EffectFactory::~EffectFactory()
{
	for(MapEffect::iterator it = m_mapEffect.begin(); it != m_mapEffect.end(); it++)
	{
		delete it->second;
	}

	m_mapEffect.clear();
	m_mapNameEffect.clear();
}


bool EffectFactory::isPropNotify(int iPropID, int& iShowPropID)
{
	map<int, int>::iterator it = m_notifyPropList.find(iPropID);
	if(it == m_notifyPropList.end() )
	{
		return false;
	}

	iShowPropID = it->second;

	return true;
}


bool EffectFactory::loadNotifyProp()
{
	IEntityFactory* pEntityFactory = getComponent<IEntityFactory>(COMPNAME_EntityFactory, IID_IEntityFactory);
	assert(pEntityFactory);

	ITable* pTable = getCompomentObjectManager()->findTable("SkillNotifyProp");
	assert(pTable);

	int iRecordCount = pTable->getRecordCount();

	for(int i = 0; i < iRecordCount; i++)
	{
		string strPropName = pTable->getString(i, "属性");
		int iPropID = pEntityFactory->getPropDefine("EntityPropDef")->getPropKey(strPropName);
		assert(iPropID >= 0);

		string strShowPropName = pTable->getString(i, "通知属性");
		int iShowPropID = pEntityFactory->getPropDefine("EntityPropDef")->getPropKey(strShowPropName);
		assert(iShowPropID >= 0);

		m_notifyPropList[iPropID] = iShowPropID;
	}

	return true;
}


bool EffectFactory::initlize(const PropertySet& /*propSet*/)
{
	preRegister();
	loadEffect();
	loadNotifyProp();

	return true;
}

void EffectFactory::preRegister()
{	
	registerEffect(new SkillEffect_AddProperty);
	registerEffect(new SkillEffect_Damage);
	registerEffect(new SkillEffect_AddBuff);
	registerEffect(new SkillEffect_RandDelBuff);
	registerEffect(new SkillEffect_CondDead);
	registerEffect(new SkillEffect_ReboundDamage);
	registerEffect(new SkillEffect_ContinueKill);
	registerEffect(new SkillEffect_Relive);
	registerEffect(new SkillEffect_PropEnhance);
	registerEffect(new SkillEffect_BuffEnhance);
	registerEffect(new SkillEffect_ActorDamageAndHeal);

	registerEffect(new SkillEffect_CondEffect);
	registerEffect(new SkillEffect_ImMunityBuff);

	registerEffect(new SkillEffect_TouchDeadEffect);

	registerEffect(new SkillEffect_PropEnhanceByHP);
	registerEffect(new SkillEffect_QiXing);
	registerEffect(new SkillEffect_ZiBao);
	
	//registerEffect(new SkillEffect_AddProperty);
	/*registerEffect(new SkillEffect_Damage);
	registerEffect(new SkillEffect_AddBuff);
	registerEffect(new SkillEffect_OpenDungeonPage);
	registerEffect(new SkillEffect_TriggerGather);
	registerEffect(new SkillEffect_TriggerKill);
	registerEffect(new SkillEffect_FinPakour);
	registerEffect(new SkillEffect_Heal);
	registerEffect(new SkillEffect_Batter);
	registerEffect(new SkillEffect_LearnSkill);
	registerEffect(new SkillEffect_AddPet);
	registerEffect(new SkillEffect_DelBuff);
	registerEffect(new SkillEffect_ImMunityBuff);
	registerEffect(new SkillEffect_LearnFormation);
	registerEffect(new SkillEffect_OpenChest);
	registerEffect(new SkillEffect_AddExp);
	registerEffect(new SkillEffect_RicherBuffMsg);
    registerEffect(new SkillEffect_ActiveMagicGhost);
	registerEffect(new SkillEffect_SwitchTeamFightScene);
	registerEffect(new SkillEffect_CombineItem);
	registerEffect(new SkillEffect_PetSoulCombine);*/
}

bool EffectFactory::loadEffect()
{
	ISkillFactory* pSkillFactory = getComponent<ISkillFactory>("SkillFactory", IID_ISkillFactory);
	assert(pSkillFactory);

	ITable* pEffectTb = getCompomentObjectManager()->findTable("Effect");
	assert(pEffectTb);

	int iRecordCount = pEffectTb->getRecordCount();
	for(int i = 0; i < iRecordCount; i++)
	{
		int iEffectID = pEffectTb->getInt(i, "效果ID");
		assert(m_mapEffectContext.find(iEffectID) == m_mapEffectContext.end() );

		int iPreTarget = pEffectTb->getInt(i, "统一目标", 0);		
		for(int iEffectIndex = 0; iEffectIndex < 5; iEffectIndex++)
		{
			stringstream ss;
			ss<<"效果"<<(iEffectIndex+1);
			string strKey = ss.str();
			string strEffectCmd = pEffectTb->getString(i, strKey);
			if(strEffectCmd.size() == 0)
			{
				continue;
			}
			
			EffectContext tmpContext;
			bool bParseResult = parseEffect(strEffectCmd, tmpContext);
			assert(bParseResult);

			tmpContext.setInt(PROP_EFFECT_USEPRETARGET, iPreTarget);
			m_mapEffectContext[iEffectID].push_back(tmpContext);
		}
	}

	return true;
}

bool EffectFactory::parseEffect(const string& strEffectCmd, EffectContext& context)
{
	vector<string> paramList = TC_Common::sepstr<string>(strEffectCmd, Effect_MagicSep);
	if(paramList.size() == 0)
	{
		return false;
	}
	
	string strTmpCmdV = paramList[0];

	MapNameEffect::iterator it = m_mapNameEffect.find(strTmpCmdV);
	if(it == m_mapNameEffect.end() )
	{
		return false;
	}

	ISkillEffect* pSkillEffect = it->second;
	assert(pSkillEffect);
	
	bool bResult = pSkillEffect->parseEffect(strEffectCmd, context);
	
	return bResult;
}

bool EffectFactory::doSingleEffect(HEntity hEntity, const EffectContext& context)
{
	int iEffectType = context.getInt(PROP_EFFECT_TYPE, (Int32)0);
	ISkillEffect* pSkillEffect = getEffect(iEffectType);
	if(!pSkillEffect)
	{
		return false;
	}

	bool bResult = pSkillEffect->doEffect(hEntity, context);

	return bResult;
}

bool EffectFactory::isImMunityBuff(int iBuffID, int iEffectID, const EffectContext& effectContext)
{
	MapEffectContext::iterator it2 = m_mapEffectContext.find(iEffectID);
	if(it2 == m_mapEffectContext.end() )
	{
		return false;
	}

	const vector<EffectContext>& effectList = it2->second;
	for(size_t i = 0; i < effectList.size(); i++)
	{
		EffectContext tmpContext = effectList[i];
		tmpContext += effectContext;

		int iEffectType = tmpContext.getInt(PROP_EFFECT_TYPE, (Int32)0);
		ISkillEffect* pSkillEffect = getEffect(iEffectType);
		if(!pSkillEffect)
		{
			continue;
		}

		if(pSkillEffect->isImMunityBuff(iBuffID, tmpContext) )
		{
			return true;
		}
	}

	return false;
}

bool EffectFactory::doEffect(HEntity hEntity, int iEffectID, const EffectContext& effectContext)
{
	PROFILE_MONITOR("EffectFactory::doEffect_ID");
	MapEffectContext::iterator it = m_mapEffectContext.find(iEffectID);
	if(it == m_mapEffectContext.end() )
	{
		return false;
	}

	bool bFinalResult = true;
	vector<EffectContext>& effectList = it->second;

	vector<HEntity> preTargetList;
	for(size_t i = 0; i < effectList.size(); i++)
	{
		EffectContext tmpContext = effectList[i];
		tmpContext += effectContext;

		tmpContext.setInt64(PROP_EFFECT_PRETARGET, (Int64)&preTargetList);
		
		bool bResult = doSingleEffect(hEntity, tmpContext);
		if(!bResult)
		{
			bFinalResult = false;
			SvrErrLog("Run Effect Fail%d %d", iEffectID, (int)i);
			continue;
		}
	}
	
	return bFinalResult;
}

ISkillEffect* EffectFactory::getEffect(Int32 nEffectType)
{
	MapEffect::iterator it = m_mapEffect.find(nEffectType);
	if(it == m_mapEffect.end() )
	{
		return NULL;
	}

	return it->second;
}

bool EffectFactory::registerEffect(ISkillEffect* pSkillEffect)
{
	assert(pSkillEffect);
	Int32 nEffectType = pSkillEffect->getEffectType();
	pair<MapEffect::iterator,bool> result = m_mapEffect.insert(std::make_pair(nEffectType, pSkillEffect) );
	bool bRegisterResult = result.second;
	assert(bRegisterResult);

	vector<string> strTypeStingList = pSkillEffect->getTypeString();
	for(size_t i = 0; i < strTypeStingList.size(); i++)
	{
		string strTypeString = strTypeStingList[i];
		pair<MapNameEffect::iterator, bool> nameResult = m_mapNameEffect.insert(std::make_pair(strTypeString, pSkillEffect) );
		if(!nameResult.second)
		{
			SvrErrLog("Conflict EffectTypeString %s", strTypeString.c_str() );
			assert(false);
		}
	}

	return bRegisterResult;
}


void onSkillEffectChangeHPProcess(HEntity hGiver, HEntity hEntity, int iChgValue, const EffectContext& ctx)
{
	SkillEffect_Helper::onChangeHPProcess(hGiver, hEntity, iChgValue, ctx);
}


