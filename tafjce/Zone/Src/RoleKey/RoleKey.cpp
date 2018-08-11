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

using namespace std;

int main(int argc, char* argv[])
{
	if(argc != 4)
	{
		cout<<"useage account rolePos wolrdID"<<endl;
		return 0;
	}

	ServerEngine::PKRole roleKey;
	roleKey.strAccount = argv[1];
	roleKey.rolePos = atoi(argv[2]);
	roleKey.worldID = atoi(argv[3]);

	string strData = ServerEngine::JceToStr(roleKey);
	string strBase64Key = TC_Base64::encode(strData);

	cout<<strBase64Key<<endl;

	return 0;
}


