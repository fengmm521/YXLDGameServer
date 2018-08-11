#ifndef __IJZENTITY_FACTORY_H__
#define __IJZENTITY_FACTORY_H__

struct LevelStepGrowCfg
{
	LevelStepGrowCfg():iLevelStep(0), iNeedProgress(0), iGrowParam(0), iBasePropParam(0), iNeedSoulCount(0), iNeedSilver(0), iExtraSoulCount(0){}

	int iLevelStep;
	int iNeedProgress;
	int iGrowParam;
	int iBasePropParam; // 基础属性修正系数
	int iNeedSoulCount;
	int iNeedSilver;
	int iExtraSoulCount;
};

struct FunctionOpenDesc
{
	FunctionOpenDesc():iFunctionID(0), iErrorCode(0){}

	int iFunctionID;
	int iErrorCode;
};

struct CreateHeroDesc
{
	CreateHeroDesc():iHeroID(0), iFormationPos(0), iLevel(0), iLevelStep(0), iQuality(0){}

	int iHeroID;
	int iFormationPos;
	int iLevel;
	int iLevelStep;
	int iQuality;
};

class IJZEntityFactory
{
public:

	virtual ~IJZEntityFactory(){}

	// 功能: 创建怪物组,创建出来的怪物组放在各自的位置上
	// 返回的vector列表size是固定的9个位置，没有怪物则是0
	// 参数: [iMonsterGrpID] 怪物组ID
	virtual vector<HEntity> createMonsterGrp(int iMonsterGrpID) = 0;

	// 功能: 查询等阶配置
	virtual const LevelStepGrowCfg* queryLevelStepCfg(int iLevelStep) = 0;

	// 功能: 计算等阶
	virtual int calcLevelStep(int iProgress) = 0;

	// 功能: 获取IEntityFactory 接口
	virtual IEntityFactory* getEntityFactory() = 0;

	virtual int getGodAnimalLevelLimit(int iLevelStep) = 0;

	virtual int getVisibleMonsterID(int iMonsterGrpID) = 0;

	virtual IEntity* createMachine(const string& strAccount, const string& strName, const vector<CreateHeroDesc>& heroIDList) = 0;

	virtual const FunctionOpenDesc* getEnableFunctionData(int iLevel) = 0;

	virtual const FunctionOpenDesc* getFunctionData(int iFunction) = 0;

	virtual void calcHeroNewLevelExp(HEntity hHero, long ddAddExp, int& iNewLevel, int& iNewExp) = 0;

	virtual int calcHeroSumExp(int iLevel, int iCurExp) = 0;
};


#endif
