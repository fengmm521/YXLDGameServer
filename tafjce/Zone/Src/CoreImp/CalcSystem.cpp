#include "CoreImpPch.h"
#include "CalcSystem.h"
#include <limits>
#include "IVIPFactory.h"

extern "C" IObject* createCalcSystem()
{
	return new CalcSystem;
}


static bool s_bInited = false;
static vector<PropCalcHelperBase*> s_szPropCalcArray[PROP_ENTITY_MAX];
static vector<PropCalcHelperBase*> s_initDstCalcList;

void CommonPropCalc::doPropCalc(CalcSystem* pCalcPropSystem, const EventArgs& args)
{
	HEntity hEntity = pCalcPropSystem->getMasterHandle();
	IEntity* pEntity = getEntityFromHandle(hEntity);
	assert(pEntity);

	double dBaseValue = 0;
	if(m_iBasePropID) dBaseValue = pEntity->getProperty(m_iBasePropID, 0);

	// 根据属性系数修正基础值
	if( (PROP_ENTITY_BASEMAXHP == m_iBasePropID) || (PROP_ENTITY_BASEATT == m_iBasePropID) || (PROP_ENTITY_BASEDEF == m_iBasePropID) )
	{
		int iBasePropFix = pEntity->getProperty(PROP_ENTITY_BASEPROP_FIXPARAM, 0);
		dBaseValue = dBaseValue * (double)iBasePropFix/10000;
	}

	int iAddValue = 0;
	if(m_iAddVPropID) iAddValue = pEntity->getProperty(m_iAddVPropID, 0);

	int iAddPercent = 0;
	if(m_iAddPercentPropID) iAddPercent = pEntity->getProperty(m_iAddPercentPropID, 0);

	int iGrowValue = 0;
	if(m_iGrowPropID) iGrowValue = pEntity->getProperty(m_iGrowPropID, 0);


	int iLevel = pEntity->getProperty(PROP_ENTITY_LEVEL, 0);
	double dResult = (double)(dBaseValue + (double)iLevel * (double)iGrowValue/10000)*(1.0 + (double)iAddPercent/10000) + iAddValue;
	if(dResult > (double)std::numeric_limits<int>::max() )
	{
		dResult = (double)std::numeric_limits<int>::max();
	}

	int iResultValue = (int)dResult;
	pEntity->setProperty(m_iTargetPropID, iResultValue);
}



CalcSystem::CalcSystem()
{
}

CalcSystem::~CalcSystem()
{
	IEntity* pEntity = getEntityFromHandle(m_hEntity);
	if(pEntity)
	{
		pEntity->getEventServer()->unsubscribeEvent(EVENT_ENTITY_PROPCHANGE, this, &CalcSystem::onEventPropChg);
	}
}


void CalcSystem::onEventPropChg(EventArgs& args)
{
	EventArgs_PropChange& propChgArgs = (EventArgs_PropChange&)args;
	
	vector<PropCalcHelperBase*>& calcObjList = s_szPropCalcArray[propChgArgs.iPropID];
	if(calcObjList.size() == 0)
	{
		return;
	}
	

	for(size_t i = 0; i < calcObjList.size(); i++)
	{
		calcObjList[i]->doPropCalc(this, args);
	}
}


Uint32 CalcSystem::getSubsystemID() const
{
	return IID_ICalcSystem;
}

Uint32 CalcSystem::getMasterHandle()
{
	return m_hEntity;
}

void CalcSystem::calcAllProperty()
{
	EventArgs noUse;

	for(size_t i = 0; i < s_initDstCalcList.size(); i++)
	{
		PropCalcHelperBase* pCalcObj = s_initDstCalcList[i];
		assert(pCalcObj);

		pCalcObj->doPropCalc(this, noUse);
	}
}

bool CalcSystem::create(IEntity* pEntity, const std::string& strData)
{
	if(!s_bInited)
	{
		s_bInited = true;
		bindPropInfo();
		bindFightValueCalc();
		bindVIPProp();
	}

	assert(pEntity);
	m_hEntity = pEntity->getHandle();

	// 先进行已有数据初始化
	calcAllProperty();
	// 注册事件
	pEntity->getEventServer()->subscribeEvent(EVENT_ENTITY_PROPCHANGE, this, &CalcSystem::onEventPropChg);
	
	return true;
}

