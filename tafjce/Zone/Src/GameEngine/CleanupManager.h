#ifndef __CLEAN_UP_MANAGER_H__
#define __CLEAN_UP_MANAGER_H__

class CleanUpManager:public ComponentBase<ICleanUpManager, IID_ICleanUpManager>
{
public:

	// Constructor/Destructor
	CleanUpManager();
	~CleanUpManager();

	// IComponent Interface
	virtual bool initlize(const PropertySet& propSet);

	// ICleanUpManager Interface
	virtual void doCleanUp(bool boIsKickOffRole = true);
	virtual void heatBeat();
};


#endif

