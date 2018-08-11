#ifndef __LOG_HELP_H__
#define __LOG_HELP_H__

#define PLAYERLOG(pEntity)    FDLOG("Player")<<pEntity->getProperty(PROP_ACTOR_ACCOUNT, "")<<"|"<<pEntity->getProperty(PROP_ACTOR_WORLD, 0)<<"|"<<pEntity->getProperty(PROP_ENTITY_UUID, "")\
	<<"|"<<pEntity->getProperty(PROP_ENTITY_LEVEL, 0)<<"|"

#define PLAYEROFFLINELOG(roleInfo, roleSaveData) FDLOG("Player")<<roleInfo.strAccount<<"|"<<(int)roleInfo.worldID<<"|"<<roleSaveData.basePropData.roleStringPropset[PROP_ENTITY_UUID]<<"|"<<roleSaveData.basePropData.roleIntPropset[PROP_ENTITY_LEVEL]<<"|"

#define MASTER(pHero) getEntityFromHandle(pHero->getProperty(PROP_ENTITY_MASTER, 0))

#define HEROLOG(pEntity)  FDLOG("Hero")<<MASTER(pEntity)->getProperty(PROP_ACTOR_ACCOUNT, "")<<"|"<<MASTER(pEntity)->getProperty(PROP_ENTITY_UUID, "")<<"|"\
	<<MASTER(pEntity)->getProperty(PROP_ENTITY_LEVEL, 0)<<"|"<<pEntity->getProperty(PROP_ENTITY_UUID, "")<<"|"<<pEntity->getProperty(PROP_ENTITY_BASEID, 0)<<"|"\
	<<pEntity->getProperty(PROP_ENTITY_LEVEL, 0)<<"|"<<pEntity->getProperty(PROP_ENTITY_LEVELSTEP, 0)<<"|"


#define GODANIMALLOG(pEntity)  FDLOG("GodAnimal")<<MASTER(pEntity)->getProperty(PROP_ACTOR_ACCOUNT, "")<<"|"<<MASTER(pEntity)->getProperty(PROP_ENTITY_UUID, "")<<"|"\
	<<MASTER(pEntity)->getProperty(PROP_ENTITY_LEVEL, 0)<<"|"<<pEntity->getProperty(PROP_ENTITY_UUID, "")<<"|"<<pEntity->getProperty(PROP_ENTITY_BASEID, 0)<<"|"\
	<<pEntity->getProperty(PROP_ENTITY_LEVEL, 0)<<"|"<<pEntity->getProperty(PROP_ENTITY_LEVELSTEP, 0)<<"|"

#define FIGHTLOG	FDLOG("Fight")

// 调试用日志工具
#define	PLAYER_LOG(_playerE, _log)	\
	FDLOG(_log) \
	<<_playerE->getProperty(PROP_ACTOR_ACCOUNT, "")<<"|" 	\
	<<_playerE->getProperty(PROP_ACTOR_WORLD, 0)<<"|" 		\
	<<_playerE->getProperty(PROP_ENTITY_UUID, "")<<"|" 		\
	<<_playerE->getProperty(PROP_ENTITY_LEVEL, 0)<<"|"

#define DEBUG_LOG(_playerE)\
	PLAYER_LOG(_playerE, "DEBUG") 



////FFDLOG("gn","merge", logName),  由于现在服务器不区分World,所以日志分开打印
#define STAT_LOG(logName)     FDLOG(logName)  
#define STAT_LOG_LOGIN 		STAT_LOG("Login")			// STAT_LOG_LOGIN<<RoleName|Account|WorldID
#define STAT_LOG_LOGOUT		STAT_LOG("Logout")			// STAT_LOG_LOGOUT<<RoleName|Account|WorldID|iActorLevel
#define STAT_NEWROLE		STAT_LOG("NewRole")		    // STAT_NEWROLE<<RoleName|Account|WorldID\channellavel
#define STAT_RECHARGE		STAT_LOG("ReCharge")        // 格式 STAT_RECHARGE<<Money|bFirstPay|Name|Account|WorldID
#define STAT_ONLINE			STAT_LOG("Online")			// STAT_ONLINE<<Count
#define STAT_MAXONLINE		STAT_LOG("MaxOnline")		// STAT_ONLINE<<Count
#define STAT_GAMECOIN       STAT_LOG("GameCoin")        // STAT_GAMECOIN<<iLifeAtt|Count|ChangeReason|WorldID
#define STAT_FIRSTCONSUME   STAT_LOG("FirstConsume")    // STAT_FIRSTCONSUME<<iLifeAtt|Count


#endif

