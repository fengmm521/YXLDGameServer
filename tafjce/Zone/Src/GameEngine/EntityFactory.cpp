

#include "GameEnginePch.h"
#include "EntityFactory.h"

extern "C" IComponent* createEntityFactory(Int32)
{
	return new EntityFactory;
}


EntityFactory::EntityFactory()
{
}

EntityFactory::~EntityFactory()
{	
}

bool EntityFactory::initlize(const PropertySet& propSet)
{	
	return true;
}

IEntity* EntityFactory::createEntity(const std::string& strClassName, const std::string& strMsg)
{
	PROFILE_MONITOR("EntityFactory::createEntity");
	IEntity* pEntity = static_cast<IEntity*>(getCompomentObjectManager()->createObject(strClassName) );
	assert(pEntity);


	if(!pEntity->createEntity(strMsg) )
	{
		MINIAPR_ERROR("create Entity Fail|%s\n", strMsg.c_str() );
		return NULL;
	}
	
	return pEntity;
}

