#include "GameEnginePch.h"
#include "Entity.h"
#include "RoleData.h"

extern "C" IObject* createEntity()
{
	return new Entity;
}

static HandleManager<IEntity*> g_entityHandleMgr;


IEntity* getEntityFromHandle(HEntity hEntity)
{
	IEntity* pEntity = g_entityHandleMgr.getDataPoint(hEntity);
	
	return pEntity;
}

bool SaveOneSubsystemData(IEntity *pEntity, ServerEngine::PIRole& roleInfo, Uint32 dwSubsystemID)
{
	//使用者需要在if 里面添加系统类型 ，防止存入不需要存的数据
	if(dwSubsystemID != IID_IManorSystem)
	{
		assert(0);
	}


	ServerEngine::RoleSaveData data;
	ServerEngine::JceToObj( roleInfo.roleData, data);
	
	map<taf::Int32, std::string>::iterator iter = data.subsystemData.find(dwSubsystemID);
	assert(iter != data.subsystemData.end());
	
	ServerEngine::ManorSystemData manorSystemData;
	ServerEngine::JceToObj(iter->second, manorSystemData);
	
	//打包数据
	IEntitySubsystem* pSubsystem = pEntity->querySubsystem(dwSubsystemID);
	assert(pSubsystem);
	string strNewManorData;
	pSubsystem->packSaveData(strNewManorData);
	if(strNewManorData.size() >0)
	{
		ServerEngine::ManorSystemData manorSystemDataNew;
		ServerEngine::JceToObj(strNewManorData, manorSystemDataNew);
		manorSystemDataNew.iVigorLastSecond = manorSystemData.iVigorLastSecond;
		iter->second = ServerEngine::JceToStr(manorSystemDataNew);
	}
	
	//存数据
	roleInfo.roleData = ServerEngine::JceToStr(data);

	return true;
}

Entity::Entity()
{
	IEntity** pPEntity = g_entityHandleMgr.Aquire(m_hHandle);
	*pPEntity = this;
    // 提前确定分配大小     
	setProperty(256, 0);
}

Entity::~Entity()
{	
	EventArgs args;
	args.context.setInt("entity", m_hHandle);
	getEventServer()->setEvent(EVENT_ENTITY_PRE_RELEASE, args);

	while(m_subsystemMap.size() > 0)
	{
		SubsystemMap::iterator it = m_subsystemMap.begin();
		delete it->second;
		m_subsystemMap.erase(it);
	}
		
	g_entityHandleMgr.Release(m_hHandle);
}


bool Entity::initlize(const PropertySet& /*propSet*/)
{
	return true;
}


bool Entity::inject(const std::string& strClassName, IObject* pObject)
{
	IEntitySubsystem* pSubsystem = static_cast<IEntitySubsystem*>(pObject);
	assert(pSubsystem);

	Uint32 dwSubsystemID = pSubsystem->getSubsystemID();
	std::pair<SubsystemMap::iterator, bool> ret = m_subsystemMap.insert(std::make_pair(dwSubsystemID, pSubsystem) );
	if(!ret.second)
	{
		assert(false);
		return false;
	}
	
	const std::vector<Uint32>& msgList = pSubsystem->getSupportMessage();
	for(std::vector<Uint32>::const_iterator it = msgList.begin();
		it != msgList.end(); it++)
	{
		assert(m_subsystemMsgMap.find(*it) == m_subsystemMsgMap.end() );
		m_subsystemMsgMap[*it] = pSubsystem;
		//m_subsystemMsgMap.insert(std::make_pair(*it, pSubsystem) );
	}

	return true;
}

void Entity::setProperty(PropertySet::PropertyKey key, Int32 nValue)
{
	Int32 nOldValue = getProperty(key, (Int32)0);
	ObjectBase<IEntity>::setProperty(key, nValue);

    if( nOldValue == nValue )
    {
    	return;
    }

	EventArgs_PropChange tmpContext;
	tmpContext.hEntity = m_hHandle;
	tmpContext.iPropID = key;
	tmpContext.iType = dtInt;
	tmpContext.iOldValue = nOldValue;
	tmpContext.iValue = nValue;
	getEventServer()->setEvent(EVENT_ENTITY_PROPCHANGE, (EventArgs&)tmpContext);
}

