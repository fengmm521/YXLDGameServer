#include "LJSDKImp.h"
#include <sys/time.h>
#include <sys/resource.h>
#include "util/tc_http.h"
#include "sig_check.h"
// #include "util/tc_json.h"
#include "util/tc_md5.h"
#include "util/tc_logger.h"
#include "util/tc_base64.h"
#include "httpclient.h"


//const string g_appKey =  "KfvpPibrLiA4sj41&";

extern TC_HttpAsync g_httpAsync;

LJSDKImp::LJSDKImp()
{
}

LJSDKImp::~LJSDKImp()
{
}


void LJSDKImp::initialize()
{
	//TC_Config tmpCfg;
	//string strConfigFile = ServerConfig::ConfigFile;
	//tmpCfg.parseFile(strConfigFile);

	// 设置最大连接数限制
	/*struct rlimit tmpLimit;
	tmpLimit.rlim_cur = 30000;
	tmpLimit.rlim_max = 30000;
	if(0 != setrlimit(RLIMIT_NOFILE, &tmpLimit) )
	{
		int iErrNO = errno;
		FDLOG("Error")<<"set socket rlimit fail|"<<iErrNO<<endl;
		assert(false);
		return;
	}

	string strEndPoint = tmpCfg.get("/taf/application/server/Push<endpoint>");
	m_strLocalPushObject = ServerConfig::Application + "." + "TestPayServer" + ".PushObj@ " + strEndPoint;
	g_strLocalPushObj = m_strLocalPushObject;

	loadSnapshotServer();*/	
		/*
	string strConfigFile = ServerConfig::ServerName + ".conf";
	TC_Config tmpCfg;
	tmpCfg.parseFile(strConfigFile);

	m_strAPPID = tmpCfg.get("/LJSDKServer<productCode>", "");
	m_strProductSecretKey = tmpCfg.get("/LJSDKServer<productSecret>", "");
	FDLOG("LJSDKImp")<<"ReadArgs|"<<m_strAPPID<<endl;

	vector<string> gameNameList;
	if(!tmpCfg.getDomainVector("/LJSDKServer/OuterObj/GameServerlist", gameNameList) )
	{
		throw runtime_error("invalid config /LJSDKServer/OuterObj/GameServerlist");
		return;
	}
	
	for(size_t i = 0; i < gameNameList.size(); i++)
	{
		string strTmpServerName = gameNameList[i];
		string strSection = string("/LJSDKServer/OuterObj/GameServerlist/") + strTmpServerName;
		int iWorldID = TC_Common::strto<int>(tmpCfg.get(strSection + "<WorldID>", "-1") );
		if(iWorldID < 0) continue;
		string strObject = tmpCfg.get(strSection + "<GameObj>", "");
		ServerEngine::GamePrx gamePrx = Application::getCommunicator()->stringToProxy<ServerEngine::GamePrx>(strObject);

		std::pair<map<int , ServerEngine::GamePrx>::iterator,bool> ret =  m_worldList.insert(make_pair(iWorldID,gamePrx));
		assert(ret.second);
	}*/
	
}
/*
int LJSDKImp::doRequest(taf::JceCurrentPtr current, vector<char>& response)
{
	//current->setResponse(false);
	//给玩家充钱
	const vector<char>& requestBuffer = current->getRequestBuffer();
	if(requestBuffer.size() < 2)
	{
		current->close();
		return 0;
	}
	string strMsg = string(&requestBuffer[0], requestBuffer.size() );
	FDLOG("LJSDKPay_URL")<<"strMsg | "<<strMsg <<endl;
	
	TC_HttpRequest request;
	request.decode(&requestBuffer[0], requestBuffer.size() );
	string strRequestUrl = request.getRequestUrl();

	//验证合法性
	string strPayDes;
	int iAmount = 0;
	string strChannel;
	string strOrderId;
	bool bVerifyResoult = verifyUrl(request,strPayDes,iAmount,strChannel,strOrderId);
	//验证通过
	if(bVerifyResoult)
	{
		//通过验证
		vector<string> vec = TC_Common::sepstr<string>(strPayDes, "_");
		if(vec.size() != 2)
		{
			FDLOG("Pay_ERROR")<<"PayDes = |"<<strPayDes <<"|"<<strRequestUrl<<endl;	
			current->close();
			return 0 ;
		}

		int iWorldId = TC_Common::strto<int>(vec[0], "-1");
		if(iWorldId == -1)
		{
			current->close();
			FDLOG("Pay_ERROR")<<"iWoldId ERROR  |"<<vec[0] <<endl;	
			return 0 ;
		}
		
		map<int , ServerEngine::GamePrx>::iterator iter = m_worldList.find(iWorldId);
		if(iter == m_worldList.end())
		{	
			FDLOG("Pay_AddMoney_ERROR")<<"iWorldId= "<<iWorldId <<"|" <<strMsg<<endl;	
			current->close();
			return 0;
		}

		string strAccount = vec[1];
		const ServerEngine::GamePrx gamePrt = iter->second;
		iAmount = iAmount /10;
		int iResoult = gamePrt->LJSDK_Pay_AddMoney(strAccount, iWorldId, iAmount,strChannel,strOrderId);
		if(iResoult == -1)
		{
			current->close();
			FDLOG("Pay_AddMoney_ERROR")<<"paydes = "<<strPayDes << "amount"<<iAmount<<endl;	
			return 0;
		}
		
		//给玩家充钱
		string strResponse = "success";
		TC_HttpResponse httpResponse;
		httpResponse.setResponse(strResponse.c_str(),strResponse.length());
 		string strRet = httpResponse.encode();
		response.insert(response.end(), strRet.begin(), strRet.end() );
	}
	else
	{
		current->close();
	}
	return -1;
}

bool LJSDKImp::verifyUrl(TC_HttpRequest request, string& strPayDes, int& iAmount,string& strChannel, string& strOrderId)
{
	string strRequestParam = request.getRequestParam();
	
	if(strRequestParam.size() == 0)
	{
		return false;
	}
	
	map<string, string> paramsMap;
	vector<string> tmpList = TC_Common::sepstr<string>(strRequestParam, "&");
	for(size_t i = 0; i < tmpList.size(); i++)
	{
		vector<string> keyValue = TC_Common::sepstr<string>(tmpList[i], "=");
		if(keyValue.size() != 2)
		{
			FDLOG("keyValue")<<"keyValue= |"<< tmpList[i]<<endl;	
			return false;
		}

		paramsMap.insert(make_pair(keyValue[0],keyValue[1]));
	}

	strOrderId = getParamByKey("orderId",paramsMap);
	strChannel = getParamByKey("channelLabel",paramsMap);
	string strPrice = getParamByKey("price",paramsMap);
	string strCallbackInfo = getParamByKey("callbackInfo",paramsMap);
	string strSign = getParamByKey("sign",paramsMap);
	  
	string signSourceString = strOrderId + strPrice + strCallbackInfo + m_strProductSecretKey;
	string md5StrSign = TC_MD5::md5str(signSourceString);
	if(strSign != md5StrSign)
	{
		return false;
	}


	strPayDes = TC_Base64::decode(strCallbackInfo);
	iAmount = TC_Common::strto<int>(strPrice,"0");
	
	return true;
}

string  LJSDKImp::getParamByKey(const string strKey, map<string,string>& paramMap)
{
	map<string,string>::iterator iter = paramMap.find(strKey);
	if(iter != paramMap.end())
	{
		return iter->second;
	}
	return "";
}



void LJSDKImp::verifyFailedRetrun(vector<char>& response)
{
	//验证失败
	string strResponse ="fail";
	TC_HttpResponse httpResponse;
	httpResponse.setResponse(strResponse.c_str(),strResponse.length());
	string strRet = httpResponse.encode();
	response.insert(response.end(), strRet.begin(), strRet.end() );
}
*/
class verifyUserLoginCallback:public TC_HttpAsync::RequestCallback
{
public:

