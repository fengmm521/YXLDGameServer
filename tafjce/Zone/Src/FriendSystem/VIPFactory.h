#ifndef VIPFACTORY_H_
#define VIPFACTORY_H_

#include "IVIPFactory.h"

class VIPFactory:public ComponentBase<IVIPFactory,IID_IVIPFactory>
{
public:
	VIPFactory();
	virtual ~VIPFactory();
public:
	//component interface 
	virtual bool initlize(const PropertySet& propSet);
	//IVIPFactory
	virtual int getVipPropByHEntity(HEntity hEntity,unsigned int propKey);
	
	virtual int getVipPropByVIPLevel(int vipLevel, unsigned int propKey);
	virtual int getVipTopLevel();
private:
	std::map<int, std::vector<int> > m_VipDataMap;
};
#endif
