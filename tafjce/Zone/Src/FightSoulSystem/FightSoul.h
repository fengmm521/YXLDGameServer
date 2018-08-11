#ifndef __FIGHTSOUL_H__
#define __FIGHTSOUL_H__

class FightSoul:public ObjectBase<IFightSoul>
{
public:

	friend class FightSoulFactory;
	
	FightSoul();
	~FightSoul();

	// IFightSoul interface
	virtual int getID();
	virtual int getQuality();
	virtual const map<int, int>& getEffectMap();
	virtual int getExp();
	virtual int getCombineExp();
	virtual int getLevel();
	virtual void addExp(int iAddExp);
	virtual Uint32 getHandle();
	virtual void changeProperty(HEntity hHero, bool bOn);
	virtual bool isLocked();
	virtual void setLock(bool bLocked);
	virtual bool isTochMax();
	virtual string getName();

	int getType();
	
	void fillScData(int iPos, GSProto::FightSoulItem& scItem);

private:

	PROFILE_OBJ_COUNTER(FightSoul);

	HFightSoul m_hHandle;
	int m_iFightSoulID;
	int m_iExp;
	bool m_bLocked;
};


#endif
