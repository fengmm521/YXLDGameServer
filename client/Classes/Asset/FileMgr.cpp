#include "FileMgr.h"
#include "FmXmlWrapper.h"
#include "Asset/UrlFile.h"
#include "VersionConfig.h"

#include "../Scene/Launcher.h" //hyf add

//#include <curl/easy.h>

//#include <stdio.h>
//#include <vector>
//
#if (CC_TARGET_PLATFORM != CC_PLATFORM_WIN32)
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#endif

#include "support/zip_support/unzip.h"

NS_FM_BEGIN


FileMgr::FileMgr()
{
	m_VersionPreapred = false;
	m_Prepared = false;
	m_IsInRequest = false;
	m_WritePath = CCFileUtils::sharedFileUtils()->getWritablePath();
	m_TotalLoadByteSize = 0;
	m_CurLoadByteSize = 0;
	m_Version = "-1";
	m_UpdateVersion="-1";
	m_CodeVersion = "-1";
}

FileMgr::~FileMgr()
{
	for(UrlFileMap::iterator it = m_FileMap.begin(); it != m_FileMap.end(); it++ )
	{
		delete it->second; 
	}
	m_FileMap.clear();
}

bool FileMgr::InitLocalMgr( const char* writePath,const char* resConfigName )
{
	m_WritePath = writePath;
	m_ConfigName = resConfigName;
	return true;
}


void FileMgr::SetResUrl( const char* resUrl )
{
	m_ResUrl = resUrl;
}


bool FileMgr::InitServerMgr( const char* resUrl,const char* versionNumName,const char* resConfigName )
{
	m_ResUrl = resUrl;
	m_ConfigName = resConfigName;
	m_VersionFileName = versionNumName;
	return true;
}


bool FileMgr::LoadLocalConfig()
{
	CCSAXParser parser;
	if (false == parser.init("UTF-8") )
	{
		return NULL;
	}
	char fullPath[256] ={0};
	if(m_WritePath.length() <= 1)
	{
		sprintf(fullPath,"%s",m_ConfigName.c_str());
	}
	else
	{
		sprintf(fullPath,"%s/%s",m_WritePath.c_str(),m_ConfigName.c_str());
	}
	
	CCFileUtils::sharedFileUtils()->setPopupNotify(false);
	parser.setDelegator(this);
	if(parser.parse(fullPath) == false)
	{
		CCLog("Parse Error %s ",m_ConfigName.c_str());
	}
	CCFileUtils::sharedFileUtils()->setPopupNotify(true);
	return true;
}

bool FileMgr::RequestVersionNum()
{
	char url[1024] = {0};
	sprintf(url,"%s/%s",m_ResUrl.c_str(),m_VersionFileName.c_str());
	RequestUrlFile( url ,m_VersionFileName.c_str());
	return true;
}

bool FileMgr::RequestResConfig()
{
	char url[1024] = {0};
	sprintf(url,"%s/%s",m_ResUrl.c_str(),m_ConfigName.c_str());
	RequestUrlFile( url ,m_ConfigName.c_str());
	return true;
}


//bool FileMgr::InitWithUrl( const char* url )
//{
//	m_ResUrl = url;
//	RequestUrlFile( url ,s_ConfigName.c_str());
//	return true;
//}

bool FileMgr::InitFilesWithBuff( const char* buffer )
{
	CCSAXParser parser;
	if (false == parser.init("UTF-8") )
	{
		return NULL;
	}
	int len = strlen(buffer);
	parser.setDelegator(this);
	if(parser.parse(buffer,len) == false)
		return NULL;
	m_Prepared = true;
	return true;
}


bool FileMgr::InitVersionNumWithBuff( const char* buffer )
{
	CCSAXParser parser;
	if (false == parser.init("UTF-8") )
	{
		return NULL;
	}
	int len = strlen(buffer);
	parser.setDelegator(this);
	if(parser.parse(buffer,len) == false)
		return NULL;
	m_VersionPreapred = true;
	return true;
}


