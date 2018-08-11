#include "FightSoulSystemPch.h"
#include "FightSoul.h"
#include "FightSoulFactory.h"
#include "FightSoulProperty.h"

static HandleManager<IFightSoul*> g_fightSoulHandleMgr;

static map<int, int> s_map2EntityProperty;
static map<int, int> s_map2FinalEntityProperty;

extern int Prop2LifeAtt(int iPropID);

IFightSoul* getFightSoulFromHandle(HFightSoul hFightSoul)
{
	IFightSoul* pPFightSoul = g_fightSoulHandleMgr.getDataPoint(hFightSoul);	
	return pPFightSoul;
}


string makeQualityString(int iQuality, const string& strMsg)
{
	const static string strPre = "<lb cl=\"";
	const static string strEnd = "\"/>";

	static string szColorString[] = {
	"",
	"255 255 255",
	"12 255 0",
	"0 198 255",
	"240 0 255",
	"255 108 0",
	};

	assert( (iQuality >= GSProto::en_Quality_Begin) && (iQuality < GSProto::en_Quality_End) );
	
	string strResult = strPre + szColorString[iQuality] + "\"" + "c=\"" + strMsg + strEnd;

	return strResult;
}


FightSoul::FightSoul():m_iFightSoulID(0), m_iExp(0), m_bLocked(false)
{
	IFightSoul** ppFightSoul = g_fightSoulHandleMgr.Aquire(m_hHandle);
	*ppFightSoul = this;

	if(s_map2EntityProperty.size() == 0)
	{
		s_map2EntityProperty[PROP_FIGHTSOUL_ATT] = PROP_ENTITY_ATTAVALUE;
		s_map2EntityProperty[PROP_FIGHTSOUL_MAXHP] = PROP_ENTITY_MAXHPAVALUE;
		s_map2EntityProperty[PROP_FIGHTSOUL_HIT] = PROP_ENTITY_HITAVALUE;
		s_map2EntityProperty[PROP_FIGHTSOUL_DOGE] = PROP_ENTITY_DOGEAVALUE;
		s_map2EntityProperty[PROP_FIGHTSOUL_BLOCK] = PROP_ENTITY_BLOCKAVALUE;
		s_map2EntityProperty[PROP_FIGHTSOUL_WRECK] = PROP_ENTITY_WRECKAVALUE;

		s_map2EntityProperty[PROP_FIGHTSOUL_KNOCK] = PROP_ENTITY_KNOCKAVALUE;
		s_map2EntityProperty[PROP_FIGHTSOUL_ANTIKNOCK] = PROP_ENTITY_ANTIKNOCKAVALUE;
		s_map2EntityProperty[PROP_FIGHTSOUL_ARMOR] = PROP_ENTITY_ARMORAVALUE;

		s_map2EntityProperty[PROP_FIGHTSOUL_SUNDER] = PROP_ENTITY_SUNDERAVALUE;
		s_map2EntityProperty[PROP_FIGHTSOUL_ANGER] = PROP_ENTITY_INITANGERAVALUE;


		s_map2FinalEntityProperty[PROP_FIGHTSOUL_ATT] = PROP_ENTITY_ATT;
		s_map2FinalEntityProperty[PROP_FIGHTSOUL_MAXHP] = PROP_ENTITY_MAXHP;
		s_map2FinalEntityProperty[PROP_FIGHTSOUL_HIT] = PROP_ENTITY_HIT;
		s_map2FinalEntityProperty[PROP_FIGHTSOUL_DOGE] = PROP_ENTITY_DOGE;
		s_map2FinalEntityProperty[PROP_FIGHTSOUL_BLOCK] = PROP_ENTITY_BLOCK;
		s_map2FinalEntityProperty[PROP_FIGHTSOUL_WRECK] = PROP_ENTITY_WRECK;

		s_map2FinalEntityProperty[PROP_FIGHTSOUL_KNOCK] = PROP_ENTITY_KNOCK;
		s_map2FinalEntityProperty[PROP_FIGHTSOUL_ANTIKNOCK] = PROP_ENTITY_ANTIKNOCK;
		s_map2FinalEntityProperty[PROP_FIGHTSOUL_ARMOR] = PROP_ENTITY_ARMOR;

		s_map2FinalEntityProperty[PROP_FIGHTSOUL_SUNDER] = PROP_ENTITY_SUNDER;
		s_map2FinalEntityProperty[PROP_FIGHTSOUL_ANGER] = PROP_ENTITY_INITANGER;
	}
}

