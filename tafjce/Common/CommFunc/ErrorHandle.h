#ifndef _ERRORHANDLE_H_
#define _ERRORHANDLE_H_
/////////////////////////////////////////////////////////////////////
#include "log/taf_logger.h"
#include "servant/PropertyReport.h"

using namespace taf;

/////////////////////////////////////////////////////////////////////
namespace Aegis
{
/////////////////////////////////////////////////////////////////////


/*
 *	异常处理定义
 *	捕捉的异常、可以判断为系统错误，非业务失败的情况尽量都记到错误日志里
 *	系统正常运行的情况下错误日志应该为空
 *
 *	异常可以用__COMMON_EXCEPTION_CATCH__来处理
 *	单条记录可以使用ERRLOG来处理
 */
/////////////////////////////////////////////////////////////////////

// 运行日志
#ifndef RLOG
#define RLOG (LOG->debug())
#endif
//#define	FDLOG(name) (FDLOG(name))
// 调试日志
#ifndef TRLOG
#define TRLOG   (LOG->debug() << __FILE__ << "|" << __LINE__ << "|" << __FUNCTION__ << "|")
#endif
#ifndef TFDLOG
#define TFDLOG(name)   (FDLOG(name)<<"|" )
#endif
//#define DAYLOG   (DLOG)
#ifndef TDLOG
#define TDLOG   (DLOG << __FILE__ << "|" << __LINE__ << "|" << __FUNCTION__ << "|")
#endif
#ifndef ERRLOG
#define ERRLOG   (FDLOG("error"))
#endif
#ifndef TERRLOG
#define TERRLOG  (FDLOG("error") << __FILE__ << "|" << __LINE__ << "|" << __FUNCTION__ << "|")
#endif

/////////////////////////////////////////////////////////////////////
//统一异常处理
#ifndef  __COMMON_EXCEPTION_CATCH__
#define  __COMMON_EXCEPTION_CATCH__   \
	catch(TafException &tafEx)\
{\
    TERRLOG << "exception|taf|" << tafEx.what() << endl;\
}\
	catch ( TC_Exception &e )\
{\
	TERRLOG << "exception|tc|" << e.what() << endl;\
}\
	catch ( exception &e ) \
{\
	TERRLOG << "exception|exception|" << e.what() << endl;\
}\
	catch ( ... )\
{\
	TERRLOG << "exception|unkown" << endl;\
} 
#endif
/////////////////////////////////////////////////////////////////////
// 
//支持输入一个关键字
#ifndef  __COMMON_EXCEPTION_CATCH_EXT__
#define  __COMMON_EXCEPTION_CATCH_EXT__(KEY)   \
	catch(TafException &tafEx)\
{\
    TERRLOG << "exception|taf|" << tafEx.what() << "|" << KEY << endl;\
}\
	catch ( TC_Exception &e )\
{\
	TERRLOG << "exception|tc|" << e.what() << "|" << KEY << endl;\
}\
	catch ( exception &e ) \
{\
	TERRLOG << "exception|exception|" << e.what() << "|" << KEY << endl;\
}\
	catch ( ... )\
{\
	TERRLOG << "exception|unkown" << "|" << KEY << endl;\
} 
#endif


/////////////////////////////////////////////////////////////////////

#ifndef  __COMMON_EXCEPTION_CATCH_THROW__
#define  __COMMON_EXCEPTION_CATCH_THROW__   \
	catch(TafException &tafEx)\
{\
    TERRLOG << "exception|taf|" << tafEx.what() << endl;\
   	throw;\
}\
	catch ( TC_Exception &e )\
{\
	TERRLOG << "exception|tc|" << e.what() << endl;\
   	throw;\
}\
	catch ( exception &e ) \
{\
	TERRLOG << "exception|exception|" << e.what() << endl;\
   	throw;\
}\
	catch ( ... )\
{\
	TERRLOG << "exception|unkown" << endl;\
   	throw;\
} 
#endif


/////////////////////////////////////////////////////////////////////

#ifndef  __COMMON_EXCEPTION_CATCH_THROW_EXT__
#define  __COMMON_EXCEPTION_CATCH_THROW_EXT__(KEY)   \
	catch(TafException &tafEx)\
{\
    TERRLOG << "exception|taf|" << tafEx.what() << "|" << KEY << endl;\
   	throw;\
}\
	catch ( TC_Exception &e )\
{\
	TERRLOG << "exception|tc|" << e.what() << "|" << KEY << endl;\
   	throw;\
}\
	catch ( exception &e ) \
{\
	TERRLOG << "exception|exception|" << e.what() << "|" << KEY << endl;\
   	throw;\
}\
	catch ( ... )\
{\
	TERRLOG << "exception|unkown" << "|" << KEY << endl;\
   	throw;\
} 
#endif

//过程控制
#ifndef  __BEGIN_DO_WHILE_FALSE_PROC__
#define __BEGIN_DO_WHILE_FALSE_PROC__  do{
#endif
#ifndef  __END_DO_WHILE_FALSE_PROC__
#define __END_DO_WHILE_FALSE_PROC__    }while(0);
#endif

/////////////////////////////////////////////////////////////////////
// Property 统计
// 属性上报对象工厂，taf框架里己做了工厂处理，这里再封装一层
class PropertyReportFactory : TC_HandleBase, public TC_ThreadLock, public TC_Singleton<PropertyReportFactory>
{
public:
    map<string, PropertyReportPtr>& getProperty(){return m_mPropertyReport;}
	TC_ThreadMutex &getSelfMutex(){return m_mutex;}
private:
    map<string, PropertyReportPtr> m_mPropertyReport;
	TC_ThreadMutex m_mutex;
};

// 创建除分布以外的属性上报对象，若需要分布需要另增加定义
#define __TAF_PROPERTY_CREATE__(name) { \
	TC_LockT<TC_ThreadMutex> lock(PropertyReportFactory::getInstance()->getSelfMutex()); \
	if (PropertyReportFactory::getInstance()->getProperty().find(name) == PropertyReportFactory::getInstance()->getProperty().end()) \
	{ \
		PropertyReportFactory::getInstance()->getProperty()[name] = Application::getCommunicator()->getStatReport()->createPropertyReport(name, PropertyReport::sum(), PropertyReport::avg(), PropertyReport::count(), PropertyReport::max(), PropertyReport::min()); \
	} \
}

