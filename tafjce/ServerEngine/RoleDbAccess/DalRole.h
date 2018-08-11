//////////////////////////////////////////////////////////////////////////
//    作为DBPool的模板参数,提供对Message的实际落地操作
//////////////////////////////////////////////////////////////////////////
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

    class roledb_table_policy
    {
    	public:
        static string tblSuffix(const string& sKey)
        {
        	string strOrgKey = TC_Base64::decode(sKey);
			ServerEngine::PKRole roleKey;
			ServerEngine::JceToObj(strOrgKey, roleKey);
		  	unsigned int dwValue = Str2HashInt(roleKey.strAccount.c_str(), roleKey.strAccount.size() );
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
    class DalRoleDB : public DalBaseDB<string>
    {
    public:
        
        DalRoleDB(const map<string,string>& dbconf) : DalBaseDB<string>(dbconf)
        {
        }
        virtual ~DalRoleDB()
        {
        }
        
        
        virtual int Get(const string& key,string& sValue);
        virtual int Set(const string& key,const string& sValue);
        virtual int Del(const string& key);
    };

    template<typename TablePolicy>
    int DalRoleDB<TablePolicy>::Get(const string& key,string& sValue)
    {
        int iRet = eDbUnknownError;
       	PKRole roleKey;
		string strOrgKey = TC_Base64::decode(key);
		JceToObj(strOrgKey, roleKey);
        PROC_BEGIN
        try
        {
			string sTbl = _sTablePrefix + TablePolicy::tblSuffix(key);
			string sDb  = _sDbPrefix ;

			stringstream sstream;
			sstream<<"select * from "<<sDb<<"."<<sTbl<<" where Account=\'"<<roleKey.strAccount
				<<"\' and Pos="<<(int)roleKey.rolePos;

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
			FDLOG("GetRole_error") << roleKey.strAccount<<"|"<<(int)roleKey.rolePos<<"|" << "RoleDB(GET) catch mysql_execfail:" << e.what() << endl;
			iRet = eDbError;
        }
        catch(runtime_error& e)
        {
			FDLOG("GetRole_error") << roleKey.strAccount<<"|"<<(int)roleKey.rolePos<<"|" << "RoleDB(GET) catch runtime_error:" << e.what() << endl;
			iRet = eDbUnknownError;
        }
        catch (taf::TC_Exception& e)
    	{
			FDLOG("GetRole_error") << roleKey.strAccount<<"|"<<(int)roleKey.rolePos<<"|" << "RoleDB(GET) catch TC_Exception:" << e.what() << endl;
			iRet = eDbUnknownError;
    	}
    	catch (...)
    	{
			FDLOG("GetRole_error") << roleKey.strAccount<<"|"<<(int)roleKey.rolePos<<"|" << "RoleDB(GET) catch unknown exception." << endl;
			iRet = eDbUnknownError;
    	}

        PROC_END
        
        return iRet;
		
    }

    template<typename TablePolicy>
    int DalRoleDB<TablePolicy>::Set(const string& key,const string& sValue)
    {   
	int iRet = eDbUnknownError;
	PKRole roleKey;
	string strOrgKey = TC_Base64::decode(key);
	JceToObj(strOrgKey, roleKey);
	
	PROC_BEGIN
	try
	{
		string sTbl = _sTablePrefix + TablePolicy::tblSuffix(key);
		string sDb  = _sDbPrefix;


		TC_Mysql::RECORD_DATA updateData;
		updateData["Account"] = make_pair(TC_Mysql::DB_STR,roleKey.strAccount);
		updateData["Pos"] = make_pair(TC_Mysql::DB_INT,TC_Common::tostr((int)roleKey.rolePos) );
		updateData["data"] = make_pair(TC_Mysql::DB_STR, sValue);

		size_t iAffectRows = _pMysql->replaceRecord(sDb +"." + sTbl,updateData);

		FDLOG( "SetRole" ) << roleKey.strAccount<<"|"<<(int)roleKey.rolePos << iAffectRows << " |"<<iAffectRows << endl;
		
		if ( 0 >= iAffectRows)
		{
			FDLOG("SetRole_error") << roleKey.strAccount<<"|"<<(int)roleKey.rolePos<< "|" << "execute insert error." <<endl;
			iRet = eDbError;
			break;
		}

		iRet = eDbSucc;
		
        }
        catch(TC_Mysql_Exception & e)
        {
			FDLOG("SetRole_error") << roleKey.strAccount<<"|"<<(int)roleKey.rolePos << "|" << "RoleDB(Set) catch mysql_execfail:" << e.what() << endl;
			iRet = eDbError;
        }
        catch(runtime_error& e)
        {
			FDLOG("SetRole_error") << roleKey.strAccount<<"|"<<(int)roleKey.rolePos<< "|" << "RoleDB(Set) catch runtime_error:" << e.what() << endl;
			iRet = eDbUnknownError;
        }
        catch (taf::TC_Exception& e)
    	{
			FDLOG("SetRole_error") << roleKey.strAccount<<"|"<<(int)roleKey.rolePos << "|" << "RoleDB(Set) catch TC_Exception:" << e.what() << endl;
			iRet = eDbUnknownError;
    	}
    	catch (...)
    	{
			FDLOG("SetRole_error") << roleKey.strAccount<<"|"<<(int)roleKey.rolePos<< "|" << "RoleDB(Set) catch unknown exception." << endl;
			iRet = eDbUnknownError;
    	}

        PROC_END
       	
        return iRet;
		
    }

    template<typename TablePolicy>
    int DalRoleDB<TablePolicy>::Del(const string& key)
    {
	int iRet = eDbUnknownError;
	PKRole roleKey;
	string strOrgKey = TC_Base64::decode(key);
	JceToObj(strOrgKey, roleKey);

	PROC_BEGIN
	try
	{
		string sTbl = _sTablePrefix + TablePolicy::tblSuffix(key);
		string sDb  = _sDbPrefix;

		//string sSql = string("WHERE Name=")+"'"+key+"'";
		string strCondition;
		stringstream ss;
		ss<<"where Account=\'"<<roleKey.strAccount<<"\' and Pos="<<(int)roleKey.rolePos;
		strCondition = ss.str();

		//处理记录
		size_t iAffectRows = _pMysql->deleteRecord(sDb + "." +sTbl, strCondition);

		FDLOG("DelRole") <<roleKey.strAccount<<"|"<<(int)roleKey.rolePos<<"|" << iAffectRows << "|" <<iRet<<endl;
		
		if ( 0 > iAffectRows)
		{
			FDLOG("DelRole_error") << roleKey.strAccount<<"|"<<(int)roleKey.rolePos << "|" << "execute delete error" << endl;
			iRet = eDbError;
			break;
		}

		iRet = eDbSucc;
	}
	catch(TC_Mysql_Exception & e)
	{
		FDLOG("DelRole_error") << roleKey.strAccount<<"|"<<(int)roleKey.rolePos<< "|" << "RoleDB(GET) catch mysql_execfail:" << e.what() << endl;
		iRet = eDbError;
	}
	catch(runtime_error& e)
	{
		FDLOG("DelRole_error") << roleKey.strAccount<<"|"<<(int)roleKey.rolePos<< "|" << "RoleDB(Del) catch runtime_error:" << e.what() << endl;
		iRet = eDbUnknownError;
	}
	catch (taf::TC_Exception& e)
	{
		FDLOG("DelRole_error") << roleKey.strAccount<<"|"<<(int)roleKey.rolePos << "|" << "RoleDB(Del) catch TC_Exception:" << e.what() << endl;
		iRet = eDbUnknownError;
	}
	catch (...)
	{
		FDLOG("DelRole_error") << roleKey.strAccount<<"|"<<(int)roleKey.rolePos << "|" << "RoleDB(Del) catch unknown exception." << endl;
		iRet = eDbUnknownError;
	}

	PROC_END
		
	return iRet;
	
    }

    typedef DalRoleDB<roledb_table_policy> DefaultRoleDb;

}

#endif