void Entity::setProperty(PropertySet::PropertyKey key, const char* strValue)
{
	string strOldValue = getProperty(key, "");
	ObjectBase<IEntity>::setProperty(key, strValue);


	EventArgs_PropChange tmpContext;
	tmpContext.hEntity = m_hHandle;
	tmpContext.iPropID = key;
	tmpContext.iType = dtString;
	tmpContext.strOldValue = strOldValue;
	tmpContext.strValue = strValue;
	
	getEventServer()->setEvent(EVENT_ENTITY_PROPCHANGE, (EventArgs&)tmpContext);
}

void Entity::setProperty(PropertySet::PropertyKey key, float fValue)
{
	float fOldValue = getProperty(key, (float)0.0);
	ObjectBase<IEntity>::setProperty(key, fValue);
	
	EventArgs_PropChange tmpContext;
	tmpContext.hEntity = m_hHandle;
	tmpContext.iPropID = key;
	tmpContext.iType = dtFloat;
	tmpContext.fOldValue = fOldValue;
	tmpContext.fValue = fValue;

	getEventServer()->setEvent(EVENT_ENTITY_PROPCHANGE, (EventArgs&)tmpContext);
}

void Entity::setInt64Property(PropertySet::PropertyKey key, Int64 value)
{
	Int64 ddOldValue = getInt64Property(key, (Int64)0);
	ObjectBase<IEntity>::setInt64Property(key, value);

    if( ddOldValue == value )
        return;
    
	EventArgs_PropChange tmpContext;
	tmpContext.hEntity = m_hHandle;
	tmpContext.iPropID = key;
	tmpContext.iType = dtInt64;
	tmpContext.i64OldValue = ddOldValue;
	tmpContext.i64Value= value;
	getEventServer()->setEvent(EVENT_ENTITY_PROPCHANGE, (EventArgs&)tmpContext);
}

IEntitySubsystem* Entity::querySubsystem(Uint32 dwSubsystemID)
{
	SubsystemMap::iterator it = m_subsystemMap.find(dwSubsystemID);
	if(it == m_subsystemMap.end() )
	{
		return NULL;
	}

	return it->second;
}

std::vector<Uint32> Entity::getSubsystemList()
{
	std::vector<Uint32> result;
	for(SubsystemMap::iterator it = m_subsystemMap.begin(); it != m_subsystemMap.end(); it++)
	{
		result.push_back(it->first);
	}

	return result;
}


EventServer* Entity::getEventServer()
{
	return &m_eventServer;
}

void Entity::onMessage(QxMessage* pMessage)
{  
	SubsystemMsgMap::iterator it = m_subsystemMsgMap.find(pMessage->dwMsgID);
	if(it == m_subsystemMsgMap.end() )
	{
		return;
	}

	IEntitySubsystem* pSubsystem = it->second;
	assert(pSubsystem);

	pSubsystem->onMessage(pMessage);
}

void Entity::save()
{
	// todo
}


void Entity::packSaveData(ServerEngine::RoleSaveData& data)
{
	// todo
}


HEntity Entity::getHandle()
{
	return m_hHandle;
}

bool Entity::createEntity(const std::string& strData)
{
	return true;
}

void Entity::changeProperty(PropertySet::PropertyKey iKey, int iChgValue, int iReason)
{
	int iValue  = getProperty(iKey, (Int32)0);

	long ddValue = (long)iValue + (long)iChgValue;
	if(ddValue > MAX_INT_VALUE)
	{
		ddValue = MAX_INT_VALUE;
	}
	
	int iResultValue = (int)ddValue;
	
	setProperty(iKey, (Int32)iResultValue);
}

