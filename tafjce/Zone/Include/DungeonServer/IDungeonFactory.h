#ifndef __IDUNGEON_FACTORY_H__
#define __IDUNGEON_FACTORY_H__

struct DungeonSection
{
	DungeonSection():iSectionID(0), iDependSecionID(0), iSectionType(0), iLevelCondition(0){}

	int iSectionID;
	int iDependSecionID;
	int iSectionType;
	int iLevelCondition;
	vector<int> sceneIDList;
	vector<int> perfectIDList;
};


struct ChanceCfg
{
	ChanceCfg():iID(0), iChance(0){}

	int iID;
	int iChance;
};

struct DungeonScene
{
	DungeonScene():iSceneID(0), iPreSceneID(0), iExp(0), iSilver(0), iOwnSectionID(0), bConditionAddHero(false), iHeroGetExp(0), 
		bBeginDlg(false), iLimitPassCount(0){}

	int iSceneID;
	int iPreSceneID;
	vector<int> monsterGrpList;
	//int iMonsterGrpID;
	int iExp;
	int iSilver;

	vector<int> firstDropIDList;
	vector<int> dropIDList;
	vector<int> firstPerfectIDList;
	int iOwnSectionID;
	bool bConditionAddHero;
	int iHeroGetExp;
	vector<int> dlgList;
	bool bBeginDlg;

	int iLimitPassCount;

	// ÐÂÔö
	map<int, int> fpChanceList;
	vector<ChanceCfg> eliteEquipList;
	vector<ChanceCfg> normalEquipList;
};


class IDungeonFactory:public IComponent
{
public:

	virtual const DungeonSection* querySection(int iSectionID) = 0;

	virtual const DungeonScene* queryScene(int iSceneID) = 0;

	virtual int getFirstSection() = 0;

	virtual int getFirstScene(int iSectionID) = 0;

	virtual int getNextSection(int iScetionID) = 0;

	virtual int getNextScene(int iSceneID) = 0;

	virtual int getSceneOwnScetion(int iSceneID) = 0;

	virtual vector<int> getSectionList(int iSectionType) = 0;
};



#endif
