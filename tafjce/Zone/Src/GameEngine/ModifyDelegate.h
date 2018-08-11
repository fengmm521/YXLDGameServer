#ifndef __MODIFY_DELEGATE_H__
#define __MODIFY_DELEGATE_H__

#include "IModifyDelegate.h"

struct OpDetail
{
	vector<DelegateTask> opList;
};


class ModofyGetRoleCb:public ServerEngine::RolePrxCallback
{
public:

	ModofyGetRoleCb(const ServerEngine::PKRole& roleKey):m_roleKey(roleKey){}
	virtual void callback_getRole(taf::Int32 ret,  const ServerEngine::PIRole& roleInfo);
	virtual void callback_getRole_exception(taf::Int32 ret);

private:

	const ServerEngine::PKRole m_roleKey;
};


class ModifyUpdateRoleCb:public ServerEngine::RolePrxCallback
{
public:

	ModifyUpdateRoleCb(const ServerEngine::PKRole& roleKey):m_roleKey(roleKey){}

	virtual void callback_updateRole(taf::Int32 ret);
    virtual void callback_updateRole_exception(taf::Int32 ret);

private:

	const ServerEngine::PKRole m_roleKey;
};


class ModifyDelegate:public ComponentBase<IModifyDelegate, IID_IModifyDelegate>
{
public:

	// IModifyDelegate Interface
	virtual void submitDelegateTask(const ServerEngine::PKRole &stRoleKey, DelegateTask oneTask);
	void callOnelineTask(const ServerEngine::PKRole &stRoleKey, HEntity hEntity);
	void callTaskException(const ServerEngine::PKRole &stRoleKey);
	void callTaskOffline(const ServerEngine::PKRole &stRoleKey, int iRet, const ServerEngine::PIRole& roleInfo);

private:

	map<ServerEngine::PKRole, OpDetail> m_taskList;
};


#endif
