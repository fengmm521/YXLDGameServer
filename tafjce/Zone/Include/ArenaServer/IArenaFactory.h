#ifndef __IARENA_FACTORY_H__
#define __IARENA_FACTORY_H__

class IArenaFactory:public IComponent
{
public:

	virtual int getArenaRank(const string& strName) = 0;

	virtual void saveArenaData(bool bSync) = 0;

	virtual void getDomainList(HEntity hActor, vector<ServerEngine::ArenaSavePlayer>& playerList) = 0;

	virtual bool getDreamLandList(HEntity hEntity, ServerEngine::ArenaSavePlayer& player, int imin, int iMax) = 0;
	
	virtual void moveArenaAward(const string& strActorName, vector<ServerEngine::MailData>& mailDataList) = 0;

	//virtual void getDomainRandomActorList(HEntity hActor, vector<ServerEngine::ArenaSavePlayer>&playerList,int ivecSize) = 0;
	virtual bool getManorWillLootActor(HEntity hActor, ServerEngine::ArenaSavePlayer& player) = 0;

	virtual void updateActorInfo(HEntity hActor) = 0;

	virtual void updateActorBeManorLootInfo(HEntity hActor)= 0;

	virtual bool isInitFinish() = 0;

	virtual bool getActorForCityBattle(vector<ServerEngine::ArenaSavePlayer>&playerList, int iCount) = 0;

};

#endif