bool FileMgr::VersionMatched( const FileMgr* fm1,const FileMgr* fm2 )
{
	return fm1->m_Version.compare(fm2->m_Version) == 0;
}


bool FileMgr::SynWithFileMgr( FileMgr* fmserver )
{
	const UrlFileMap& smap = fmserver->GetFileMap();
	//先修改版本号
	m_UpdateVersion = fmserver->m_Version;
	//添加需要更新或加载的文件
	for(UrlFileMap::const_iterator it = smap.begin(); it!=smap.end();it++)
	{
		UrlFile* sFile = (UrlFile*)it->second; 
		if(sFile == NULL)
			continue;
		UrlFile* lFile = GetFileByName(sFile->GetTagName().c_str());
		//新加
		if(lFile == NULL) 
		{
			lFile = new UrlFile();
			lFile->SetTagName(sFile->GetTagName().c_str());
			lFile->SetRefresh(true);
			lFile->SetFileSize(sFile->GetFileSize());
			lFile->SetMd5(sFile->GetMd5().c_str());
			m_FileMap[lFile->GetTagName()] = lFile;
		}
		else //更新
		{
			if(lFile->GetMd5().compare(sFile->GetMd5()) != 0)
			{
				lFile->SetRefresh(true);
				lFile->SetFileSize(sFile->GetFileSize());
				lFile->SetMd5(sFile->GetMd5().c_str());
			}
		}
	}
	//删除文件
	for(UrlFileMap::iterator it = m_FileMap.begin(); it != m_FileMap.end(); it++ )
	{
		UrlFile* tfile = (UrlFile*)it->second; 
		if(tfile == NULL)
			continue;
		if(fmserver->GetFileByName(tfile->GetTagName().c_str()) == NULL)
			tfile->SetDelete(true);
	}
	//计算下载大小
	for(UrlFileMap::iterator it = m_FileMap.begin(); it != m_FileMap.end(); it++ )
	{
		UrlFile* tfile = (UrlFile*)it->second; 
		if(tfile == NULL)
			continue;
		if(tfile->NeedRefresh())
			m_TotalLoadByteSize += tfile->GetFileSize();
	}

	CCTime::gettimeofdayCocos2d(&m_LoadStartTime, NULL); 
	CCTime::gettimeofdayCocos2d(&m_LastSaveTime, NULL);
	return true;
}


void FileMgr::RequestUrlFile( const char* url ,const char* tag)
{
	cocos2d::extension::CCHttpRequest* request = new cocos2d::extension::CCHttpRequest();
	request->setUrl(url);
	request->setRequestType(cocos2d::extension::CCHttpRequest::kHttpGet);
	request->setResponseCallback(this, callfuncND_selector(FileMgr::onHttpRequestCompleted));
	request->setTag(tag);
	cocos2d::extension::CCHttpClient::getInstance()->send(request);
	request->release();
}