bool CalcSystem::createComplete()
{
	return true;
}

const std::vector<Uint32>& CalcSystem::getSupportMessage()
{
	static std::vector<Uint32> resultList;
	return resultList;
}


void CalcSystem::bindCommPropCalc(int iTargetPropID, int iBasePropID, int iAddValuePropID, int iAddPercentPropID, int iGrowPropID)
{
	PropCalcHelperBase* pCalcObj = new CommonPropCalc(iTargetPropID, iBasePropID, iAddValuePropID, iAddPercentPropID, iGrowPropID);
	if(iBasePropID > 0)
	{
		assert(iBasePropID < PROP_ENTITY_MAX);
		s_szPropCalcArray[iBasePropID].push_back(pCalcObj);

		if( (PROP_ENTITY_BASEMAXHP == iBasePropID) || (PROP_ENTITY_BASEATT == iBasePropID) )
		{
			s_szPropCalcArray[PROP_ENTITY_BASEPROP_FIXPARAM].push_back(pCalcObj);
		}
	}

	if(iAddValuePropID > 0)
	{
		assert(iAddValuePropID < PROP_ENTITY_MAX);
		s_szPropCalcArray[iAddValuePropID].push_back(pCalcObj);
	}

	if(iAddPercentPropID > 0)
	{
		assert(iAddPercentPropID < PROP_ENTITY_MAX);
		s_szPropCalcArray[iAddPercentPropID].push_back(pCalcObj);
	}

	if(iGrowPropID > 0)
	{
		assert(iGrowPropID < PROP_ENTITY_MAX);
		s_szPropCalcArray[iGrowPropID].push_back(pCalcObj);

		// 如果有成长,需要加入等级监控
		s_szPropCalcArray[PROP_ENTITY_LEVEL].push_back(pCalcObj);
	}

	s_initDstCalcList.push_back(pCalcObj);
}


void CalcSystem::bindFightValueCalc()
{
	PropCalcHelperBase* pCalcObj = new FightValuePropCalc;
	s_szPropCalcArray[PROP_ENTITY_MAXHP].push_back(pCalcObj);
	s_szPropCalcArray[PROP_ENTITY_ATT].push_back(pCalcObj);
	s_szPropCalcArray[PROP_ENTITY_DOGE].push_back(pCalcObj);
	s_szPropCalcArray[PROP_ENTITY_HIT].push_back(pCalcObj);
	s_szPropCalcArray[PROP_ENTITY_ANTIKNOCK].push_back(pCalcObj);
	s_szPropCalcArray[PROP_ENTITY_KNOCK].push_back(pCalcObj);

	s_szPropCalcArray[PROP_ENTITY_BLOCK].push_back(pCalcObj);
	s_szPropCalcArray[PROP_ENTITY_WRECK].push_back(pCalcObj);

	s_szPropCalcArray[PROP_ENTITY_ARMOR].push_back(pCalcObj);
	s_szPropCalcArray[PROP_ENTITY_SUNDER].push_back(pCalcObj);

	
	s_szPropCalcArray[PROP_ENTITY_RELIVEENHANCE].push_back(pCalcObj);
	s_szPropCalcArray[PROP_ENTITY_SKILLDAMAGE].push_back(pCalcObj);
	s_szPropCalcArray[PROP_ENTITY_SKILLDEF].push_back(pCalcObj);

	s_szPropCalcArray[PROP_ENTITY_DEF].push_back(pCalcObj);
	s_szPropCalcArray[PROP_ENTITY_SMALLSKILLDAMAGE].push_back(pCalcObj);
	
	

	s_initDstCalcList.push_back(pCalcObj);
}


