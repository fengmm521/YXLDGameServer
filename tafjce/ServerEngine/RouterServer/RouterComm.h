#ifndef __ROUTERCOMM_H__
#define __ROUTERCOMM_H__

#include "servant/Application.h"
#include "log/taf_logger.h"
#include "servant/PropertyReport.h"
#include "util/tc_singleton.h"

#define TRLOG   (LOG->debug() << __FILE__ << "|" << __LINE__ << "|" << __FUNCTION__ << "|")



#ifndef RLOG
#define RLOG (LOG->debug())
#endif


#endif