void FileMgr::onHttpRequestCompleted( cocos2d::CCNode *sender, void *data )
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
	if(Launcher::GetInstance() == NULL)
		return;

	if (!response->isSucceed()) 
	{
		Launcher::GetInstance()->m_RequestFailCounter++;
		CCLog("response failed");
		CCLog("error buffer: %s", response->getErrorBuffer());
		//返回失败 则重新请求
		//URL 资源配置列表文件
		if(m_ConfigName.compare(response->getHttpRequest()->getTag())==0)
		{
			RequestResConfig();
		}
		else if(m_VersionFileName.compare(response->getHttpRequest()->getTag())==0)
		{
			RequestVersionNum();
		}
		else
		{
			ActiveNextRequest();
		}

		return;
	}
	// dump data
	std::vector<char> *buffer = response->getResponseData();
	//URL 资源配置列表文件
	if(m_ConfigName.compare(response->getHttpRequest()->getTag())==0)
	{
		char* fileBuffer = new char[buffer->size()+1];
		int size =  buffer->size();
		for (unsigned int i = 0; i < buffer->size(); i++)
		{
			fileBuffer[i] = (*buffer)[i];
		}
		fileBuffer[buffer->size()] = 0;

		//存文件调试
				//string ServerConfigName = "ServerConfig.xml";
				//string spath = CCFileUtils::sharedFileUtils()->getWriteablePath() + ServerConfigName ; 
				//FILE* file = fopen(spath.c_str(), "wb");  
				//if(file)
				//{
				//	fwrite( fileBuffer, 1, buffer->size(), file );
				//	fclose(file);  
				//}
				//InitWithPath(ServerConfigName.c_str());
		InitFilesWithBuff(fileBuffer);
		delete []fileBuffer;
		fileBuffer = NULL;
	}
	//资源版本号文件
	else if(m_VersionFileName.compare(response->getHttpRequest()->getTag())==0)
	{
		char* fileBuffer = new char[buffer->size()+1];
		int size =  buffer->size();
		for (unsigned int i = 0; i < buffer->size(); i++)
		{
			fileBuffer[i] = (*buffer)[i];
		}
		fileBuffer[buffer->size()] = 0;

		InitVersionNumWithBuff(fileBuffer);
		delete []fileBuffer;
		fileBuffer = NULL;
	}
	else //保存文件
	{
		//第一获取储存的文件路径  
		string tagName = response->getHttpRequest()->getTag();
		string path = m_WritePath + tagName; 
		UrlFile* urlfile = GetFileByName(tagName.c_str());
		if(urlfile)
		{
			//先确保路径
			size_t tp = path.find_last_of("/");
			if(tp != string::npos)
			{
				string folderPath = path.substr(0,path.find_last_of("/")+1);
				CCFileUtils::sharedFileUtils()->createDirectorys(folderPath.c_str());
			}

			CCLOG("wanna save file path = %s",path.c_str());  
			#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
				std::replace( path.begin(), path.end(), '/', '\\' );
			#else
				std::replace( path.begin(), path.end(), '\\', '/' );
			#endif

			FILE* file = fopen(path.c_str(), "wb");  
			if (file)
			{  
				int size =  buffer->size();
				for (unsigned int i = 0; i < buffer->size(); i++)
				{
					fputc((*buffer)[i], file);
				}
				fclose(file);  
				//<TODO> 本地效验？？！
				urlfile->SetRefresh(false);
				urlfile->SetPackRes(false);
				m_CurLoadByteSize += urlfile->GetFileSize();
				if(ActiveNextRequest() == false)
					m_IsInRequest = false;

				CCLOG("save file suc path = %s",path.c_str());  
			}  
			else  
				CCLOG("save file error.");  
		}
		struct cc_timeval now; 
		CCTime::gettimeofdayCocos2d(&now, NULL); 
		double duration = CCTime::timersubCocos2d((struct cc_timeval *)&m_LastSaveTime, (struct cc_timeval *)&now);
		if(duration > 1000*10)
		{
			SavingConfigFile(false);
			CCTime::gettimeofdayCocos2d(&m_LastSaveTime, NULL); 
		}
	}
	//response->release();
}


static const char* FValueForKey(const char *key, std::map<std::string, std::string>* dict)
{
	if (dict)
	{
		std::map<std::string, std::string>::iterator it = dict->find(key);
		return it!=dict->end() ? it->second.c_str() : "";
	}
	return "";
}