void VIPPropCalc::doPropCalc(CalcSystem* pCalcPropSystem, const EventArgs& args)
{
	HEntity hEntity = pCalcPropSystem->getMasterHandle();
	
	IVIPFactory* pVipFactory = getComponent<IVIPFactory>(COMPNAME_VIPFactory, IID_IVIPFactory);
	assert(pVipFactory);


	int iValue = pVipFactory->getVipPropByHEntity(hEntity, m_iVIPPropID);
	
	IEntity* pEntity = getEntityFromHandle(hEntity);
	assert(pEntity);

	pEntity->setProperty(m_iPropID, iValue);
}


void FightValuePropCalc::doPropCalc(CalcSystem* pCalcPropSystem, const EventArgs& args)
{
	HEntity hEntity = pCalcPropSystem->getMasterHandle();
	IEntity* pEntity = getEntityFromHandle(hEntity);
	assert(pEntity);

	int iClassID = pEntity->getProperty(PROP_ENTITY_CLASS, 0);
	if( (GSProto::en_class_Actor == iClassID) || (GSProto::en_class_Ghost == iClassID) )
	{
		return;
	}

	int iMaxHP = pEntity->getProperty(PROP_ENTITY_MAXHP, 0);
	int iAtt = pEntity->getProperty(PROP_ENTITY_ATT, 0);
	int iDoge = pEntity->getProperty(PROP_ENTITY_DOGE, 0);
	int iHit = pEntity->getProperty(PROP_ENTITY_HIT, 0);
	int iAntiKnock = pEntity->getProperty(PROP_ENTITY_ANTIKNOCK, 0);
	int iKnock = pEntity->getProperty(PROP_ENTITY_KNOCK, 0);

	int iBock = pEntity->getProperty(PROP_ENTITY_BLOCK, 0);
	int iWreck = pEntity->getProperty(PROP_ENTITY_WRECK, 0);

	int iArmor = pEntity->getProperty(PROP_ENTITY_ARMOR, 0);
	int iSunder = pEntity->getProperty(PROP_ENTITY_SUNDER, 0);

	double dEffectHP = (double)iMaxHP * (1.0 + ((double)iDoge + iAntiKnock + iBock + iArmor)/1000);//(double)iMaxHP*(1.0 + (double)iDoge/1000)*(1.0 + (double)iAntiKnock/1000)*(1.0 + (double)iBock/1000)*(1.0 + (double)iArmor/1000);
	double dEffectAtt = (double)iAtt * (1.0 + ((double)iHit + iKnock + iWreck + iSunder)/1000);//(double)iAtt *(1.0 + (double)iHit/1000) *(1.0 + (double)iKnock/1000) * (1.0 + (double)iWreck/1000) * (1.0 + (double)iSunder/1000);

	int iBaseHP = pEntity->getProperty(PROP_ENTITY_BASEMAXHP, 0);
	int iBaseAtt = pEntity->getProperty(PROP_ENTITY_BASEATT, 0);

	int iRelivePercent = pEntity->getProperty(PROP_ENTITY_RELIVEENHANCE, 0);
	int iSkillDamage = pEntity->getProperty(PROP_ENTITY_SKILLDAMAGE, 0);
	int iSkillDef = pEntity->getProperty(PROP_ENTITY_SKILLDEF, 0);

	// 新增防御和小招伤害
	int iDefValue = pEntity->getProperty(PROP_ENTITY_DEF, 0);
	int iSmallSkillDamage = pEntity->getProperty(PROP_ENTITY_SMALLSKILLDAMAGE, 0);

	int iResultValue = (dEffectHP/25) * (1.0 + (double)iRelivePercent/20000)  + dEffectAtt/5 +
		(double)iBaseHP * (double)iBaseAtt/100 + (double)iSkillDamage/5 + (double)iSkillDef/5 + (double)iDefValue/2.5 + (double)iSmallSkillDamage/5;


	pEntity->setProperty(PROP_ENTITY_FIGHTVALUE, iResultValue);

	{
		EventArgs tmpArgs;
		tmpArgs.context.setInt("entity", hEntity);
		pEntity->getEventServer()->setEvent(EVENT_ENTITY_FIGHTVALUECHG, tmpArgs);
	}
}



