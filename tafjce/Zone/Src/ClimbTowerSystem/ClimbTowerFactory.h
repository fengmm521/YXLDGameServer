#ifndef _CLIMBTOWERFACTORY_H_
#define _CLIMBTOWERFACTORY_H_


struct SingleTowerLayerInfo{
	SingleTowerLayerInfo()
		:iLayerId(0),
		iCanGetExp(0){
		}
	~SingleTowerLayerInfo()
		{
			dropIdVec.clear();		
		}
	int iLayerId;
	//int iMonsterGroupId;
	vector<int> monsterGrpList;
	vector<int> dropIdVec;
	int iCanGetExp;
};

class ClimbTowerFactory : public ComponentBase<IClimbTowerFactory, IID_IClimbTowerFactory>
{
public:
	ClimbTowerFactory();
	virtual ~ClimbTowerFactory();
	//component interface 
	virtual bool initlize(const PropertySet& propSet);
	//IClimbTowerFactory 
	virtual int getNextLayerCanGetExp(int currentLayerId);
	virtual vector<int> getLayerMonsterGropIdList(int currentLayerId);
	virtual std::vector<int>& getLayerDropIdVec(int currentLayerId);
	virtual int getLayerCanGetHeroExp(int currentLayerId);
	
private:
	void loadAllTower();
private:
	typedef std::map<Int32,SingleTowerLayerInfo > MapTowerStaticProp;
	MapTowerStaticProp	m_TowerStaticPropMap;
	//总共塔的层数
		int m_iTowerCount;   
};

#endif































