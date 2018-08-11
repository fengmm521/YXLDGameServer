#ifndef __MESSAGEWRAPPER_H__
#define __MESSAGEWRAPPER_H__

struct QxMessage
{
	std::string strRsAddress;
	Int64	ddCon;
	//MINIAPR::Uint32	dwUin;
	string strAccount;
	MINIAPR::Uint32	dwMsgID;
	char* 	pMsgDataBuff;
	MINIAPR::Uint32	dwMsgLen;
	int iWorldID;
};

#endif

