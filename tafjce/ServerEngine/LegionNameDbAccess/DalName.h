#ifndef __DAL_NAME_H__
#define __DAL_NAME_H__

#include "servant/Application.h"
#include "DalDbInterface.h"
#include "DbAccess.h"
#include "DalMoDef.h"
#include "CommLogic.h"
#include "comm_func.h"

using namespace std;
using namespace taf;

namespace ServerEngine
{

    class namedb_table_policy
    {
    	public:
        static string tblSuffix(const string& sKey)
        {
            size_t len = Str2HashInt(sKey.c_str(), sKey.size() );//sKey.length();
            return TC_Common::tostr<size_t>(len%5);	
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
    class DalNameDB : public DalBaseDB<string>
    {
    public:
        
        DalNameDB(const map<string,string>& dbconf) : DalBaseDB<string>(dbconf)
        {
        }
        virtual ~DalNameDB()
        {
        }
        
        
        virtual int Get(const string& key,string& sValue);
        virtual int Set(const string& key,const string& sValue);
        virtual int Del(const string& key);

    protected:
    };

    template<typename TablePolicy>
    int DalNameDB<TablePolicy>::Get(const string& key,string& sValue)
    {
       	int iRet = eDbUnknownError;
       
        PROC_BEGIN
        try
        {

			string sTbl = _sTablePrefix + TablePolicy::tblSuffix(key);
			string sDb  = _sDbPrefix ;

			string sSql = "SELECT * FROM "+ (sDb + "." + sTbl) +" WHERE name="+"'"+key+"'";	

			taf::TC_Mysql::MysqlData tMsgData = _pMysql->queryRecord(sSql);

			//处理记录
			size_t iMsgRows = tMsgData.size();
			
			FDLOG( "GetName" ) << "Name:" << key << " Rows:" << iMsgRows << " SQL:" << sSql << endl;
				
			if ( 0 == iMsgRows)
			{
				iRet = eDbRecordNotExist;
				break;
			}

			sValue = tMsgData[0]["data"];
			iRet = eDbSucc;
			
        }
        catch(TC_Mysql_Exception & e)
        {
			FDLOG("GetName_error") << key<< " keylen:" << key.length() << "|" << "NameDB(GET) catch mysql_execfail:" << e.what() << endl;
			iRet = eDbError;
        }
        catch(runtime_error& e)
        {
			FDLOG("GetName_error") << key<< " keylen:" << key.length() << "|" << "NameDB(GET) catch runtime_error:" << e.what() << endl;
			iRet = eDbUnknownError;
        }
        catch (taf::TC_Exception& e)
    	{
			FDLOG("GetName_error") << key<< " keylen:" << key.length() << "|" << "NameDB(GET) catch TC_Exception:" << e.what() << endl;
			iRet = eDbUnknownError;
    	}
    	catch (...)
    	{
			FDLOG("GetName_error") << key << " keylen:" << key.length() << "|" << "NameDB(GET) catch unknown exception." << endl;
			iRet = eDbUnknownError;
    	}

        PROC_END
        
        return iRet;
		
    }

    template<typename TablePolicy>
    int DalNameDB<TablePolicy>::Set(const string& key,const string& sValue)
    {   
	int iRet = eDbUnknownError;
	
	PROC_BEGIN
	try
	{

		string sTbl = _sTablePrefix + TablePolicy::tblSuffix(key);
		string sDb  = _sDbPrefix;

		TC_Mysql::RECORD_DATA updateData;
		updateData["name"] = make_pair(TC_Mysql::DB_STR,key);	

		
		updateData["data"] = make_pair(TC_Mysql::DB_STR, sValue);
		

		size_t iAffectRows = _pMysql->replaceRecord(sDb +"." + sTbl,updateData);

		FDLOG( "SetName" ) << key << "|"<< sValue.size() << " |" << iAffectRows << " |"<<iRet << endl;
		
		if ( 0 >= iAffectRows)
		{
			FDLOG("SetName_error") << key<< "|" << "execute insert error." <<endl;
			iRet = eDbError;
			break;
		}

		iRet = eDbSucc;
		
        }
        catch(TC_Mysql_Exception & e)
        {
			FDLOG("SetName_error") << key << "|" << "NameDB(GET) catch mysql_execfail:" << e.what() << endl;
			iRet = eDbError;
        }
        catch(runtime_error& e)
        {
			FDLOG("SetName_error") << key<< "|" << "NameDB(Set) catch runtime_error:" << e.what() << endl;
			iRet = eDbUnknownError;
        }
        catch (taf::TC_Exception& e)
    	{
			FDLOG("SetName_error") << key << "|" << "NameDB(Set) catch TC_Exception:" << e.what() << endl;
			iRet = eDbUnknownError;
    	}
    	catch (...)
    	{
			FDLOG("SetName_error") << key<< "|" << "NameDB(Set) catch unknown exception." << endl;
			iRet = eDbUnknownError;
    	}

        PROC_END
       	
        return iRet;
		
    }

    template<typename TablePolicy>
    int DalNameDB<TablePolicy>::Del(const string& key)
    {
	int iRet = eDbUnknownError;

	PROC_BEGIN
	try
	{

		string sTbl = _sTablePrefix + TablePolicy::tblSuffix(key);
		string sDb  = _sDbPrefix;

		string sSql = string("WHERE name=")+"'"+key+"'";

		//处理记录
		size_t iAffectRows = _pMysql->deleteRecord(sDb + "." +sTbl,sSql);

		FDLOG("DelName") <<key<<"|" << iAffectRows << "|" <<iRet<<endl;
		
		if ( 0 > iAffectRows)
		{
			FDLOG("DelName_error") << key << "|" << "execute delete error" << endl;
			iRet = eDbError;
			break;
		}

		iRet = eDbSucc;
	}
	catch(TC_Mysql_Exception & e)
	{
		FDLOG("DelName_error") << key<< "|" << "NameDB(GET) catch mysql_execfail:" << e.what() << endl;
		iRet = eDbError;
	}
	catch(runtime_error& e)
	{
		FDLOG("DelName_error") << key<< "|" << "NameDB(Del) catch runtime_error:" << e.what() << endl;
		iRet = eDbUnknownError;
	}
	catch (taf::TC_Exception& e)
	{
		FDLOG("DelName_error") << key << "|" << "NameDB(Del) catch TC_Exception:" << e.what() << endl;
		iRet = eDbUnknownError;
	}
	catch (...)
	{
		FDLOG("DelName_error") << key << "|" << "NameDB(Del) catch unknown exception." << endl;
		iRet = eDbUnknownError;
	}

	PROC_END
		
	return iRet;
	
    }

    typedef DalNameDB<namedb_table_policy> DefaultNameDb;

}



#endif