	verifyUserLoginCallback(taf::JceCurrentPtr current, ServerEngine::VerifyKey verifykey, const string& strUrl):m_current(current)
		, m_verifyKey(verifykey), m_strUrl(strUrl){}

	virtual void onException(const string &ex) 
    {
        //cout << "onException:" << m_strUrl << ":" << ex << endl;
       	FDLOG("verifyUserLoginCallback")<<"onException=|"<<m_strUrl<<endl;
    }
	
    virtual void onResponse(bool bClose, TC_HttpResponse &stHttpRep) 
    {
    	string strContent = stHttpRep.getContent();
		FDLOG("verifyUserLoginCallback")<<"onResponse=|"<<strContent<<endl;
		if(strContent == "true")
		{
			
		}
		else if(strContent == "false")
		{
			//kill user
			ServerEngine::LJSDK::async_response_verifyUserLogin(m_current,ServerEngine::en_INVALIDLOGIN);
		}
    }
	
    virtual void onTimeout() 
    {
    	FDLOG("verifyUserLoginCallback")<<"onTimeout=|"<<m_strUrl<<endl;
		ServerEngine::LJSDK::async_response_verifyUserLogin(m_current,ServerEngine::en_INVALIDLOGIN);
    }
	
    virtual void onClose()
    {
    	//FDLOG("verifyUserLoginCallback")<<"onClose=|"<<m_strUrl<<endl;
		//ServerEngine::LJSDK::async_response_verifyUserLogin(m_current,ServerEngine::en_INVALIDLOGIN);
    }

private:

