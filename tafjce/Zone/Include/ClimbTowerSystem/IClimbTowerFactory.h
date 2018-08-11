#ifndef _ICLIMBTOWERFACTORY_H_
#define _ICLIMBTOWERFACTORY_H_

#include "RoleData.h"


class IClimbTower:public IObject
{
	public:
		//virtual int getExpByTowerId(int towerId) = 0;
		//virtual int getTotalExp() = 0;
		//virtual int getHightestTowerLayer() = 0;
		//virtual int getCurrentTowerLayer() = 0;
	//	virtual int getNextLayerExp() = 0;
		
};

class IClimbTowerFactory :public IComponent
{
public:
	virtual int getNextLayerCanGetExp(int currentLayerId) = 0;
	virtual vector<int> getLayerMonsterGropIdList(int currentLayerId) = 0;
	virtual vector<int>& getLayerDropIdVec(int currentLayerId) = 0;
	virtual int getLayerCanGetHeroExp(int currentLayerId) = 0;
};

#endif




