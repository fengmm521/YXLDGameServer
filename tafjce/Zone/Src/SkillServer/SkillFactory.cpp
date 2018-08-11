#include "SkillServerPch.h"
#include <cmath>
#include <limits>

#include "SkillFactory.h"
#include "Skill.h"

#include "TargetSelect/TargetSelect_Base.h"
#include "TargetSelect/TargetSelect_Normal.h"
#include "TargetSelect/TargetSelect_SelfAll.h"
#include "TargetSelect/TargetSelect_TargetAll.h"
#include "TargetSelect/TargetSelect_TargetRow.h"
#include "TargetSelect/TargetSelect_TargetCol.h"

#include "TargetSelect/TargetSelect_LastTarget.h"
#include "TargetSelect/TargetSelect_LastTargetRow.h"
#include "TargetSelect/TargetSelect_RandTarget.h"
#include "TargetSelect/TargetSelect_Self.h"
#include "TargetSelect/TargetSelect_Life.h"
#include "TargetSelect/TargetSelect_Around.h"
#include "TargetSelect/TargetSelect_Female.h"


#include "SkillResult/SkillResult_Base.h"
#include "SkillResult/SkillResult_Effect.h"


#include "SkillCondition/SkillCondition_Base.h"

//#include "SkillCondition/SkillCondition_HPNotFull.h"
//#include "SkillCondition/SkillCondition_TargetCountOver.h"

extern "C" IComponent* createSkillFactory(Int32)
{
	return new SkillFactory;
}



SkillFactory::SkillFactory()
{
}

SkillFactory::~SkillFactory()
{
}

bool SkillFactory::initlize(const PropertySet& propSet)
{
	loadTargetSelectStrategy();
	if(!loadSkill())
	{
		assert(false);
        TERRLOG << "loadSkill fail" << endl;
        return false;
	}
	
	return true;
}

bool SkillFactory::loadSkill()
{
	ITable* pTable = getCompomentObjectManager()->findTable("Skill");
	assert(pTable);

	int nRecordCount = pTable->getRecordCount();
	for(int i = 0; i < nRecordCount; i++)
	{
		Skill* pNewSkill = new Skill;
		pNewSkill->m_nSkillID = pTable->getInt(i, "技能ID");
		pNewSkill->m_iActiveLevelStep = pTable->getInt(i, "开启等阶");
		pNewSkill->m_iActiveQuality = pTable->getInt(i, "开启品质");
		
		pNewSkill->m_nSkillType = pTable->getInt(i, "技能类型");
		
		pNewSkill->m_iBatterAttack = pTable->getInt(i, "连击技能");
		pNewSkill->m_iSkillGroupID   = pTable->getInt(i, "技能系列");
		pNewSkill->m_nSkillLv = pTable->getInt(i, "技能等级");
		pNewSkill->m_iMaxLvl = pTable->getInt(i, "技能最高等级");
		
		pNewSkill->m_bCanBackAttack = pTable->getInt(i, "可被反击") != 0;
		pNewSkill->m_iSmallSkillChance = pTable->getInt(i, "小技能概率");
		
		string strSpecialCond = pTable->getString(i, "特殊条件");
		if(strSpecialCond.size() > 0)
		{
			SkillCondition_Base* pCondition = parseSkillCondition(strSpecialCond);
			assert(pCondition);

			pNewSkill->m_conditionList.push_back(pCondition);
		}
		
		// 加载通用技能效果
		for(int iEffectIndex = 0; iEffectIndex < 4; iEffectIndex++)
		{
			stringstream ss;
			ss<<"技能效果"<<(iEffectIndex+1);
			string strKey = ss.str();
			int iEffectID = pTable->getInt(i, strKey);
			if(0 == iEffectID) continue;

			pNewSkill->m_effectList.push_back(iEffectID);
		}
		
		pNewSkill->m_iNeedAnger = pTable->getInt(i, "怒气条件");
		pNewSkill->m_iConsumeAnger = pTable->getInt(i, "消耗怒气");
		
		if(pNewSkill->m_iSkillGroupID > 0)
        {
        	m_skillGroup[pNewSkill->m_iSkillGroupID][pNewSkill->m_nSkillLv] = pNewSkill;
		}

		// 加载特殊效果(待扩展)
		m_skillProtoMap[pNewSkill->m_nSkillID] = pNewSkill;
	}

    //检查技能系列配置正确性
    for(map<int,map<int,ISkill*> >::iterator it = m_skillGroup.begin();it != m_skillGroup.end();++it)
    {
        map<int,ISkill*> &mGroupSkill = it->second;
        if(mGroupSkill.size() > 0)
        {
            ISkill* pSkill = mGroupSkill.begin()->second;
            int iMaxLvl   = pSkill->getMaxLevel();
            for(int i = 1 ;i < iMaxLvl;++i)
            {
                if(mGroupSkill.end() == mGroupSkill.find(i))
                {
                    TERRLOG << "Skill Conf error|skillseqID:" << it->first 
                        << "|skilllvl:" << i 
                        << "|maxlvl:" << iMaxLvl << endl;
                    return false;
                }
            }
        }
    }
    
	return true;
}

