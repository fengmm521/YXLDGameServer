#ifndef __IFIGHTSOUL_FACTORY_H__
#define __IFIGHTSOUL_FACTORY_H__

class IFightSoul:public IObject
{
public:

	virtual int getID() = 0;

	virtual int getQuality() = 0;

	virtual const map<int, int>& getEffectMap() = 0;

	virtual int getExp() = 0;

	virtual int getCombineExp() = 0;

	virtual int getLevel() = 0;

	virtual void addExp(int iAddExp) = 0;

	virtual Uint32 getHandle() = 0;

	virtual void changeProperty(HEntity hHero, bool bOn) = 0;

	virtual bool isLocked() = 0;

	virtual void setLock(bool bLocked) = 0;

	virtual bool isTochMax() = 0;

	virtual string getName() = 0;
};


typedef Handle<IFightSoul*> HFightSoul;


class IFightSoulFactory:public IComponent
{
public:

	virtual IFightSoul* createFightSoul(int iFightSoul) = 0;

	virtual IFightSoul* createFightSoulFromDB(const ServerEngine::FightSoulItem& fightSoulData) = 0;

	virtual const map<int, int>* getEffectMap(int iFightSoulID, int iLevel) = 0; 
};


IFightSoul* getFightSoulFromHandle(HFightSoul);


#endif
