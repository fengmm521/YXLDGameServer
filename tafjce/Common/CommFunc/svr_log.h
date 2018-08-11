#ifndef _SVR_NATIVE_LOG_20100827_H_
#define _SVR_NATIVE_LOG_20100827_H_

//#include "og_log.h"

//extern LogFile g_stRunLog;
//extern LogFile g_stErrLog;
#include "ErrorHandle.h"

#ifdef __cplusplus
extern "C" {
#endif

void InitSvrLog( const char szLogDir[], const char szRunLogFile[], const char szErrorLogFile[], 
	short shShiftType );

	void _AddRunLog( char szFormat[], ...  );
	void _AddErrorLog( char szFormat[], ... );


#ifdef __cplusplus
}
#endif

#define SvrRunLog( _s_, ... ) do{char szLog[1024];  snprintf( szLog, 1024-1, _s_, ##__VA_ARGS__ ); szLog[1024-1] = 0; RLOG<<szLog<<endl;}while(0)
#define SvrErrLog( _s_, ... ) do{char szMsg[1024];   snprintf( szMsg, 1024-1, _s_, ##__VA_ARGS__ ); szMsg[1024-1] = 0; 	ERRLOG<<szMsg<<"["<<__FILE__<<":"<<__FUNCTION__<<":"<<__LINE__<<"]"<<endl;}while(0)


#ifdef DEBUG_LOG_MIDDLE
#define SR_DEBUG_LOG(str,uin) TFDLOG("send_recieve")<<str<<" "<<__FUNCTION__<<" Uin:"<<uin<<endl;
#else
#define SR_DEBUG_LOG(str,uin)
#endif

#endif

