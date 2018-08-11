#include "Common.h"
#include "FmStringUtil.h"
#include "Json/Cjson.h"
#include "FmSysUtil.h"
#include "Script/MyLuaEngine.h"
#include "Script/FmScriptSys.h"
//#include "SDK/TBTSDK.h"

NS_FM_BEGIN

std::string Common::GetDeviceInfo( const char* key )
{
	string info = CCDevice::GetDeviceInfor();
	vector<string> vInfos = StringUtil::Split(info, "^");

	if (vInfos.size() > 0)
	{
		static char re[256];
		for(uint i = 0; i < vInfos.size(); i++)
		{
			if(i == 0)
			{
				sprintf(re,"%s",vInfos[i].c_str());
			}
			else
			{
				if(vInfos[i].length() > 3)
				{
					sprintf(re,"%s^%s",re,vInfos[i].c_str());
				}
			}
			CCLOG(">>>>>>>>>>>>>>>>>>CPP Device Infor %s", vInfos[i].c_str());
		}
		CCLOG(">>>>>>>>>>>>>>>>>>Device Infor Foe Server %s", re);
		return re;
	}
	else
		return "";
	//if( key == NULL )
	//{
	//	Assert( false );
	//	return "";
	//}
	//// imei
	//if( strcmp( "imei", key ) == 0 )
	//{
	//	return "imei";
	//}
	//// imsi
	//else if( strcmp( "imsi", key ) == 0 )
	//{
	//	return "imsi";
	//}
	//// uuid
	//else if( strcmp( "uuid", key ) == 0 )
	//{
	//	return "uuid";
	//}
	//// macaddress
	//else if( strcmp( "macaddress", key ) == 0 )
	//{
	//	return "macaddress";
	//}
	//return "";
}

string Common::GetChannelInfor()
{
//#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
	Json::Value root;
	//root["Channel"] = "our";
	root["Channel"] = "our";
	root["Version"] = "1.1.6";
	string re = root.toStyledString();
	return re;
//#else
//	string info = CCDevice::ChannelGet();
//	vector<string> vInfos = StringUtil::Split(info, "^");
//
//	if (vInfos.size() > 0)
//	{
//		static char re[2048];
//		sprintf(re,"%s",vInfos[0].c_str());
//		CCLOG(">>>>>>>>>>>>>>>>>>Device ChannelInfo Fixed %s", re);
//		return re;
//	}
//	else
//		return "";
//#endif
}

static ChannelLoginInfo s_loginInfo;

extern "C" void channel_Login_callBack(const ChannelLoginInfo& channelLogin)
{
	if(channelLogin.bSuccess)
	{
		CCLog("Call Back Ret:%s|%s", channelLogin.strChannel.c_str(), channelLogin.strOpenID.c_str() );
		s_loginInfo = channelLogin;
	}

	ScriptSys::GetInstance().Execute_1("channelLogin_Callback", (void*)&channelLogin, "ChannelLoginInfo");
}

extern "C" void notifyNotIntallWx()
{
	ScriptSys::GetInstance().Execute("notifyNotIntallWx");
}

extern "C" ChannelLoginInfo getLocalLoginInfo()
{
	return s_loginInfo;
}

extern "C" void clearLocalLoginInfo()
{
	s_loginInfo = ChannelLoginInfo();
}

extern "C" void requestRefreshGold()
{
	ScriptSys::GetInstance().Execute("requestRefreshGold");
}

extern "C" void anzuoInitSdkBack()
{

}
extern "C" void anzuoSdkLogin(const ChannelSdkInfo& sdkInfo)
{
#if CC_TARGET_SDK == CC_PLATFORM_SDK_AZUC 
	if (sdkInfo.bSuccess)
	{
		CCLOG("anzuoSdkLogin:%s",sdkInfo.UserId.c_str());
		ScriptSys::GetInstance().Execute_1("UCChannelLogin_CallBack",(void*)&sdkInfo, "ChannelSdkInfo");
		TBTSDK::GetInstance().s_channelSdkInfo = sdkInfo;
	}
	else
	{
		TBTSDK::GetInstance().callsdkLogin();
	}
#elif CC_TARGET_SDK == CC_PLATFORM_SDK_JSFUCK
	if (sdkInfo.bSuccess)
	{
		CCLOG("anzuoSdkLogin:%s",sdkInfo.UserId.c_str());
		ScriptSys::GetInstance().Execute_1("JSFuckCHannelLogin_CallBack",(void*)&sdkInfo, "ChannelSdkInfo");
		TBTSDK::GetInstance().s_channelSdkInfo = sdkInfo;
	}
	else
	{
		TBTSDK::GetInstance().callsdkLogin();
	}
#endif
}

extern "C" void anzuoPayBack()
{
#if CC_TARGET_SDK == CC_PLATFORM_SDK_JSFUCK
	Freeman::ScriptSys::GetInstance().Execute("ReChargeSystem_JSPayLayout"); 
#endif
	Freeman::ScriptSys::GetInstance().Execute("RechargeSystem_ShowLayout"); 
}

extern "C" const char* getChannelID()
{
#if BIGHERO_CHANNEL == 0
	return "PC";
	//return "QQYYB";
#elif BIGHERO_CHANNEL == 1
	return "QQYYB";
#elif BIGHERO_CHANNEL == 2
    return "TBT";
#elif BIGHERO_CHANNEL == 3
	return "UC";
#elif BIGHERO_CHANNEL == 4 
	return "LJ";
#elif BIGHERO_CHANNEL == 5
	return "JSFUCK";
#elif BIGHERO_CHANNEL == 6
	return "VIVO";
#else
	return "UnKnow";
#endif
};


extern "C" const char* getOSID()
{
#if CC_TARGET_PLATFORM == CC_PLATFORM_IOS
    return "IOS";
#elif CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID
    return "ANDROID";
#elif CC_TARGET_PLATFORM == CC_PLATFORM_WIN32
    return "WIN32";
#else
    return "UNKNOWN";
#endif
}

NS_FM_END

// 非应用宝渠道
#if BIGHERO_CHANNEL != 1
	extern "C" void callQQYYBLogin()
	{
	}

	extern "C" void callWeiXinLogin()
	{
	}

	extern "C" void callLogOut()
	{
	}

	extern "C" void callRequestSaveMoney(int iZoneID, int iMoney)
	{
		
	}

#endif

#if CC_TARGET_SDK != CC_PLATFORM_SDK_AZUC && CC_TARGET_SDK != CC_PLATFORM_SDK_JSFUCK 
	extern "C" void callstaticSDKHandler(int tag)
	{

	}
	extern "C" void cpaySdk(const char * productID,const char *  productName,float productPrice,const char *  tradeNo,const char *  userID,const char *  notifyUrl,const char *  extString)
	{

	}
	extern "C" void submitExternData(int severId,const char *roleId,const char *roleName,const char *level,const char *severName)
	{

	}
#endif


