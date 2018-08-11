#ifndef __LJSDK_IMP_H__
#define __LJSDK_IMP_H__

#include <iostream>
#include "servant/Application.h"
#include "util/tc_timeout_queue.h"
#include "util/tc_config.h"
#include "MiniApr.h"
#include "GameInterface.h"
#include "LJSDK.h"
#include "util/tc_http_async.h"

class LJSDKImp: public ServerEngine::LJSDK
{
public:

	LJSDKImp();
	~LJSDKImp();

	 /**
     * 对象初始化
     */
    virtual void initialize();

    /**
     * 处理客户端的主动请求
     * @param current
     * @param response
     * @return int
     */
    //virtual int doRequest(taf::JceCurrentPtr current, vector<char>& response);

    /**
     * @param resp
     * @return int
     */
   // virtual int doResponse(ReqMessagePtr resp);

	/**
     * @param resp
     * @return int
     */
	//int doResponseException(ReqMessagePtr resp);

	/**
     * @param resp
     * @return ints
     */
	//int doResponseNoRequest(ReqMessagePtr resp);


	//void processAddGameGold(taf::JceCurrentPtr current, TC_HttpRequest request, vector<char>& response);
	virtual void verifyUserLogin(const ServerEngine::VerifyKey & verifyKey,taf::Int32 &iState,taf::JceCurrentPtr current);
	virtual void getPhoneStream(const std::string & strUrl,taf::JceCurrentPtr current);
	virtual void VIVOPostPerPay(const std::string & strPostContext,std::string &strAccessKey,std::string &strOrderNumber,taf::Int32 &price,taf::JceCurrentPtr current);

	/**
	 * @author : xionghai
	 * process ios payment.
	 */
	virtual void iosPay(	taf::Int32 			worldID,
							const std::string& 	account,
							taf::Int32 			price,
							const std::string& 	receipt,

							std::string& 					outTransID,
							std::string& 					outProductID,
							taf::Int32&						outSPrice,
							taf::Int32&						outQuantity,
							ServerEngine::IOSVerifyRetcode &outRetcode,
							taf::JceCurrentPtr 				current);

	/**
     * 对象销毁
     */
	virtual void destroy();
public:

	//bool verifyUrl(TC_HttpRequest request, string& strPayDes, int& iAmount, string& strChannel, string& strOrderId);
	//string  getParamByKey(const string strKey, map<string,string>& paramMap);
	//void verifyFailedRetrun(vector<char>& response);
private:
	string m_strAPPID ;
	string m_strProductSecretKey;

	map<int , ServerEngine::GamePrx> m_worldList;

};



#endif