void FileMgr::startElement( void *ctx, const char *name, const char **atts )
{
	CC_UNUSED_PARAM(ctx);
	//读取版本号
	if(strcmp(name,"s") == 0)
	{
		if(atts && atts[0])
		{
			for(int i = 0; atts[i]; i += 2) 
			{
				std::string key = (char*)atts[i];
				std::string value = (char*)atts[i+1];
				if(key.compare("v")==0)
				{
					m_Version = value;
					continue;
				}
				else if(key.compare("cv")==0)
				{
					m_CodeVersion = value;
					continue;
				}
			}
		}
	}
	//读取文件列表
	else if(strcmp(name,"f") == 0)
	{
		std::map<std::string, std::string> pDict;
		if(atts && atts[0])
		{
			for(int i = 0; atts[i]; i += 2) 
			{
				std::string key = (char*)atts[i];
				std::string value = (char*)atts[i+1];
				pDict.insert(pair<std::string, std::string>(key, value));
			}
			std::string pFile = FValueForKey("n",&pDict);
			std::string pMd5 = FValueForKey("m",&pDict);
			int size =  (int)atoi(FValueForKey("l",&pDict));
			int packRes =  (int)atoi(FValueForKey("p",&pDict));

			UrlFile* ufile = GetFileByName(pFile.c_str());
			if(ufile == NULL)
			{
				ufile = new UrlFile();
				m_FileMap[pFile] = ufile;
			}
			//CCLog("FFFFFFFFF: %s ",pFile.c_str());
			ufile->SetTagName(pFile.c_str());
			ufile->SetMd5(pMd5.c_str());
			ufile->SetFileSize(size);
			ufile->SetPackRes(packRes==1);

		}
	}
}

void FileMgr::endElement( void *ctx, const char *name )
{
	CC_UNUSED_PARAM(ctx);
	//	this->setContentSize(CCSizeMake(m_Rect.width,fabsf(m_CurY)));
}

void FileMgr::textHandler( void *ctx, const char *ch, int len )
{
	CC_UNUSED_PARAM(ctx);
}

UrlFile* FileMgr::GetFileByName( const char* name )
{
	UrlFileMap::iterator it = m_FileMap.find(name);
	if(it != m_FileMap.end())
		return it->second;
	else
		return NULL;
}

bool FileMgr::ActiveNextRequest()
{
	Launcher::GetInstance()->UpdateLoadProgress(m_TotalLoadByteSize,m_CurLoadByteSize);
	for(UrlFileMap::iterator it = m_FileMap.begin(); it != m_FileMap.end(); it++)
	{
		UrlFile* file = (UrlFile* )it->second;
		if(file == NULL)
			continue;
		if(file->NeedRefresh())
		{
			char url[1024] = {0};
			sprintf(url,"%s/%s",m_ResUrl.c_str(),file->GetTagName().c_str());
			RequestUrlFile( url ,file->GetTagName().c_str());
			int nextDestVal = m_CurLoadByteSize + file->GetFileSize();
			Launcher::GetInstance()->SetDestProgressValue(nextDestVal);
			//Launcher::GetInstance()->UpdateLoadTips(file->GetTagName().c_str());
			return true;
		}
	}
	return false;
}

bool FileMgr::StartRequest()
{
	m_IsInRequest = true;
	Launcher::GetInstance()->InitLoadProgressVal(m_TotalLoadByteSize,m_CurLoadByteSize);
	return ActiveNextRequest();
}

const UrlFileMap& FileMgr::GetFileMap()
{
	return m_FileMap;
}

void FileMgr::Clear()
{
	for(UrlFileMap::iterator it = m_FileMap.begin(); it != m_FileMap.end(); it++)
	{
		if(it->second)
			delete it->second;
	}
	m_FileMap.clear();
}

bool FileMgr::OnLoadFinished()
{
	for(UrlFileMap::iterator it = m_FileMap.begin(); it != m_FileMap.end(); )
	{
		UrlFile* file = (UrlFile* )it->second;
		if(file && file->NeedDelete())
		{
			string path = m_WritePath + file->GetTagName(); 
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
			std::replace( path.begin(), path.end(), '/', '\\' );
#else
			std::replace( path.begin(), path.end(), '\\', '/' );
#endif
			std::remove(path.c_str());
			CCLog("Del File : %s ",path.c_str());
			m_FileMap.erase(it++);
		}
		else
		{
			it++;
		}
	}
	return SavingConfigFile();
}

