#ifndef __FMINNATESKILL_H__
#define __FMINNATESKILL_H__


#include "FmEntity.h"
#include "FmGeometry.h"

NS_FM_BEGIN

class InnatesKillData 
{
public:
	InnatesKillData(int32 tallentID,int32 iLevel,bool bCanUpgrade)
	{
		m_TallentID = tallentID;
		m_iLevel = iLevel;
		m_bCanUpgrade = bCanUpgrade;
	}
	~InnatesKillData();
	int32 m_TallentID;
	int32 m_iLevel;
	bool m_bCanUpgrade;
};
class InnatesKill : public Component
{
private:
	vector<InnatesKillData*>  m_InnatesKillData;
public:
	vector<InnatesKillData*> & GetInnateskillData(){return m_InnatesKillData;}
	InnatesKill(Entity* entity);

};


NS_FM_END
#endif