#include "comm_func.h"
#include "servant/Application.h"

void StrCpy( char *szDst, const char *szSrc, int iBufLen ){
    strncpy(szDst,szSrc,iBufLen - 1);
    szDst[iBufLen-1] = 0;
}


WORD GetIconMagicWord()
{
	return 0xFEE9;
}

unsigned int MsPass(struct timeval* pstTv1, struct timeval* pstTv2){
	int iSec;
    iSec = pstTv1->tv_sec - pstTv2->tv_sec;
    if (iSec < 0 || iSec >100000) iSec = 100000;
    return iSec * 1000 + (pstTv1->tv_usec - pstTv2->tv_usec)/1000;
}


void UTF8_2_UNICODE(char *szDst, unsigned int &dwDstStrLen, const char *szSrc, int iSrcSize, bool bZeroEnd){
    dwDstStrLen = 0;
    int s = 0;
    
	while(s <iSrcSize){
        if(bZeroEnd && (0 == szSrc[s]) )
        {
        	break;
        }
		
		if ( ( szSrc[s] & 0x80) == 0){// ASCII
		     szDst[dwDstStrLen++] = szSrc[s];
             szDst[dwDstStrLen++] = 0;

			 //char currentCh = szSrc[s];
             s++;

			 // $特殊处理，客户端图标设置导致的
			 /*if( ('$' == currentCh) 
			 	&& (s <= (iSrcSize - 2) ) )
			 {
			 	memcpy(szDst + dwDstStrLen, szSrc + s, sizeof(short) );
				s+=2;
				dwDstStrLen+=2;
			 }*/
			 
             continue;
		}

		if(s <= (iSrcSize - 4) )
		{
			WORD wValue = *(WORD*)(&szSrc[s]);
			if(GetIconMagicWord() == wValue)
			{
				memcpy(szDst + dwDstStrLen, szSrc + s, 4);
				s+=4;
				dwDstStrLen += 4;
				continue;
			}
		}
        
		if (0xe0 == ( szSrc[s] & 0xe0) ){  // 0x800 .. 0xffff
           // TODO越界和后续字符的检查 
           *(short*)(&szDst[dwDstStrLen]) = (((short)(szSrc[s] &0x0f)) << 12) | (((short) (szSrc[s+1] &0x3f))<<6) |(( szSrc[s + 2] &0x3f));
           dwDstStrLen += 2;
           s += 3;
           continue;
		    
        }
        
		if (0xc0  == (szSrc[s] &0xc0)){ // 0x80 .. 0x7ff
		    // TODO越界和后续字符的检查
		    *(short*)(&szDst[dwDstStrLen]) = (((short)(szSrc[s] &0x1f)) << 6) | (szSrc[s+1] &0x3f);
            dwDstStrLen +=2;
            s +=2;
            continue;
		}

        // OTHER 直接过滤
        s++;
	}
}

void CapStrCpy(char * szDst, unsigned char &cDstStrLen, const char *szSrc, int iBufLen,bool bUnicode ){
    if( bUnicode){
        unsigned int dwDstStrLen =0;
        UTF8_2_UNICODE( szDst,dwDstStrLen, szSrc, iBufLen );
        cDstStrLen  = (unsigned char) dwDstStrLen;
        szDst[cDstStrLen ++] = 0;
    }
    else{
    	StrCpy(szDst,szSrc,iBufLen);
    	cDstStrLen = (unsigned char) strlen(szDst) + 1;
    }
}

//added by stonesun for task
void CapLongStrCpy(char * szDst, unsigned short &wDstStrLen, const char *szSrc, int iBufLen, bool bUnicode ){
    if( bUnicode){
        unsigned int dwDstStrLen =0;
        UTF8_2_UNICODE( szDst,dwDstStrLen, szSrc, iBufLen );
        wDstStrLen  = (unsigned short) dwDstStrLen;
        szDst[wDstStrLen ++] = 0;
    }
    else{
    	strncpy(szDst, szSrc, iBufLen - 1);
    	szDst[iBufLen-1] = 0;
    	wDstStrLen = (unsigned short) (strlen(szDst) + 1);
    }
}
//added end

