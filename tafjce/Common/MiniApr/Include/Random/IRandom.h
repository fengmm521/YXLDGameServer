#ifndef __IRANDOM_H__
#define __IRANDOM_H__

BEGIN_MINIAPR_NAMESPACE

#define IID_IMiniAprRandom	1

typedef TC_Functor<void, TL::TLMaker<long int>::Result> DelegateRandRcv;

class IRandom:public IComponent
{
public:

	virtual long int random() = 0;

	virtual void setRandomValue(const list<long int>& valueList) = 0;

	virtual void setRandRcv(DelegateRandRcv rcvCb) = 0;

	virtual void clearRandRcv() = 0;

	virtual void clearValueList() = 0;
};


END_MINIAPR_NAMESPACE

#endif