FightSoul::~FightSoul()
{
	g_fightSoulHandleMgr.Release(m_hHandle);
}

int FightSoul::getID()
{
	return m_iFightSoulID;
}


int FightSoul::getQuality()
{
	FightSoulFactory* pFightSoulFactory = (FightSoulFactory*)(getComponent<IFightSoulFactory>("FightSoulFactory", IID_IFightSoulFactory) );
	assert(pFightSoulFactory);

	const FightSoulData* pFightSoulData = pFightSoulFactory->queryFightSoulData(m_iFightSoulID);
	assert(pFightSoulData);

	return pFightSoulData->iQuality;
}

int FightSoul::getType()
{
	FightSoulFactory* pFightSoulFactory = (FightSoulFactory*)(getComponent<IFightSoulFactory>("FightSoulFactory", IID_IFightSoulFactory) );
	assert(pFightSoulFactory);

	const FightSoulData* pFightSoulData = pFightSoulFactory->queryFightSoulData(m_iFightSoulID);
	assert(pFightSoulData);

	return pFightSoulData->iTypeID;
}

int FightSoul::getCombineExp()
{
	FightSoulFactory* pFightSoulFactory = static_cast<FightSoulFactory*>(getComponent<IFightSoulFactory>("FightSoulFactory", IID_IFightSoulFactory) );
	assert(pFightSoulFactory);

	const FightSoulData* pCfgData = pFightSoulFactory->queryFightSoulData(m_iFightSoulID);
	assert(pCfgData);
	

	return getExp() + pCfgData->iInitExp;
}



const map<int, int>& FightSoul::getEffectMap()
{	
	 IFightSoulFactory* pFightSoulFactory = getComponent<IFightSoulFactory>("FightSoulFactory", IID_IFightSoulFactory);
	 assert(pFightSoulFactory);

	 const map<int, int>* pPropMap = pFightSoulFactory->getEffectMap(m_iFightSoulID, getLevel() );
	 assert(pPropMap);

	 return *pPropMap;
}

int FightSoul::getExp()
{
	return m_iExp;
}

int FightSoul::getLevel()
{
	FightSoulFactory* pFightSoulFactory = (FightSoulFactory*)(getComponent<IFightSoulFactory>("FightSoulFactory", IID_IFightSoulFactory) );
	assert(pFightSoulFactory);

	int iResultLevel = pFightSoulFactory->calcLevel(m_iFightSoulID, m_iExp);
	
	return iResultLevel;
}

void FightSoul::addExp(int iAddExp)
{
	m_iExp += iAddExp;

	// 判断是否超过最大经验，如果超过，不获得经验
	FightSoulFactory* pFightSoulFactory = (FightSoulFactory*)(getComponent<IFightSoulFactory>("FightSoulFactory", IID_IFightSoulFactory) );
	assert(pFightSoulFactory);

	int iQuality = getQuality();
	int iMaxExp = pFightSoulFactory->getMaxExpByQuality(iQuality);
	if(m_iExp > iMaxExp)
	{
		m_iExp = iMaxExp;
	}
}

bool FightSoul::isTochMax()
{
	FightSoulFactory* pFightSoulFactory = (FightSoulFactory*)(getComponent<IFightSoulFactory>("FightSoulFactory", IID_IFightSoulFactory) );
	assert(pFightSoulFactory);

	int iQuality = getQuality();
	int iMaxExp = pFightSoulFactory->getMaxExpByQuality(iQuality);


	return m_iExp >= iMaxExp;
}


