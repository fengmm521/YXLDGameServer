#ifndef __IDUNGEON_SYSTEM_H__
#define __IDUNGEON_SYSTEM_H__

struct CloseAttCommUP
{
	CloseAttCommUP(HEntity hEntity);
	
	virtual ~CloseAttCommUP();
private:
	IEntity* m_pEntity;
	
};


class IDungeonSystem:public IEntitySubsystem
{
public:

	virtual bool getSceneStar(int iSceneID, int& iPassStar) = 0;

	virtual bool isSectionOpend(int iSectionID) = 0;

	virtual int getCurSectionID() = 0;

	virtual bool canSkipCurScene() = 0;

	//add by hyf 2014/7/2

	virtual bool getSectionHavePass(int iSectionID) = 0;
	virtual bool getSeceneHavePass(int isceneID) = 0;
};


#endif
