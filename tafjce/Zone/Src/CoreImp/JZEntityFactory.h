#ifndef __JZ_ENTITYFACTORY_H__
#define __JZ_ENTITYFACTORY_H__

#include "IEntity.h"
#include "IEntityFactory.h"
#include "IJZEntityFactory.h"



class JZEntityFactory:public EntityFactory, public IJZEntityFactory
{
public:

	// IComponent Interface
	virtual bool initlize(const PropertySet& propSet);
	virtual void* queryInterface(IID interfaceId);

	// IEntityFactory Interface
	virtual IEntity* createEntity(const std::string& strClassName, const std::string& strMsg);

	// IJZEntityFactory Interface
	virtual vector<HEntity> createMonsterGrp(int iMonsterGrpID);
	virtual const LevelStepGrowCfg* queryLevelStepCfg(int iLevelStep);
	virtual int calcLevelStep(int iProgress);
	virtual IEntityFactory* getEntityFactory();
	virtual int getGodAnimalLevelLimit(int iLevelStep);
	virtual int getVisibleMonsterID(int iMonsterGrpID);
	virtual IEntity* createMachine(const string& strAccount, const string& strName, const vector<CreateHeroDesc>& heroIDList);
	virtual const FunctionOpenDesc* getEnableFunctionData(int iLevel);
	virtual const FunctionOpenDesc* getFunctionData(int iFunction);
	virtual void calcHeroNewLevelExp(HEntity hHero, long ddAddExp, int& iNewLevel, int& iNewExp);
	virtual int calcHeroSumExp(int iLevel, int iCurExp);

private:

	void loadLevelStepCfg();
	void loadGodAnimalLevelLimit();
	void loadFunctionCfg();

private:

	map<int, LevelStepGrowCfg> m_mapLevelStepCfg;
	vector<int> m_progressList;
	map<int, int> m_godAnimalLevelLimit; // 神兽等阶限制等级
	map<int, FunctionOpenDesc> m_levelOpenFunctionMap;
	map<int, FunctionOpenDesc> m_functionDataMap;
};


#endif