bool FileMgr::SavingConfigFile(bool finishsave)
{ 
	//版本号
	string as;
	char vc[1024] = {0};
	if(finishsave)
		sprintf(vc,"<s v=\"%s\" cv=\"%s\" >\n",m_UpdateVersion.c_str(),s_CodeVerion.c_str());
	else
		sprintf(vc,"<s v=\"%s\" cv=\"%s\" >\n",m_Version.c_str(),s_CodeVerion.c_str());
	as.append(vc);
	for(UrlFileMap::iterator it = m_FileMap.begin(); it != m_FileMap.end(); it++)
	{
		UrlFile* file = (UrlFile* )it->second;
		if(file == NULL)
			continue;
		if(file->NeedDelete())
			continue;
		if(file->NeedRefresh())
			continue;
		char fc[1024] = {0};
		sprintf(fc,"	<f n=\"%s\" m=\"%s\" l=\"%d\" p=\"%d\" />\n",file->GetTagName().c_str(),file->GetMd5().c_str(),file->GetFileSize(),file->IsPackRes()?1:0);
		as.append(fc);
	}
	char ec[1024] = {0};
	sprintf(ec,"</s>\n ");
	as.append(ec);
	string spath = m_WritePath + m_ConfigName ; 
	FILE* file = fopen(spath.c_str(), "wb"); 
	if(file)
	{
		fwrite( as.c_str(), 1, as.length(), file );
		fclose(file);  
	}
	return true;
}

bool FileMgr::HasGetVersionNum()
{
	return m_VersionPreapred;
}

bool FileMgr::MergeWithFileMgr( FileMgr* fmApp )
{
	const UrlFileMap& smap = fmApp->GetFileMap();
	//合并放在程序包的资源
	for(UrlFileMap::const_iterator it = smap.begin(); it!=smap.end();it++)
	{
		UrlFile* sFile = (UrlFile*)it->second; 
		if(sFile == NULL)
			continue;
		UrlFile* lFile = GetFileByName(sFile->GetTagName().c_str());
		//新加
		if(lFile == NULL) 
		{
			lFile = new UrlFile();
			lFile->SetTagName(sFile->GetTagName().c_str());
			lFile->SetFileSize(sFile->GetFileSize());
			lFile->SetMd5(sFile->GetMd5().c_str());
			lFile->SetPackRes(true);
			m_FileMap[lFile->GetTagName()] = lFile;
		}
	}
	m_Prepared = true;
	return true;
}

bool FileMgr::CodeVersionMatched( const string codeVersion )
{
	if(codeVersion.compare(m_CodeVersion)==0)
		return true;
	else
		return false;
}


bool FileMgr::CodeVersionIsLowerThan( const string codeVersion )
{
	int myCode = atoi(m_CodeVersion.c_str());
	int taCode = atoi(codeVersion.c_str());
	bool re = (myCode < taCode);
	return re;
}


bool FileMgr::CheckPackRes()
{
	//版本号一致，不需要清理
	if(m_CodeVersion.compare(s_CodeVerion) == 0)
		return false;
	//清理pack res
	for(UrlFileMap::iterator it = m_FileMap.begin(); it != m_FileMap.end(); )
	{
		UrlFile* file = (UrlFile* )it->second;
		if(file && file->IsPackRes())
		{
			m_FileMap.erase(it++);
		}
		else
		{
			it++;
		}
	}
	return true;
}




