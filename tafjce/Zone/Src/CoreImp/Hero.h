#ifndef __HERO_H__
#define __HERO_H__

class Hero:public Entity
{
public:

	Hero();

	virtual bool createEntity(const std::string& strData);
	virtual void packSaveData(ServerEngine::RoleSaveData& data);
	void setProperty(PropertySet::PropertyKey key, Int32 nValue);
	virtual void addExp(int iExp);

	void initHeroProp();
	void onEventCalcGrow(EventArgs& args);
	void reCalcGrow();
	
	int getHeroPrice(int process);

private:
	PROFILE_OBJ_COUNTER(Hero);

	bool m_bFinished;
};



#endif
