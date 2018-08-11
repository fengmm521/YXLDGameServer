#include "GameEnginePch.h"
#include "MessageDispatch.h"

extern "C" IComponent* createMessageDispatch(Int32)
{
	return new MessageDispatch;
}

bool MessageDispatch::initlize(const PropertySet& propSet)
{
	// do nothing
	return true;
}

bool MessageDispatch::inject(const std::string& componentName, IComponent* pComponent)
{
	if(pComponent->queryInterface(IID_IMessageListener) )
	{
		IMessageListener* pMsgListener = static_cast<IMessageListener*>(pComponent->queryInterface(IID_IMessageListener) );
		assert(pMsgListener);

		registerListener(pMsgListener);
	}

	return true;
}

void MessageDispatch::registerListener(IMessageListener* pListerner)
{
	std::vector<Uint32> supportMsgList = pListerner->getSupportMsgList();
	for(std::vector<Uint32>::iterator it = supportMsgList.begin(); 
		it != supportMsgList.end(); it++)
	{
		m_mapMsgListener.insert(std::make_pair(*it, pListerner) );
	}
}

bool MessageDispatch::dispatchMessage(Uint32 dwMsgID, const char* pMsgBuff, Int32 nBuffLen)
{
	MapListener::iterator it = m_mapMsgListener.find(dwMsgID);
	if(it == m_mapMsgListener.end() )
	{
		//MINIAPR_ERROR("Unsupport MsgID:%d", dwMsgID);
		return false;
	}

	IMessageListener* pTargetListener = it->second;
	assert(pTargetListener);

	pTargetListener->onMessage(dwMsgID, pMsgBuff, nBuffLen);
    
    return true;
}


