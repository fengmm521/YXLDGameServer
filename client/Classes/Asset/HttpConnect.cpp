
#include "Asset/HttpConnect.h"
#include "FmStringUtil.h"
#include "Scene/SceneMgr.h"
#include "Enum/FmComponentEnum.h"
#include "GameEntity/FmEntityRender.h"
#include "Script/MyLuaEngine.h"
#include "Script/FmScriptSys.h"
#include "Scene/Launcher.h"
NS_FM_BEGIN


HttpConnect::HttpConnect()
{
}

HttpConnect::~HttpConnect()
{

}

void HttpConnect::RequestUrlFile( const char* url,const char* tag )
{
	CCLog("Request: %s", url);
	cocos2d::extension::CCHttpRequest* request = new cocos2d::extension::CCHttpRequest();
	request->setUrl(url);
	request->setRequestType(cocos2d::extension::CCHttpRequest::kHttpGet);
	request->setResponseCallback(this, callfuncND_selector(HttpConnect::onHttpRequestCompleted));
	request->setTag(tag);
	cocos2d::extension::CCHttpClient::getInstance()->send(request);
	request->release();
}

void HttpConnect::onHttpRequestCompleted( cocos2d::CCNode *sender, void *data )
{
	cocos2d::extension::CCHttpResponse *response = (cocos2d::extension::CCHttpResponse*)data;
	if (!response)
	{
		return;
	}
	// You can get original request type from: response->request->reqType
	if (0 != strlen(response->getHttpRequest()->getTag())) 
	{
		CCLog("%s completed", response->getHttpRequest()->getTag());
	}

	int statusCode = response->getResponseCode();
	//char statusString[256] = { NULL };
	//sprintf(statusString, "HTTP Status Code: %d, tag = %s", statusCode, response->getHttpRequest()->getTag());
	//	m_labelStatusCode->setString(statusString);
	CCLog("response code: %d", statusCode);

	string tagName = response->getHttpRequest()->getTag();

	if (!response->isSucceed()) 
	{
		CCLog("response failed");
		CCLog("error buffer: %s", response->getErrorBuffer());
		string errorBuf = response->getErrorBuffer();
		CCString* respData = CCString::create(errorBuf);
		CCString* tag = CCString::create(tagName);
		ScriptSys::GetInstance().Execute_2( "HttpConnect_Response",tag,"CCString",respData,"CCString" );

		if(tagName.compare("verCheck")==0)
		{
			Launcher::GetInstance()->OnVersionResponse(errorBuf.c_str());
		}
		//char script[2048] = { NULL };
		//sprintf( script, "HttpConnect_Response('%s','%s')", tagName.c_str(), response->getErrorBuffer() );
		//ScriptSys::GetInstance().ExecuteString( script );
		return;
	}
	// dump data
	std::vector<char> *buffer = response->getResponseData(); 
	char* fileBuffer = new char[buffer->size()+1];
	int size =  buffer->size();
	for (unsigned int i = 0; i < buffer->size(); i++)
	{
		fileBuffer[i] = (*buffer)[i];
	}
	fileBuffer[buffer->size()] = 0;
	CCString* respData = CCString::create(fileBuffer);
	CCString* tag = CCString::create(tagName);
	ScriptSys::GetInstance().Execute_2( "HttpConnect_Response",tag,"CCString",respData,"CCString" );

	if(tagName.compare("verCheck")==0)
	{
		Launcher::GetInstance()->OnVersionResponse(fileBuffer);
	}
}


NS_FM_END