#ifndef __IMODIFY_DELEGATE_H__
#define __IMODIFY_DELEGATE_H__


typedef TC_Functor<void, TL::TLMaker<int, HEntity, ServerEngine::PIRole&>::Result> DelegateTask;

class IModifyDelegate:public IComponent
{
public:
	
	virtual void submitDelegateTask(const ServerEngine::PKRole &stRoleKey, DelegateTask oneTask) = 0;
};


#endif

