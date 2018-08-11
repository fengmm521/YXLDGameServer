#ifndef _I_DREAMLANDFACTORY_H_
#define _I_DREAMLANDFACTORY_H_

struct DreamLandSection
{
	DreamLandSection():iSectionId(0){}
	int iSectionId;
	std::vector<int> sceneIdVec;
};

enum enSceneType
{
	eSeceneType_None = 0,
	eSeceneType_Normal = 1,
	eSeceneType_Special = 2,
};

struct DreamLandScene
{
	DreamLandScene():iSceneId(0),iFightMaxWeight(0),iFightMinWeight(0),iDreamLandBoxId(0),iMonsterGrupId(-1){}
	int iSceneId; 	
	enSceneType iSceneType;   //类型 
	int iFightMaxWeight; 		//筛选上限人数
	int iFightMinWeight;		//筛选下限人数

	map<int,int> addFightmap;   //战力加成

	int iDreamLandBoxId;        //幻境宝箱 ID

	int iMonsterGrupId;        //怪物组ID
};

struct DreamLandBox
{
	DreamLandBox():iDreamLandBoxId(0){}
	int iDreamLandBoxId;

	vector<int> m_RewardVec;

	vector<int> m_magicRewardVec;			//神秘礼物掉落ID(掉落表中使用)
};


class IDreamLandFactory :public IComponent
{
public:
	//获取章节信息
	virtual bool querySectionInfo(int iDreamLandId,DreamLandSection& section) = 0;

	//获取 第一个章节
	virtual bool getFristSection(DreamLandSection& section) = 0;

	//获取关卡信息
	virtual  bool  querySceneInfo(int iSceneID,DreamLandScene& scene) = 0;

	//获取宝箱信息
	virtual  bool queryDreamLandBoxInfo(int iDreamLandBoxId,DreamLandBox& box) = 0;

	//获取下一个章节信息
	virtual int queryNextSectionId(int iNowSectionId) = 0;

	virtual int queryNextSceneinfo(int iSectionId, int iSceneId,DreamLandScene& scene) = 0;

};

#endif




