/********************************************************************
created:	2013-2-18
author:		SXH
summary:	øÕªß∂ÀÕ®”√Ω”ø⁄
*********************************************************************/
#pragma once

#include "cocos2d.h"
#include "FmConfig.h"

USING_NS_CC;

NS_FM_BEGIN

class Common
{
public:
	// ªÒ»° ÷ª˙”≤º˛±Í ∂¬Î
	static string GetDeviceInfo( const char* key );

	// ªÒ»°«˛µ¿–≈œ¢
	static string GetChannelInfor();
};

NS_FM_END

struct ChannelLoginInfo
{
	bool bSuccess;
	string strChannel;
	string strOpenID;
	string strOpenKey;
	string strPayToken;
	string strPf;
	string strPfKey;
};

struct ChannelSdkInfo 
{
	bool bSuccess;
	string UserId;
public:
	ChannelSdkInfo()
	{
		bSuccess = false;
		UserId = "";
	}
};

extern "C" const char* getChannelID();

/**
 * get operation system id
 * @author XiongHai
 * @return string like "WIN32" "IOS" "ANDROID"
 */
extern "C" const char* getOSID();


extern "C" void callQQYYBLogin();
extern "C" void callWeiXinLogin();
extern "C" void callLogOut();
extern "C" void callRequestSaveMoney(int iZoneID, int iMoney);
extern "C" ChannelLoginInfo getLocalLoginInfo();
extern "C" void clearLocalLoginInfo();
extern "C" void notifyNotIntallWx();
extern "C" void anzuoInitSdkBack();
extern "C" void anzuoPayBack();
extern "C" void callstaticSDKHandler(int tag);
extern "C" void anzuoSdkLogin(const ChannelSdkInfo& sdkInfo);
extern "C" void cpaySdk(const char * productID,const char *  productName,float productPrice,const char *  tradeNo,const char *  userID,const char *  notifyUrl,const char *  extString);
extern "C" void submitExternData(int severId,const char *roleId,const char *roleName,const char *level,const char *severName);


// xionghai add
// use this defination to debug ios login under ios
// #define XH_DEBUG_IOS_LOGIN_UNDER_WIN32
// xionghai end


//版本管理
#define CC_PLATFORM_SDK_PC 0
#define CC_PLATFORM_SDK_IOSNOSDK 1
#define CC_PLATFORM_SDK_AZYYB 2
#define CC_PLATFORM_SDK_IOSTBT 3
#define CC_PLATFORM_SDK_AZUC 4
#define CC_PLATFORM_SDK_LJAZ 5
#define CC_PLATFORM_SDK_JSFUCK 6
#define CC_PLATFORM_SDK_VIVO 7


#define CC_TARGET_SDK CC_PLATFORM_SDK_PC

//PC
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32) && (BIGHERO_CHANNEL == 0)
#define CC_TARGET_SDK CC_PLATFORM_SDK_PC
#endif

//TBT
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS) && (BIGHERO_CHANNEL == 2)
#define CC_TARGET_SDK CC_PLATFORM_SDK_IOSTBT
#endif

//yyb
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID) && (BIGHERO_CHANNEL == 1)
#define CC_TARGET_SDK CC_PLATFORM_SDK_AZYYB
#endif

//ios no sdk
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS) && (BIGHERO_CHANNEL == 0)
#define CC_TARGET_SDK CC_PLATFORM_SDK_IOSNOSDK
#endif

//az uc 
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID) && (BIGHERO_CHANNEL == 3)
#define CC_TARGET_SDK CC_PLATFORM_SDK_AZUC   
#endif

//lj az 
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID) && (BIGHERO_CHANNEL == 4)
#define CC_TARGET_SDK CC_PLATFORM_SDK_LJAZ
#endif

//2144 fuck
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID) && (BIGHERO_CHANNEL == 5)
#define CC_TARGET_SDK CC_PLATFORM_SDK_JSFUCK
#endif

//vivo 
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID) && (BIGHERO_CHANNEL == 6)
#define CC_TARGET_SDK CC_PLATFORM_SDK_VIVO
#endif