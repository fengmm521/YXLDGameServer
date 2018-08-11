#ifndef __IMANOR_FACTORY_H__
#define __IMANOR_FACTORY_H__

class IManorFactory :public IComponent 
{
public:
	virtual int getRefreshTieJiangPuOrWuHunDianCost(int iNowRefreshTimes) = 0;
};

#endif
