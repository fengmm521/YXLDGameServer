#include "QQYYBPayServer.h"
#include "QQYYBPayImp.h"
#include "util/tc_http.h"
// #include "util/tc_json.h"
// #include "sig_check.h"

using namespace taf;

#define PAYLOG(strMethod, queryKey) FDLOG("QQYYBPay")<<strMethod<<"|"<<queryKey.strOpenID<<"|"<<queryKey.strPf<<"|"<<queryKey.strPfKey<<"|"

extern TC_HttpAsync g_httpAsync;

void QQYYBPayImp::initialize()
{
	string strConfigFile = ServerConfig::ServerName + ".conf";
	TC_Config tmpCfg;
	tmpCfg.parseFile(strConfigFile);

	m_strAPPID = tmpCfg.get("/QQYYBPayServer<AppID>", "");
	m_strAppKey = tmpCfg.get("/QQYYBPayServer<AppKey>", "");

	bool bDebug = TC_Common::strto<int>(tmpCfg.get("/QQYYBPayServer<Debug>", "1"))== 1;
	if(bDebug)
	{
		m_strUrlHost = tmpCfg.get("/QQYYBPayServer<DebugHost>", "");
	}
	else
	{
		m_strUrlHost = tmpCfg.get("/QQYYBPayServer<ReleaseHost>", "");
	}

	FDLOG("QQYYBPay")<<"ReadArgs|"<<m_strAPPID<<"|"<<m_strAppKey<<"|"<<m_strUrlHost<<endl;
}

void QQYYBPayImp::destroy()
{

}

void QQYYBPayImp::fillCookie(TC_HttpRequest& request, const string& strAcType, const string& strUrl)
{
	// NS_SIG_CHECK::CSigCheck helper;

	// // 坑了，客户端写的时候反了，这里也反过来吧
	// stringstream ss;
	// if(strAcType == "qq")
	// {
	// 	ss<<"session_id=hy_gameid; session_type=wc_actoken; org_loc=" + helper.url_encode(strUrl);
		
	// }
	// else if(strAcType == "wx")
	// {
	// 	ss<<"session_id=openid; session_type=kp_actoken; org_loc=" + helper.url_encode(strUrl);
	// }

	// string strCookie = ss.str();
	// request.setCookie(strCookie);

	// FDLOG("QQYYBPay")<<"Cookie:"<<strCookie<<endl;
}


class QueryYuanBaoCallback:public TC_HttpAsync::RequestCallback
{
public:

	QueryYuanBaoCallback(taf::JceCurrentPtr current, ServerEngine::QQYYBPayQueryKey key, const string& strUrl):m_current(current)
		, m_queryKey(key), m_strUrl(strUrl){}

	virtual void onException(const string &ex) 
    {
        //cout << "onException:" << m_strUrl << ":" << ex << endl;
        PAYLOG("queryYuanBao", m_queryKey)<< "onException:" << m_strUrl << ":" << ex << endl;
    }
	
    virtual void onResponse(bool bClose, TC_HttpResponse &stHttpRep) 
    {
		string strContent = stHttpRep.getContent();
		PAYLOG("queryYuanBao", m_queryKey)<<"HttpQueryResp|"<<strContent<<endl;

		// JsonValuePtr retValue = TC_Json::getValue(strContent);
		// JsonValueObjPtr pObj=JsonValueObjPtr::dynamicCast(retValue);
		
		// JsonValuePtr retPtr = pObj->value["ret"];
		// int queryRet = JsonValueNumPtr::dynamicCast(retPtr)->value;

		// int iCurCoin = 0;
		// int iSendCoin = 0;
		// bool bFirstSave = false;
		// int iSumCoin = 0;
		
		// if(pObj->value.find("balance") != pObj->value.end() ) iCurCoin = JsonValueNumPtr::dynamicCast(pObj->value["balance"])->value;
		// if(pObj->value.find("gen_balance") != pObj->value.end() )  iSendCoin = JsonValueNumPtr::dynamicCast(pObj->value["gen_balance"])->value;
		// if(pObj->value.find("first_save") != pObj->value.end() )   bFirstSave = JsonValueNumPtr::dynamicCast(pObj->value["first_save"])->value == 1;
		// if(pObj->value.find("save_amt") != pObj->value.end() )  iSumCoin = JsonValueNumPtr::dynamicCast(pObj->value["save_amt"])->value;

		
		// if(0 == queryRet)
		// {
		// 	ServerEngine::QQYYBPay::async_response_queryYuanBao(m_current, ServerEngine::en_QQYYBPayRet_OK, iCurCoin, iSendCoin, bFirstSave, iSumCoin);
		// 	return ;
		// }
		// else if(1018 == queryRet)
		// {
		// 	ServerEngine::QQYYBPay::async_response_queryYuanBao(m_current, ServerEngine::en_QQYYBPayRet_INVALIDLOGIN, iCurCoin, iSendCoin, bFirstSave, iSumCoin);
		// 	return;
		// }

		// ServerEngine::QQYYBPay::async_response_queryYuanBao(m_current, ServerEngine::en_QQYYBPayRet_ERROR, iCurCoin, iSendCoin, bFirstSave, iSumCoin);
    
    }
	
