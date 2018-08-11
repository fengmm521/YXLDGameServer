#ifndef __GOLD_ANIMAL_H__
#define __GOLD_ANIMAL_H__

class GoldAnimal:public Entity
{
public:

	GoldAnimal();

	virtual bool createEntity(const std::string& strData);
	virtual void packSaveData(ServerEngine::RoleSaveData& data);
	virtual void addExp(int iExp);
	void initGoldAnimalProp();

	void onEventCalcGrow(EventArgs& args);
	void reCalcGrow();

private:

	PROFILE_OBJ_COUNTER(GoldAnimal);
};


#endif
