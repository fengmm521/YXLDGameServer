#ifndef ISHOPSYSTEM_H_
#define ISHOPSYSTEM_H_
#include "IEntity.h"

struct AddHeroReturn:public Detail::EventHandle
{
	AddHeroReturn(HEntity hEntity)
		:iSoulId(0), iSoulCount(0),
		iHeroId(0),
		m_hEntity(hEntity)
	{
		IEntity *pEntity = getEntityFromHandle(m_hEntity);
		assert(pEntity);
		pEntity->getEventServer()->subscribeEvent(EVENT_ENTITY_ADDHEROSOUL_FROMHERO, this, &AddHeroReturn::eventReturn);
	}
	virtual ~AddHeroReturn()
	{
		IEntity* pEntity = getEntityFromHandle(m_hEntity);
		if(pEntity)
		{
			pEntity->getEventServer()->unsubscribeEvent(EVENT_ENTITY_ADDHEROSOUL_FROMHERO, this, &AddHeroReturn::eventReturn);	
			
		}
	}

	void eventReturn(EventArgs& args )
	{
		iSoulId = args.context.getInt("itemid");
		iSoulCount = args.context.getInt("itemcount");
		iHeroId = args.context.getInt("heroId");
		assert(iHeroId != 0);
	}
	
	int iSoulId;
	int iSoulCount;
	int iHeroId;
	HEntity m_hEntity;
};

class IShopSystem:
public IEntitySubsystem
{
	public:
		virtual  void refreshActorHonorConvertShop() = 0;
		virtual void normalShopRefreshCallback() = 0;
		virtual  bool addGoldForPayment(int iGameCoinCount, string strChanel,const string& strOrderId="") = 0;
		
};

#endif