#include    "GameEnginePch.h"
#include    <stdlib.h>
#include    <assert.h>
#include    "ZoneTime.h"


extern "C" IComponent* createZoneTime(Int32)
{
	return new CZoneTime;
}

CZoneTime::CZoneTime():m_dwLastSecond(0),m_dwMaxID(1),m_iCurSecondGapForTest(0){
    UpdateTime();
    SetZoneTimeOff();

}

void CZoneTime::UpdateTime(){
    gettimeofday(&m_stCurrTv, NULL);  

    if (m_dwLastSecond != (DWORD)m_stCurrTv.tv_sec){
        m_dwLastSecond  = m_stCurrTv.tv_sec;
        m_dwMaxID       = 1;
    }
}

const char * CZoneTime::GetCurTimeStr(){
    static char    s_szTime[128];
    struct tm    stCurr;

    localtime_r(&m_stCurrTv.tv_sec,&stCurr);

    if (stCurr.tm_year > 50){
        snprintf(s_szTime, sizeof(s_szTime),
                    "%04d-%02d-%02d %02d:%02d:%02d", 
                    stCurr.tm_year+1900, stCurr.tm_mon+1, stCurr.tm_mday,
                    stCurr.tm_hour, stCurr.tm_min, stCurr.tm_sec);
    }
    else{
        snprintf(s_szTime, sizeof(s_szTime), 
                    "%04d-%02d-%02d %02d:%02d:%02d", 
                    stCurr.tm_year+2000, stCurr.tm_mon+1, stCurr.tm_mday,
                    stCurr.tm_hour, stCurr.tm_min, stCurr.tm_sec);
    }
                
    return s_szTime;
}

const char * CZoneTime::GetCurVIVOTimeStr()
{
	 static char    s_szTime[128];
    struct tm    stCurr;

    localtime_r(&m_stCurrTv.tv_sec,&stCurr);

    if (stCurr.tm_year > 50){
        snprintf(s_szTime, sizeof(s_szTime),
                    "%04d%02d%02d%02d%02d%02d", 
                    stCurr.tm_year+1900, stCurr.tm_mon+1, stCurr.tm_mday,
                    stCurr.tm_hour, stCurr.tm_min, stCurr.tm_sec);
    }
    else{
        snprintf(s_szTime, sizeof(s_szTime), 
                    "%04d%02d%02d%02d%02d%02d", 
                    stCurr.tm_year+2000, stCurr.tm_mon+1, stCurr.tm_mday,
                    stCurr.tm_hour, stCurr.tm_min, stCurr.tm_sec);
    }
                
    return s_szTime;
}


DWORD CZoneTime::StrTime2Seconds( const char *szTime ,const char *szFormat){
	struct tm curr={0};

	if(NULL == strptime( szTime, szFormat,&curr))
		return 0;

	return (DWORD)mktime(&curr);
}


void CZoneTime::SetZoneTimeOff(){
    time_t         lNow;
    struct tm    curr;
    
    time(&lNow);
       curr = *localtime(&lNow);

    m_itOff = curr.tm_gmtoff;
}

DWORD    CZoneTime::GetCurDayLeftSeconds(){
    DWORD    ulNow = time(NULL) + m_itOff;

    DWORD    ulReturnSeconds = ONE_DAY_SECONDS - (ulNow  % ONE_DAY_SECONDS);

    return    ulReturnSeconds; 
}

DWORD    CZoneTime::GetCurWeekLeftSeconds(){
    DWORD    ulNow = time(NULL) + m_itOff;

    DWORD    ulReturnSeconds = ONE_WEEK_SECONDS - (( ulNow + 4 * ONE_DAY_SECONDS ) % ONE_WEEK_SECONDS);

    return  ulReturnSeconds;
}

int CZoneTime::GetWeekNo(DWORD ulTime)
{    
    // ( 天数 + 星期几) / 7 +1 =第几周
    int iWeekNo = ( ulTime + m_itOff);        
    iWeekNo /= ONE_DAY_SECONDS;        // 算天数
    iWeekNo += 4;    // 1970-1-1 星期四
    iWeekNo /= 7;    // 每周7天
    iWeekNo += 1;    // 第1天就是第一周
    
    return iWeekNo;
}

