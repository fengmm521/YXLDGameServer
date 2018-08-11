#ifndef __NAMEIMP_H__
#define __NAMEIMP_H__

#include "QQYYBPay.h"
#include "DbAccess.h"
#include "hiredis.h"
#include "MiniApr.h"
#include "util/tc_logger.h"
#include "util/tc_http_async.h"


using namespace MINIAPR;


class QQYYBPayImp:public ServerEngine::QQYYBPay
{
public:

	// Servant Interface
	virtual void initialize();
    virtual void destroy();

	virtual taf::Int32 queryYuanBao(const ServerEngine::QQYYBPayQueryKey & queryKey,taf::Int32 &iCurCoin,taf::Int32 &iSendCoin, taf::Bool &bFirstSave,taf::Int32 &iSumCoin,taf::JceCurrentPtr current);
    virtual taf::Int32 payYuanBao(const ServerEngine::QQYYBPayQueryKey & queryKey,taf::Int32 iCostNum,taf::JceCurrentPtr current);

	void fillParamMap(map<string, string>& paramMap, const ServerEngine::QQYYBPayQueryKey & queryKey);

	void fillCookie(TC_HttpRequest& request, const string& strAcType, const string& strUrl);


private:

	string m_strAPPID;
	string m_strAppKey;
	string m_strUrlHost;
};



#endif