void 	CapBinaryStrCpy(char *szDst, unsigned char& cDstStrLen, const char *szSrc, int iBufLen)
{
	unsigned int dwDstStrLen =0;
    UTF8_2_UNICODE( szDst,dwDstStrLen, szSrc, iBufLen, false);
    cDstStrLen  = (unsigned char) dwDstStrLen;
    szDst[cDstStrLen ++] = 0;
}

void    CapLongBinaryStrCpy(char * szDst, unsigned short &wDstStrLen, const char *szSrc, int iBufLen)
{
	unsigned int dwDstStrLen =0;
    UTF8_2_UNICODE( szDst,dwDstStrLen, szSrc, iBufLen, false);
    wDstStrLen  = (unsigned short) dwDstStrLen;
    szDst[wDstStrLen ++] = 0;
}


char* MakeIPStr( int iIPAddr ){
    return inet_ntoa(*(struct in_addr *)(&iIPAddr));
}

int StartDaemonProcess(const char szWorkDir [ ]){
	if( 0 != fork())
		exit(0);

	if( setsid() < 0 )
		return -1;

	if( 0 != fork() )
		exit( 0);

	umask(0);

	signal(SIGINT,SIG_IGN);
    signal(SIGHUP,SIG_IGN);
    signal(SIGQUIT,SIG_IGN);
    signal(SIGPIPE,SIG_IGN);
    signal(SIGTTOU,SIG_IGN);
    signal(SIGTTIN,SIG_IGN);
    signal(SIGUSR1,SIG_IGN);
    signal(SIGUSR2,SIG_IGN);
    signal(SIGALRM,SIG_IGN);

      if (chdir(szWorkDir) != 0)
        return    -1;

    fclose(stdin);
    fclose(stdout);
    fclose(stderr);

    return 0;
}
 

bool 	IsProcessSingle(char * szProcessName){
	char 	szCmd[128];
	char 	szResult[128];
	FILE 	*pFile;

	snprintf(szCmd,sizeof(szCmd),
            "ps -ef | grep \"%s\" | " 
            "grep -v grep | "
            "wc -l",
            basename(szProcessName) );

	pFile = popen( szCmd, "r" );
    if( NULL == pFile )
        return  FALSE;

    fgets( szResult, sizeof(szResult), pFile );
    pclose( pFile );

	if( atoi(szResult) - 1 > 0 )
        return  FALSE;
	
	return TRUE;
	
}

int StopProcess( char *szProcessName){
	
	char	szCmd[128];
	signal( SIGUSR1, SIG_IGN );
  
	snprintf( szCmd, sizeof(szCmd), "killall -SIGUSR1 %s",
			basename(szProcessName) );
  
	system( szCmd );

	usleep(2000 * 1000);

	if ( IsProcessSingle( szProcessName) < 0 ){
		snprintf( szCmd, sizeof(szCmd),"killall -SIGKILL %s",
                basename(szProcessName) );
   
        system( szCmd );
	}

	return 0;
}


int AnalysizeZoneIndex ( int iEntryID , int *piWorldIdx , int *piZoneIdx ){
    unsigned char szTemp[4] ;
    
    memcpy(&szTemp[0] , &iEntryID , sizeof(int) ) ;
    
    *piWorldIdx = szTemp[0] ;    

    *piZoneIdx = szTemp[3] ;

    if ( *piWorldIdx < 0 || *piZoneIdx < 0 )
        return -1 ;
    
    return (int)szTemp[2] ;
}

//PHP中采用的字符串Hash算法
unsigned int Str2HashInt( const char * arKey, unsigned int nKeyLength){
	unsigned int h = 0,g; 
    const char *arEnd = arKey + nKeyLength; 
     
    while (arKey < arEnd) {
		
        h = (h << 4) + *arKey++; 
        if ((g = (h & 0xF0000000))){
            h = h ^ (g >> 24); 
            h = h ^ g; 
        } 
    }

    return  h; 
}


