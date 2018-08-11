#ifndef __FIGHTSOULSYSTEM_H__
#define __FIGHTSOULSYSTEM_H__


class FightSoulSystem:public ObjectBase<IFightSoulSystem>, public IFightSoulContainer, public Detail::EventHandle
{
public:

	FightSoulSystem();
	~FightSoulSystem();

	// IEntitySubSystem Interface
	virtual Uint32 getSubsystemID() const;
	virtual Uint32 getMasterHandle();
	virtual bool create(IEntity* pEntity, const std::string& strData);
	virtual bool createComplete();
	virtual const std::vector<Uint32>& getSupportMessage();
	virtual void onMessage(QxMessage* pMessage);
	virtual void packSaveData(string& data);
	virtual void* queryInterface(int iInterfaceID);

	// IFightSoulSystem Interface
	virtual Uint32 addFightSoul(int iFightSoulID, bool bNotifyErr);
	virtual Uint32 getHFightSoulFromPos(int iPos);
	virtual bool practice(bool bNotofyError);
	virtual void autoCombine();
	virtual void enableAdvanceMode();
	virtual void exchangeFightSoul(int iExchangeID);

	// IFightSoulContainer Interface
	virtual Int32 getSpace();
	virtual Int32 getSize();
	virtual Uint32 getFightSoul(int iPos);
	virtual bool setFightSoul(int iPos, Uint32 hFightSoulHandle, GSProto::FightSoulChgItem* pOutItem);
	virtual HEntity getOwner();
	virtual bool canSetFightSoul(int iPos, Uint32 hFightSoulHandle, bool bNoitifyErr);
	virtual void sendContainerChg(const vector<int>& posList);
	virtual void combine(int iSrcConType, int iSrcPos, int iDstConType, int iDstPos, Uint32 dwHeroObjectID);

	int getCallAnvanceLeftCount();
	
private:
	
	void initFightSoulData(const ServerEngine::FightSoulSystemData& saveData);
	void onEventSend2Client(EventArgs& args);
	bool isFull();
	
	void onMoveFightSoulMsg(const GSProto::CSMessage& req);
	void onReqAutoCombine(const GSProto::CSMessage& req);
	void onReqQueryHeroFightSoul(const GSProto::CSMessage& req);
	void onReqEnableAdvance(const GSProto::CSMessage& req);
	void onReqLock(const GSProto::CSMessage& req);
	void onReqExchange(const GSProto::CSMessage& req);
	IFightSoulContainer* getContainer(int iType, HEntity hHero);
	bool findAutoCombHost(vector<int>& posList, int& iTargetPos);
	int getCurrentGrade();
	void _doFSMove(int iSrcConType, int iSrcPos, int iDstConType, int iDstPos, HEntity hHero, bool bConfirm);
	bool checkMoveValid(IFightSoulContainer* pSrcContainer, int iSrcPos, IFightSoulContainer* pDstContainer, int iDstPos);
	void notifyGoldFightSoul(int iFightSoulID);
	void autoPractice();

private:

	HEntity m_hEntity;
	vector<HFightSoul> m_fightSoulBag;
	ServerEngine::FightSoulPractice m_practiceStatus;
	TimeRangeKeeper m_callAdvanceCount;
	bool m_bUserPracticeFightSoul;
};


#endif
