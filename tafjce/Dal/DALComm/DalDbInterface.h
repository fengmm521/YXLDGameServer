//////////////////////////////////////////////////////////////////////////
//    DBPool的模板参数基类，约束一组数据落地操作
//    author: forrestliu@tencent.com
//////////////////////////////////////////////////////////////////////////
#ifndef _DAL_DB_INTERFACE_H_
#define _DAL_DB_INTERFACE_H_

#include <string>
#include <vector>
#include <map>
#include "util/tc_mysql.h"
#include "servant/Application.h"

using namespace taf;
namespace ServerEngine
{

//默认分库本表策略
class default_table_policy
{
public:
    static string tblSuffix(const string& sKey)
    {
        size_t len = sKey.length();
        if(len<=1)
        {
            return "0"+sKey;
        }
        else
        {
            return "0"+sKey.substr(len - 1);
        }
        
    }

    static string dbSuffix(const string& sKey)
    {
        size_t len = sKey.length();
        if(len<2)
        {
            return "0"+sKey;
        }
        else
        {
            return sKey.substr(len - 2);
        }
    }
}; 

	
template<typename K>
class DalBaseDB
{
public:
    
    DalBaseDB(const map<string,string>& dbconf):_pMysql(NULL)
    {
        init(dbconf);
    }
    
    virtual ~DalBaseDB()
    {
        if(_pMysql)
        {
            delete _pMysql;
             _pMysql = NULL;
        }  
    }
    
    void init(const map<string,string>& dbconf)
    {
        LOG->debug()<<"into DalBaseDB::init..."<<endl;
        //init FSM
    	map<string,string>::const_iterator it;
        string sHost,sUser,sPass,sCharset,sPort;

        it = dbconf.find("time-stamp");
    	if(it == dbconf.end())
    	{
    		setTimeStamp(time(NULL));
    	}
    	else
    	{
    		setTimeStamp(TC_Common::strto<int>(it->second));
    	}

        it = dbconf.find("Host");
        if(it == dbconf.end())
        {
            FDLOG("error")<<"Invalid db init Host"<<endl;
            return;
        }
        sHost = it->second;

        it = dbconf.find("User");
        if(it == dbconf.end())
        {
            FDLOG("error")<<"Invalid db init User"<<endl;
            return;
        }
        sUser = it->second;

        it = dbconf.find("Pass");
        if(it != dbconf.end())
        {
            sPass = it->second;
        }
        else
        {
            sPass = "";
        }
		
		it = dbconf.find("Port");
        if(it != dbconf.end())
        {
            sPort = it->second;
        }
        else
        {
            sPort = "3306";
        }

        it = dbconf.find("CharSet");
        if(it != dbconf.end())
        {
            sCharset = it->second;
        }
        else
        {
            sCharset = "gbk";
        }
        _sCharset = sCharset;

        it = dbconf.find("TablePrefix");
        if(it != dbconf.end())
        {
            _sTablePrefix= it->second;
        }
        else
        {
            _sTablePrefix = "";
        }

        it = dbconf.find("DbPrefix");
        if(it != dbconf.end())
        {
            _sDbPrefix = it->second;
        }
        else
        {
            _sDbPrefix = "";
        }

        try
        {
            _pMysql = new TC_Mysql;
            _pMysql->init(sHost,sUser,sPass,"",_sCharset,TC_Common::strto<int>(sPort));
        }
        catch(TC_Mysql_Exception & ex)
        {
            FDLOG("error")<<"init mysql catch exception("<<ex.what()<<")"<<endl;
        }

        /*it = dbconf.find("DB");
        if(it != dbconf.end())
        {
            sDb = it->second;
            _pMysql->use(sDb);
        }
        else
        {
            sDb = "";
        }*/

        LOG->debug()<<"Init DalDb:"<<sHost<<"|"<<sUser<<"|"<<sPass<<"|"<<sPort<<"|"<<_sTablePrefix<<"|"<<_sDbPrefix<<endl;
    }

    inline bool isValid(int iTimeStamp)
    {
        //return this->_pMysql != NULL;
        return ((getTimeStamp() == iTimeStamp) && (_pMysql != NULL));
    }

    int getTimeStamp() const
    {
        return _iTimeStamp;
    }

    void setTimeStamp(int iTimeStamp)
    {
        _iTimeStamp = iTimeStamp;
        return;
    }
    
	inline string t2s(const time_t t,const string& format="%Y-%m-%d %H:%M:%S")
	{
		struct tm curr;
		curr = *localtime(&t);
		char sTmp[1024];
		strftime(sTmp,sizeof(sTmp),format.c_str(),&curr);
		return string(sTmp);
	}

	inline time_t s2t(const std::string &s,const std::string& format="%Y-%m-%d %H:%M:%S")
	{
		struct tm curr;

		if(strptime(s.c_str(),format.c_str(),&curr) == NULL) {
			return 0;
		}
		return mktime(&curr);
	}

    virtual int Get(const K& key,string& sValue) = 0;
    virtual int Set(const K& key,const string& sValue) = 0;
    virtual int Del(const K& key) = 0;

protected:
    static string now()
    {
        time_t now = time (NULL);
        char buf[32] = {0};
        struct tm* localtime_ = localtime (&now);

        sprintf (buf, "%04d-%02d-%02d %02d:%02d:%02d", 
             localtime_->tm_year + 1900, 
             localtime_->tm_mon + 1,
             localtime_->tm_mday,
             localtime_->tm_hour,
             localtime_->tm_min,
             localtime_->tm_sec);
    
        return string(buf);
    }

    void Replace(string& sStr, string sSearStr, string sReplaceStr) const
    {
        string::size_type index = sStr.find(sSearStr);

        while(index != string::npos)
        {
            sStr.replace(index,sSearStr.length(),sReplaceStr);
            index = sStr.find(sSearStr,index+1);
        }
    }

protected:
    TC_Mysql* _pMysql;
    string  _sTablePrefix;
    string  _sDbPrefix;
    string  _sCharset;

    int     _iTimeStamp;
};

}
#endif

