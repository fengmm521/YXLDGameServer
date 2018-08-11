#ifndef _DEFINE_H
#define _DEFINE_H

#include    <sys/types.h>
#include    <sys/time.h>
#include    <limits.h>
#include 	<errno.h>
#include <iostream>
#include <map>
#include <vector>
using namespace std;


#define TRUE	1
#define FALSE	0


#define INVALID_POS			-1
#define INVALID_FD			-1
#define INVALID_INDEX		-1



//#define CLTPKGHEADSIZE		6
#define MAX_SVR_PKG_DATA   131072 
#define MAX_CLIET_ID		10000
#define MAX_WORD_VALUE		65535
#define MAX_CLT_PKG_LEN     4096

#define ONE_HUNDRED	 100 	
#define ONE_THOUSAND 1000
#define TEN_THOUSAND 10000
#define ONEDAY_SECOND 86400

#define WORK_DIR 	taf::ServerConfig::DataPath
#define CONN_SHM_KEY		70100
#define ACCOUNT_SHM_KEY		70101
#define PLAYER_SHM_KEY		70102
#define VIEW_SHM_KEY		70103
#define MONSTER_SHM_KEY		70104

#define DEFAULT_PROTO_VER 1

#define __CONN_DEBUG__ 		1


//============================================================
// data type define
//============================================================

typedef long long           INT64;
typedef unsigned long long  DWORD64;
typedef unsigned int       	DWORD;
typedef unsigned short      WORD; 
typedef unsigned char       BYTE;
typedef unsigned char       UCHAR;


/*typedef unsigned long     ULONG;
typedef unsigned int        DWORD;
typedef unsigned short      WORD;
typedef unsigned char       UCHAR;*/
typedef bool				BOOL;
typedef struct timeval  	TIME_VAL;
typedef int					INT32;


// °æ±¾¿ØÖÆºê
#define _ROLE_BIND_GOLD_JAMES_20130619


#endif


