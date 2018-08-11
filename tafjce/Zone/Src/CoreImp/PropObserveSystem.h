#ifndef __PROP_OBSERVE_SYSTEM_H__
#define __PROP_OBSERVE_SYSTEM_H__

#include "IEntity.h"

class PropObserveSystem:public ObjectBase<IEntitySubsystem>, public Detail::EventHandle
{
public:

	PropObserveSystem();
	~PropObserveSystem();

	virtual Uint32 getSubsystemID() const{return IID_IPropObserveSystem;}
	virtual Uint32 getMasterHandle(){return m_hEntity;}
	virtual bool create(IEntity* pEntity, const std::string& strData);
	virtual bool createComplete();
	virtual const std::vector<Uint32>& getSupportMessage();
	virtual void onMessage(QxMessage* pMessage){}
	virtual void packSaveData(string& data){}

	bool isMasterCreateFinish();

	void onEventPropChg(EventArgs& args);
	//void onActorCreateFinish(EventArgs& args);
	void registerActorFinish();
	void unRegisterActorFinish();

private:

	HEntity m_hEntity;
	//bool m_bCreateFinish;
};


#endif