void StrTrim(char * strInput){
	char *pStrHead;
	char *pStrEnd;
	int iTempLength;

	if( strInput == NULL )
		return;

	iTempLength = strlen(strInput);
	if( iTempLength == 0 )
		return;
	
	pStrHead = strInput;
	while (((*pStrHead == ' ') || (*pStrHead == '\t') || (*pStrHead == '\n') || (*pStrHead == '\r')) && (*pStrHead != 0)){
		pStrHead ++;
	}
	
	pStrEnd= &strInput[iTempLength-1];
	while ((pStrEnd >= pStrHead) && ((*pStrEnd == ' ') || (*pStrEnd == '\t') || (*pStrEnd == '\n') || (*pStrEnd == '\r')))
	{
		pStrEnd --;
	}
	
	*(pStrEnd+1) = '\0';

	strcpy( strInput, pStrEnd );
	
}


int TimeValMinus(timeval& tvA, TIME_VAL& tvB, TIME_VAL& tvResult){
	
    if(tvA.tv_sec < tvB.tv_sec || (tvA.tv_sec == tvB.tv_sec && tvA.tv_usec < tvB.tv_usec)){
        tvResult.tv_sec = 0;
        tvResult.tv_usec = 0;
        return 0;
    }
    
    timeval tvTemp;
    if( tvA.tv_usec < tvB.tv_usec ){
        tvTemp.tv_usec = (1000000 + tvA.tv_usec) - tvB.tv_usec;
        tvTemp.tv_sec = tvA.tv_sec - tvB.tv_sec - 1;
    }
    else{
        tvTemp.tv_usec = tvA.tv_usec - tvB.tv_usec;
        tvTemp.tv_sec  = tvA.tv_sec - tvB.tv_sec;
    }
    
    tvResult.tv_sec = tvTemp.tv_sec;
    tvResult.tv_usec = tvTemp.tv_usec;
    
    return 0;
}

float FastInvSqrt(float x){
    // to see why works, read paper on www.lomont.org
    float xhalf = 0.5f*x;
    int i;
    
    _fval_to_ival(x,i);      // get bits for floating value
    i = 0x5f375a86 - (i>>1); // gives initial guess y0 with magic number
    _ival_to_fval(i,x);      // convert bits back to float
    x = x*(1.5f-xhalf*x*x);  // Newton step, repeating increases accuracy
    return x;
}

int GetUinxTime(const char *sTime/*,time_t *pUinxTime*/){
    char *pLine,sTemp[50];
    struct tm *pstTime;
    time_t ltime=time(NULL);
    int iYear,iMonth,iDate;

    strncpy(sTemp,sTime,sizeof(sTemp));
    pLine=sTemp;
	if ( pLine){
    	iYear=atoi(strsep(&pLine,"-"));
    	iMonth=atoi(strsep(&pLine,"-"));
    	iDate=atoi(strsep(&pLine,"-"));
	}

    struct tm result;
    pstTime = localtime_r( &ltime, &result );
    pstTime->tm_hour=0;
    pstTime->tm_sec=0;
    pstTime->tm_min=0;
    pstTime->tm_year=iYear-1900;
    pstTime->tm_mon=iMonth-1;
    pstTime->tm_mday=iDate;
    ltime=mktime(pstTime);
    //*pUinxTime=ltime;
    return ltime;
}

void GetStringTime(time_t timeNow, char * pszTime, int iLen){
	if(iLen < 32 || iLen >64)
		return;

	time_t tTime = timeNow;
	struct tm result;
	struct tm *pstTime = localtime_r(&tTime, &result );

	snprintf(pszTime, iLen, "%.4d-%.2d-%.2d %.2d:%.2d:%.2d", pstTime->tm_year+1900, pstTime->tm_mon+1, 
		pstTime->tm_mday,pstTime->tm_hour, pstTime->tm_min, pstTime->tm_sec);
}

