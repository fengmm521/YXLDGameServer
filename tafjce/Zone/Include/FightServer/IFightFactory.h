#ifndef __IFIGHT_FACTORY_H__
#define __IFIGHT_FACTORY_H__

typedef TC_Functor<void, TL::TLMaker<taf::Int32, ServerEngine::BattleData&>::Result> DelegateQueryFight;

typedef TC_Functor<void, TL::TLMaker<taf::Int32, const ServerEngine::BattleData&>::Result> DelegatePVEFight;
typedef TC_Functor<void, TL::TLMaker<taf::Int32, ServerEngine::BattleData&>::Result> DelegatePVPFight;


typedef std::map<std::string, std::string> FightClientParam;


enum enQueryBattleResult
{
	en_QueryBattle_OK = 0,
	en_QueryBattle_NotExist,  // 战报不存在
	en_QueryBattle_SysError,  // 系统错误
};

enum
{
	en_FightResult_OK,
	en_FightResult_SysError,
};


class IFightFactory:public IComponent
{
public:

	virtual void queryBattleRecord(const ServerEngine::PKFight& fightKey, DelegateQueryFight cb) = 0;

	virtual void saveBattleRecord(const ServerEngine::BattleData& data) = 0;

	virtual string makeFightStringKey(const ServerEngine::PKFight& key) = 0;

	virtual bool decodeFightStringKey(const std::string& strKey, ServerEngine::PKFight& key) = 0;

	virtual void generateFightKey(int iWorldID, ServerEngine::BattleData& data) = 0;

	virtual void AsyncPVEFight(DelegatePVEFight cb, const ServerEngine::ActorCreateContext& attacker, int iMonsterGrpID, const ServerEngine::FightContext& ctx) = 0;

	virtual void AsyncPVPFight(DelegatePVPFight cb, const ServerEngine::ActorCreateContext& attacker, const ServerEngine::ActorCreateContext& target, const ServerEngine::FightContext& ctx) = 0;

	virtual int doPVEFight(ServerEngine::BattleData& battleInfo, const ServerEngine::ActorCreateContext& roleInfo, int iMonsterGrpID, const ServerEngine::FightContext& ctx) = 0;

	virtual int doPVPFight(ServerEngine::BattleData& battleInfo, const ServerEngine::ActorCreateContext& attRoleInfo, const ServerEngine::ActorCreateContext& targetRileInfo, const ServerEngine::FightContext& ctx) = 0;	

	virtual void AsyncMultiPVEFight(DelegatePVEFight cb, const ServerEngine::ActorCreateContext& roleInfo, vector<int> monsterGrpList, const ServerEngine::FightContext& ctx) = 0;

	virtual int doMultiPVEFight(ServerEngine::BattleData& battleInfo, const ServerEngine::ActorCreateContext& roleInfo, vector<int> monsterGrpList, const ServerEngine::FightContext& ctx) = 0;

	virtual int continuePVEFight(ServerEngine::BattleData& battleInfo, const ServerEngine::ActorCreateContext& roleInfo, int iMonsterGrpID, const ServerEngine::FightContext& ctx) = 0;

	virtual int continueMultiPVEFight(ServerEngine::BattleData& battleInfo, const ServerEngine::ActorCreateContext& roleInfo, vector<int> monsterGrpList, const ServerEngine::FightContext& ctx) = 0;

	virtual int continuePVPFight(ServerEngine::BattleData& battleInfo, const ServerEngine::ActorCreateContext& roleInfo, const ServerEngine::ActorCreateContext& targetRileInfo, const ServerEngine::FightContext& ctx) = 0;

	virtual void AsyncContinuePVEFight(DelegatePVEFight cb, const ServerEngine::ActorCreateContext& roleInfo, int iMonsterGrpID, const ServerEngine::FightContext& ctx) = 0;
	
	virtual void AsyncContinueMultiPVEFight(DelegatePVEFight cb, const ServerEngine::ActorCreateContext& roleInfo, vector<int> monsterGrpList, const ServerEngine::FightContext& ctx) = 0;

	virtual void AsyncContinuePVPFight(DelegatePVPFight cb, const ServerEngine::ActorCreateContext& roleInfo, const ServerEngine::ActorCreateContext& targetRileInfo, const ServerEngine::FightContext& ctx) = 0;

	virtual EventServer* getEventServer() = 0;

	virtual const vector<HEntity>* getMemberList() = 0;

	virtual int getCurrentRound() = 0;
};

#endif