void Entity::intitBaseProp(const ServerEngine::RoleBase& roleBaseData)
{
	for(map<taf::Int32, taf::Int32>::const_iterator it = roleBaseData.roleIntPropset.begin(); it != roleBaseData.roleIntPropset.end(); it++)
	{
		int iKey = it->first;
		int iValue = it->second;

		assert(iKey < PROP_ENTITY_MAX);
		setProperty(iKey, iValue);
	}

	for(map<taf::Int32, std::string>::const_iterator it = roleBaseData.roleStringPropset.begin(); it != roleBaseData.roleStringPropset.end(); it++)
	{
		int iKey = it->first;
		string strValue = it->second;

		assert(iKey < PROP_ENTITY_MAX);
		setProperty(iKey, strValue.c_str() );
	}

	for(map<taf::Int32, taf::Float>::const_iterator it = roleBaseData.roleFloatPropset.begin(); it != roleBaseData.roleFloatPropset.end(); it++)
	{
		int iKey = it->first;
		Int64 i64Value = it->second;

		assert(iKey < PROP_ENTITY_MAX);
		setInt64Property(iKey, i64Value);
	}
}


void Entity::initSubsystem(const ServerEngine::RoleSaveData& roleSaveData)
{	
	for(SubsystemMap::iterator it = m_subsystemMap.begin(); it != m_subsystemMap.end(); it++)
	{
		IEntitySubsystem* pSubSystem = it->second;
		assert(pSubSystem);

		int iSubSysID = it->first;

		string strSubSysData;
		if(roleSaveData.subsystemData.find(iSubSysID) != roleSaveData.subsystemData.end() )
		{
			strSubSysData = roleSaveData.subsystemData.at(iSubSysID);
		}
		
		if(!pSubSystem->create(this, strSubSysData) )
		{
			assert(false);
		}
	}
}

void Entity::completeSubsystem()
{
	for(SubsystemMap::iterator it = m_subsystemMap.begin(); it != m_subsystemMap.end(); it++)
	{
		IEntitySubsystem* pSubsystem = it->second;
		assert(pSubsystem);
	
		bool bResult = pSubsystem->createComplete();
		assert(bResult);
	}
}


void Entity::packBaseProp(ServerEngine::RoleSaveData& data, int* pszData, int iSize)
{
	const PropertySet& tmpSet = getPropertySet();
	for(int i = 0; i < iSize; i++)
	{
		int nPropKey = pszData[i];
		PropType curType = tmpSet.getPropTypeByKey(nPropKey);

		switch(curType)
		{
			case dtInt:
				data.basePropData.roleIntPropset[nPropKey] = getProperty(nPropKey, (Int32)0);
				break;

			case dtString:
				data.basePropData.roleStringPropset[nPropKey] = getProperty(nPropKey, "");
				break;

			case dtFloat:
				data.basePropData.roleFloatPropset[nPropKey] = getProperty(nPropKey, (float)0.0);
				break;

			default:
				break;
		}
	}
}


void Entity::packSubsystem(ServerEngine::RoleSaveData& data)
{
	for(SubsystemMap::iterator it = m_subsystemMap.begin(); it != m_subsystemMap.end(); it++)
	{
		IEntitySubsystem* pSubsystem = it->second;
		assert(pSubsystem);
		
		int iSubSystemID = it->first;
		string strTmpData;
		pSubsystem->packSaveData(strTmpData);
		if(strTmpData.size() == 0) continue;

		data.subsystemData[iSubSystemID] = strTmpData;
	}
}

void Entity::rebindHandle(HEntity hNewHandle)
{
	g_entityHandleMgr.Release(m_hHandle);
	m_hHandle = hNewHandle;

	IEntity** pPEntity = g_entityHandleMgr.AcuireFromIndex(m_hHandle);
	*pPEntity = this;
}

void Entity::sendMessage(const GSProto::SCMessage& scMessage)
{
}




