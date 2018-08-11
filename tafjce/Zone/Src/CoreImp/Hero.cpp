#include "CoreImpPch.h"
#include "../GameEngine/Entity.h"
#include "Hero.h"
#include "IHeroSystem.h"
#include "LogHelp.h"


static int s_szHeroSaveProp[] = {PROP_ENTITY_BASEID, PROP_ENTITY_HP, PROP_ENTITY_LEVEL, PROP_ENTITY_LEVELSTEP,
	  PROP_ENTITY_HEROEXP,PROP_HERO_LVSTEPPROGRESS, PROP_ENTITY_QUALITY,
	PROP_ENTITY_UUID,
	};


extern "C" IObject* createHero()
{
	return new Hero;
}


Hero::Hero():m_bFinished(false)
{
	setProperty(PROP_ENTITY_CLASS, (Int32)GSProto::en_class_Hero);
}

bool Hero::createEntity(const std::string& strData)
{
	ServerEngine::RoleSaveData saveData;
	ServerEngine::JceToObj(strData, saveData);

	// 如果有ReplaceHandle, 重新绑定
	HEntity hReplaceHandle = saveData.basePropData.roleIntPropset[PROP_ENTITY_REPLACEHANDLE];
	if(0 != hReplaceHandle)
	{
		rebindHandle(hReplaceHandle);
	}
	
	// 初始化存档属性值
	intitBaseProp(saveData.basePropData);

	// 初始化英雄属性数据
	initHeroProp();

	// 初始化子系统数据
	initSubsystem(saveData);

	// 先设置下HP,防止后续系统运行出问题，被认为死亡
	int iMaxHP = getProperty(PROP_ENTITY_MAXHP, 0);
	setProperty(PROP_ENTITY_HP, iMaxHP);
	
	// 执行子系统createComplete 函数
	completeSubsystem();

	// 设置HP
	iMaxHP = getProperty(PROP_ENTITY_MAXHP, 0);
	setProperty(PROP_ENTITY_HP, iMaxHP);

	// 设置下怒气
	int iInitAnger = getProperty(PROP_ENTITY_INITANGER, 0);
	setProperty(PROP_ENTITY_ANGER, iInitAnger);

	int iTotalProgress = getProperty( PROP_HERO_LVSTEPPROGRESS, 0);
	int iPrice = getHeroPrice(iTotalProgress);
	setProperty(PROP_HERO_PRICE,iPrice);
	
	m_bFinished = true;

	// 注册成长重新计算事件
	getEventServer()->subscribeEvent(EVENT_ENTITY_CALCGROW, this, &Hero::onEventCalcGrow);
	
	return true;
}

int Hero::getHeroPrice(int process)
{
	//收入铜币
	IGlobalCfg* pGlobalCfg = getComponent<IGlobalCfg>(COMPNAME_GlobalCfg, IID_IGlobalCfg);
	assert(pGlobalCfg);

	int iheroBase = pGlobalCfg->getInt("出卖英雄获得铜币倍数",10);
	int iTotalBase = pGlobalCfg->getInt("出卖英雄总倍数",100);
	
	int iCanGetSilver = (1 * iheroBase + process) * iTotalBase;
	
	return  iCanGetSilver;
}