int CZoneTime::GetChineseWeekNo(DWORD dwTime)
{
	DWORD dwWeekNo = dwTime + m_itOff;
	dwWeekNo = dwWeekNo/ONE_DAY_SECONDS;
	dwWeekNo += 3;	 // 1970-1-1 星期四,但是一般人认为星期一是一周开始, 这里要调下
	dwWeekNo/=7;

	dwWeekNo += 1;// 好吧，第一天是第一周
	return dwWeekNo;
}


int CZoneTime::GetDayInWeek(DWORD dwTime){
    time_t tTime = dwTime;
    struct tm Tm;

    if (!localtime_r(&tTime,&Tm))
        return    -1;

    if (Tm.tm_wday == 0)
        return    7;

    return    Tm.tm_wday;
}
int CZoneTime::GetDayInMonth(DWORD dwTime)
{
    time_t tTime = dwTime;
    struct tm Tm;

    if (!localtime_r(&tTime,&Tm))
        return    -1;

    return    Tm.tm_mday;
}

int CZoneTime::GetHourInDay(DWORD dwTime)
{
    time_t tTime = dwTime;
    struct tm Tm;

    if (!localtime_r(&tTime,&Tm))
        return    -1;

    return    Tm.tm_hour;
}

int CZoneTime::GetMinInHour(DWORD dwTime){
    time_t tTime = dwTime;
    struct tm Tm;

    if (!localtime_r(&tTime,&Tm))
        return    -1;

    return    Tm.tm_min;
}

int	CZoneTime::GetDayInYear(DWORD dwTime){
	time_t tTime = dwTime;
	struct tm *pstNowTime = localtime(&tTime);
	
    return pstNowTime->tm_yday;
}

int	CZoneTime::GetChineseDay(DWORD dwTime){
	DWORD dwDayNo = dwTime + m_itOff;
	dwDayNo = dwDayNo / ONE_DAY_SECONDS;
	dwDayNo++;
	return dwDayNo;
}



bool CZoneTime::IsInSameWeek( DWORD ulTime1, DWORD ulTime2 ){
    int iWeek1 = GetWeekNo(ulTime1);    
    int iWeek2 = GetWeekNo(ulTime2);

    return ( (iWeek1 == iWeek2) && (abs((int)ulTime1 - (int)ulTime2) <= ONE_DAY_SECONDS * 7) );
}

bool CZoneTime::IsInSameChineseWeek(DWORD ulTime, DWORD ulNowTime)
{
	int nCWeek1 = GetChineseWeekNo(ulTime);
	int nCWeek2 = GetChineseWeekNo(ulNowTime);

    return ( (nCWeek1 == nCWeek2) && (abs((int)ulTime - (int)ulNowTime) <= ONE_DAY_SECONDS * 7) );
}


bool CZoneTime::IsInSameDay( time_t ulTime, time_t ulNowTime ){
    struct tm *pstTime = localtime(&ulTime);
    int iDay = pstTime->tm_mday;

    struct tm *pstNowTime = localtime(&ulNowTime);
    int iNowDay = pstNowTime->tm_mday;

    if((iDay == iNowDay) && (abs(ulTime - ulNowTime) < ONE_DAY_SECONDS))
        return true;

    return false;
}

bool CZoneTime::IsInSameMonth( time_t ulTime, time_t ulNowTime )
{
    struct tm *pstTime = localtime(&ulTime);
    int iMonth = pstTime->tm_mon;

    struct tm *pstNowTime = localtime(&ulNowTime);
    int iNowMonth = pstNowTime->tm_mon;

    if((iMonth == iNowMonth) && (abs(ulTime - ulNowTime) < ONE_DAY_SECONDS*31))
        return true;

    return false;
}

bool CZoneTime::IsLastDayOfMonth( time_t ulTime ){
    struct tm *pstTime = localtime(&ulTime);
    int iMonth  = pstTime->tm_mon;
    int iDay    = pstTime->tm_mday;
    if( (0 == iMonth) || (2 == iMonth) || (4 == iMonth)||(6 == iMonth)||
        (7 == iMonth) || (9 == iMonth) || (11 == iMonth))
        return (31 == iDay );
    else if( 1== iDay)
        return ((28== iDay) || (29 == iDay));
    else 
        return (30 == iDay );
}

DWORD CZoneTime::GetCurMaxID(){
    DWORD    dwRet = m_dwMaxID;

    m_dwMaxID ++;

    return    dwRet;
}

