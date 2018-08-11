#include "EffectServerPch.h"
#include "EffectSystem.h"
#include "EffectFactory.h"

extern "C" IObject* createEffectSystem()
{
	return new EffectSystem;
}


EffectSystem::EffectSystem()
{
}

EffectSystem::~EffectSystem()
{
}

bool EffectSystem::addEffect(EffectContext& effect)
{
	IEffectFactory* pEffectFactoryInerface = getComponent<IEffectFactory>("EffectFactory", IID_IEffectFactory);
	assert(pEffectFactoryInerface);

	EffectFactory* pEffectFacObj = static_cast<EffectFactory*>(pEffectFactoryInerface);
	assert(pEffectFacObj);


	bool bResult = pEffectFacObj->doSingleEffect(m_hMaster, effect);

	return bResult;
}

bool EffectSystem::addEffect(int iEffectID, const EffectContext& context)
{
	IEffectFactory* pEffectFactoryInerface = getComponent<IEffectFactory>("EffectFactory", IID_IEffectFactory);
	assert(pEffectFactoryInerface);

	bool bResult = pEffectFactoryInerface->doEffect(m_hMaster, iEffectID, context);

	return bResult;
}


Uint32 EffectSystem::getSubsystemID() const
{
	return IID_IEffectSystem;
}

Uint32 EffectSystem::getMasterHandle()
{
	return m_hMaster;
}

bool EffectSystem::create(IEntity* pEntity, const std::string& strData)
{
	m_hMaster = pEntity->getHandle();
	
	return true;
}

bool EffectSystem::createComplete()
{
	return true;
}

const std::vector<Uint32>& EffectSystem::getSupportMessage()
{
	static std::vector<Uint32> resultList;

	return resultList;
}


void EffectSystem::addPropEnhance(int iSkillID, int iPropID, int iValue)
{
	Int64 key = (((Int64)iSkillID)<<16)|iPropID;
	m_propEnhanceMap[key] += iValue;
}

void EffectSystem::subPropEnhance(int iSkillID, int iPropID, int iValue)
{
	Int64 key = (((Int64)iSkillID)<<16)|iPropID;
	m_propEnhanceMap[key] -= iValue;	
}

int EffectSystem::getPropEnhance(int iSkillID, int iPropID)
{
	Int64 key = (((Int64)iSkillID)<<16)|iPropID;
	map<Int64, int>::iterator it = m_propEnhanceMap.find(key);
	if(it != m_propEnhanceMap.end() )
	{
		int iValue = it->second;
		return iValue;
	}

	return 0;
}

void EffectSystem::addPropPercentEnhance(int iSkillID, int iPropID, int iValue)
{
	Int64 key = (((Int64)iSkillID)<<16)|iPropID;
	m_propEnhancePercentMap[key] += iValue;
}

void EffectSystem::subPropPercentEnhance(int iSkillID, int iPropID, int iValue)
{
	Int64 key = (((Int64)iSkillID)<<16)|iPropID;
	m_propEnhancePercentMap[key] -= iValue;
}

int EffectSystem::getPropPercentEnhance(int iSkillID, int iPropID)
{
	Int64 key = (((Int64)iSkillID)<<16)|iPropID;
	map<Int64, int>::iterator it = m_propEnhancePercentMap.find(key);
	if(it != m_propEnhancePercentMap.end() )
	{
		int iValue = it->second;
		return iValue;
	}

	return 0;
}


void EffectSystem::addBuffChance(int iSkillID, int iBuffID, int iValue)
{
	Int64 key = (((Int64)iSkillID)<<16)|iBuffID;

	m_buffChanceEnhanceMap[key] += iValue;
}

void EffectSystem::subBuffChance(int iSkillID, int iBuffID, int iValue)
{
	Int64 key = (((Int64)iSkillID)<<16)|iBuffID;

	m_buffChanceEnhanceMap[key] -= iValue;
}

int EffectSystem::getBuffEnhance(int iSkillID, int iBuffID)
{
	Int64 key = (((Int64)iSkillID)<<16)|iBuffID;

	map<Int64, int>::iterator it = m_buffChanceEnhanceMap.find(key);
	if(it != m_buffChanceEnhanceMap.end() )
	{
		int iValue = it->second;

		return iValue;
	}

	return 0;
}





