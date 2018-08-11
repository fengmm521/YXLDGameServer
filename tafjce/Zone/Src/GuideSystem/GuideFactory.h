#ifndef __GUIDE_FACTORY_H__
#define __GUIDE_FACTORY_H__


class GuideConditionBase
{
public:

	virtual ~GuideConditionBase(){}
	virtual bool checkCondition(HEntity hEntity, int iEventID, EventArgs& args) const = 0;
	virtual int getEventID() const = 0;
};

class GuideCondition_Event:public GuideConditionBase
{
public:

	GuideCondition_Event(int iEventID):m_iEventID(iEventID){}
	virtual bool checkCondition(HEntity hEntity, int iEventID, EventArgs& args) const;
	virtual int getEventID() const;

private:

	int m_iEventID;
};

class GuideCondition_PassScene:public GuideConditionBase
{
public:

	GuideCondition_PassScene(int iSceneID):m_iSceneID(iSceneID){}
	virtual bool checkCondition(HEntity hEntity, int iEventID, EventArgs& args) const;
	virtual int getEventID() const;

private:

	int m_iSceneID;
};

class GuideCondition_FinishGuide:public GuideConditionBase
{
public:

	GuideCondition_FinishGuide(int iFinishGuideID):m_iFinishGuideID(iFinishGuideID){}
	virtual bool checkCondition(HEntity hEntity, int iEventID, EventArgs& args) const;
	virtual int getEventID() const;
	
private:

	int m_iFinishGuideID;
};

class GuideCondition_FunctionOpen:public GuideConditionBase
{
public:

	GuideCondition_FunctionOpen(int iFunctionID):m_iFunctionID(iFunctionID){}
	virtual bool checkCondition(HEntity hEntity, int iEventID, EventArgs& args) const;
	virtual int getEventID() const;

private:

	int m_iFunctionID;
};

class GuideCondition_PassSection:public GuideConditionBase
{
public:

	GuideCondition_PassSection(int iSectionID, bool bFirstPerfectPass):m_iSectionID(iSectionID), m_bFirstPerfectPass(bFirstPerfectPass){}
	virtual bool checkCondition(HEntity hEntity, int iEventID, EventArgs& args) const;
	virtual int getEventID() const;

private:

	int m_iSectionID;
	bool m_bFirstPerfectPass;
};

class GuideCondition_FinishTask:public GuideConditionBase
{
public:

	GuideCondition_FinishTask(int iTaskID):m_iTaskID(iTaskID){}

	virtual bool checkCondition(HEntity hEntity, int iEventID, EventArgs& args) const;
	virtual int getEventID() const;

private:

	int m_iTaskID;
};

class GuideCondition_Level:public GuideConditionBase
{
public:

	GuideCondition_Level(int iLevel):m_iLevel(iLevel){}

	virtual bool checkCondition(HEntity hEntity, int iEventID, EventArgs& args) const;
	virtual int getEventID() const;

private:

	int m_iLevel;
};


/*
class GuideCondition_None : public GuideConditionBase
{
	public:
		GuideCondition_None(){};

		virtual bool checkCondition(HEntity hEntity, int iEventID, EventArgs& args) const
			{
				return false;
			}

		virtual int getEventID() const
			{
				return EVENT_ENTITY_BEGIN;
			}
};
*/

class Guide
{
public:

	friend class GuideFactory;

	Guide();
	~Guide();
	bool checkOpen(HEntity hEntity, int iEventID, EventArgs& args) const;
	
private:
	
	int m_iGuideID;
	int m_iDependGuideID;
	bool m_bMustDo;
	int m_iMinLv;
	int m_iMaxLv;
	vector<GuideConditionBase*> m_conditionList;
	bool m_bRepeat;
	bool m_bAutoFinish;
};


class GuideFactory:public ComponentBase<IGuideFactory, IID_IGuideFactory>
{
public:

	GuideFactory();
	~GuideFactory();
	
	virtual bool initlize(const PropertySet& propSet);
	void loadGuide();
	set<int> getNewOpenGuideList(HEntity hEntity, int iEventID, EventArgs& args);
	GuideConditionBase* parseGuideCondition(const string& strCondition);
	Guide* getGuide(int iGuideID);
	const set<int>& getRelationEventList();
	bool isAutoFinish(int iGuideID);

private:
	map<int, Guide*> m_guideList;
	map<int, set<int> > m_guideEventRelationMap;
	set<int> m_eventList;
};

#endif
