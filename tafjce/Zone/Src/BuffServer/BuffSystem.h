#ifndef __BUFF_SYSTEM_H__
#define __BUFF_SYSTEM_H__

class BuffSystem:public ObjectBase<IBuffSystem>
{
public:

	BuffSystem();
	~BuffSystem();

	// IEntitySubSystem Interface
	virtual Uint32 getSubsystemID() const;
	virtual Uint32 getMasterHandle();
	virtual bool create(IEntity* pEntity, const std::string& strData);
	virtual bool createComplete();
	virtual const std::vector<Uint32>& getSupportMessage(){static std::vector<Uint32> nullResult; return nullResult;}
	virtual void onMessage(QxMessage* pMessage){}
	virtual void packSaveData(string& data);

	// IBuffSysem Intetrface
	virtual IEntityBuff* addBuff(HEntity hGiver, int iBuffID, const EffectContext& preContext=EffectContext() );
	virtual IEntityBuff* getBuff(const std::string& strUUID);
	virtual void delBuff(int iBaseID);
	virtual void delBuffByUUID(const std::string& strUUID);
	virtual void delBuffByType(int iBuffType);
	virtual void delBuffByGroup(int iGroupID);
	virtual bool isImMinityBuff(int iBuffID);
	virtual bool randomDelBuffByType(int iBuffType);
	virtual bool existBuff(int iBuffID);
	IEntityBuff* getBuffByID(int iBuffID);

private:

	//bool initBuffList(const Aegis::BuffSystemSaveData& data);
	IEntityBuff* doAddNewBuff(HEntity hGiver, int iBuffID, int iInitOverlap, const EffectContext& preContext = EffectContext() );

private:

	HEntity m_hEntity;

	typedef std::vector<IEntityBuff*> BuffList;
	BuffList m_buffList;
	string m_strCacheData;
};

#endif
