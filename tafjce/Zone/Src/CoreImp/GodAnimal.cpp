#include "CoreImpPch.h"
#include "../GameEngine/Entity.h"
#include "GodAnimal.h"



static int s_szGodAnimalSaveProp[] = {PROP_ENTITY_BASEID, PROP_ENTITY_EXP, PROP_ENTITY_LEVEL, PROP_ENTITY_LEVELSTEP, PROP_ENTITY_UUID,
	};


extern "C" IObject* createGoldAnimal()
{
	return new GoldAnimal;
}


GoldAnimal::GoldAnimal()
{
	setProperty(PROP_ENTITY_CLASS, (Int32)GSProto::en_class_GodAnimal);
}


bool GoldAnimal::createEntity(const std::string& strData)
{
	ServerEngine::RoleSaveData saveData;
	ServerEngine::JceToObj(strData, saveData);

	intitBaseProp(saveData.basePropData);

	initGoldAnimalProp();

	// 初始化子系统数据
	initSubsystem(saveData);
	
	// 执行子系统createComplete 函数
	completeSubsystem();

	// 设置HP,由于神兽不会被攻击，无需为后续的MaxHP做处理
	int iMaxHP = getProperty(PROP_ENTITY_MAXHP, 0);
	setProperty(PROP_ENTITY_HP, iMaxHP);

	// 设置下怒气
	int iInitAnger = getProperty(PROP_ENTITY_INITANGER, 0);
	setProperty(PROP_ENTITY_ANGER, iInitAnger);

	// 注册成长重新计算事件
	getEventServer()->subscribeEvent(EVENT_ENTITY_CALCGROW, this, &GoldAnimal::onEventCalcGrow);

	return true;
}


void GoldAnimal::packSaveData(ServerEngine::RoleSaveData& data)
{
	packBaseProp(data, s_szGodAnimalSaveProp, (int)(sizeof(s_szGodAnimalSaveProp)/sizeof(s_szGodAnimalSaveProp[0])) );
	packSubsystem(data);
}

void GoldAnimal::initGoldAnimalProp()
{
	ITable* pGodAnimalTb = getCompomentObjectManager()->findTable("GodAnimal");
	assert(pGodAnimalTb);

	int iBaseID = getProperty(PROP_ENTITY_BASEID, 0);
	int iRecord = pGodAnimalTb->findRecord(iBaseID);
	assert(iRecord >= 0);

	int iBaseMaxHP = pGodAnimalTb->getInt(iRecord, "生命");
	int iBaseAtt = pGodAnimalTb->getInt(iRecord, "攻击");
	int iBaseHit = pGodAnimalTb->getInt(iRecord, "命中");
	int iBaseKnock = pGodAnimalTb->getInt(iRecord, "暴击");
	int iBaseWreck = pGodAnimalTb->getInt(iRecord, "破击");
	
	int iBaseInitAnger = pGodAnimalTb->getInt(iRecord, "初始怒气");
	/*int iBaseMaxHPGrow = pGodAnimalTb->getInt(iRecord, "生命成长");
	int iBaseAttGrow = pGodAnimalTb->getInt(iRecord, "攻击成长");
	int iBaseHitGrow = pGodAnimalTb->getInt(iRecord, "命中成长");
	int iBaseKnockGrow = pGodAnimalTb->getInt(iRecord, "暴击成长");*/

	int iChangeItemID = pGodAnimalTb->getInt(iRecord, "兑换ItemID");
	int iChangeItemCount = pGodAnimalTb->getInt(iRecord, "兑换数量");
	
	setProperty(PROP_ENTITY_BASEMAXHP, iBaseMaxHP);
	setProperty(PROP_ENTITY_BASEATT, iBaseAtt);
	setProperty(PROP_ENTITY_BASEHIT, iBaseHit);     // 特殊处理，命中和暴击不随等阶成长，这里要X100
	setProperty(PROP_ENTITY_BASEKNOCK, iBaseKnock);
	setProperty(PROP_ENTITY_BASEWRECK, iBaseWreck);
	
	setProperty(PROP_ENTITY_BASEINITANGER, iBaseInitAnger);
	setProperty(PROP_GODANIMALSOUL_ITEMID, iChangeItemID);
	setProperty(PROP_GODANIMALSOUL_ITEMCOUNT, iChangeItemCount);

	// 计算成长
	reCalcGrow();
}

