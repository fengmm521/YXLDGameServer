#include "MiniApr.h"

using namespace MINIAPR;

#include "servant/Application.h"

#include "util/tc_file.h"
#include "define.h"
#include "comm_func.h"
#include "CommLogic.h"
#include "svr_log.h"
#include "RoleData.h"
#include "util/tc_base64.h"
#include "Role.h"
#include "EntityProperty.h"

using namespace std;

int main(int argc, char* argv[])
{
	if(argc != 5)
	{
		cout<<"useage [export|import] account rolePos wolrdID"<<endl;
		return 0;
	}

	taf::CommunicatorPtr connPtr = new Communicator();

	TC_Config tmpCfg;
	tmpCfg.parseFile("config.conf");

	string strRoleObj = tmpCfg.get("/OuterFactory<RoleServer>", "");

	ServerEngine::PKRole roleKey;
	roleKey.strAccount = argv[2];
	roleKey.rolePos = atoi(argv[3]);
	roleKey.worldID = atoi(argv[4]);

	stringstream ss;
	ss<<roleKey.worldID<<"_"<<roleKey.strAccount<<"_"<<(int)roleKey.rolePos<<".log";
	string strFileName = ss.str();
	
	string strMode = argv[1];
	try
	{
		ServerEngine::RolePrx rolePrx;
		connPtr->stringToProxy<ServerEngine::RolePrx>(strRoleObj, rolePrx);

		if(strMode == "export")
		{
			ServerEngine::PIRole roleInfo;
			int iRet = rolePrx->getRole(roleKey, roleInfo);
			if(iRet != ServerEngine::en_RoleRet_OK)
			{
				cout<<"GetRole Fail|"<<iRet<<endl;
				return 2;
			}

			string strOrgData = ServerEngine::JceToStr(roleInfo);
			string strBase64Data = TC_Base64::encode(strOrgData);

			std::ofstream fs (strFileName.c_str(), ios_base::out|std::fstream::binary);
			if(!fs.good() )
			{
				cout<<"cannot openfile|"<<strFileName<<endl;
				return 3;
			}
			
			fs.write(strBase64Data.c_str(), strBase64Data.size() );

			if(!fs.good() )
			{
				cout<<"write fileData fail|"<<strFileName<<endl;
				return 4;
			}
			
			fs.close();
		}
		else if(strMode == "import")
		{
			std::ifstream fs (strFileName.c_str(), ios_base::in|std::fstream::binary);
			if(!fs.good() )
			{
				cout<<"cannot openfile|"<<strFileName<<endl;
				return 3;
			}
			
			fs.seekg(0, ios_base::end);
			std::streampos fileSize = fs.tellg();
			fs.seekg(0, ios_base::beg);

			char* pRoleData = new char[fileSize];
			fs.read(pRoleData, fileSize);
			if(!fs.good() )
			{
				cout<<"read fileData fail|"<<strFileName<<endl;
				delete []pRoleData;
				return 4;
			}

			string strBase64Data(pRoleData, fileSize);
			delete []pRoleData;

			fs.close();
			string strOrgData = TC_Base64::decode(strBase64Data);

			ServerEngine::PIRole roleInfo;
			ServerEngine::JceToObj(strOrgData, roleInfo);
		
			roleInfo.strAccount = roleKey.strAccount;
			roleInfo.rolePos = roleKey.rolePos;
			roleInfo.worldID = roleKey.worldID;

			ServerEngine::RoleSaveData roleSaveData;
			ServerEngine::JceToObj(roleInfo.roleData, roleSaveData);

			roleSaveData.basePropData.roleStringPropset[PROP_ACTOR_ACCOUNT] = roleKey.strAccount;
			roleSaveData.basePropData.roleIntPropset[PROP_ACTOR_ROLEPOS] = roleKey.rolePos;
			roleSaveData.basePropData.roleIntPropset[PROP_ACTOR_WORLD] = roleKey.worldID;
			
			roleInfo.roleData = ServerEngine::JceToStr(roleSaveData);
			
			int iRet = rolePrx->updateRole(roleKey, roleInfo);
			if(ServerEngine::en_RoleRet_OK != iRet)
			{
				cout<<"SaveRole Fail|"<<iRet<<endl;
				return 2;
			}
		}
		else
		{
			cout<<"invalid OpMode must be export|import"<<endl;
			return 6;
		}
	}
	catch(std::exception& e)
	{
		cout<<"exception|"<<e.what()<<endl;
		return 1;
	}

	cout<<"Success!!"<<endl;

	return 0;
}