void CalcSystem::bindVIPProp()
{
	s_szPropCalcArray[PROP_ACTOR_VIPLEVEL].push_back(new VIPPropCalc(PROP_ENTITY_PHYSTRENGTHLIMIT, VIP_PROP_PHYSTRENGTH_UPLIMIT) );
	s_initDstCalcList.push_back(new VIPPropCalc(PROP_ENTITY_PHYSTRENGTHLIMIT, VIP_PROP_PHYSTRENGTH_UPLIMIT) );

	s_szPropCalcArray[PROP_ACTOR_VIPLEVEL].push_back(new VIPPropCalc(PROP_ENTITY_SAODANGLIMI, VIP_PROP_SAODANG_TIMES) );
	s_initDstCalcList.push_back(new VIPPropCalc(PROP_ENTITY_SAODANGLIMI, VIP_PROP_SAODANG_TIMES) );
	
}


void CalcSystem::bindPropInfo()
{
	// 注意，这里主要处理的是所有的战斗属性修改，
	// 战斗属性修改由于导致的修改点多，这里统一处理。
	// 其他由玩家单次行为导致的存储修改(例如成长目前规则是这样的),在触发点修改,
	// 这里不做统一注册处理
	bindCommPropCalc(PROP_ENTITY_MAXHP, PROP_ENTITY_BASEMAXHP, PROP_ENTITY_MAXHPAVALUE, PROP_ENTITY_MAXHPAPERCENT, PROP_ENTITY_HPGROW);
	bindCommPropCalc(PROP_ENTITY_ATT, PROP_ENTITY_BASEATT, PROP_ENTITY_ATTAVALUE, PROP_ENTITY_ATTAPERCENT, PROP_ENTITY_ATTGROW);
	bindCommPropCalc(PROP_ENTITY_DEF, PROP_ENTITY_BASEDEF, PROP_ENTITY_DEFAVALUE, PROP_ENTITY_DEFAPERCENT, PROP_ENTITY_DEFGROW);
	
	bindCommPropCalc(PROP_ENTITY_HIT, PROP_ENTITY_BASEHIT, PROP_ENTITY_HITAVALUE, PROP_ENTITY_HITAPERCENT, PROP_ENTITY_HITGROW);
	bindCommPropCalc(PROP_ENTITY_DOGE, PROP_ENTITY_BASEDOGE, PROP_ENTITY_DOGEAVALUE, PROP_ENTITY_DOGEAPERCENT, 0);
	bindCommPropCalc(PROP_ENTITY_KNOCK, PROP_ENTITY_BASEKNOCK, PROP_ENTITY_KNOCKAVALUE, PROP_ENTITY_KNOCKAPERCENT, PROP_ENTITY_KNOCKGROW);
	bindCommPropCalc(PROP_ENTITY_ANTIKNOCK, PROP_ENTITY_BASEANTIKNOCK, PROP_ENTITY_ANTIKNOCKAVALUE, PROP_ENTITY_ANTIKNOCKAPERCENT, 0);
	bindCommPropCalc(PROP_ENTITY_BLOCK, PROP_ENTITY_BASEBLOCK, PROP_ENTITY_BLOCKAVALUE, PROP_ENTITY_BLOCKAPERCENT, 0);
	bindCommPropCalc(PROP_ENTITY_WRECK, PROP_ENTITY_BASEWRECK, PROP_ENTITY_WRECKAVALUE, PROP_ENTITY_WRECKAPERCENT, PROP_ENTITY_WRECKGROW);
	bindCommPropCalc(PROP_ENTITY_ARMOR, PROP_ENTITY_BASEARMOR, PROP_ENTITY_ARMORAVALUE, PROP_ENTITY_ARMORAPERCENT, 0);
	bindCommPropCalc(PROP_ENTITY_SUNDER, PROP_ENTITY_BASESUNDER, PROP_ENTITY_SUNDERAVALUE, PROP_ENTITY_SUNDERAPERCENT, 0);
	bindCommPropCalc(PROP_ENTITY_INITANGER, PROP_ENTITY_BASEINITANGER, PROP_ENTITY_INITANGERAVALUE, PROP_ENTITY_INITANGERAPERCENT, 0);
}