	taf::JceCurrentPtr m_current;
	ServerEngine::VerifyKey m_verifyKey;
	string m_strUrl;
};
class verifyViVOLoginCallback:public TC_HttpAsync::RequestCallback
{
public:

	verifyViVOLoginCallback(taf::JceCurrentPtr current, ServerEngine::VerifyKey verifykey, const string& strUrl):m_current(current)
		, m_verifyKey(verifykey), m_strUrl(strUrl){}

	virtual void onException(const string &ex) 
	{
		//cout << "onException:" << m_strUrl << ":" << ex << endl;
		FDLOG("verifyUserLoginCallback")<<"onException=|"<<m_strUrl<<endl;
	}
	
	virtual void onResponse(bool bClose, TC_HttpResponse &stHttpRep) 
	{
		string strContent = stHttpRep.getContent();
		FDLOG("verifyViVOLoginCallback")<<"onResponse=|"<<strContent<<endl;
		
		// JsonValuePtr retValue = TC_Json::getValue(strContent);
		// JsonValueObjPtr pObj=JsonValueObjPtr::dynamicCast(retValue);
		
		// if(pObj->value.find("stat") == pObj->value.end())
		// {
		// 				//kill user
		// 	ServerEngine::LJSDK::async_response_verifyUserLogin(m_current,ServerEngine::en_INVALIDLOGIN);
		// }
		// else
		// {

		// }
	}
	
	virtual void onTimeout() 
	{
		FDLOG("verifyUserLoginCallback")<<"onTimeout=|"<<m_strUrl<<endl;
		ServerEngine::LJSDK::async_response_verifyUserLogin(m_current,ServerEngine::en_INVALIDLOGIN);
	}
	
	virtual void onClose()
	{
		//FDLOG("verifyUserLoginCallback")<<"onClose=|"<<m_strUrl<<endl;
		//ServerEngine::LJSDK::async_response_verifyUserLogin(m_current,ServerEngine::en_INVALIDLOGIN);
	}

private:

	taf::JceCurrentPtr m_current;
	ServerEngine::VerifyKey m_verifyKey;
	string m_strUrl;
};


