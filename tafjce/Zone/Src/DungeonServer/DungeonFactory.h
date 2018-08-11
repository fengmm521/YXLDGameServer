#ifndef __DUNGEON_FACTORY_H__
#define __DUNGEON_FACTORY_H__


struct PerfectAwardCfg
{
	PerfectAwardCfg():iID(0), iType(0), iItemID(0), iCount(0), iLevelStep(0){}

	int iID;
	int iType;
	int iItemID;
	int iCount;
	int iLevelStep;
};


class DungeonFactory:public ComponentBase<IDungeonFactory, IID_IDungeonFactory>
{
public:

	DungeonFactory();
	~DungeonFactory();

	// IComponent Interface
	virtual bool initlize(const PropertySet& propSet);

	// IDungeonFactory Interface
	virtual const DungeonSection* querySection(int iSectionID);
	virtual const DungeonScene* queryScene(int iSceneID);
	virtual int getFirstSection();
	virtual int getFirstScene(int iSectionID);
	virtual int getNextSection(int iScetionID);
	virtual int getNextScene(int iSceneID);
	virtual int getSceneOwnScetion(int iSceneID);
	virtual vector<int> getSectionList(int iSectionType);

	const PerfectAwardCfg* queryPerfectAward(int iID);

	void loadSection();
	void loadScene();
	void loadPerfectAward();

	void parseFpChanceMap(const std::string& strFpChance, map<int, int>& chanceMap);
	void parseFpEquipChance(const std::string& strChance, vector<ChanceCfg>& equipChanceList);

private:

	typedef map<int, DungeonSection> MapSection;
	typedef map<int, DungeonScene> MapScene;

	MapSection m_sectionMap;
	MapScene m_sceneMap;

	vector<int> m_sectionIDList;
	map<int, int> m_sceneOwnMap;
	map<int, PerfectAwardCfg> m_perfectAwardMap;

	map<int, vector<int> > m_sectionIDListMap;
};

#endif
