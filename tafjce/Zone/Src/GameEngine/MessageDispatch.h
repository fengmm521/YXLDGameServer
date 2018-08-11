#ifndef __MESSAGE_DISPATCH_H__
#define __MESSAGE_DISPATCH_H__

#include "IMessageDispatch.h"

class MessageDispatch:public  ComponentBase<IMessageDispatch, IID_IMessageDispatch>
{
public:

	// IComponent Interface
	virtual bool initlize(const PropertySet& propSet);
	virtual bool inject(const std::string& componentName, IComponent* pComponent);

	// IMessageDispatch Interface
	virtual void registerListener(IMessageListener* pListerner);
	virtual bool dispatchMessage(Uint32 dwMsgID, const char* pMsgBuff, Int32 nBuffLen);

private:

	typedef std::map<Uint32, IMessageListener*>		MapListener;
	MapListener	m_mapMsgListener;
};

#endif