void Hero::initHeroProp()
{
	ITable* pTable = getCompomentObjectManager()->findTable(TABLENAME_Hero);
	assert(pTable);

	int iHeroID = getProperty(PROP_ENTITY_BASEID, 0);
	int iRecord = pTable->findRecord(iHeroID);

	assert(iRecord >= 0);
	int iBaseMAXHP = pTable->getInt(iRecord, "生命");
	int iBaseAtt = pTable->getInt(iRecord, "攻击");
	int iBaseHit = pTable->getInt(iRecord, "命中");
	int iBaseDoge = pTable->getInt(iRecord, "闪避");
	int iBaseKnock = pTable->getInt(iRecord, "暴击");
	int iBaseAntiKnock = pTable->getInt(iRecord, "韧性");
	int iBaseBlock = pTable->getInt(iRecord, "抵挡");
	int iBaseWreck = pTable->getInt(iRecord, "破击");
	int iBaseArmor = pTable->getInt(iRecord, "护甲");
	int iBaseSunder = pTable->getInt(iRecord, "破甲");
	int iBaseDef = pTable->getInt(iRecord, "防御");
	int iBaseInitAnger = pTable->getInt(iRecord, "初始怒气");
	int iHeroSex = pTable->getInt(iRecord, "性别");
	int iJob = pTable->getInt(iRecord, "英雄类型");

	setProperty(PROP_ENTITY_BASEMAXHP, iBaseMAXHP);
	setProperty(PROP_ENTITY_BASEATT, iBaseAtt);
	setProperty(PROP_ENTITY_BASEHIT, iBaseHit);
	setProperty(PROP_ENTITY_BASEDOGE, iBaseDoge);
	setProperty(PROP_ENTITY_BASEKNOCK, iBaseKnock);
	setProperty(PROP_ENTITY_BASEANTIKNOCK, iBaseAntiKnock);
	setProperty(PROP_ENTITY_BASEBLOCK, iBaseBlock);
	setProperty(PROP_ENTITY_BASEWRECK, iBaseWreck);
	setProperty(PROP_ENTITY_BASEARMOR, iBaseArmor);
	setProperty(PROP_ENTITY_BASESUNDER, iBaseSunder);
	setProperty(PROP_ENTITY_BASEINITANGER, iBaseInitAnger);
	setProperty(PROP_ENTITY_BASEDEF, iBaseDef);
	setProperty(PROP_ENTITY_SEX, iHeroSex);
	setProperty(PROP_ENTITY_JOB, iJob);

	assert(iJob > 0);

	// 填充生命成长和攻击成长,成长本身并不需要存储，但是由于修改点少，
	// 这里保存下,初始时计算下,后面成长变更时，再计算下,英雄只有生命+攻击成长
	reCalcGrow();
}

void Hero::onEventCalcGrow(EventArgs& args)
{
	reCalcGrow();
}

void Hero::reCalcGrow()
{
	ITable* pTable = getCompomentObjectManager()->findTable(TABLENAME_Hero);
	assert(pTable);

	int iHeroID = getProperty(PROP_ENTITY_BASEID, 0);
	int iRecord = pTable->findRecord(iHeroID);
	assert(iRecord >= 0);

	int iHpBaseGrow = pTable->getInt(iRecord, "生命成长");
	int iAttBaseGrow = pTable->getInt(iRecord, "攻击成长");
	int iDefBaseGrow = pTable->getInt(iRecord, "防御成长");

	IJZEntityFactory* pEntityFactory = getComponent<IJZEntityFactory>(COMPNAME_EntityFactory, IID_IJZEntityFactory);
	assert(pEntityFactory);

	int iLevelStep = getProperty(PROP_ENTITY_LEVELSTEP, 0);
	const LevelStepGrowCfg* pLevelStepGrowInfo = pEntityFactory->queryLevelStepCfg(iLevelStep);
	assert(pLevelStepGrowInfo);

	int iResultHPGrow = iHpBaseGrow * pLevelStepGrowInfo->iGrowParam;
	int iResultAttGrow = iAttBaseGrow * pLevelStepGrowInfo->iGrowParam;
	int iResultDefGrow = iDefBaseGrow * pLevelStepGrowInfo->iGrowParam;

	setProperty(PROP_ENTITY_HPGROW, iResultHPGrow);
	setProperty(PROP_ENTITY_ATTGROW, iResultAttGrow);
	setProperty(PROP_ENTITY_DEFGROW, iResultDefGrow);

	// 设置属性系数(按照品质)
	int iQuality = getProperty(PROP_ENTITY_QUALITY, 0);
	ITable* pQualityPropTb = getCompomentObjectManager()->findTable(TABLENAME_QualityProp);
	assert(pQualityPropTb);

	if(iQuality == 0)
	{
		iQuality = 1;
		setProperty(PROP_ENTITY_QUALITY, iQuality);
	}
	int iQualityRecord = pQualityPropTb->findRecord(iQuality);
	assert(iQualityRecord >= 0);

	int iBasePropFixParam = pQualityPropTb->getInt(iQualityRecord, "属性系数");
	setProperty(PROP_ENTITY_BASEPROP_FIXPARAM, iBasePropFixParam);
}


