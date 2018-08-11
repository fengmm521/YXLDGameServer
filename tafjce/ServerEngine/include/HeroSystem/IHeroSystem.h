#ifndef __IHERO_SYSTEM_H__
#define __IHERO_SYSTEM_H__

#include "IEntity.h"

class IHeroSystem:public IEntitySubsystem
{
public:

	// 功能: 获取英雄列表
	virtual vector<HEntity> getHeroList() = 0;

	// 功能: 添加英雄
	// [iHero] 英雄ID
	virtual HEntity addHero(int iHeroID, bool bNotifyError, int iReason) = 0;

	virtual HEntity addHeroWithLevelStep(int iHeroID, int iLevelStep, bool bNotifyError, int iReason) = 0;

	// 功能: 销毁英雄
	// 参数: [hHero]英雄句柄 
	//@brief bNotifyError  通知
 	virtual bool destroyHero(HEntity hHero, bool bNotifyError) = 0;

	// 功能: 根据UUDID获取英雄
	// 参数: [strUUID] UUID
	virtual HEntity getHeroFromUUID(const string& strUUID) = 0;

	// 功能: 执行英雄等阶上升
	//virtual bool doHeroLevelStepUp(HEntity hTargetHero, const set<HEntity>& comsumeList, bool bNotifyMsg) = 0;

	// 功能: 判断英雄是否已经满了(不代表实际满，达到最大上限-10认为是满了)
	virtual bool checkHeroFull() = 0;

	virtual void gmSetAllHeroLevelStep(int iLevelStep) = 0;

	virtual void gmSetAllHeroLevel(int iLevel) = 0;

	virtual void packHeroBaseData(IEntity* pHero, GSProto::HeroBaseData& scHeroBase) = 0;

	virtual void sendHeroUpdateInfo(HEntity hHero) = 0;

	//计算等阶会产生的魂魄数
	virtual int calcHeroSoulCount(int iLevelStep) = 0;

	virtual void fillFemaleHeroList(vector<ServerEngine::FemailHeroDesc>& femaleList) = 0;

	virtual int getHeroItemID(int iHeroID) = 0;

	virtual HEntity callHero(int iHeroID) = 0;

	virtual int getHeroCountByQuality(int iQuality) = 0;
	virtual int getHeroCount() = 0;

	virtual void GMOnlineGetRoleHeroData(string& strJson)=0;
};


#endif