bool SkillFactory::loadTargetSelectStrategy()
{
	m_targetSelectList[en_SkillTargetSelect_Normal] = new SkillTargetSelect_Normal;
	m_targetSelectList[en_SkillTargetSelect_SelfAll] = new SkillTargetSelect_SelfAll;
	m_targetSelectList[en_SkillTargetSelect_TargetAll] = new SkillTargetSelect_TargetAll;

	m_targetSelectList[en_SkillTargetSelect_TargetCol] = new SkillTargetSelect_TargetCol;
	m_targetSelectList[en_SkillTargetSelect_TargetRow] = new SkillTargetSelect_TargetRow;

	m_targetSelectList[en_SkillTargetSelect_TargetLast] = new SkillTargetSelect_LastTarget;
	m_targetSelectList[en_SkillTargetSelect_TargetLastRow] = new SkillTargetSelect_LastTargetRow;

	m_targetSelectList[en_SkillTargetSelect_TargetRand] = new SkillTargetSelect_RandTarget;
	m_targetSelectList[en_SkillTargetSelect_Self] = new SkillTargetSelect_Self;

	m_targetSelectList[en_SkillTargetSelect_TargetAndAround] = new SkillTargetSelect_ARound(en_SkillTargetSelect_Normal);
	m_targetSelectList[en_SkillTargetSelect_RandAndAround] = new SkillTargetSelect_ARound(en_SkillTargetSelect_TargetRand);
	m_targetSelectList[en_SkillTargetSelect_MinLife] = new SkillTargetSelect_Life(false, false);
	m_targetSelectList[en_SkillTargetSelect_MaxLife] = new SkillTargetSelect_Life(false, true);

	m_targetSelectList[en_SkillTargetSelect_Female] = new SkillTargetSelect_Female();
	
	return true;
}



SkillTargetSelect_Base* SkillFactory::getTargetSelect(int nSelectStrategy)
{
	if(m_targetSelectList.find(nSelectStrategy) == m_targetSelectList.end() )
	{
		return NULL;
	}

	return m_targetSelectList[nSelectStrategy];
}

int SkillFactory::getSkillID(int iSkillGrpID, int iLevel)
{
	map<int,map<int,ISkill*> >::iterator it = m_skillGroup.find(iSkillGrpID);
	if(it == m_skillGroup.end() )
	{
		return 0;
	}

	const map<int,ISkill*>& skillList = it->second;
	for(map<int,ISkill*>::const_iterator skillIt = skillList.begin(); skillIt != skillList.end(); skillIt++)
	{
		const ISkill* pSkill = skillIt->second;
		assert(pSkill);

		if(pSkill->getSkillLevel() == iLevel)
		{
			return pSkill->getSkillID();
		}
	}

	return 0;
}

SkillCondition_Base* SkillFactory::parseSkillCondition(const std::string& strCondition)
{
	vector<string> paramList = TC_Common::sepstr<string>(strCondition, Effect_MagicSep);
	if(paramList.size() == 0)
	{
		return NULL;
	}

	/*if( (paramList[0] == "目标伤血") && (2 == paramList.size() ) )
	{
		int iTargetSelect = AdvanceAtoi(paramList[1]);
		return new SkillCondition_HPNotFull(iTargetSelect);
	}
	else if( (paramList[0] == "目标人数大于") && (paramList.size() == 3) )
	{
		int iTargetSelect = AdvanceAtoi(paramList[1]);
		int iNum = AdvanceAtoi(paramList[2]);

		return new SkillCondition_TargetCountOver(iTargetSelect, iNum);
	}*/

	return NULL;
}


const ISkill* SkillFactory::getSkillPrototype(int nSillID)
{	
	SkillMap::iterator it = m_skillProtoMap.find(nSillID);
	if(it == m_skillProtoMap.end() )
	{
		return NULL;
	}

	ISkill* pSkillPrototype = it->second;

	return pSkillPrototype;
}



int SkillFactory::getSkillType(int iSkillID)
{
	const ISkill* pSkillProp = getSkillPrototype(iSkillID);
	if(!pSkillProp)
	{
		return -1;
	}

	return pSkillProp->getSkillType();
}


int SkillFactory::calcDamageHitResult(HEntity hAttacker, HEntity hTarget, int iDamageType)
{	
	// 是否命中
	if(!calcHit(hAttacker, hTarget) )
	{
		return GSProto::en_SkillHitResult_Doge;
	}

	// 计算抵挡
	if(calcBlock(hAttacker, hTarget) )
	{
		return GSProto::en_SkillHitResult_Block;
	}

	// 计算暴击
	if(calcKnock(hAttacker, hTarget) )
	{
		return GSProto::en_SkillHitResult_Knock;
	}

	return GSProto::en_SkillHitResult_Hit;
}

