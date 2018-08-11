#ifndef __IMAIL_SYSTEM_H__
#define __IMAIL_SYSTEM_H__

class IMailSystem:public IEntitySubsystem
{
public:

	virtual void addMail(const std::string& strSender, const std::string& strTitle, const std::string& strContent, const GSProto::FightAwardResult& awardInfo, const string& strReason) = 0;
	virtual void addMail(const std::string& strUUID, const ServerEngine::MailData& mailData, const string& strReason ) = 0;
	virtual void sendPayReturnMail(int iMoney) = 0;
	virtual void sendLifeLongVIP(int iVIPLevel) = 0;
};

#endif