    virtual void onTimeout() 
    {
        PAYLOG("queryYuanBao", m_queryKey) << "onTimeout:" << m_strUrl << endl;
    }
	
    virtual void onClose()
    {
        PAYLOG("queryYuanBao", m_queryKey) << "onClose:" << m_strUrl << endl;
    }

private:

	taf::JceCurrentPtr m_current;
	ServerEngine::QQYYBPayQueryKey m_queryKey;
	string m_strUrl;
};


taf::Int32 QQYYBPayImp::queryYuanBao(const ServerEngine::QQYYBPayQueryKey & queryKey,taf::Int32 &iCurCoin,taf::Int32 &iSendCoin,taf::Bool &bFirstSave,taf::Int32 &iSumCoin,taf::JceCurrentPtr current)
{	
	current->setResponse(false);

	TC_HttpRequest httpRequest;

	// 验证渠道
	string strChannel = queryKey.strChannel;

	size_t sPos = strChannel.find("_");
	if(sPos == string::npos)
	{
		return ServerEngine::en_QQYYBPatRet_INVALIDCHANNEL;
	}


	// 只支持QQYYB
	string strMainChannel = strChannel.substr(0, sPos);
	if(strMainChannel != "QQYYB")
	{
		return ServerEngine::en_QQYYBPatRet_INVALIDCHANNEL;
	}

	string strAcType = strChannel.substr(sPos + 1);

	// 填充Cookie
	fillCookie(httpRequest, strAcType, "/mpay/get_balance_m");
	
	httpRequest.setCacheControl("no-cache");
	httpRequest.setUserAgent("Mozilla/4.0 (compatible; MSIE 6.0; Windows NT 5.0; InfoPath.1; .NET CLR 1.1.4322)");

	// stringstream ss;
	// ss<<m_strUrlHost<<"/mpay/get_balance_m?";


	// NS_SIG_CHECK::StSigInfo sigData;
	// sigData.method = "GET";
	// sigData.url_path = "/mpay/get_balance_m";
	// sigData.share_secret = m_strAppKey + "&";

	// fillParamMap(sigData.params, queryKey);

	// NS_SIG_CHECK::CSigCheck sigCheckObj;
	// for(map<string, string>::const_iterator it = sigData.params.begin(); it != sigData.params.end(); it++)
	// {
	// 	if(it == sigData.params.begin() )
	// 	{
	// 		ss<<it->first<<"="<<it->second;
	// 	}
	// 	else
	// 	{
	// 		ss<<"&"<<it->first<<"="<<it->second;
	// 	}
	// }
	
	
	// string strSig = sigCheckObj.Create(sigData);
	
	// PAYLOG("queryYuanBao", queryKey)<<"sig|"<<strSig<<endl;
	
	// ss<<"&sig="<<sigCheckObj.url_encode(strSig);
	// string strRequestFullUrl = ss.str();
	
	// httpRequest.setGetRequest(strRequestFullUrl, true);

	
	// PAYLOG("queryYuanBao", queryKey)<<"RequestUrl|"<<strRequestFullUrl<<endl;

	// TC_HttpAsync::RequestCallbackPtr callbackPtr = new QueryYuanBaoCallback(current, queryKey, strRequestFullUrl);
	// g_httpAsync.doAsyncRequest(httpRequest, callbackPtr);

	return 0;
}


void QQYYBPayImp::fillParamMap(map<string, string>& paramMap, const ServerEngine::QQYYBPayQueryKey & queryKey)
{
	paramMap["openid"] = queryKey.strOpenID;

	// 微信要交换下位置, 注意渠道在客户端的时候写反了
	paramMap["openkey"] = queryKey.strOpenKey;
	paramMap["pay_token"] = queryKey.strPayToken;
	if(queryKey.strChannel == "QQYYB_qq")
	{
		paramMap["pay_token"] = "";
	}
	
	paramMap["appid"] = m_strAPPID;
	paramMap["ts"] = TC_Common::tostr<long>( (long)time(0));
	paramMap["pf"] = queryKey.strPf;
	paramMap["pfkey"] = queryKey.strPfKey;
	paramMap["zoneid"] = TC_Common::tostr<int>(queryKey.iZoneID);
	//paramMap["accounttype"] = "common";
 	paramMap["format"] = "json";
}

class PayYuanBaoCallback:public TC_HttpAsync::RequestCallback
{
public:

	PayYuanBaoCallback(taf::JceCurrentPtr current, ServerEngine::QQYYBPayQueryKey key, const string& strUrl):m_current(current)
		, m_queryKey(key), m_strUrl(strUrl){}

	virtual void onException(const string &ex) 
    {
        PAYLOG("payYuanBao", m_queryKey)<< "onException:" << m_strUrl << ":" << ex << endl;
    }
	
