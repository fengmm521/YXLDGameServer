#include "MiniApr.h"

using namespace MINIAPR;
using namespace std;
extern "C" IComponent* createTimerComponent(Int32 nVersion);
extern "C" IComponent* createMiniAprRandom(Int32 nVersion);
extern "C" IComponent* createProfilerManager(Int32 nVersion);
extern "C" IComponent* createProfileTimer(Int32 nVersion);
extern "C" IComponent* createProfileStrategyAverage(Int32 nVersion);
extern "C" IComponent* createScriptEngine(Int32 nVersion);
extern "C" IComponent* createBuffFactory(Int32 nVersion);
extern "C" IComponent* createSkillFactory(Int32 nVersion);
extern "C" IComponent* createEffectFactory(Int32 nVersion);
extern "C" IComponent* createMessageDispatch(Int32 nVersion);
extern "C" IComponent* createJZEntityFactory(Int32 nVersion);
extern "C" IComponent* createLoginManager(Int32 nVersion);
extern "C" IComponent* createGlobalCfg(Int32 nVersion);
extern "C" IComponent* createReserveWordCheck(Int32 nVersion);
extern "C" IComponent* createJZMessageLayer(Int32 nVersion);
extern "C" IComponent* createFightFactory(Int32 nVersion);
extern "C" IComponent* createDungeonFactory(Int32 nVersion);
extern "C" IComponent* createItemFactory(Int32 nVersion);
extern "C" IComponent* createDropFactory(Int32 nVersion);
extern "C" IComponent* createZoneTime(Int32 nVersion);
extern "C" IComponent* createModifyDelegate(Int32 nVersion);
extern "C" IComponent* createTimeRangeValueFactory(Int32 nVersion);
extern "C" IComponent* createFightSoulFactory(Int32 nVersion);
extern "C" IComponent* createCleanupManager(Int32 nVersion);
extern "C" IComponent* createClimbTowerFactory(Int32 nVersion);
extern "C" IComponent* createShopFactory(Int32 nVersion);
extern "C" IComponent* createLegionFactory(Int32 nVersion);
extern "C" IComponent* createCampBattleFactory(Int32 nVersion);
extern "C" IComponent* createHeroTallentFactory(Int32 nVersion);
extern "C" IComponent* createBossFactory(Int32 nVersion);
extern "C" IComponent* createArenaFactory(Int32 nVersion);
extern "C" IComponent* createVIPFactory(Int32 nVersion);
extern "C" IComponent* createTaskFactory(Int32 nVersion);
extern "C" IComponent* createGuideFactory(Int32 nVersion);
extern "C" IComponent* createDreamLandFactory(Int32 nVersion);
extern "C" IComponent* createManorFactory(Int32 nVersion);
extern "C" IComponent* createOperateFactory(Int32 nVersion);
extern "C" IObject* createActor();
extern "C" IObject* createGhost();
extern "C" IObject* createMonster();
extern "C" IObject* createHero();
extern "C" IObject* createGoldAnimal();
extern "C" IObject* createCalcSystem();
extern "C" IObject* createPropObserveSystem();
extern "C" IObject* createHeroSystem();
extern "C" IObject* createGMSystem();
extern "C" IObject* createFormationSystem();
extern "C" IObject* createFightSystem();
extern "C" IObject* createSkillSystem();
extern "C" IObject* createBuffSystem();
extern "C" IObject* createEffectSystem();
extern "C" IObject* createItemSystem();
extern "C" IObject* createActorSaveSystem();
extern "C" IObject* createDungeonSystem();
extern "C" IObject* createGodAnimalSystem();
extern "C" IObject* createFightSoulSystem();
extern "C" IObject* createHeroEquipSystem();
extern "C" IObject* createClimbTowerSystem();
extern "C" IObject* createLegionSystem();
extern "C" IObject* createShopSystem();
extern "C" IObject* createCampBattleSystem();
extern "C" IObject* createArenaSystem();
extern "C" IObject* createFriendSystem();
extern "C" IObject* createTaskSystem();
extern "C" IObject* createGuideSystem();
extern "C" IObject* createEquipBagSystem();
extern "C" IObject* createHeroRealEquipSys();
extern "C" IObject* createDreamLandSystem();
extern "C" IObject* createMailSystem();
extern "C" IObject* createTallentSystem();
extern "C" IObject* createManorSystem();
extern "C" IObject* createOperateSystem();

