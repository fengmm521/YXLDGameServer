#ifndef __HERO_EQUIPSYSTEM_H__
#define __HERO_EQUIPSYSTEM_H_

class HeroEquipSystem:public ObjectBase<IHeroEquipSystem>, public IFightSoulContainer
{
public:

	HeroEquipSystem();
	~HeroEquipSystem();

	// IEntitySubSystem Interface
	virtual Uint32 getSubsystemID() const;
	virtual Uint32 getMasterHandle();
	virtual bool create(IEntity* pEntity, const std::string& strData);
	virtual bool createComplete();
	virtual const std::vector<Uint32>& getSupportMessage();
	virtual void onMessage(QxMessage* pMessage){}
	virtual void packSaveData(string& data);
	virtual void* queryInterface(int iInterfaceID);
	
	// IHeroEqupSystem Interface
	

	// IFightSoulContainer Interface
	virtual Int32 getSpace();
	virtual Int32 getSize();
	virtual Uint32 getFightSoul(int iPos);
	virtual bool setFightSoul(int iPos, Uint32 hFightSoulHandle, GSProto::FightSoulChgItem* pOutItem);
	virtual HEntity getOwner();
	virtual bool canSetFightSoul(int iPos, Uint32 hFightSoulHandle, bool bNoitifyErr);
	virtual void sendContainerChg(const vector<int>& posList);
	virtual bool hasFightSoul();

private:

	void initHeroEquipData(const ServerEngine::HeroEquipSystemData& data);

private:

	HEntity m_hEntity;
	vector<HFightSoul> m_fightSoulWear;
	
};


#endif