Uint32 FightSoul::getHandle()
{
	return m_hHandle;
}

void FightSoul::fillScData(int iPos, GSProto::FightSoulItem& scItem)
{
	FightSoulFactory* pFightSoulFactory = (FightSoulFactory*)(getComponent<IFightSoulFactory>("FightSoulFactory", IID_IFightSoulFactory) );
	assert(pFightSoulFactory);

	scItem.set_dwobjectid(m_hHandle);
	scItem.set_ibaseid(m_iFightSoulID);
	scItem.set_iexp(m_iExp);
	scItem.set_ipos(iPos);
	scItem.set_blocked(m_bLocked);
	scItem.set_ilevel(getLevel() );

	int iNextLvNeedExp = pFightSoulFactory->getLevelNeedExp(m_iFightSoulID, getLevel() + 1);
	scItem.set_inextlvexp(iNextLvNeedExp);

	int iLevel = getLevel();
	const map<int, int>* pEffectMap = pFightSoulFactory->getEffectMap(m_iFightSoulID, iLevel);
	assert(pEffectMap);

	for(map<int, int>::const_iterator it = pEffectMap->begin(); it != pEffectMap->end(); it++)
	{
		int iTmpPropID = it->first;
		assert(s_map2FinalEntityProperty.find(iTmpPropID) != s_map2FinalEntityProperty.end() );
		
		int iEntityPropID = s_map2FinalEntityProperty[iTmpPropID];
		int iValue = it->second;

		int iLifeAttID = Prop2LifeAtt(iEntityPropID);
		assert(iLifeAttID >= 0);

		GSProto::PropItem* pNewPropItem = scItem.mutable_szproplist()->Add();
		assert(pNewPropItem);

		pNewPropItem->set_ilifeattid(iLifeAttID);
		pNewPropItem->set_ivalue(iValue);
	}

	int iEatExp = getCombineExp() - getExp();
	scItem.set_ieatexp(iEatExp);
}


void FightSoul::changeProperty(HEntity hHero, bool bOn)
{
	IEntity* pHero = getEntityFromHandle(hHero);
	assert(pHero);

	FightSoulFactory* pFightSoulFactory = (FightSoulFactory*)(getComponent<IFightSoulFactory>("FightSoulFactory", IID_IFightSoulFactory) );
	assert(pFightSoulFactory);

	int iLevel = getLevel();
	const map<int, int>* pEffectMap = pFightSoulFactory->getEffectMap(m_iFightSoulID, iLevel);
	assert(pEffectMap);

	for(map<int, int>::const_iterator it = pEffectMap->begin(); it != pEffectMap->end(); it++)
	{
		int iTmpPropID = it->first;
		assert(s_map2EntityProperty.find(iTmpPropID) != s_map2EntityProperty.end() );

		int iEntityPropID = s_map2EntityProperty[iTmpPropID];
		int iValue = it->second;

		if(bOn)
		{
			pHero->changeProperty(iEntityPropID, iValue, 0);
		}
		else
		{
			pHero->changeProperty(iEntityPropID, -iValue, 0);
		}
	}
}

bool FightSoul::isLocked()
{
	return m_bLocked;
}

void FightSoul::setLock(bool bLocked)
{
	m_bLocked = bLocked;
}

string FightSoul::getName()
{
	FightSoulFactory* pFightSoulFactory = (FightSoulFactory*)(getComponent<IFightSoulFactory>("FightSoulFactory", IID_IFightSoulFactory) );
	assert(pFightSoulFactory);

	const FightSoulData* pFightSoulData = pFightSoulFactory->queryFightSoulData(m_iFightSoulID);
	assert(pFightSoulData);


	return makeQualityString(getQuality(), pFightSoulData->strName);
}




