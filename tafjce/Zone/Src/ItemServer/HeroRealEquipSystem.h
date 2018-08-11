#ifndef __HEROREAL_EQUIP_SYSTEM_H__
#define __HEROREAL_EQUIP_SYSTEM_H__

class HeroRealEquipSystem:public ObjectBase<IHeroRealEquipSystem>, public IEquipContainer
{
public:

	HeroRealEquipSystem();
	~HeroRealEquipSystem();

	// IEntitySubSystem Interface
	virtual Uint32 getSubsystemID() const;
	virtual Uint32 getMasterHandle();
	virtual bool create(IEntity* pEntity, const std::string& strData);
	virtual bool createComplete();
	virtual const std::vector<Uint32>& getSupportMessage();
	virtual void onMessage(QxMessage* pMessage);
	virtual void packSaveData(string& data);
	virtual void* queryInterface(int iInterfaceID);

	// IHeroRealEquipSystem
	virtual bool hasEquip();

	// IEquipContainer Interface
	virtual int getSize();
	virtual Uint32 getEquip(int iPos);
	virtual bool setEquip(int iPos, Uint32 hEquipHandle, GSProto::EquipChgItem* pOutItem);
	virtual HEntity getOwner();
	virtual void sendContainerChg(const vector<int>& posList);
	virtual bool canSetEquip(int iPos, Uint32 hEquipHandle, bool bNotifyError);
	virtual int getContainerType(){return GSProto::en_EquipContainer_HeroWear ;}

	void initHeroEquip(const ServerEngine::HeroRealEquipSystemData& saveData);
	bool calcSuitProp();
	void useSuitProp(bool bPuton);
	void fillSuitState(google::protobuf::RepeatedPtrField<GSProto::EquipSuitState>* pMutableStateList);
	
private:

	HEntity m_hEntity;
	vector<HItem> m_equipList;
	map<int, int> m_suitStateMap;
};


#endif