typedef IComponent* (*COMPONENT_CREATEFUN)(Int32);
typedef IObject* (*OBJECT_CREATEFUN)();
static std::map<std::string, COMPONENT_CREATEFUN> s_mapComponentCreateFun;
static std::map<std::string, OBJECT_CREATEFUN> s_mapObjectCreateFun;

void initComponentCreateFun()
{
	static bool s_bInitComponentFun = false;
	if(!s_bInitComponentFun) 
		s_bInitComponentFun = true;
	else
 		return;
	s_mapComponentCreateFun["TimeAxis"] = &createTimerComponent;
	s_mapComponentCreateFun["Random"] = &createMiniAprRandom;
	s_mapComponentCreateFun["ProfileManager"] = &createProfilerManager;
	s_mapComponentCreateFun["ProfileTimer"] = &createProfileTimer;
	s_mapComponentCreateFun["StrategyTimer"] = &createProfileStrategyAverage;
	s_mapComponentCreateFun["ScriptEngine"] = &createScriptEngine;
	s_mapComponentCreateFun["BuffFactory"] = &createBuffFactory;
	s_mapComponentCreateFun["SkillFactory"] = &createSkillFactory;
	s_mapComponentCreateFun["EffectFactory"] = &createEffectFactory;
	s_mapComponentCreateFun["MessageDispatch"] = &createMessageDispatch;
	s_mapComponentCreateFun["EntityFactory"] = &createJZEntityFactory;
	s_mapComponentCreateFun["LoginManager"] = &createLoginManager;
	s_mapComponentCreateFun["GlobalCfg"] = &createGlobalCfg;
	s_mapComponentCreateFun["ReserveWordCheck"] = &createReserveWordCheck;
	s_mapComponentCreateFun["MessageLayer"] = &createJZMessageLayer;
	s_mapComponentCreateFun["FightFactory"] = &createFightFactory;
	s_mapComponentCreateFun["DungeonFactory"] = &createDungeonFactory;
	s_mapComponentCreateFun["ItemFactory"] = &createItemFactory;
	s_mapComponentCreateFun["DropFactory"] = &createDropFactory;
	s_mapComponentCreateFun["ZoneTime"] = &createZoneTime;
	s_mapComponentCreateFun["ModifyDelegate"] = &createModifyDelegate;
	s_mapComponentCreateFun["TimeRangeValue"] = &createTimeRangeValueFactory;
	s_mapComponentCreateFun["FightSoulFactory"] = &createFightSoulFactory;
	s_mapComponentCreateFun["CleanUpManager"] = &createCleanupManager;
	s_mapComponentCreateFun["ClimbTowerFactory"] = &createClimbTowerFactory;
	s_mapComponentCreateFun["ShopFactory"] = &createShopFactory;
	s_mapComponentCreateFun["LegionFactory"] = &createLegionFactory;
	s_mapComponentCreateFun["CampBattleFactory"] = &createCampBattleFactory;
	s_mapComponentCreateFun["HeroTallentFactory"] = &createHeroTallentFactory;
	s_mapComponentCreateFun["BossFactory"] = &createBossFactory;
	s_mapComponentCreateFun["ArenaFactory"] = &createArenaFactory;
	s_mapComponentCreateFun["VIPFactory"] = &createVIPFactory;
	s_mapComponentCreateFun["TaskFactory"] = &createTaskFactory;
	s_mapComponentCreateFun["GuideFactory"] = &createGuideFactory;
	s_mapComponentCreateFun["DreamLandFactory"] = &createDreamLandFactory;
	s_mapComponentCreateFun["ManorFactory"] = &createManorFactory;
	s_mapComponentCreateFun["OperateFactory"] = &createOperateFactory;
}

