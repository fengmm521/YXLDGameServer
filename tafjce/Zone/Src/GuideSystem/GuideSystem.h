#ifndef __GUIDE_SYSTEM_H__
#define __GUIDE_SYSTEM_H__

struct GuideEventHelper:public Detail::EventHandle
{
	GuideEventHelper(HEntity hEntity, int iEventID):m_hEntity(hEntity), m_iEventID(iEventID){}
	void onEventArgs(EventArgs& args);

	HEntity m_hEntity;
	int m_iEventID;
};

class GuideSystem:public ObjectBase<IGuideSystem>, public Detail::EventHandle
{
public:

	GuideSystem();
	~GuideSystem();

	// IEntitySubSystem Interface
	virtual Uint32 getSubsystemID() const;
	virtual Uint32 getMasterHandle();
	virtual bool create(IEntity* pEntity, const std::string& strData);
	virtual bool createComplete();
	virtual const std::vector<Uint32>& getSupportMessage();
	virtual void onMessage(QxMessage* pMessage);
	virtual void packSaveData(string& data);

	// IGudeSystem Interface
	virtual bool isGuideOpened(int iGuideID);
	virtual bool isGuideFinished(int iGuideID);

	void onEventSend2Client(EventArgs& args);
	void checkGuideOpen(int iEventID, EventArgs& args, set<int>& newOpenList);
	void notifyGuide(int iGuideID);
	void onGuideRelationEvent(int iEventID, EventArgs& args);
	void registerGuideEvent();
	void onReqGuideFinish(const GSProto::CSMessage& msg);
	bool finishGuide(int iGuideID);

private:

	HEntity m_hEntity;
	set<int> m_finishedList;
	set<int> m_opendList;
	vector<GuideEventHelper*> m_eventHelperList;
};

#endif
