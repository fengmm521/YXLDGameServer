#ifndef _ZONE_TIME_20100914_H_
#define _ZONE_TIME_20100914_H_

#include    <stdio.h>
#include    <string.h>
#include    <time.h>
#include    <sys/time.h>
#include    "define.h"
//#include    "og_comm.h"
//#include 	"CProto.h"
#include    "IZoneTime.h"

class   CZoneTime:public ComponentBase<IZoneTime, IID_IZoneTime>
{
	public:
	    CZoneTime();

	    void     UpdateTime();
	    
	    struct timeval *GetCurTime() { return &m_stCurrTv;}
	    
	    DWORD    GetCurSecond() {return m_stCurrTv.tv_sec + m_iCurSecondGapForTest;}
	    DWORD    GetCurUSecond() {return m_stCurrTv.tv_usec;}

	    const char *GetCurTimeStr();
        DWORD    StrTime2Seconds( const char *szTime ,const char *szFormat);

	    DWORD    TimePass(struct timeval *pstTime) {return MsPass(&m_stCurrTv, pstTime);}
	    
	    int      GetZoneTimeOff() {return m_itOff;}
	    DWORD    GetCurDayLeftSeconds();
	    DWORD    GetCurWeekLeftSeconds();
	    // ulTime的星期数
	    int      GetWeekNo(  DWORD ulTime  );

		int 	 GetChineseWeekNo(DWORD dwTime);
		
	    // dwTime是星期几 返回值 1-7
	    int      GetDayInWeek(DWORD dwTime);
	    // 当天所在0-23哪个小时
	    int      GetHourInDay(DWORD dwTime);
	    // 在当前小时的0-59哪分钟
	    int      GetMinInHour(DWORD dwTime);
		//在一年中的第几天
		int		 GetDayInYear(DWORD dwTime);
		//从原始时间开始的第几天
		int		 GetChineseDay(DWORD dwTime);
		//在一月中的第几天
	    int		 GetDayInMonth(DWORD dwTime);

		//
		int GetMonthInYear(DWORD dwTime);

	    bool     IsInSameWeek( DWORD ulTime, DWORD ulNowTime );
		bool 	 IsInSameChineseWeek(DWORD ulTime, DWORD ulNowTime);
	    bool     IsInSameDay( time_t ulTime, time_t ulNowTime );
	    bool     IsInSameMonth( time_t ulTime, time_t ulNowTime );
        bool     IsLastDayOfMonth( time_t ulTime );
	    DWORD    GetCurMaxID(); 
	    long     GetIntDayAToB( long lATime, long lBTime );
        
        //added by learli
        //void     GetTimeInt(time_t sometime,CProto::DATETIME *stDateTime);
        int      GetDiff(){return m_itOff;}
        //added end

        DWORD GetFirstSecondsOnSameDay(DWORD dwSeconds);
        DWORD GetFirstSecondsOnSameChinesWeek(DWORD dwSeconds);
        DWORD GetFirstSecondsOnSameMonth(DWORD dwSeconds);

        //added by stonesun
        DWORD    GetEarliestTimeOnSameDay(DWORD dwTime);
        DWORD    GetNextDayEarliestTime(DWORD dwTime);
		DWORD    GetDayBeginSecond(DWORD dwTime);
         //added end

         void    SetCurSecondGapForTest(int iSetVal) {m_iCurSecondGapForTest = iSetVal;};

		const  char *GetCurVIVOTimeStr();
            
	private:
	    struct timeval   m_stCurrTv;
	    int              m_itOff;            // 和UTC时区之间的差值
	    DWORD            m_dwLastSecond;     // 生成唯一标识的最后时间
	    DWORD            m_dwMaxID;          // 当前秒的最大ID   

        //该值仅用于测试,正常情况下必为0
	    int              m_iCurSecondGapForTest;    // 获取当前秒时需要将该时间加上该值 
	    void    SetZoneTimeOff();
};

#endif

