#ifndef __ACTOR_SAVE_SYSTEM_H__
#define __ACTOR_SAVE_SYSTEM_H__

#include "IEntity.h"
#include "IActorSaveSystem.h"

class ActorSaveSystem:public ObjectBase<IActorSaveSystem>, public ITimerCallback
{
public:

	ActorSaveSystem();
	~ActorSaveSystem();

	virtual Uint32 getSubsystemID() const;
	virtual Uint32 getMasterHandle();
	virtual bool create(IEntity* pEntity, const std::string& strData);
	virtual bool createComplete(){return true;}
	virtual const std::vector<Uint32>& getSupportMessage();
	virtual void onMessage(QxMessage* pMessage){}
	virtual void packSaveData(string& data){}

	//IActorSaveSystem Interface
	virtual void doSave(bool bSync);

	// ITimerCallback Interface
	virtual void onTimer(int nEventId);

private:

	HEntity m_hHandle;
	ITimerComponent::TimerHandle m_hSaveHandle;
};


#endif
