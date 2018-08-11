#ifndef __ILEGION_FACTORY_H__
#define __ILEGION_FACTORY_H__


typedef TC_Functor<void, TL::TLMaker<int>::Result> DelegateCreateLegion;

class ILegion;
typedef Handle<ILegion*> HLegion;

class ILegion:public IObject
{
public:

	virtual bool initFromDB(const ServerEngine::LegionData& data) = 0;

	virtual Uint32 getHandle() = 0;

	virtual string getLegionName() = 0;

	virtual void onMessage(HEntity hActor, const GSProto::CSMessage& msg) = 0;

	virtual void broadcastMessage(const GSProto::SCMessage& msg) = 0;

	virtual void saveData(bool bSync) = 0;

	virtual void gmSetLevel(int iLevel) = 0;

	virtual void GMOnlineGetActorInfo(string& strJson) = 0;

	virtual void getMemeberNameList(vector<string>& memberNamelist) = 0;
	
};


class ILegionFactory:public IComponent
{
public:

	// 功能: 根据军团名字查找军团
	virtual ILegion* findLegion(const string& strLegionName) = 0;

	// 功能: 根据玩家名字查找所属军团
	virtual ILegion* getActorLegion(const string& strActorName) = 0;

	// 功能: 新建军团
	virtual bool newLegion(HEntity hCreater, const string& strLegionName, DelegateCreateLegion cb, int iLegionIconId) = 0;

	// 功能: 是否准备好
	virtual bool isReady() = 0;

	virtual void cleanupSave() = 0;

	virtual void gmStartFight() = 0;

	virtual void updateActorInfo(HEntity hActor) = 0;

	virtual int GMOnlineGetLegionMemberNameList(string strLegionName, string &strJson) = 0;

	virtual void actorLogionOut(HEntity hActor,bool bIsOut) = 0;
};

ILegion* getLegionFromHandle(HLegion handle);

#endif
