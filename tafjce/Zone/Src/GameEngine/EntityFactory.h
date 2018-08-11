#ifndef __ENTITY_FACTORY_H__
#define __ENTITY_FACTORY_H__

#include "IEntity.h"
#include "IEntityFactory.h"


class EntityFactory:public ComponentBase<IEntityFactory, IID_IEntityFactory>
{
public:

	friend class EntityPropertyRule_StardEffect;

	// Constructor/Destructor
	EntityFactory();
	~EntityFactory();

	// IComponent Interface
	virtual bool initlize(const PropertySet& propSet);

	// IEntityFactory Interface
	virtual IEntity* createEntity(const std::string& strClassName, const std::string& strMsg);
};

#endif
