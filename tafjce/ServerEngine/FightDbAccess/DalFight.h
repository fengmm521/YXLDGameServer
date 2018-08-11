#ifndef _DAL_ROLE_DB_
#define _DAL_ROLE_DB_
#include "servant/Application.h"
#include "DalDbInterface.h"
#include "DbAccess.h"
#include "DalMoDef.h"
#include "CommLogic.h"
#include "comm_func.h"
#include "util/tc_base64.h"


using namespace std;
using namespace taf;

namespace ServerEngine
{

    class Fightdb_table_policy
    {
    	public:
        static string tblSuffix(const string& sKey)
        {
        	string strOrgKey = TC_Base64::decode(sKey);
		  	unsigned int dwValue = Str2HashInt(strOrgKey.c_str(), strOrgKey.size() );
		   	return  TC_Common::tostr<size_t>(dwValue % 10);
        }
		
	    static string dbSuffix(const string& sKey)
	    {
	        return "";
	    }
	};
	
    template
    <
        typename TablePolicy
    >
    class DalFightDB : public DalBaseDB<string>
    {
    public:
        
        DalFightDB(const map<string,string>& dbconf) : DalBaseDB<string>(dbconf)
        {
        }
        virtual ~DalFightDB()
        {
        }
        
        
        virtual int Get(const string& key,string& sValue);
        virtual int Set(const string& key,const string& sValue);
        virtual int Del(const string& key);
    };

    template<typename TablePolicy>
    int DalFightDB<TablePolicy>::Get(const string& key,string& sValue)
    {
        int iRet = eDbUnknownError;
		
        PROC_BEGIN
        try
        {
			string sTbl = _sTablePrefix + TablePolicy::tblSuffix(key);
			string sDb  = _sDbPrefix ;

			stringstream sstream;
			sstream<<"select * from "<<sDb<<"."<<sTbl<<" where UUID=\'"<<key
				<<"\'";

			string sSql = sstream.str();
			taf::TC_Mysql::MysqlData tMsgData = _pMysql->queryRecord(sSql);

			//处理记录
			size_t iMsgRows = tMsgData.size();
			if(0 == iMsgRows)
			{
				iRet = eDbRecordNotExist;
				break;
			}
			
			sValue=tMsgData[0]["data"];

			iRet = eDbSucc;
			
        }
        catch(TC_Mysql_Exception & e)
        {
			FDLOG("GetFight_error") <<key<<"|" << "RoleDB(GET) catch mysql_execfail:" << e.what() << endl;
			iRet = eDbError;
        }
        catch(runtime_error& e)
        {
			FDLOG("GetFight_error") <<key<<"|" << "RoleDB(GET) catch runtime_error:" << e.what() << endl;
			iRet = eDbUnknownError;
        }
        catch (taf::TC_Exception& e)
    	{
			FDLOG("GetFight_error") <<key<<"|" << "RoleDB(GET) catch TC_Exception:" << e.what() << endl;
			iRet = eDbUnknownError;
    	}
    	catch (...)
    	{
			FDLOG("GetFight_error") <<key<<"|" << "RoleDB(GET) catch unknown exception." << endl;
			iRet = eDbUnknownError;
    	}

        PROC_END
        
        return iRet;
		
    }

    template<typename TablePolicy>
    int DalFightDB<TablePolicy>::Set(const string& key,const string& sValue)
    {   
	int iRet = eDbUnknownError;
	
	PROC_BEGIN
	try
	{
		string sTbl = _sTablePrefix + TablePolicy::tblSuffix(key);
		string sDb  = _sDbPrefix;


		TC_Mysql::RECORD_DATA updateData;
		updateData["UUID"] = make_pair(TC_Mysql::DB_STR, key);
		updateData["data"] = make_pair(TC_Mysql::DB_STR, sValue);

		size_t iAffectRows = _pMysql->replaceRecord(sDb +"." + sTbl,updateData);

		FDLOG( "SetFight" ) <<key<<"|"<<iAffectRows << " |"<<iAffectRows << endl;
		
		if ( 0 >= iAffectRows)
		{
			FDLOG("SetFight_error") <<key<<"|" << "execute insert error." <<endl;
			iRet = eDbError;
			break;
		}

		iRet = eDbSucc;
		
        }
        catch(TC_Mysql_Exception & e)
        {
			FDLOG("SetFight_error") <<key<<"|"<< "RoleDB(Set) catch mysql_execfail:" << e.what() << endl;
			iRet = eDbError;
        }
        catch(runtime_error& e)
        {
			FDLOG("SetFight_error") <<key<<"|"<<"RoleDB(Set) catch runtime_error:" << e.what() << endl;
			iRet = eDbUnknownError;
        }
        catch (taf::TC_Exception& e)
    	{
			FDLOG("SetFight_error") <<key<<"|"<< "RoleDB(Set) catch TC_Exception:" << e.what() << endl;
			iRet = eDbUnknownError;
    	}
    	catch (...)
    	{
			FDLOG("SetFight_error") <<key<<"|"<< "RoleDB(Set) catch unknown exception." << endl;
			iRet = eDbUnknownError;
    	}

        PROC_END
       	
        return iRet;
		
    }

    template<typename TablePolicy>
    int DalFightDB<TablePolicy>::Del(const string& key)
    {
	int iRet = eDbUnknownError;

	PROC_BEGIN
	try
	{
		string sTbl = _sTablePrefix + TablePolicy::tblSuffix(key);
		string sDb  = _sDbPrefix;

		string strCondition;
		stringstream ss;
		ss<<"where UUID=\'"<<key<<"\'";
		strCondition = ss.str();

		//处理记录
		size_t iAffectRows = _pMysql->deleteRecord(sDb + "." +sTbl, strCondition);

		FDLOG("DelFight") <<key<<"|" << iAffectRows << "|" <<iRet<<endl;
		
		if ( 0 > iAffectRows)
		{
			FDLOG("DelFight_error") <<key<< "|" << "execute delete error" << endl;
			iRet = eDbError;
			break;
		}

		iRet = eDbSucc;
	}
	catch(TC_Mysql_Exception & e)
	{
		FDLOG("DelFight_error") <<key<< "|" << "RoleDB(GET) catch mysql_execfail:" << e.what() << endl;
		iRet = eDbError;
	}
	catch(runtime_error& e)
	{
		FDLOG("DelFight_error") <<key<< "|" << "RoleDB(Del) catch runtime_error:" << e.what() << endl;
		iRet = eDbUnknownError;
	}
	catch (taf::TC_Exception& e)
	{
		FDLOG("DelFight_error") <<key<< "|" << "RoleDB(Del) catch TC_Exception:" << e.what() << endl;
		iRet = eDbUnknownError;
	}
	catch (...)
	{
		FDLOG("DelFight_error") <<key<< "|" << "RoleDB(Del) catch unknown exception." << endl;
		iRet = eDbUnknownError;
	}

	PROC_END
		
	return iRet;
	
    }

    typedef DalFightDB<Fightdb_table_policy> DefaultFightDb;

}

#endif


