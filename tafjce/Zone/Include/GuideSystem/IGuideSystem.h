#ifndef __IGUIDESYSTEM_H__
#define __IGUIDESYSTEM_H__

class IGuideSystem:public IEntitySubsystem
{
public:

	virtual bool isGuideOpened(int iGuideID) = 0;

	virtual bool isGuideFinished(int iGuideID) = 0;
};


#endif