bool FileMgr::UnCompress( const char* filename,const char* path )
{
	if(filename == NULL)
		return false;

	unsigned long nSize = 0;
	unsigned char* pBuffer = CCFileUtils::sharedFileUtils()->getFileData(filename, "rb", &nSize);
	CCLOG(">>>>>>>>>>>>>>>  File  %s  len %lu",filename , nSize);

	// Open the zip file
	unzFile zipfile = unzOpen(filename);
	if (! zipfile)
	{
		CCLOG("can not open downloaded zip file %s", filename);
		return false;
	}

	// Get info about the zip file
	unz_global_info global_info;
	if (unzGetGlobalInfo(zipfile, &global_info) != UNZ_OK)
	{
		CCLOG("can not read file global info of %s", filename);
		unzClose(zipfile);
		return false;
	}

	// Buffer to hold data read from the zip file
	char readBuffer[8192];

	CCLOG("start uncompressing");

	string _storagePath = path;
	// Loop to extract all files.
	uLong i;
	for (i = 0; i < global_info.number_entry; ++i)
	{
		// Get info about current file.
		unz_file_info fileInfo;
		char fileName[512];
		if (unzGetCurrentFileInfo(zipfile,
			&fileInfo,
			fileName,
			512,
			NULL,
			0,
			NULL,
			0) != UNZ_OK)
		{
			CCLOG("can not read file info");
			unzClose(zipfile);
			return false;
		}

		string fullPath = _storagePath + fileName;

		// Check if this entry is a directory or a file.
		const size_t filenameLength = strlen(fileName);
		if (fileName[filenameLength-1] == '/')
		{
			// Entry is a direcotry, so create it.
			// If the directory exists, it will failed scilently.
			if (!createDirectory(fullPath.c_str()))
			{
				CCLOG("can not create directory %s", fullPath.c_str());
				unzClose(zipfile);
				return false;
			}
		}
		else
		{
			// Entry is a file, so extract it.

			// Open current file.
			if (unzOpenCurrentFile(zipfile) != UNZ_OK)
			{
				CCLOG("can not open file %s", fileName);
				unzClose(zipfile);
				return false;
			}

			//先确保路径
			size_t tp = fullPath.find_last_of("/");
			if(tp != string::npos)
			{
				string folderPath = fullPath.substr(0,fullPath.find_last_of("/")+1);
				CCFileUtils::sharedFileUtils()->createDirectorys(folderPath.c_str());
			}

			// Create a file to store current file.
			FILE *out = fopen(fullPath.c_str(), "wb");
			if (! out)
			{
				CCLOG("can not open destination file %s", fullPath.c_str());
				unzCloseCurrentFile(zipfile);
				unzClose(zipfile);
				return false;
			}

			// Write current file content to destinate file.
			int error = UNZ_OK;
			do
			{
				error = unzReadCurrentFile(zipfile, readBuffer, 8152);
				if (error < 0)
				{
					CCLOG("can not read zip file %s, error code is %d", fileName, error);
					unzCloseCurrentFile(zipfile);
					unzClose(zipfile);
					return false;
				}

				if (error > 0)
				{
					fwrite(readBuffer, error, 1, out);
				}
			} while(error > 0);

			fclose(out);
		}

		unzCloseCurrentFile(zipfile);

		// Goto next entry listed in the zip file.
		if ((i+1) < global_info.number_entry)
		{
			if (unzGoToNextFile(zipfile) != UNZ_OK)
			{
				CCLOG("can not read next file");
				unzClose(zipfile);
				return false;
			}
		}
	}

	CCLOG("end uncompressing");

	return true;
}

/*
 * Create a direcotry is platform depended.
 */
bool FileMgr::createDirectory(const char *path)
{
#if (CC_TARGET_PLATFORM != CC_PLATFORM_WIN32)
    mode_t processMask = umask(0);
    int ret = mkdir(path, S_IRWXU | S_IRWXG | S_IRWXO);
    umask(processMask);
    if (ret != 0 && (errno != EEXIST))
    {
        return false;
    }
    
    return true;
#else
    BOOL ret = CreateDirectoryA(path, NULL);
	if (!ret && ERROR_ALREADY_EXISTS != GetLastError())
	{
		return false;
	}
    return true;
#endif
}


NS_FM_END