void Hero::packSaveData(ServerEngine::RoleSaveData& data)
{
	packBaseProp(data, s_szHeroSaveProp, (int)(sizeof(s_szHeroSaveProp)/sizeof(s_szHeroSaveProp[0])) );
	packSubsystem(data);
}

void Hero::setProperty(PropertySet::PropertyKey key, Int32 nValue)
{
	// HP/MaxHP 处理
	if( ( (PROP_ENTITY_HP == key) || (PROP_ENTITY_MAXHP == key) ) && (nValue < 0) )
	{
		nValue = 0;
	}

	if(PROP_ENTITY_HP == key)
	{
		int iMaxHP = getProperty(PROP_ENTITY_MAXHP, 0);
		if(nValue > iMaxHP)
		{
			nValue = iMaxHP;
		}
	}
	
	//价格跟进度相关
	if( (PROP_HERO_LVSTEPPROGRESS == key) && m_bFinished)
	{
		int iPrice = getHeroPrice(nValue);
		Entity::setProperty(PROP_HERO_PRICE,iPrice);
	}

	int iOrgValue = getProperty(key, 0);
	Entity::setProperty(key, nValue);

	// MaxHP处理
	if( (PROP_ENTITY_MAXHP == key) && m_bFinished)
	{
		int iMaxHP = getProperty(PROP_ENTITY_MAXHP, 0);
		int iCurHP = getProperty(PROP_ENTITY_HP, 0);

		// 如果生命上限+了，生命也要+
		if(nValue > iOrgValue)
		{
			setProperty(PROP_ENTITY_HP, iCurHP + (nValue - iOrgValue) );
			iCurHP = getProperty(PROP_ENTITY_HP, 0);
		}
		
		if(iCurHP > iMaxHP)
		{
			Entity::setProperty(PROP_ENTITY_HP, iMaxHP);
		}
	}

	
}


void Hero::addExp(int iExp)
{
	IJZEntityFactory* pJZEntityFactory = getComponent<IJZEntityFactory>(COMPNAME_EntityFactory, IID_IJZEntityFactory);
	assert(pJZEntityFactory);

	int iNewLevel = 0;
	int iNewExp  = 0;

	int iOldExp = getProperty(PROP_ENTITY_HEROEXP, 0);
	int iPreLevel = getProperty(PROP_ENTITY_LEVEL, 0);
	pJZEntityFactory->calcHeroNewLevelExp(getHandle(), iExp, iNewLevel, iNewExp);
	
	// 设置等级
	setProperty(PROP_ENTITY_HEROEXP, iNewExp);
	setProperty(PROP_ENTITY_LEVEL, iNewLevel);

	HEROLOG(this)<<"LevelUp|"<<iPreLevel<<"|"<<iNewLevel<<"|"<<iOldExp<<"|"<<iNewExp<<endl;

	// 通知客户端更新
	HEntity hMaster = getProperty(PROP_ENTITY_MASTER, 0);
	IEntity* pMaster = getEntityFromHandle(hMaster);
	if(pMaster)
	{
		IHeroSystem* pHeroSys = static_cast<IHeroSystem*>(pMaster->querySubsystem(IID_IHeroSystem));
		assert(pHeroSys);

		pHeroSys->sendHeroUpdateInfo(getHandle() );
	}

	if(iPreLevel != iNewLevel)
	{
		EventArgs args;
		args.context.setInt("HeroLevel",iNewLevel);
		args.context.setInt("entity",hMaster);
		pMaster->getEventServer()->setEvent(EVENT_ENTITY_TASKFINISH_HEROLEVEL, args);
	}
}