//从一个整型获得ZoneSvr在Dir树中的位置标记，包括:世界组ID,世界ID,线组ID,线ID
void IntToSvrID(const unsigned int iComID, char& cWorldGrpID,char& cWorldID,char& cZoneGrpID,char& cZoneSvrID)
{
    cZoneSvrID = iComID & 0xFF;
    cZoneGrpID = (iComID >> 8) & 0xFF;
    cWorldID   = (iComID >> 16) & 0xFF;
    cWorldGrpID = (iComID >> 24) & 0xFF;
}
//将世界组ID,世界ID,线组ID,线ID组合生成ZoneSvr在Dir树中的int形式的位置标记
//unsigned int内容格式如下(各ID占1个字节):
//cWorlGrpID|cWorldID|cZoneGrpID|cZoneSvrID
//      8bit|    8bit|      8bit|     8bit|      
unsigned int SvrIDToInt(const char cWorldGrpID,const char cWorldID,const char cZoneGrpID,const char cZoneSvrID)
{
    unsigned int iComID = cZoneSvrID;
    iComID += cZoneGrpID << 8;
    iComID += cWorldID << 16;
    iComID += cWorldGrpID << 24;
    return iComID;
}

bool IsStringNum(char *szNumString, int iLen)
{
    if ( !szNumString )
    {
        return false;
    }

    int iStrLen = strlen(szNumString);
    
    int iMinLen = iLen;

    if ( iLen > iStrLen)
    {
        iMinLen = iStrLen;
    }

    if ( 0 == iMinLen )
    {
        return false;
    }
    else if ( 1 == iMinLen )
    {
        if (szNumString[0] <  '0' || szNumString[0] > '9' )
        {
            return false;
        }
    }
    else
    {
        for ( int i = 0; i < iMinLen; i++ )
        {
            if (szNumString[i] < '0' || szNumString[i] > '9' )
            {
                return false;
            }            
        }
    }
    return true;   
}

int AdvanceAtoi(const std::string& strValue)
{
	char* pEndStr = NULL;
	long int iValue = strtol(strValue.c_str(), &pEndStr, 10);
	if(pEndStr == strValue.c_str() )
	{
		return 0;
	}

	if(*pEndStr == 'X' || *pEndStr == 'x')
	{
		return (int)strtol(strValue.c_str(), &pEndStr, 16);
	}

	return (int)iValue;
}

void MakeTextFromTemplatePara(const string &sTemplateCont, map<string, string> &mParam, string &sRetText)
{
    sRetText = sTemplateCont;
    string::size_type pos1,pos2;
    pos1 = sRetText.find("$");
    map<string, string>::iterator it = mParam.begin();
    map<string, string>::iterator iter;

    while(string::npos != pos1 && (pos1+1) < sRetText.length())
    {
        string key;
        if(sRetText.at(pos1+1) == '{')
        {
            pos2 = sRetText.find("}", pos1+1);
            key = sRetText.substr(pos1+2, pos2-pos1-2);
            if((iter=mParam.find(key)) != mParam.end() )
            {
                sRetText.replace(pos1, key.length()+3, iter->second);
            }
            pos2 = pos2-key.length()-3+iter->second.length();
            pos1 = sRetText.find("$", pos2+1);

        }
    }
}

bool getRightValByLevel(const std::string& sDesc, int iLevel, int &iHitVal)
{
	iHitVal = 0;
    if(sDesc.size() <= 0)
        return false;
	vector<string> paramList = TC_Common::sepstr<string>(sDesc, "#");
	if( (paramList.size() % 2) != 0)
	{
		return false;
	}
	
	int iLevelSize = (int)paramList.size()/2;
	for(int i = 0; i < iLevelSize; i++)
	{
		int iMinLevel = AdvanceAtoi(paramList[i*2]);
		if(iLevel < iMinLevel)
		{
			break;
		}
		iHitVal = AdvanceAtoi(paramList[i*2 + 1]);
	}

	if(0 == iHitVal)
	{
		return false;
	}

	return true;
}

