#ifndef __IGODANIMAL_SYSTEM_H__
#define __IGODANIMAL_SYSTEM_H__

class IGodAnimalSystem:public IEntitySubsystem
{
public:

	virtual vector<HEntity> getGodAnimalList() = 0;

	virtual HEntity getActiveGodAnimal() = 0;

	virtual HEntity addGodAnimal(int iBaseID, bool bNotifyError, int iLevelStep = 0) = 0;
	
	virtual HEntity addGodAnimalWithLevelStep(int iBaseID, bool bNotifyError, int iLevelStep) = 0;

	virtual void autoActiveGodAnimal() = 0;
};


#endif


