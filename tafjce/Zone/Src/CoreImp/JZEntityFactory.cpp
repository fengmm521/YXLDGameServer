#include "CoreImpPch.h"
#include "../GameEngine/EntityFactory.h"
#include "JZEntityFactory.h"
#include "LoginManager.h"


extern "C" IComponent* createJZEntityFactory(Int32)
{
	return new JZEntityFactory;
}


vector<HEntity> JZEntityFactory::createMonsterGrp(int iMonsterGrpID)
{
	vector<HEntity> resultList;

	ITable* pTable = getCompomentObjectManager()->findTable("MonsterGroup");
	assert(pTable);

	int iRecord = pTable->findRecord(iMonsterGrpID);
	assert(iRecord >= 0);

	resultList.resize(GSProto::MAX_BATTLE_MEMBER_SIZE/2, 0);

	for(size_t i = 0; i < GSProto::MAX_BATTLE_MEMBER_SIZE/2; i++)
	{
		stringstream ss;
		ss<<(i+1)<<"号位成员";
		string strKey = ss.str();
		int iMonsterID = pTable->getInt(iRecord, strKey);
		if(0 == iMonsterID) continue;

		ServerEngine::MNCreateContext createCtx;
		createCtx.nBaseId = iMonsterID;
		string strData = ServerEngine::JceToStr(createCtx);
		
		IEntity* pTmpMonster = createEntity("Monster", strData);
		assert(pTmpMonster);

		resultList[i] = pTmpMonster->getHandle();
	}

	return resultList;
}

int JZEntityFactory::getVisibleMonsterID(int iMonsterGrpID)
{
	ITable* pTable = getCompomentObjectManager()->findTable("MonsterGroup");
	assert(pTable);

	int iRecord = pTable->findRecord(iMonsterGrpID);
	assert(iRecord >= 0);

	int iVisibleMonsterID = pTable->getInt(iRecord, "显示怪");

	return iVisibleMonsterID;
}


IEntity* JZEntityFactory::createEntity(const std::string& strClassName, const std::string& strMsg)
{
	IEntity* pResult = EntityFactory::createEntity(strClassName, strMsg);
	if(pResult)
	{
		EventArgs args;
		args.context.setInt("entity", pResult->getHandle() );
		pResult->getEventServer()->setEvent(EVENT_ENTITY_CREATED, args);
	}

	return pResult;
}

IEntity* JZEntityFactory::createMachine(const string& strAccount, const string& strName, const vector<CreateHeroDesc>& heroIDList)
{	
	LoginManager* pLoginManager = static_cast<LoginManager*>(getComponent<IUserStateManager>(COMPNAME_LoginManager, IID_IUserStateManager));
	assert(pLoginManager);

	ServerEngine::ActorCreateContext createCtx;
	createCtx.strAccount = strAccount;

	// 机器人不存储数据，不在意来源World
	pLoginManager->fillNewActorData(createCtx.data, strAccount, 0, strName, heroIDList);

	string strData = ServerEngine::JceToStr(createCtx);
	IEntity* pNewEntity = createEntity("Ghost", strData);
	assert(pNewEntity);

	pLoginManager->addReserveGhost(pNewEntity->getHandle() );

	return pNewEntity;
}


bool JZEntityFactory::initlize(const PropertySet& propSet)
{
	EntityFactory::initlize(propSet);

	loadLevelStepCfg();
	loadGodAnimalLevelLimit();
	loadFunctionCfg();
	
	return true;
}

void JZEntityFactory::loadLevelStepCfg()
{
	ITable* pLevelStepGrow = getCompomentObjectManager()->findTable("LevelStepGrow");
	assert(pLevelStepGrow);

	int iRecordCount = pLevelStepGrow->getRecordCount();
	for(int i = 0; i < iRecordCount; i++)
	{
		LevelStepGrowCfg tmpCfg;
		tmpCfg.iLevelStep = pLevelStepGrow->getInt(i, "ID");
		tmpCfg.iNeedProgress = pLevelStepGrow->getInt(i, "进度");
		tmpCfg.iGrowParam = pLevelStepGrow->getInt(i, "系数");
		tmpCfg.iBasePropParam = pLevelStepGrow->getInt(i, "属性系数");
		tmpCfg.iNeedSoulCount = pLevelStepGrow->getInt(i, "消耗魂魄数量");
		tmpCfg.iNeedSilver = pLevelStepGrow->getInt(i, "需要铜币");
		tmpCfg.iExtraSoulCount = pLevelStepGrow->getInt(i, "转化魂魄数");
		
		m_mapLevelStepCfg[tmpCfg.iLevelStep] = tmpCfg;
		m_progressList.push_back(tmpCfg.iNeedProgress);
	}
}

const LevelStepGrowCfg* JZEntityFactory::queryLevelStepCfg(int iLevelStep)
{
	map<int, LevelStepGrowCfg>::iterator it = m_mapLevelStepCfg.find(iLevelStep);
	if(it == m_mapLevelStepCfg.end() )
	{
		return NULL;
	}

	return &(it->second);
}

int JZEntityFactory::calcLevelStep(int iProgress)
{
	vector<int>::const_iterator findIt = upper_bound(m_progressList.begin(), m_progressList.end(), iProgress);

	if(findIt == m_progressList.end() )
	{
		findIt = --m_progressList.end();
	}

	int iLevel = (int)(findIt - m_progressList.begin());
	
	return iLevel;
}