void LJSDKImp::verifyUserLogin(const ServerEngine::VerifyKey & verifyKey,taf::Int32 &iState,taf::JceCurrentPtr current)
{
	

	TC_HttpRequest httpRequest;
	
	/*
	http://gameproxy.xinmei365.com/game_agent/checkLogin?productCode=p001
	&token=NmUwOWZjODZhGfkmYjUwMWMwMjEuMzQ1NjMxMjYuMTQxOTcyMzI5NDkyNA%253D%253D
	&channel=b9071b9d5bce456992c51fb554490624
	&userId=100010110
	*/
	if(verifyKey.strChannelId != "vivo")
	{
		current->setResponse(false);
		string strUrlHost = "http://gameproxy.xinmei365.com/game_agent/checkLogin";
		stringstream ss;
		ss<<strUrlHost
			<<"?productCode="<<verifyKey.strProductCode.c_str()
			<<"&token="<<verifyKey.strToken.c_str()
			<<"&channel="<<verifyKey.strChannelId.c_str()
			<<"&userId="<<verifyKey.strUserId.c_str();


		FDLOG("verifyUserLogin") << verifyKey.strUserId<<"|"<<verifyKey.strChannelId<<endl;

		string strRequestFullUrl = ss.str();

		FDLOG("verifyUserLogin_URL")<<"RequestUrl|" << ss.str()<<endl;

		httpRequest.setGetRequest(strRequestFullUrl, true);

		TC_HttpAsync::RequestCallbackPtr callbackPtr = new verifyUserLoginCallback(current, verifyKey, strRequestFullUrl);
		g_httpAsync.doAsyncRequest(httpRequest, callbackPtr);
	}
	else
	{
		 
		string strUrlHost = "https://usrsys.inner.bbk.com/auth/user/info";

		string strbody = "access_token="+verifyKey.strToken;

		FDLOG("verifyUserLogin") << verifyKey.strUserId<<"|"<<verifyKey.strChannelId<<endl;


		std::string strRes;
		CHttpClient* pClient = new CHttpClient();
		pClient->Posts(strUrlHost.c_str(),strbody.c_str(),strRes);

		FDLOG("verifyViVOLoginCallback")<<"onResponse=|"<<strRes<<endl;
		
		// JsonValuePtr retValue = TC_Json::getValue(strRes);
		// JsonValueObjPtr pObj=JsonValueObjPtr::dynamicCast(retValue);
		
		// if(pObj->value.find("stat") != pObj->value.end())
		// {
		// 	string stat = JsonValueStringPtr::dynamicCast(pObj->value["stat"])->value;
		//  	FDLOG("verifyViVOLoginCallback")<<"stat=|"<<stat.c_str()<<endl;
		// 	//kill user
		// 	ServerEngine::LJSDK::async_response_verifyUserLogin(current,ServerEngine::en_INVALIDLOGIN);
		// }
		// else
		// {
			
		// }
		
		//cout<<"strRes  | " << strRes<<endl;
	
		//httpRequest.setPostRequest(strUrlHost, strbody,false);

		//TC_HttpAsync::RequestCallbackPtr callbackPtr = new verifyViVOLoginCallback(current, verifyKey, strUrlHost);
		//g_httpAsync.doAsyncRequest(httpRequest, callbackPtr);
	}

	//return 0;
}

class PhoneStreamCallback:public TC_HttpAsync::RequestCallback
{
public:
	PhoneStreamCallback(){}
	PhoneStreamCallback(string url):m_strUrl(url){}

	virtual void onException(const string &ex) 
    {
    	FDLOG("PhoneStreamCallback")<<"onException=|"<<m_strUrl<<endl;
    }
	
    virtual void onResponse(bool bClose, TC_HttpResponse &stHttpRep) 
    {
    	string strContent = stHttpRep.getContent();
		FDLOG("PhoneStreamCallback")<<"onResponse=|"<<strContent<<"|"<<m_strUrl<<endl;
    }
	
    virtual void onTimeout() 
    {
    	FDLOG("PhoneStreamCallback")<<"onTimeout=|"<<m_strUrl<<endl;
    }
	
    virtual void onClose()
    {
    }

private:
	string m_strUrl;

};


void LJSDKImp::getPhoneStream(const std::string & strUrl,taf::JceCurrentPtr current)
{
	current->setResponse(false);
	TC_HttpRequest httpRequest;
	httpRequest.setGetRequest(strUrl, true);
	TC_HttpAsync::RequestCallbackPtr callbackPtr = new PhoneStreamCallback(strUrl);
	g_httpAsync.doAsyncRequest(httpRequest, callbackPtr);
}