// 创建特殊的属性上报对象
#define __TAF_PROPERTY_CREATE_SPEC__(name,policy)  { \
	TC_LockT<TC_ThreadMutex> lock(PropertyReportFactory::getInstance()->getSelfMutex()); \
	if (PropertyReportFactory::getInstance()->getProperty().find(name) == PropertyReportFactory::getInstance()->getProperty().end()) \
	{ \
		PropertyReportFactory::getInstance()->getProperty()[name] = Application::getCommunicator()->getStatReport()->createPropertyReport(name, PropertyReport::sum(), policy); \
	} \
} 

// 属性上报
#define __TAF_PROPERTY_REPORT__(name,value)  { \
	map<string, PropertyReportPtr>::iterator it = PropertyReportFactory::getInstance()->getProperty().find(name); \
	if (it != PropertyReportFactory::getInstance()->getProperty().end()) \
	{ \
		PropertyReportFactory::getInstance()->getProperty()[name]->report(value); \
	} \
}

/////////////////////////////////////////////////////////////////////
// tafstat接口监控，用于非taf接口的监控
// 入参:
//      obj       被调模块. 一般采用app.servername 例如:Comm.BindServer
//      func      被调接口名
//      host      被调IP
//      port      被调port
//      result    结果值(StatReport::StatResult类型):成功STAT_SUCC，超时 STAT_TIMEOUT，异常STAT_EXC
//      costtime  耗时(单位ms)
#define __TAF_REPORT_TO_STAT__(obj,func,host,port,result,costtime) { \
    StatReport* stat = Application::getCommunicator()->getStatReport(); \
    if (stat) \
    { \
        stat->report(obj, func, host, port,(StatReport::StatResult)result, costtime, 0, true); \
    } \
}

// 异常上报
/////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////
}
/////////////////////////////////////////////////////////////////////
#endif //_SNSCLI_ERRORHANDLE_H_
/////////////////////////////////////////////////////////////////////