void GoldAnimal::onEventCalcGrow(EventArgs& args)
{
	reCalcGrow();
}

void GoldAnimal::reCalcGrow()
{
	ITable* pGodAnimalTb = getCompomentObjectManager()->findTable("GodAnimal");
	assert(pGodAnimalTb);

	int iBaseID = getProperty(PROP_ENTITY_BASEID, 0);
	int iRecord = pGodAnimalTb->findRecord(iBaseID);
	assert(iRecord >= 0);

	int iBaseMaxHPGrow = pGodAnimalTb->getInt(iRecord, "生命成长");
	int iBaseAttGrow = pGodAnimalTb->getInt(iRecord, "攻击成长");
	int iBaseHitGrow = pGodAnimalTb->getInt(iRecord, "命中成长");
	int iBaseKnockGrow = pGodAnimalTb->getInt(iRecord, "暴击成长");
	int iBaseWreckGrow = pGodAnimalTb->getInt(iRecord, "破击成长");
	

	int iLevelStep = getProperty(PROP_ENTITY_LEVELSTEP, 0);
	ITable* pGodAnimalLvStep = getCompomentObjectManager()->findTable(TABLENAME_GodAnimalLevelStep);
	assert(pGodAnimalLvStep);

	int iLvStepRecord = pGodAnimalLvStep->findRecord(iLevelStep);
	assert(iLvStepRecord >= 0);

	int iParam = pGodAnimalLvStep->getInt(iLvStepRecord, "成长系数");
	setProperty(PROP_ENTITY_HPGROW, iBaseMaxHPGrow*iParam);
	setProperty(PROP_ENTITY_ATTGROW, iBaseAttGrow * iParam);

	setProperty(PROP_ENTITY_HITGROW, iBaseHitGrow * 100);
	setProperty(PROP_ENTITY_KNOCKGROW, iBaseKnockGrow * 100);

	// 新增神兽破击
	setProperty(PROP_ENTITY_WRECKGROW, iBaseWreckGrow * 100);

	// 设置基础属性系数
	int iBasePropParam = pGodAnimalLvStep->getInt(iLvStepRecord, "属性系数");
	setProperty(PROP_ENTITY_BASEPROP_FIXPARAM, iBasePropParam);
}


void GoldAnimal::addExp(int iExp)
{
	ITable* pExpTable = getCompomentObjectManager()->findTable(TABLENAME_GodAnimalLevelExp);
	assert(pExpTable);

	int iCurLevel = getProperty(PROP_ENTITY_LEVEL, 0);
	int iCurExp = getProperty(PROP_ENTITY_EXP, 0);
	int iLevelStep = getProperty(PROP_ENTITY_LEVELSTEP, 0);
	int iTmpExp = iExp;
	
	while(true)
	{
		int iRecord = pExpTable->findRecord(iCurLevel);
		assert(iRecord >= 0);

		int iNeedExp = pExpTable->getInt(iRecord, "升级经验");
		int iConsumeExp = iNeedExp - iCurExp;
		if(iTmpExp < iConsumeExp)
		{
			iCurExp += iTmpExp;
			break;
		}

		// 否则升级,先判断是否满级
		int iNextLvRecord = pExpTable->findRecord(iCurLevel + 1);
		if(iNextLvRecord < 0)
		{
			iCurExp = iNeedExp - 1;
			break;
		}

		// 是否等阶限制了
		int iLimitLevelStep = pExpTable->getInt(iNextLvRecord, "等阶限制");
		if(iLimitLevelStep > iLevelStep)
		{
			iCurExp = iNeedExp - 1;
			break;
		}

		// 等级
		iCurLevel++;
		iTmpExp -= iConsumeExp;
		iCurExp = 0;
	}

	// 更新等级和经验
	setProperty(PROP_ENTITY_LEVEL, iCurLevel);
	setProperty(PROP_ENTITY_EXP, iCurExp);
}




