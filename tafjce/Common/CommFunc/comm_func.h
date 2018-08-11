#ifndef _COMM_FUNC_H
#define _COMM_FUNC_H

// 基础公共函数
#include <libgen.h>
#include <libgen.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/time.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "define.h"

#define _fval_to_ival(fval,ival) (ival) = *(int*)&(fval)
#define _ival_to_fval(ival,fval) (fval) = *(float*)&(ival)

#define MAX_CAT_LEN 131072
#define StrCat( str, iLen, format, ... ) do{\
    char szMsg[MAX_CAT_LEN];\
	snprintf( szMsg, MAX_CAT_LEN-1, format, ##__VA_ARGS__ ); \
    szMsg[MAX_CAT_LEN-1] = 0;\
    int iCatLen = iLen - strnlen(str, iLen);\
	if( iCatLen > 0 )\
	{\
		strncat( str, szMsg, iCatLen );\
	}\
}while(0)

//MASK标记
//MASK直接对应了相应的MASK值
#define CLS_MASK(DATA,MASK) (DATA) = ((DATA) & (~(MASK)))
#define SET_MASK(DATA,MASK) (DATA) = ((DATA) | (MASK))
#define IS_MASK(DATA,MASK)  ((((DATA) & (MASK)) > 0)?true:false)
//MASKPOS对应了MASK的bit位置,从0开始，0对应最右边的bit位
#define CLS_MASK_BYPOS(DATA,MASKPOS) (DATA) = ((DATA) & (~(1<<(MASKPOS))))
#define SET_MASK_BYPOS(DATA,MASKPOS) (DATA) = ((DATA) | (1<<(MASKPOS)))
#define IS_MASK_BYPOS(DATA,MASKPOS)  ((((DATA) & (1<<(MASKPOS))) > 0)?true:false)

void 	StrCpy( char *szDst, const char *szSrc, int iBufLen);
void    UTF8_2_UNICODE(char *szDst, unsigned int &dwDstStrLen, const char *szSrc, int iSrcSize, bool bZeroEnd = true);
void 	CapStrCpy(char *szDst, unsigned char& cDstStrLen, const char *szSrc, int iBufLen, bool bUnicode = false );
void    CapLongStrCpy(char * szDst, unsigned short &wDstStrLen, const char *szSrc, int iBufLen,bool bUniCode = false );

void 	CapBinaryStrCpy(char *szDst, unsigned char& cDstStrLen, const char *szSrc, int iBufLen);
void    CapLongBinaryStrCpy(char * szDst, unsigned short &wDstStrLen, const char *szSrc, int iBufLen);
WORD 	GetIconMagicWord();


char 	*MakeIPStr( int iIPAddr);
int 	StartDaemonProcess(const char szWorkDir [ ]);
bool 	IsProcessSingle(char * szProcessName);
int 	StopProcess(char * szProcessName);
int 	AnalysizeZoneIndex(int iEntryID, int *piWorldIndex, int *piZoneIndex);
unsigned int Str2HashInt( const char * arkey, unsigned int nKeyLength);
void 	StrTrim(char * strInput);

int 	TimValMinus( timeval & tvA, TIME_VAL &tvB, TIME_VAL &tvResult);
float   FastInvSqrt(float x);
void 	GetStringTime( time_t timeNow, char *pszTime, int len);

//从一个整型获得ZoneSvr在Dir树中的位置标记，包括:世界组ID,世界ID,线组ID,线ID
void         IntToSvrID(const unsigned int iComID, char& cWorldGrpID,char& cWorldID,char& cZoneGrpID,char& cZoneSvrID);
//将世界组ID,世界ID,线组ID,线ID组合生成ZoneSvr在Dir树中的int形式的位置标记
//unsigned int内容格式如下(各ID占1个字节):
//cWorlGrpID|cWorldID|cZoneGrpID|cZoneSvrID
//      8bit|    8bit|      8bit|     8bit|      
unsigned int SvrIDToInt(const char cWorldGrpID,const char cWorldID,const char cZoneGrpID,const char cZoneSvrID);

bool IsStringNum(char *szNumString, int iLen);
int AdvanceAtoi(const std::string& strValue);

unsigned int MsPass(struct timeval* pstTv1, struct timeval* pstTv2);



//模板中的变量参数格式为${parakey},其中A为实际的键值名,在mParam中填充的是parakey对应的值
void MakeTextFromTemplatePara(const string &sTemplateCont, map<string, string> &mParam, string &sRetText);

//sDesc格式必须为(#分隔):最低等级1#100#最低等级2#300
//如:  1#100#50#120
//返回false表明没有找到对应配置值或者格式错误,此时出参iHitVal保证为0
bool getRightValByLevel(const std::string& sDesc, int iLevel, int &iHitVal);

inline unsigned int GetClientIpFromConnPos( INT64 iConnPos )
{ 
    return (unsigned int)( iConnPos >> 16 );
}
inline unsigned short GetClientPortFromConnPos( INT64 iConnPos )
{
	return (unsigned short)(iConnPos &0xFFFF);
}
inline string IntIpToStrIp( unsigned int iIPAddr)
{
	char* sIp = inet_ntoa(*(struct in_addr *)(&iIPAddr));
	return string(sIp,strlen(sIp));
}


#endif

