#include "LegionPch.h"
#include "BossImp.h"
#include "IJZEntityFactory.h"


#define MAX_BOSSRANK_SIZE	10

BossImp::BossImp(int iMonsterGrpID, const ServerEngine::CreateBossCtx& bossCtx):m_bBusy(false), m_dwLastFightTime(0), m_dwCreateTime(0)
{
	m_iMonsterGrpID = iMonsterGrpID;
	m_createCtx = bossCtx;
	m_dwCreateTime = time(0);
}

BossImp::~BossImp()
{
}

bool BossImp::AsynFightBoss(Uint32 dwActor, const ServerEngine::AttackBossCtx& attackCtx, DelegateBossFight cb)
{
	if(m_createCtx.iHP <= 0)
	{
		return false;
	}

	BossFightOp tmpBossOp;
	tmpBossOp.hActor = dwActor;
	tmpBossOp.attCtx = attackCtx;
	tmpBossOp.cb = cb;
	m_fightOpList.push_back(tmpBossOp);

	popNextFight();

	return true;
}

struct BossPVECb
{
	BossPVECb(HEntity hActor, Detail::EventHandle::Proxy prx, DelegateBossFight cb):m_hActor(hActor), m_bossImpPrx(prx), m_cb(cb){}

	void operator()(taf::Int32 iRet, const ServerEngine::BattleData& data)
	{
		if(!m_bossImpPrx.get() )
		{
			return;
		}

		BossImp* pBossImp = static_cast<BossImp*>(m_bossImpPrx.get() );
		assert(pBossImp);
		
		pBossImp->processBattleResult(m_hActor, iRet, data, m_cb);
	}

private:

	HEntity m_hActor;
	Detail::EventHandle::Proxy m_bossImpPrx;
	DelegateBossFight m_cb;
};


bool BossImp::getKiller(ServerEngine::PKRole& roleKey, string& strName)
{
	if(m_createCtx.iHP > 0)
	{
		return false;
	}

	m_killerKey = roleKey;
	m_strRoleName = strName;

	return true;
}

Uint32 BossImp::getCreateTime()
{
	return m_dwCreateTime;
}


int BossImp::getDamage(const string& strName)
{
	return m_damageMap[strName];
}

void BossImp::processBattleResult(HEntity hActor, int iRet, const ServerEngine::BattleData& data, DelegateBossFight cb)
{
	// 去掉Busy标记
	m_bBusy = false;

	if(iRet != en_FightResult_OK)
	{
		popNextFight();
		return;
	}

	// 同步HP
	GSProto::FightDataAll fightDataAll;
	fightDataAll.ParseFromString(data.strBattleBody);

	int iDamage = 0;

	for(int i = 0; i < fightDataAll.szfightobject().size(); i++)
	{
		const GSProto::FightObj& tmpFightObj = fightDataAll.szfightobject().Get(i);

		if(tmpFightObj.iobjecttype() == GSProto::en_class_Monster)
		{
			iDamage = m_createCtx.iHP - tmpFightObj.ifinhp();
			m_createCtx.iHP = tmpFightObj.ifinhp();
			break;
		}
	}

	// 增加到统计数据表
	IEntity* pActor = getEntityFromHandle(hActor);
	if(pActor)
	{
		addDamageRecord(pActor, iDamage);
	}

	Detail::EventHandle::Proxy prxHandle = getEventHandle();
	cb(iRet, data, iDamage);

	if(!prxHandle.get() ) return;

	popNextFight();
}


struct BossDamageSort
{
	bool operator()(const BossDamageRecord& first, const BossDamageRecord& second) const
	{
		if(first.iDamageValue != second.iDamageValue)
		{
			return first.iDamageValue > second.iDamageValue;
		}

		return first.strName > second.strName;
	}
};

void BossImp::addDamageRecord(IEntity* pActor, int iDamage)
{
	assert(pActor);

	bool bFind = false;
	string strActorName = pActor->getProperty(PROP_ENTITY_NAME, "");
	for(size_t i = 0; i < m_rankList.size(); i++)
	{
		if(m_rankList[i].strName == strActorName)
		{
			bFind = true;
			m_rankList[i].iDamageValue += iDamage;
		}
	}

	m_damageMap[strActorName] += iDamage;

	if(!bFind)
	{
		BossDamageRecord newRecord;
		newRecord.roleKey.strAccount = pActor->getProperty(PROP_ACTOR_ACCOUNT, "");
		newRecord.roleKey.rolePos = 0;
		newRecord.roleKey.worldID = pActor->getProperty(PROP_ACTOR_WORLD, 0);
		newRecord.iDamageValue = m_damageMap[strActorName];
		newRecord.strName = pActor->getProperty(PROP_ENTITY_NAME, "");
		
		m_rankList.push_back(newRecord);
	}

	// 排序
	std::sort(m_rankList.begin(), m_rankList.end(), BossDamageSort() );

	// 如果超过10个,删除多余的
	
	if(m_rankList.size() > MAX_BOSSRANK_SIZE)
	{
		m_rankList.resize(MAX_BOSSRANK_SIZE);
	}
}


void BossImp::popNextFight()
{
	// 如果Busy,检测是不是死锁了(超过6S无反映)
	if(m_bBusy && (time(0) - m_dwLastFightTime > 6) )
	{
		m_bBusy = false;
	}

	if(m_bBusy) return;

	// 找到一个合法的开始战斗
	while( (m_fightOpList.size() > 0) && (m_createCtx.iHP > 0) )
	{
		BossFightOp tmpOp = m_fightOpList.front();
		m_fightOpList.pop_front();

		IEntity* pActor = getEntityFromHandle(tmpOp.hActor);
		if(!pActor) continue;
		
		m_bBusy = true;
		m_dwLastFightTime = time(0);

		// 战斗吧
		IFightSystem* pFightSys = static_cast<IFightSystem*>(pActor->querySubsystem(IID_IFightSystem));
		assert(pFightSys);

		ServerEngine::FightContext ctx;
		ctx.iFightType = GSProto::en_FightType_Boss;
		map<std::string, std::string>& fightCtxMap = ctx.context;
		fightCtxMap["BossContext"] = ServerEngine::JceToStr(m_createCtx);
		fightCtxMap["AttackerContext"] = ServerEngine::JceToStr(tmpOp.attCtx);
		
		pFightSys->AsyncPVEFight(BossPVECb(tmpOp.hActor, getEventHandle(), tmpOp.cb) , m_iMonsterGrpID, ctx);
		break;		
	}
}


int BossImp::getBossHP()
{
	return m_createCtx.iHP;
}

int BossImp::getBossMaxHP()
{
	return m_createCtx.iFixMaxHP;
}

void BossImp::getDamageRankList(int iLimitSize, vector<BossDamageRecord>& rankList)
{
	size_t copySize = std::min((size_t)iLimitSize, m_rankList.size() );
	std::copy(m_rankList.begin(), m_rankList.begin() + copySize, std::inserter(rankList, rankList.end() ) );
}

int BossImp::getVisibleMonsterID()
{
	IJZEntityFactory* pJZEntityFactory = getComponent<IJZEntityFactory>(COMPNAME_EntityFactory, IID_IJZEntityFactory);
	assert(pJZEntityFactory);

	int iVisibleMonster = pJZEntityFactory->getVisibleMonsterID(m_iMonsterGrpID);

	return iVisibleMonster;
}