int SkillFactory::rangeValue(int iMinValue, int iMaxValue, int iValue)
{
	if(iValue < iMinValue) return iMinValue;
	if(iValue > iMaxValue) return iMaxValue;

	return iValue;
}


bool SkillFactory::calcHit(HEntity hAttacker, HEntity hTarget)
{
	IEntity* pAttacker = getEntityFromHandle(hAttacker);
	IEntity* pTarget = getEntityFromHandle(hTarget);

	assert(pAttacker && pTarget);

	int iAttackerHit = 1000 + pAttacker->getProperty(PROP_ENTITY_HIT, 0);
	iAttackerHit = rangeValue(0, 5000, iAttackerHit);

	int iTargetDoge = pTarget->getProperty(PROP_ENTITY_DOGE, 0);
	iTargetDoge = rangeValue(0, 5000, iTargetDoge);
	

	int iResultValue  = iAttackerHit - iTargetDoge;
	//int iResultValue = (int)(((double)iAttackerHit/((double)iAttackerHit + iTargetDoge) + 0.27)*10000);

	iResultValue = rangeValue(200, 1000, iResultValue);
	//int iResultValue = 1000 + iAttackerHit - iTargetDoge;
	//iResultValue = rangeValue(0, 1000, iResultValue);

	IRandom* pRandom = getComponent<IRandom>("Random", IID_IMiniAprRandom);
	assert(pRandom);

	int iRandV = pRandom->random() % 1000;
	if(iRandV < iResultValue)
	{
		return true;
	}
	
	return false;
}

bool SkillFactory::calcBlock(HEntity hAttacker, HEntity hTarget)
{
	IEntity* pAttacker = getEntityFromHandle(hAttacker);
	IEntity* pTarget = getEntityFromHandle(hTarget);

	assert(pAttacker && pTarget);

	int iTargetBlock = pTarget->getProperty(PROP_ENTITY_BLOCK, 0);
	iTargetBlock = rangeValue(0, 5000, iTargetBlock);
	
	int iAttWreck = pAttacker->getProperty(PROP_ENTITY_WRECK, 0);
	iAttWreck = rangeValue(0, 5000, iAttWreck);

	int iResultV = iTargetBlock - iAttWreck;
	//int iResultV = (int)(((double)iTargetBlock/((double)iTargetBlock + iAttWreck) - 0.27)*10000);
	iResultV = rangeValue(0, 800, iResultV);
	//int iResultV = iTargetBlock - iAttWreck;
	//iResultV = rangeValue(0, 1000, iResultV);

	IRandom* pRandom = getComponent<IRandom>("Random", IID_IMiniAprRandom);
	assert(pRandom);

	int iRandV = pRandom->random() % 1000;
	if(iRandV < iResultV)
	{
		return true;
	}

	return false;
}

bool SkillFactory::calcKnock(HEntity hAttacker, HEntity hTarget)
{
	IEntity* pAttacker = getEntityFromHandle(hAttacker);
	IEntity* pTarget = getEntityFromHandle(hTarget);

	assert(pAttacker && pTarget);

	int iAttKnock = pAttacker->getProperty(PROP_ENTITY_KNOCK, 0);
	iAttKnock = rangeValue(0, 5000, iAttKnock);
	
	int iTargetAntiKnock = pTarget->getProperty(PROP_ENTITY_ANTIKNOCK, 0);
	iTargetAntiKnock = rangeValue(0, 5000, iTargetAntiKnock);

	int iResultValue = iAttKnock - iTargetAntiKnock;

	//int iResultValue = ((((double)iAttKnock - (double)iTargetAntiKnock)/((double)iAttKnock + 50))*0.7+0.05)*10000;
	iResultValue = rangeValue(0, 1000, iResultValue);
	//int iResultValue = iAttKnock - iTargetAntiKnock;
	//iResultValue = rangeValue(0, 1000, iResultValue);

	IRandom* pRandom = getComponent<IRandom>("Random", IID_IMiniAprRandom);
	assert(pRandom);

	int iRandV = pRandom->random() % 1000;
	if(iRandV < iResultValue)
	{
		return true;
	}

	return false;
}

void SkillFactory::selectTarget(HEntity hAttacker ,int iSelectStrategy, vector<HEntity>& resultList, 
	const vector<HEntity>& memberList)
{
	resultList.clear();
	SkillTargetSelect_Base* pTargetSelect = getTargetSelect(iSelectStrategy);
	if(!pTargetSelect) return;

	pTargetSelect->selectTarget(hAttacker, resultList, memberList);
}

ISkill* SkillFactory::getNextLvlSkillPrototype(ISkill* pCurSkill)
{
    assert(pCurSkill);
    map<int,map<int,ISkill*> >::iterator it = m_skillGroup.find(pCurSkill->getSkillGroupID());
    if(it == m_skillGroup.end())
    {
        return NULL;
    }

    map<int,ISkill*>::iterator it2 = it->second.find(pCurSkill->getSkillLevel() + 1);
    if(it2 == it->second.end())
    {
        return NULL;
    }
    
    return it2->second;
}