void LJSDKImp::VIVOPostPerPay(const std::string & strPostContext,std::string &strAccessKey,std::string &strOrderNumber,taf::Int32 &price,taf::JceCurrentPtr current)
{
	current->setResponse(false);
	TC_HttpRequest httpRequest;
	string strUrlHost = "https://pay.vivo.com.cn/vcoin/trade";

	string strbody = strPostContext;

	FDLOG("VIVOPostPerPay") <<"|"<<strbody<<endl;


	std::string strRes;
	CHttpClient* pClient = new CHttpClient();
	pClient->Posts(strUrlHost.c_str(),strbody.c_str(),strRes);

	FDLOG("VIVOPostPerPay")<<"onResponse=|"<<strRes<<endl;
		
	// JsonValuePtr retValue = TC_Json::getValue(strRes);
	// JsonValueObjPtr pObj=JsonValueObjPtr::dynamicCast(retValue);
		
	// if(pObj->value.find("respCode") != pObj->value.end())
	// {
	// 	string stat = JsonValueStringPtr::dynamicCast(pObj->value["respCode"])->value;
	// 	if(stat !="200") return;
			
		
	// 	{"respCode":"200","signMethod":"MD5","accessKey":"650b5eda4edd30444c21dfefb6d1326e","orderAmount":"600",
	// 	"orderNumber":"2015051817183206400015130674",
	// 	"respMsg":"success",
	// 	"signature":"6d85da156d1f196b9dcfc34f93e927a6"}

	// 	string accessKey = JsonValueStringPtr::dynamicCast(pObj->value["accessKey"])->value;
	// 	std::string orderNumber = JsonValueStringPtr::dynamicCast(pObj->value["orderNumber"])->value;
	// 	string OrderAmount = JsonValueStringPtr::dynamicCast(pObj->value["orderAmount"])->value;
	// 	int  iPrice = TC_Common::strto<int>(OrderAmount,"0");
		
	// 	ServerEngine::LJSDK::async_response_VIVOPostPerPay(current,accessKey,orderNumber,iPrice);
	// }
	// else
	// {
	
	// }
		
}


static int __iosGetPriceViaProductID(const string& _productID)
{
	//TODO:urgly
	if(_productID.compare("yueka3") == 0){
		return 25;
	}else if(_productID.compare("yuanbao6480") == 0){
		return 648;
	}else if(_productID.compare("yuanbao3280") == 0){
		return 328;
	}else if(_productID.compare("yuanbao1980") == 0){
		return 198;
	}else if(_productID.compare("yuanbao980") == 0) {
		return 98;
	}else if(_productID.compare("yuanbao300") == 0){
		return 30;
	}else if(_productID.compare("yuanbao1") == 0){
		return 6;
	}else{
		return 0;
	}
}


//production URL:https://buy.itunes.apple.com/verifyReceipt
//sandbox URL:https://sandbox.itunes.apple.com/verifyReceipt
#define IOS_RECEIPT_VERIFY_CONNECTTIMEOUT	90
#define IOS_RECEIPT_VERIFY_TIMEOUT			90
#define IOS_RECEIPT_VERIFY_URL	"https://buy.itunes.apple.com/verifyReceipt"