long CZoneTime::GetIntDayAToB( long lNow, long lLast ){       
    if (0 == lLast || lNow < lLast)
        return 0;
    
    struct tm *pstNowTime = localtime( &lNow );
    struct tm tTime = {0};
    tTime.tm_year  = pstNowTime->tm_year;
    tTime.tm_mon   = pstNowTime->tm_mon;
    tTime.tm_mday  = pstNowTime->tm_mday;    
    lNow = mktime( &tTime );

    struct tm *pstNowTimeB = localtime( &lLast );
    struct tm tTimeB = {0};
    tTimeB.tm_year = pstNowTimeB->tm_year;
    tTimeB.tm_mon  = pstNowTimeB->tm_mon;
    tTimeB.tm_mday = pstNowTimeB->tm_mday;    
    lLast = mktime( &tTimeB );

    long lTime = lNow - lLast;
    lTime = lTime / ONE_DAY_SECONDS;   // 计算天数

    return lTime;
}

DWORD CZoneTime::GetDayBeginSecond(DWORD dwTime)
{
	long lTime = dwTime;
	struct tm *pstNowTime = localtime( &lTime );
    struct tm tTime = {0};
    tTime.tm_year  = pstNowTime->tm_year;
    tTime.tm_mon   = pstNowTime->tm_mon;
    tTime.tm_mday  = pstNowTime->tm_mday;    
    DWORD dwResultTime = mktime( &tTime );

	return dwResultTime;
}

//added by learli 为时装超时而写的一个函数
/*void CZoneTime::GetTimeInt(time_t sometime,CProto::DATETIME *stDateTime)
{
	struct tm    stCurr;

	localtime_r(&sometime,&stCurr);

	if (stCurr.tm_year > 50){
	stDateTime->m_dwYear=stCurr.tm_year+1900;
	stDateTime->m_bMonth=stCurr.tm_mon+1;
	stDateTime->m_bDay=stCurr.tm_mday;
	stDateTime->m_bHour=stCurr.tm_hour;
	stDateTime->m_bMin=stCurr.tm_min;
	stDateTime->m_bSec=stCurr.tm_sec;
	}
	else{
	stDateTime->m_dwYear=stCurr.tm_year+2000;
	stDateTime->m_bMonth=stCurr.tm_mon+1;
	stDateTime->m_bDay=stCurr.tm_mday;
	stDateTime->m_bHour=stCurr.tm_hour;
	stDateTime->m_bMin=stCurr.tm_min;
	stDateTime->m_bSec=stCurr.tm_sec;
	}
}*/

DWORD  CZoneTime::GetFirstSecondsOnSameDay(DWORD dwSeconds)
{
    time_t tTime = dwSeconds;
    struct tm Tm;
    if (!localtime_r(&tTime,&Tm))
        return   0;

    Tm.tm_hour  = 0;
    Tm.tm_min   = 0;
    Tm.tm_sec   = 0;

    return mktime(&Tm);
}

DWORD CZoneTime::GetFirstSecondsOnSameChinesWeek(DWORD dwSeconds)
{
    return dwSeconds + GetCurWeekLeftSeconds() - ONE_WEEK_SECONDS;
}

DWORD CZoneTime::GetFirstSecondsOnSameMonth(DWORD dwSeconds)
{
    time_t tTime = dwSeconds;
    struct tm Tm;
    if (!localtime_r(&tTime,&Tm))
        return   0;
    
    Tm.tm_sec   = 0;
    Tm.tm_min   = 0;
    Tm.tm_hour  = 0;
    Tm.tm_mday  = 1;

    return mktime(&Tm);
}
//added by stonesun
DWORD  CZoneTime::GetEarliestTimeOnSameDay(DWORD dwTime)
{
    int iHour = GetHourInDay(dwTime);
    int iMin = GetMinInHour(dwTime);

    int iSec = (iHour * 60 + iMin) * 60;

    //返回一天里最早的时刻0:0:10秒
    return (DWORD)(dwTime - iSec + 10);
}

DWORD  CZoneTime::GetNextDayEarliestTime(DWORD dwTime)
{
    int iHour = GetHourInDay(dwTime);
    int iMin = GetMinInHour(dwTime);

    int iSec = (iHour * 60 + iMin) * 60;

    return (DWORD)(1440 * 60 + dwTime - iSec + 10);
}

//added end

//add By hyf

int CZoneTime::GetMonthInYear(DWORD dwTime)
{
	time_t tTime = dwTime;
    struct tm Tm;

    if (!localtime_r(&tTime,&Tm))
        return    -1;

    return    Tm.tm_mon + 1;
}



