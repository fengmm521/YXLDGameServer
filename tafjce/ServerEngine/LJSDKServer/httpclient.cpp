
#include <iostream>
#include <string> 
#include "httpclient.h"
#include <curl/curl.h>

using namespace std;


CHttpClient::CHttpClient(void) :   
m_bDebug(false)  
{  
  
}  
  
CHttpClient::~CHttpClient(void)  
{  
  
}  
  
static int OnDebug(CURL *, curl_infotype itype, char * pData, size_t size, void *)  
{  
    if(itype == CURLINFO_TEXT)  
    {  
        //printf("[TEXT]%s\n", pData);  
    }  
    else if(itype == CURLINFO_HEADER_IN)  
    {  
        printf("[HEADER_IN]%s\n", pData);  
    }  
    else if(itype == CURLINFO_HEADER_OUT)  
    {  
        printf("[HEADER_OUT]%s\n", pData);  
    }  
    else if(itype == CURLINFO_DATA_IN)  
    {  
        printf("[DATA_IN]%s\n", pData);  
    }  
    else if(itype == CURLINFO_DATA_OUT)  
    {  
        printf("[DATA_OUT]%s\n", pData);  
    }  
    return 0;  
}  
  
static size_t OnWriteData(void* buffer, size_t size, size_t nmemb, void* lpVoid)  
{  
    std::string* str = dynamic_cast<std::string*>((std::string *)lpVoid);  
    if( NULL == str || NULL == buffer )  
    {  
        return -1;  
    }  
  
    char* pData = (char*)buffer;  
    str->append(pData, size * nmemb);  
    return nmemb;  
}  
  
int CHttpClient::Post(const std::string & strUrl, const std::string & strPost, std::string & strResponse)  
{  
    CURLcode res;  
    CURL* curl = curl_easy_init();  
    if(NULL == curl)  
    {  
        return CURLE_FAILED_INIT;  
    }  
    if(m_bDebug)  
    {  
        curl_easy_setopt(curl, CURLOPT_VERBOSE, 1);  
        curl_easy_setopt(curl, CURLOPT_DEBUGFUNCTION, OnDebug);  
    }  
    curl_easy_setopt(curl, CURLOPT_URL, strUrl.c_str());  
    curl_easy_setopt(curl, CURLOPT_POST, 1);  
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, strPost.c_str());  
    curl_easy_setopt(curl, CURLOPT_READFUNCTION, NULL);  
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, OnWriteData);  
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&strResponse);  
    curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1);  
    curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 3);  
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 3);  
    res = curl_easy_perform(curl);  
    curl_easy_cleanup(curl);  
    return res;  
}  
  
int CHttpClient::Get(const std::string & strUrl, std::string & strResponse)  
{  
    CURLcode res;  
    CURL* curl = curl_easy_init();  
    if(NULL == curl)  
    {  
        return CURLE_FAILED_INIT;  
    }  
    if(m_bDebug)  
    {  
        curl_easy_setopt(curl, CURLOPT_VERBOSE, 1);  
        curl_easy_setopt(curl, CURLOPT_DEBUGFUNCTION, OnDebug);  
    }  
	curl_easy_setopt(curl, CURLOPT_URL, strUrl.c_str());  
    curl_easy_setopt(curl, CURLOPT_READFUNCTION, NULL);  
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, OnWriteData);  
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&strResponse);  
    /** 
    * 当多个线程都使用超时处理的时候，同时主线程中有sleep或是wait等操作。 
    * 如果不设置这个选项，libcurl将会发信号打断这个wait从而导致程序退出。 
    */  
    curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1);  
    curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 3);  
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 3);  
    res = curl_easy_perform(curl);  
    curl_easy_cleanup(curl);  
    return res;  
}  
  
