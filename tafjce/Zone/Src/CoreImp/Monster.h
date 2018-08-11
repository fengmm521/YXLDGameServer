#ifndef __MONSTER_H__
#define __MONSTER_H__

class Monster:public Entity
{
public:

	Monster();
	virtual bool createEntity(const std::string& strData);
	void initBasePop(int iMonsterID);
	void initMonsterSubSystem(const std::string& strData);

	virtual void setProperty(PropertySet::PropertyKey key, Int32 nValue);

private:

	PROFILE_OBJ_COUNTER(Monster);
	bool m_bFinished;
};



#endif