void initObjCreateFun()
{
	static bool s_bInitObjectFun = false;
	if(!s_bInitObjectFun)
		s_bInitObjectFun = true;
	else
		return;
	s_mapObjectCreateFun["Actor"] = &createActor;
	s_mapObjectCreateFun["Ghost"] = &createGhost;
	s_mapObjectCreateFun["Monster"] = &createMonster;
	s_mapObjectCreateFun["Hero"] = &createHero;
	s_mapObjectCreateFun["GodAnimal"] = &createGoldAnimal;
	s_mapObjectCreateFun["CalcSystem"] = &createCalcSystem;
	s_mapObjectCreateFun["PropObserveSystem"] = &createPropObserveSystem;
	s_mapObjectCreateFun["HeroSystem"] = &createHeroSystem;
	s_mapObjectCreateFun["GMSystem"] = &createGMSystem;
	s_mapObjectCreateFun["FormationSystem"] = &createFormationSystem;
	s_mapObjectCreateFun["FightSystem"] = &createFightSystem;
	s_mapObjectCreateFun["SkillSystem"] = &createSkillSystem;
	s_mapObjectCreateFun["BuffSystem"] = &createBuffSystem;
	s_mapObjectCreateFun["EffectSystem"] = &createEffectSystem;
	s_mapObjectCreateFun["ItemSystem"] = &createItemSystem;
	s_mapObjectCreateFun["ActorSaveSystem"] = &createActorSaveSystem;
	s_mapObjectCreateFun["DungeonSystem"] = &createDungeonSystem;
	s_mapObjectCreateFun["GodAnimalSystem"] = &createGodAnimalSystem;
	s_mapObjectCreateFun["FightSoulSystem"] = &createFightSoulSystem;
	s_mapObjectCreateFun["HeroEquipSystem"] = &createHeroEquipSystem;
	s_mapObjectCreateFun["ClimbTowerSystem"] = &createClimbTowerSystem;
	s_mapObjectCreateFun["LegionSystem"] = &createLegionSystem;
	s_mapObjectCreateFun["ShopSystem"] = &createShopSystem;
	s_mapObjectCreateFun["CampBattleSystem"] = &createCampBattleSystem;
	s_mapObjectCreateFun["ArenaSystem"] = &createArenaSystem;
	s_mapObjectCreateFun["FriendSystem"] = &createFriendSystem;
	s_mapObjectCreateFun["TaskSystem"] = &createTaskSystem;
	s_mapObjectCreateFun["GuideSystem"] = &createGuideSystem;
	s_mapObjectCreateFun["EquipBagSystem"] = &createEquipBagSystem;
	s_mapObjectCreateFun["HeroRealEquipSystem"] = &createHeroRealEquipSys;
	s_mapObjectCreateFun["DreamLandSystem"] = &createDreamLandSystem;
	s_mapObjectCreateFun["MailSystem"] = &createMailSystem;
	s_mapObjectCreateFun["HeroTallentSystem"] = &createTallentSystem;
	s_mapObjectCreateFun["ManorSystem"] = &createManorSystem;
	s_mapObjectCreateFun["OperateSystem"] = &createOperateSystem;
}

extern "C" IComponent* createComponentStatic(const std::string& strComponentName, Int32 nVersion)
{

	initComponentCreateFun();
	std::map<std::string, COMPONENT_CREATEFUN>::iterator it = s_mapComponentCreateFun.find(strComponentName);
	if(it == s_mapComponentCreateFun.end() )
	{
		return NULL;
	}
	return (it->second)(nVersion);
}

extern "C" IObject* createObjectStatic(const std::string& strObjectName)
{
	initObjCreateFun();
	std::map<std::string, OBJECT_CREATEFUN>::iterator it = s_mapObjectCreateFun.find(strObjectName);
	if(it == s_mapObjectCreateFun.end() )
	{
		return NULL;
	}
	return (it->second)();
}
