#include "BuffServerPch.h"
#include "BuffFactory.h"
#include "EntityBuff.h"
#include "Condition/BuffCondition_Base.h"
/*
#include "Condition/BuffCondition_MoveGrid.h"
#include "Condition/BuffCondition_LeaveScene.h"
#include "Condition/BuffCondition_SkillusePost.h"
#include "Condition/BuffCondition_BrokenShield.h"
#include "Condition/BuffCondition_Dead.h"
#include "Condition/BuffCondition_FatalDamage.h"
#include "Condition/BuffCondition_TryAddBuff.h"
#include "Condition/BuffCondition_FightBegin.h"*/


extern "C" IComponent* createBuffFactory(Int32)
{
	return new BuffFactory;
}


BuffFactory::BuffFactory()
{
}

BuffFactory::~BuffFactory()
{
	for(BuffPrototypeMap::iterator it = m_buffPrototypeMap.begin(); it != m_buffPrototypeMap.end(); it++)
	{
		delete it->second;
	}
	m_buffPrototypeMap.clear();
}

bool BuffFactory::initlize(const PropertySet& propSet)
{	
	bool bResult = loadFightBuff();
	assert(bResult);
	
	return true;
}

bool BuffFactory::loadFightBuff()
{
	ITable* pTable = getCompomentObjectManager()->findTable("Buff");
	assert(pTable);

	int iRecordCount = pTable->getRecordCount();
	for(int i = 0; i < iRecordCount; i++)
	{
		EntityBuff* pNewBuff = new EntityBuff;
		pNewBuff->m_iBuffID = pTable->getInt(i, "状态ID");
		pNewBuff->m_strBuffName = pTable->getString(i, "状态名称");
		pNewBuff->m_iBuffGroupID = pTable->getInt(i, "状态系列");
		pNewBuff->m_iBuffType = pTable->getInt(i, "状态类型");
		pNewBuff->m_dwLifeRound = pTable->getInt(i, "持续回合");
		pNewBuff->m_bPostAttackCount = pTable->getInt(i, "是否行动后计数") == 1;
		pNewBuff->m_iMaxOverlapCount = pTable->getInt(i, "最大叠加数");
		pNewBuff->m_iRoundInteval = pTable->getInt(i, "间隔生效回合");

		string strEffect = pTable->getString(i, "特效资源");
		pNewBuff->m_bShowClient = (strEffect.size() > 0);

		for(int iEffectIndex = 0; iEffectIndex < 4; iEffectIndex++)
		{
			stringstream ss;
			ss<<"不可撤销初始效果"<<(iEffectIndex+1);
			string strKey = ss.str();
			int iTmpEffectID = pTable->getInt(i, strKey);
			if(0 == iTmpEffectID)
			{
				continue;
			}

			pNewBuff->m_initEffectList.push_back(iTmpEffectID);
		}

		for(int iEffectIndex = 0; iEffectIndex < 4; iEffectIndex++)
		{
			stringstream ss;
			ss<<"普通效果"<<(iEffectIndex+1);
			string strKey = ss.str();
			int iTmpEffectID = pTable->getInt(i, strKey);
			if(0 == iTmpEffectID)
			{
				continue;
			}

			pNewBuff->m_normalEffectList.push_back(iTmpEffectID);
		}

		for(int iEffectIndex = 0; iEffectIndex < 4; iEffectIndex++)
		{
			stringstream ss;
			ss<<"间隔效果"<<(iEffectIndex+1);
			string strKey = ss.str();
			int iEffectID = pTable->getInt(i, strKey);
			if(0 == iEffectID)
			{
				continue;
			}

			pNewBuff->m_roundEffectList.push_back(iEffectID);
		}
		
		// 特殊条件
		for(int iCondindex = 0; iCondindex < 4; iCondindex++)
		{
			stringstream ss;
			ss<<"特殊条件"<<(iCondindex+1);
			string strKey = ss.str();

			string strSpecialCond = pTable->getString(i, strKey);
			BuffCondition_Base* pCond = parseCondition(strSpecialCond, true);
			if(pCond)
			{
				pNewBuff->m_specialConditionList.push_back(pCond);
			}
		}

		pNewBuff->m_iSpecialEffectID = pTable->getInt(i, "特殊效果");

		std::pair<BuffPrototypeMap::iterator, bool> ret = m_buffPrototypeMap.insert(std::make_pair(pNewBuff->m_iBuffID, pNewBuff) );
		assert(ret.second);
	}

	return true;
}


BuffCondition_Base* BuffFactory::parseCondition(const std::string& strCmd, bool bSpecialCondition)
{
	if(strCmd.size() == 0)
	{
		return NULL;
	}

	vector<string> paramList = TC_Common::sepstr<string>(strCmd, Effect_MagicSep);
	if(paramList.size() == 0)
	{
		return NULL;
	}

	return NULL;
}


const IEntityBuff* BuffFactory::getBuffPrototype(Int32 iBuffID)
{
	BuffPrototypeMap::iterator it = m_buffPrototypeMap.find(iBuffID);
	if(it == m_buffPrototypeMap.end() )
	{
		return NULL;
	}

	return it->second;
}