int CHttpClient::Posts(	const std::string & strUrl, 
							const std::string & strPost, 
							std::string & strResponse, 
							const char * pCaPath,
							int connectTimeout,
							int timeout)  
{  
/*
	CURL *pCurl = curl_easy_init();
	   int responseCode = -1;
	   CURLcode returnCode = CURLE_OK;
	   do 
	   {
		   if(!pCurl)
		   {
			   break;
		   }
		  returnCode = curl_easy_setopt(pCurl, CURLOPT_CONNECTTIMEOUT, 10); //设置连接超时时间
		   CURL_BREAK_IF_NOT_OK(returnCode);
	
		   returnCode = curl_easy_setopt(pCurl, CURLOPT_URL, url);				//设置请求链接
		   CURL_BREAK_IF_NOT_OK(returnCode);
	
		   returnCode = curl_easy_setopt(pCurl, CURLOPT_WRITEDATA, fp); 	 //设置CURLOPT_WRITEFUNCTION的最后一个参数值
		   CURL_BREAK_IF_NOT_OK(returnCode);
	
		   returnCode = curl_easy_setopt(pCurl, CURLOPT_WRITEFUNCTION, test_save); //设置接收到数据之后的回调函数
		   CURL_BREAK_IF_NOT_OK(returnCode);
	
		   returnCode = curl_easy_perform(pCurl);									 //开始传输
		   CURL_BREAK_IF_NOT_OK(returnCode);
	
		   returnCode = curl_easy_getinfo(pCurl, CURLINFO_RESPONSE_CODE, &responseCode); //获取返回信息
	   } while (0);
	   if (pCurl) 
	   {
		   curl_easy_cleanup(pCurl);
	   }
	   return responseCode;

	   */

    CURLcode res;  
    CURL* curl = curl_easy_init();  
    if(NULL == curl)  
    {  
        return CURLE_FAILED_INIT;  
    }  
    if(m_bDebug)  
    {  
        curl_easy_setopt(curl, CURLOPT_VERBOSE, 1);  
        curl_easy_setopt(curl, CURLOPT_DEBUGFUNCTION, OnDebug);  
    }  
    curl_easy_setopt(curl, CURLOPT_URL, strUrl.c_str());  
    curl_easy_setopt(curl, CURLOPT_POST, 1);  
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, strPost.c_str());  
    curl_easy_setopt(curl, CURLOPT_READFUNCTION, NULL);  
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, OnWriteData);  
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&strResponse);  
    curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1);  
    if(NULL == pCaPath)  
    {  
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, false);  
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, false);  
    }  
    else  
    {  
        //缺省情况就是PEM，所以无需设置，另外支持DER  
        //curl_easy_setopt(curl,CURLOPT_SSLCERTTYPE,"PEM");  
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, true);  
        curl_easy_setopt(curl, CURLOPT_CAINFO, pCaPath);  
    }  
    curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, connectTimeout);  
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, timeout);  
    res = curl_easy_perform(curl);  
    curl_easy_cleanup(curl);  
    return res;  
}  
  
int CHttpClient::Gets(const std::string & strUrl, std::string & strResponse, const char * pCaPath)  
{  
    CURLcode res;  
    CURL* curl = curl_easy_init();  
    if(NULL == curl)  
    {  
        return CURLE_FAILED_INIT;  
    }  
    if(m_bDebug)  
    {  
        curl_easy_setopt(curl, CURLOPT_VERBOSE, 1);  
        curl_easy_setopt(curl, CURLOPT_DEBUGFUNCTION, OnDebug);  
    }  
    curl_easy_setopt(curl, CURLOPT_URL, strUrl.c_str());  
    curl_easy_setopt(curl, CURLOPT_READFUNCTION, NULL);  
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, OnWriteData);  
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&strResponse);  
    curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1);  
    if(NULL == pCaPath)  
    {  
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, false);  
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, false);  
    }  
    else  
    {  
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, true);  
        curl_easy_setopt(curl, CURLOPT_CAINFO, pCaPath);  
    }  
    curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 3);  
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 3);  
    res = curl_easy_perform(curl);  
    curl_easy_cleanup(curl);  
    return res;  
}  
  
///////////////////////////////////////////////////////////////////////////////////////////////  
  
void CHttpClient::SetDebug(bool bDebug)  
{  
    m_bDebug = bDebug;  
}  
/*
int main()
{
	std::string strRes;
	CHttpClient* pClient = new CHttpClient();
	pClient->Posts("https://usrsys.inner.bbk.com/auth/user/info","access_token=NzkzN2Y3MzZlYTUxZWUyN2M3MDYuNTQ0Njg5NjUuMTQzMTQ5MTI1MjQ1Nw%3D%3",strRes);
	cout<<"strRes  | " << strRes<<endl;
	return  1;
}*/
