
#ifndef __MAIL_SYSTEM_H__
#define __MAIL_SYSTEM_H__

class MailSystem:public ObjectBase<IMailSystem>,public ITimerCallback ,public Detail::EventHandle
{
public:

	MailSystem();
	~MailSystem();

	// IEntitySubSystem Interface
	virtual Uint32 getSubsystemID() const;
	virtual Uint32 getMasterHandle();
	virtual bool create(IEntity* pEntity, const std::string& strData);
	virtual bool createComplete();
	virtual const std::vector<Uint32>& getSupportMessage();
	virtual void onMessage(QxMessage* pMessage);
	virtual void packSaveData(string& data);

	// IMailSystem Interface
	virtual void addMail(const std::string& strSender, const std::string& strTitle, const std::string& strContent, const GSProto::FightAwardResult& awardInfo, const string& strReason);
	virtual void onTimer(int nEventId);
	virtual void addMail(const std::string& strUUID, const ServerEngine::MailData& mailData,const string& strReason) ;
	void sendPayReturnMail(int iMoney);
	void sendLifeLongVIP(int iVIPLevel);

public:
	void ActorLevelUp(EventArgs& args );
private:

	void initData(const std::string& strData);

	void onReqQueryMailList(const GSProto::CSMessage& msg);
	void onReqQueryMailDesc(const GSProto::CSMessage& msg);
	void onReqGetMailAward(const GSProto::CSMessage& msg);

	void fillBroadcastMail();
	void fillAranaMail();
	//addbyHyf
	void checkMailNotic();
	void fillLoginAward();
	void DayCheck();
	
private:

	HEntity m_hEntity;

	// 邮件数据存储
	map<string, ServerEngine::MailData> m_mailDataList;  // 邮件列表
	set<int> m_usedBroadcastList;    // 已经用掉的公告ID
	ITimerComponent::TimerHandle m_CheckDayResetMail;
	 taf::UInt32 m_dwChgSecond;
};



#endif