    virtual void onResponse(bool bClose, TC_HttpResponse &stHttpRep) 
    {
		string strContent = stHttpRep.getContent();
		PAYLOG("payYuanBao", m_queryKey)<<"HttpQueryResp|"<<strContent<<endl;

		// JsonValuePtr retValue = TC_Json::getValue(strContent);
		// JsonValueObjPtr pObj=JsonValueObjPtr::dynamicCast(retValue);
		 
		// int costRet = JsonValueNumPtr::dynamicCast(pObj->value["ret"])->value;

		// string strBillNO;
		// int iLeftValue = 0;
		// if(pObj->value.find("billno") != pObj->value.end() ) strBillNO = JsonValueStringPtr::dynamicCast(pObj->value["billno"])->value;
		// if(pObj->value.find("balance") != pObj->value.end() ) iLeftValue = JsonValueNumPtr::dynamicCast(pObj->value["balance"])->value;

		// PAYLOG("payYuanBao", m_queryKey)<<"Result|"<<costRet<<"|"<<strBillNO<<"|"<<iLeftValue<<endl;
		
		// if(costRet == 0)
		// {
		// 	ServerEngine::QQYYBPay::async_response_payYuanBao(m_current, ServerEngine::en_QQYYBPayRet_OK);
		// 	return;
		// }
		// else if(costRet == 1004)
		// {
		// 	ServerEngine::QQYYBPay::async_response_payYuanBao(m_current, ServerEngine::en_QQYYBPayRet_NEEDCOIN);
		// 	return ;
		// }
		// else if(costRet == 1018)
		// {
		// 	ServerEngine::QQYYBPay::async_response_payYuanBao(m_current, ServerEngine::en_QQYYBPayRet_INVALIDLOGIN);
		// 	return;
		// 	//return ServerEngine::en_QQYYBPayRet_INVALIDLOGIN;
		// }

		// ServerEngine::QQYYBPay::async_response_payYuanBao(m_current, ServerEngine::en_QQYYBPayRet_ERROR);
		//return ServerEngine::en_QQYYBPayRet_ERROR;
	}
	
    virtual void onTimeout() 
    {
        PAYLOG("payYuanBao", m_queryKey) << "onTimeout:" << m_strUrl << endl;
    }
	
    virtual void onClose()
    {
        PAYLOG("payYuanBao", m_queryKey) << "onClose:" << m_strUrl << endl;
    }

private:

	taf::JceCurrentPtr m_current;
	ServerEngine::QQYYBPayQueryKey m_queryKey;
	string m_strUrl;
};



taf::Int32 QQYYBPayImp::payYuanBao(const ServerEngine::QQYYBPayQueryKey & queryKey,taf::Int32 iCostNum,taf::JceCurrentPtr current)
{
	current->setResponse(false);

	TC_HttpRequest httpRequest;

	// 验证渠道
	string strChannel = queryKey.strChannel;

	size_t sPos = strChannel.find("_");
	if(sPos == string::npos)
	{
		return ServerEngine::en_QQYYBPatRet_INVALIDCHANNEL;
	}


	// 只支持QQYYB
	string strMainChannel = strChannel.substr(0, sPos);
	if(strMainChannel != "QQYYB")
	{
		return ServerEngine::en_QQYYBPatRet_INVALIDCHANNEL;
	}

	string strAcType = strChannel.substr(sPos + 1);

	// 填充Cookie
	fillCookie(httpRequest, strAcType, "/mpay/pay_m");

	
	httpRequest.setCacheControl("no-cache");
	httpRequest.setUserAgent("Mozilla/4.0 (compatible; MSIE 6.0; Windows NT 5.0; InfoPath.1; .NET CLR 1.1.4322)");

	// stringstream ss;
	// ss<<m_strUrlHost<<"/mpay/pay_m?";


	// NS_SIG_CHECK::StSigInfo sigData;
	// sigData.method = "GET";
	// sigData.url_path = "/mpay/pay_m";
	// sigData.share_secret = m_strAppKey + "&";

	// fillParamMap(sigData.params, queryKey);
	// sigData.params["amt"] = TC_Common::tostr<int>(iCostNum);

	// NS_SIG_CHECK::CSigCheck sigCheckObj;
	// for(map<string, string>::const_iterator it = sigData.params.begin(); it != sigData.params.end(); it++)
	// {
	// 	if(it == sigData.params.begin() )
	// 	{
	// 		ss<<it->first<<"="<<it->second;
	// 	}
	// 	else
	// 	{
	// 		ss<<"&"<<it->first<<"="<<it->second;
	// 	}
	// }
	
	
	// string strSig = sigCheckObj.Create(sigData);
	
	// PAYLOG("payYuanBao", queryKey)<<"sig|"<<strSig<<endl;
	
	// ss<<"&sig="<<sigCheckObj.url_encode(strSig);
	// string strRequestFullUrl = ss.str();
	
	// httpRequest.setGetRequest(strRequestFullUrl, true);

	
	// PAYLOG("payYuanBao", queryKey)<<"RequestUrl|"<<strRequestFullUrl<<endl;

	// TC_HttpAsync::RequestCallbackPtr callbackPtr = new PayYuanBaoCallback(current, queryKey, strRequestFullUrl);
	// g_httpAsync.doAsyncRequest(httpRequest, callbackPtr);

	return ServerEngine::en_QQYYBPayRet_OK;
}




