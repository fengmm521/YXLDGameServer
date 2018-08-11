#include "GameEnginePch.h"
#include "ModifyDelegate.h"

extern "C" IComponent* createModifyDelegate(Int32)
{
	return new ModifyDelegate;
}


void ModofyGetRoleCb::callback_getRole(taf::Int32 ret,  const ServerEngine::PIRole& roleInfo)
{
	ModifyDelegate* pModifyDelegate = static_cast<ModifyDelegate*>(getComponent<IModifyDelegate>("ModifyDelegate", IID_IModifyDelegate) );
	assert(pModifyDelegate);

	pModifyDelegate->callTaskOffline(m_roleKey, ret, roleInfo);
}

void ModofyGetRoleCb::callback_getRole_exception(taf::Int32 ret)
{
	ModifyDelegate* pModifyDelegate = static_cast<ModifyDelegate*>(getComponent<IModifyDelegate>("ModifyDelegate", IID_IModifyDelegate) );
	assert(pModifyDelegate);

	pModifyDelegate->callTaskException(m_roleKey);
}


void ModifyUpdateRoleCb::callback_updateRole(taf::Int32 ret)
{
	FDLOG("ModifyDelegate")<<"UpdateRole|Ret|"<<m_roleKey.strAccount<<"|"<<(int)m_roleKey.worldID<<"|"<<ret<<endl;
}

void ModifyUpdateRoleCb::callback_updateRole_exception(taf::Int32 ret)
{
	FDLOG("ModifyDelegate")<<"UpdateRole|Exception|"<<m_roleKey.strAccount<<"|"<<(int)m_roleKey.worldID<<"|"<<ret<<endl;
}

void ModifyDelegate::submitDelegateTask(const ServerEngine::PKRole &stRoleKey, DelegateTask oneTask)
{
	IUserStateManager* pUserStateMgr = getComponent<IUserStateManager>(COMPNAME_LoginManager, IID_IUserStateManager);
	assert(pUserStateMgr);

	HEntity hEntity = pUserStateMgr->getRichEntityByAccount(stRoleKey.strAccount);
	if(0 != hEntity)
	{
		m_taskList[stRoleKey].opList.push_back(oneTask);
		callOnelineTask(stRoleKey, hEntity);
		return;
	}

	// 请求数据
	IMessageLayer* pMsgLayer = getComponent<IMessageLayer>(COMPNAME_MessageLayer, IID_IMessageLayer);
	assert(pMsgLayer);

	try
	{
		m_taskList[stRoleKey].opList.push_back(oneTask);
		ServerEngine::RolePrxCallbackPtr cb = new ModofyGetRoleCb(stRoleKey);
		pMsgLayer->AsyncGetRole(stRoleKey, cb);
	}
	catch(...)
	{
		callTaskException(stRoleKey);
	}
}

void ModifyDelegate::callOnelineTask(const ServerEngine::PKRole &stRoleKey, HEntity hEntity)
{
	map<ServerEngine::PKRole, OpDetail>::iterator it = m_taskList.find(stRoleKey);
	if(it == m_taskList.end() )
	{
		return;
	}

	OpDetail refOp = it->second;
	m_taskList.erase(it);

	ServerEngine::PIRole emptyRoleInfo;
	for(size_t i =0; i < refOp.opList.size(); i++)
	{
		refOp.opList[i](ServerEngine::en_RoleRet_OK, hEntity, emptyRoleInfo);
	}
}

void ModifyDelegate::callTaskException(const ServerEngine::PKRole &stRoleKey)
{
	map<ServerEngine::PKRole, OpDetail>::iterator it = m_taskList.find(stRoleKey);
	if(it == m_taskList.end() )
	{
		return;
	}

	OpDetail refOp = it->second;
	m_taskList.erase(it);

	ServerEngine::PIRole emptyRoleInfo;
	for(size_t i =0; i < refOp.opList.size(); i++)
	{
		HEntity nullEntity;
		refOp.opList[i](ServerEngine::en_RoleRet_SysError, nullEntity, emptyRoleInfo);
	}

	
}

void ModifyDelegate::callTaskOffline(const ServerEngine::PKRole &stRoleKey, int iRet, const ServerEngine::PIRole& roleInfo)
{
	// 再次判断玩家是否在线
	IUserStateManager* pUserStateMgr = getComponent<IUserStateManager>(COMPNAME_LoginManager, IID_IUserStateManager);
	assert(pUserStateMgr);

	HEntity hEntity = pUserStateMgr->getRichEntityByAccount(stRoleKey.strAccount);
	if(0 != hEntity)
	{
		callOnelineTask(stRoleKey, hEntity);
		return;
	}

	// 继续处理
	map<ServerEngine::PKRole, OpDetail>::iterator it = m_taskList.find(stRoleKey);
	if(it == m_taskList.end() )
	{
		return;
	}

	ServerEngine::PIRole tmpRoleInfo = roleInfo;
	OpDetail refOp = it->second;
	m_taskList.erase(it);
	
	for(size_t i =0; i < refOp.opList.size(); i++)
	{
		HEntity nullEntity;
		refOp.opList[i](iRet, nullEntity, tmpRoleInfo);
	}
	
	if(tmpRoleInfo != roleInfo)
	{
		IMessageLayer* pMsgLayer = getComponent<IMessageLayer>(COMPNAME_MessageLayer, IID_IMessageLayer);
		assert(pMsgLayer);

		ServerEngine::RolePrxCallbackPtr cb = new ModifyUpdateRoleCb(stRoleKey);
		pMsgLayer->AsyncUpdateRole(stRoleKey, tmpRoleInfo, cb);
	}
}



