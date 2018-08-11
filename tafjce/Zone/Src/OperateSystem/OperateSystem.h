#ifndef OPERATESYSTEM_H_
#define OPERATESYSTEM_H_


class OperateSystem:public ObjectBase<IOperateSystem>, public Detail::EventHandle
,public ITimerCallback
{
public:

	OperateSystem();
	virtual ~OperateSystem();

	// IEntitySubsystem interface
	virtual Uint32 getSubsystemID() const;
	virtual Uint32 getMasterHandle();
	virtual bool create(IEntity* pEntity, const std::string& strData);
	virtual bool createComplete();
	virtual const std::vector<Uint32>& getSupportMessage();
	virtual void onMessage(QxMessage* pMessage);
	virtual void packSaveData(string& data);
	
	virtual void onTimer(int nEventId);
public:
	
	//virtual bool activeHaveBuy(int iActiveId);
	virtual void gmGetPhoneSteam(string strPhoneNum);
	virtual void gmLogin();

	virtual void IncDayPaymentPaied(int _dt);
protected:

	void onQueryCheckInInfo(const GSProto::CSMessage& msg);
	void onExeCheckIn(const GSProto::CSMessage& msg);
	bool initData(const std::string& strData);
	void checkIncheckReset();
	void checkNotice();

	void onQueryGrowUpPlanInfo(const GSProto::CSMessage& msg);
	void onBuyGrowUpPlan(const GSProto::CSMessage& msg);
	void onGetGrowUpPlanReward(const GSProto::CSMessage& msg);
	void sendGrowUpPlanInfo();

	void onQueryAccPaymentInfo(const GSProto::CSMessage& msg);
	void onGetAccPaymentReward(const GSProto::CSMessage& msg);

	void onQueryOpenaActive(const GSProto::CSMessage& msg);
	void checkGrowUpNotice();
	void checkAccPayMentNotice();
	
	void onGetPhoneStream(const GSProto::CSMessage& msg);
	void onQueryAccLogin(const GSProto::CSMessage& msg);
	void onGetAccLoginReward(const GSProto::CSMessage& msg);

	/**
	 * 查询日充值活动详细数据
	 */
	void onQueryDayPaymentDetail(const GSProto::CSMessage& _msg);

	/**
	 * 领取日充值活动奖励
	 */
	void onGetDayPaymentReward(const GSProto::CSMessage& _msg);

private:
	
	/**
	 * 重置玩家日充值相关数据(如operateId不匹配)
	 */
	void __resetDayPaymentActorData();
	
private:
	HEntity m_hEntity;

	ServerEngine::CheckInData m_CheckInData;
	
	ITimerComponent::TimerHandle m_CheckNoticeTimerHandle;
	
	ServerEngine::GrowUpData m_growUpdata;

	ServerEngine::AccPayMentData     m_accPayMentData;
	bool m_bHaveGetTelphoneSteam;

	ServerEngine::ActorLoginData m_actorLoginData;

	ServerEngine::DayPaymentData	m_dayPaymentData;	// 角色的日累积充值相关数据
	
	
};
#endif