void* JZEntityFactory::queryInterface(IID interfaceId)
{
	if(IID_IEntityFactory == interfaceId)
	{
		return (IEntityFactory*)this;
	}
	else if(IID_IJZEntityFactory == interfaceId)
	{
		return (IJZEntityFactory*)this;
	}
	else
	{
		return NULL;
	}
}


IEntityFactory* JZEntityFactory::getEntityFactory()
{
	return this;
}

void JZEntityFactory::loadGodAnimalLevelLimit()
{
	//m_godAnimalLevelLimit
	ITable* pTable = getCompomentObjectManager()->findTable(TABLENAME_GodAnimalLevelExp);
	assert(pTable);

	int iRecordCount = pTable->getRecordCount();
	for(int i = 0; i < iRecordCount; i++)
	{
		int iLevel = pTable->getInt(i, "等级");
		int iLevelStep = pTable->getInt(i, "等阶限制");

		if(iLevel > m_godAnimalLevelLimit[iLevelStep])
		{
			m_godAnimalLevelLimit[iLevelStep] = iLevel;
		}
	}
}



int JZEntityFactory::getGodAnimalLevelLimit(int iLevelStep)
{
	map<int, int>::iterator it = m_godAnimalLevelLimit.upper_bound(iLevelStep);
	it--;

	if(it != m_godAnimalLevelLimit.end() )
	{
		return it->second;
	}

	return 0;
}


void JZEntityFactory::loadFunctionCfg()
{
	ITable* pTable = getCompomentObjectManager()->findTable("OpenFunction");
	assert(pTable);

	int iRecordCount = pTable->getRecordCount();
	for(int i = 0; i < iRecordCount; i++)
	{
		int iFunction = pTable->getInt(i, "功能ID");
		int iLevel = pTable->getInt(i, "开启等级");
		int iErrorCode = pTable->getInt(i, "错误码ID");

		FunctionOpenDesc tmpDesc;
		tmpDesc.iFunctionID = iFunction;
		tmpDesc.iErrorCode = iErrorCode;

		assert(m_levelOpenFunctionMap.find(iLevel) == m_levelOpenFunctionMap.end() );
		m_levelOpenFunctionMap[iLevel]= tmpDesc;
		m_functionDataMap[tmpDesc.iFunctionID] = tmpDesc;
	}
}


const FunctionOpenDesc* JZEntityFactory::getFunctionData(int iFunction)
{
	map<int, FunctionOpenDesc>::iterator it = m_functionDataMap.find(iFunction);
	if(it == m_functionDataMap.end() ) return NULL;

	return &(it->second);
}

const FunctionOpenDesc* JZEntityFactory::getEnableFunctionData(int iLevel)
{
	map<int, FunctionOpenDesc>::iterator it = m_levelOpenFunctionMap.find(iLevel);
	if(it == m_levelOpenFunctionMap.end() )
	{
		return NULL;
	}

	return &(it->second);
}


int JZEntityFactory::calcHeroSumExp(int iLevel, int iCurExp)
{
	ITable* pHeroLvExpTb = getCompomentObjectManager()->findTable(TABLENAME_HeroLevelExp);
	assert(pHeroLvExpTb);


	int iSumExp = 0;
	for(int i = 1; i < iLevel; i++)
	{
		int iRecord = pHeroLvExpTb->findRecord(i);
		assert(iRecord >= 0);

		iSumExp += pHeroLvExpTb->getInt(iRecord, "修为");
	}

	iSumExp += iCurExp;

	return iSumExp;
}


void JZEntityFactory::calcHeroNewLevelExp(HEntity hHero, long ddAddExp, int& iNewLevel, int& iNewExp)
{	
	ITable* pHeroLvExpTb = getCompomentObjectManager()->findTable(TABLENAME_HeroLevelExp);
	assert(pHeroLvExpTb);

	IEntity* pHero = getEntityFromHandle(hHero);
	assert(pHero);

	int iCurLevel = pHero->getProperty(PROP_ENTITY_LEVEL, 0);
	long ddLeftExp = ddAddExp;
	long ddTkExp = pHero->getProperty(PROP_ENTITY_HEROEXP, 0);
	while(ddLeftExp > 0)
	{
		int iRecord = pHeroLvExpTb->findRecord(iCurLevel);
		assert(iRecord >= 0);
		
		long ddNeedExp = pHeroLvExpTb->getInt(iRecord, "修为");
		if( (ddLeftExp + ddTkExp) < ddNeedExp)
		{
			ddTkExp = ddLeftExp + ddTkExp;
			break;
		}
		else
		{
			if(pHeroLvExpTb->findRecord(iCurLevel + 1) < 0)
			{
				ddTkExp = ddNeedExp - 1;
				break;
			}
			else
			{
				iCurLevel++;
				ddLeftExp = ddLeftExp - (ddNeedExp - ddTkExp);
				ddTkExp = 0;
			}
		}
	}

	// 验证等级上限(主公等级*2)
	HEntity hMaster = pHero->getProperty(PROP_ENTITY_MASTER, 0);
	IEntity* pMaster = getEntityFromHandle(hMaster);
	assert(pMaster);

	int iMasterLevel = pMaster->getProperty(PROP_ENTITY_LEVEL, 0);
	if( (iMasterLevel < 120) && (iCurLevel > iMasterLevel * 2) )
	{
		iCurLevel = iMasterLevel * 2;

		// 当前等级修为设置为升级修为-1
		int iRecord = pHeroLvExpTb->findRecord(iCurLevel);
		assert(iRecord >= 0);
		ddTkExp = pHeroLvExpTb->getInt(iRecord, "修为") - 1;
	}

	iNewLevel = iCurLevel;
	iNewExp = (int)ddTkExp;
}








