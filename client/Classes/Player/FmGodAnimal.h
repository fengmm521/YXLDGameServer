#ifndef _FMGODANIMAL_H_
#define _FMGODANIMAL_H_
#include "FmConfig.h"
#include "../Numeric/GodAnimalData.h"
#include "FmEntity.h"
NS_FM_BEGIN

class GodAnimal: public Entity{
protected:
	GodAnimal( uint8 entityType, uint entityId, const string& name );
public:
	static Entity* Create( uint entityId, const string& entityName, EntityCreateOpt* createOpt );
	static void InitInterface();
public:
	virtual ~GodAnimal();
	void setGodAnimalData(stGodAnimalData* data ) { m_GodAnimaldata = data;}
	stGodAnimalData* getGodAnimalData() { return m_GodAnimaldata;}
private:
	stGodAnimalData* m_GodAnimaldata;  
};
NS_FM_END
#endif