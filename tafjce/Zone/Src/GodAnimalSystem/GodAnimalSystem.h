#ifndef __GOD_ANIMALSYSTEM_H__
#define __GOD_ANIMALSYSTEM_H__

class GodAnimalSystem:public ObjectBase<IGodAnimalSystem>, public Detail::EventHandle
{
public:

	GodAnimalSystem();
	~GodAnimalSystem();

	// IEntitySubSystem Interface
	virtual Uint32 getSubsystemID() const;
	virtual Uint32 getMasterHandle();
	virtual bool create(IEntity* pEntity, const std::string& strData);
	virtual bool createComplete();
	virtual const std::vector<Uint32>& getSupportMessage();
	virtual void onMessage(QxMessage* pMessage);
	virtual void packSaveData(string& data);

	// IGodAnimalSystem Interface
	virtual vector<HEntity> getGodAnimalList();
	virtual HEntity getActiveGodAnimal();
	virtual HEntity addGodAnimal(int iBaseID, bool bNotifyError, int iLevelStep = 0);
	virtual HEntity addGodAnimalWithLevelStep(int iBaseID, bool bNotifyError, int iLevelStep);
	virtual void autoActiveGodAnimal();

	void initGodAnimalData(const string& strData);
	void onEventSend2Client(EventArgs& args);
	bool fillGodAnimalData(int iBaseID, ServerEngine::RoleSaveData& data);

	void onReqTrainGodAnimal(const GSProto::CSMessage& msg);
	void onReqSetActive(const GSProto::CSMessage& msg);
	void onQueryGodAnimalLevelStep(const GSProto::CSMessage& msg);
	void onExeGodAnimalLevelStepUp(const GSProto::CSMessage& msg);
	void onQueryGodAnimalInherit(const GSProto::CSMessage& msg);
	void onExeGodAnimalInherit(const GSProto::CSMessage& msg);
	void onQueryGodAnimalDetail(const GSProto::CSMessage& msg);
	void onReqQueryDetailByID(const GSProto::CSMessage& msg);
	
	void fillGoldAnimalDetail(HEntity hGodAnimal, GSProto::GoldAnimalDetail& detailInfo, bool bSetSkill);
	void fillGoldAnimalRoleBase(HEntity hGodAnimal, GSProto::GoldAnimalRoleBase& baseInfo);
	void fillLevelStepUpConsume(HEntity hGodAnimal,int iLevelStep, google::protobuf::RepeatedPtrField< ::GSProto::ConsumeItem>* pszConsume);

	void sendGodAnimalLevelStepInfo(HEntity hGodAnimal);
	bool checkInheritValid(HEntity hPassGodAnimal, HEntity hAcceptGodAnimal);
	void fillGodAnimalLevelStep(HEntity hGodAnimal, GSProto::GoldAnimalLevelStepInfo* pLevelStepInfo);
	void sendGoldAnimalUpdate(HEntity hGodAnimal);
	void notifyLevelStepUp(int iGodAnimalID, int iPreLevelStep, int iCurLevelStep);
	//add by hyf
protected:
	void onReqQueryAnimalSoul(const GSProto::CSMessage& msg);
	void onReqActiveAnimal(const GSProto::CSMessage& msg);
	void onReqSellAnimal(const GSProto::CSMessage& msg);

private:

	HEntity m_hEntity;
	HEntity m_hActiveGodAnimal;
	vector<HEntity> m_godAnimalList;
};

#endif