#define FDLOG_INPUT				FDLOG("IOS_PAY_INPUT")
#define FDLOG_RECEIP			FDLOG("IOS_PAY_RECEIP")
#define FDLOG_HTTP_FAILED		FDLOG("IOS_PAY_HTTP_FAILED")
#define FDLOG_HTTP_RESPONSE		FDLOG("IOS_PAY_HTTP_RESPONSE")
#define FDLOG_VERIFIED_FAILED	FDLOG("IOS_PAY_VERIFIED_FAILED")
#define FDLOG_VERIFIED			FDLOG("IOS_PAY_VERIFIED")
// this function is a synchronize method
void LJSDKImp::iosPay(	taf::Int32 		worldID,
						const std::string& 	account,
						taf::Int32 			cPrice,
						const std::string& 	receipt,

						std::string& 					outTransID,
						std::string& 					outProductID,
						taf::Int32&						outSPrice,
						taf::Int32&						outQuantity,
						ServerEngine::IOSVerifyRetcode &outRetcode,
						taf::JceCurrentPtr 				current)
{
	current->setResponse(false);
	// save request data into input
	FDLOG_INPUT<<account<<"|"<<worldID<<"|"<<cPrice<<endl;
	FDLOG_RECEIP<<account<<"|"<<receipt<<endl;

	// make json
	// JsonValueObjPtr jRequestPTR = taf::TC_AutoPtr<JsonValueObj> (new JsonValueObj());
	// JsonValueStringPtr jReceitStringPTR = taf::TC_AutoPtr<JsonValueString> (new JsonValueString(receipt));
	// jRequestPTR->value.insert(make_pair("receipt-data" ,jReceitStringPTR));
	// //string strResponse = TC_Json::writeValue(objPrt.get());
	// string requestString = TC_Json::writeValue(jRequestPTR.get());
	string requestString = "";
	// post to appstore
	string response;
	
	CHttpClient* httpClient = new CHttpClient();
	int httpRet = httpClient->Posts(	IOS_RECEIPT_VERIFY_URL,
										requestString.c_str(),
										response, 
										NULL,
										IOS_RECEIPT_VERIFY_CONNECTTIMEOUT, 
										IOS_RECEIPT_VERIFY_TIMEOUT);

	// http failed.
	std::string transID;
	std::string productID;
	int sPrice = 0;
	int quantity = 0;
	ServerEngine::IOSVerifyRetcode retCode = ServerEngine::en_IOS_VERIFY_OK;

	// CURLE_OK is 0
	if(0 != httpRet || response.empty())
	{
		FDLOG_HTTP_FAILED<<account<<"|"<<worldID<<"|"<<cPrice<<"|"<<httpRet<<endl;
		retCode = ServerEngine::en_IOS_VERIFY_NET_ERROR;
	}
	else
	{
		int iosStatus = -1;
		FDLOG_HTTP_RESPONSE<<account<<"|"<<worldID<<"|"<<cPrice<<"|"<<response<<endl;
		// JsonValuePtr jResponseValue = TC_Json::getValue(response);
		// JsonValueObjPtr jResponsePTR =JsonValueObjPtr::dynamicCast(jResponseValue);
		// if(jResponsePTR->value.find("status") != jResponsePTR->value.end())
		// {
		// 	iosStatus = JsonValueNumPtr::dynamicCast(jResponsePTR->value["status"])->value;
		// 	if(iosStatus == 0)
		// 	{
		// 		JsonValueObjPtr jReceiptPTR = JsonValueObjPtr::dynamicCast(jResponsePTR->value["receipt"]);
		// 		transID= JsonValueStringPtr::dynamicCast(jReceiptPTR->value["transaction_id"])->value;
		// 		productID = JsonValueStringPtr::dynamicCast(jReceiptPTR->value["product_id"])->value;
		// 		// get price via product id.
		// 		sPrice = __iosGetPriceViaProductID(productID);
		// 		std::string quantityString = JsonValueStringPtr::dynamicCast(jReceiptPTR->value["quantity"])->value;
		// 		quantity = TC_Common::strto<int>(quantityString, "0");
		// 		retCode = ServerEngine::en_IOS_VERIFY_OK;

		// 		FDLOG_VERIFIED	<< worldID	<< "|"
		// 						<< account	<< "|"
		// 						<< cPrice 	<< "|"
		// 						<< transID	<< "|"
		// 						<< productID<< "|"
		// 						<< quantity	<< "|"
		// 						<< sPrice	<< endl;
		// 	}
		// 	else
		// 	{
		// 		retCode = ServerEngine::en_IOS_VERIFY_VERIFY_FALSE;
		// 		FDLOG_VERIFIED_FAILED<<account<<"|"<<worldID<<"|"<<cPrice<<endl;
		// 	}
		// }
		// else
		// {
		// 	retCode = ServerEngine::en_IOS_VERIFY_NET_ERROR;
			
		// }
	}

	
	// send data back
	ServerEngine::LJSDK::async_response_iosPay( current,
												transID,
												productID,
												sPrice,
												quantity,
												retCode);
}


/*int LJSDKImp::doResponse(ReqMessagePtr resp)
{
	return -1;
}


int LJSDKImp::doResponseException(ReqMessagePtr resp)
{
	return -1;
}


int LJSDKImp::doResponseNoRequest(ReqMessagePtr resp)
{
	return -1;
}
*/

void LJSDKImp::destroy()
{
}



