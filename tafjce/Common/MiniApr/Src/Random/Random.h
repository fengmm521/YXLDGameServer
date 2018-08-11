#ifndef __RANDOM_H__
#define __RANDOM_H__

BEGIN_MINIAPR_NAMESPACE

class MiniAprRandom:public ComponentBase<IRandom, IID_IMiniAprRandom>
{
public:

	MiniAprRandom();

	virtual bool initlize(const PropertySet& propSet);
	virtual long int random();
	virtual void setRandomValue(const list<long int>& valueList);
	virtual void setRandRcv(DelegateRandRcv rcvCb);
	virtual void clearRandRcv() ;
	virtual void clearValueList();

private:

	list<long int> m_valueList;
	DelegateRandRcv m_randRcv;
	bool m_bUseRcv;
};

END_MINIAPR_NAMESPACE

#endif
