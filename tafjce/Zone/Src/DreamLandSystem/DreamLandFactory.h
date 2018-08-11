#ifndef _DREAMLANDFACTORY_H
#define _DREAMLANDFACTORY_H

class DreamLandFactory : public ComponentBase<IDreamLandFactory,IID_IDreamLandFactory>
{
public:
	DreamLandFactory();
	virtual ~DreamLandFactory();

	//interface

	virtual bool initlize(const PropertySet& propSet);
		//获取章节信息
	virtual bool querySectionInfo(int iDreamLandId,DreamLandSection& section);

	//获取 第一个章节
	virtual bool getFristSection(DreamLandSection& section);

	//获取关卡信息
	virtual  bool  querySceneInfo(int iSceneID,DreamLandScene& scene);

	//获取宝箱信息
	virtual bool queryDreamLandBoxInfo(int iDreamLandBoxId,DreamLandBox& box);
	//获取下一个章节
	virtual int queryNextSectionId(int iNowSectionId);
	
	virtual int queryNextSceneinfo(int iSectionId, int iSceneId,DreamLandScene& scene);

protected:
	void loadData();

private:

	//章节列表

	std::vector<int> m_VecSectionIdVec;

	//章节数据
	std::map<int,DreamLandSection> m_MapSectionInfo;

	//关卡数据
	std::map<int, DreamLandScene> m_MapSceneInfo;

	//宝箱数据
	std::map<int, DreamLandBox> m_MapBoxInfo;
};

#endif
