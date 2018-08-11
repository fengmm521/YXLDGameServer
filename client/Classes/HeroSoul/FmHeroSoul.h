#ifndef __FMHEROSOUL_H__
#define  __FMHEROSOUL_H__

#include "FmEntity.h"
#include "FmGeometry.h"

NS_FM_BEGIN

class HeroSoulData 
{
public:
   HeroSoulData(int32 iHeroId,int32 iSoulCount)
  {
	  m_iHeroID = iHeroId;
	  m_iSoulCount = iSoulCount;
   }
   ~HeroSoulData();
   int32 m_iHeroID;
   int32 m_iSoulCount;
};

class HeroSoulBag : public Component
{
private:
	vector<HeroSoulData *>      m_HeroSoulDatalist;
public:
	vector<HeroSoulData *>& GetHeroSoulData(){
		return m_HeroSoulDatalist;
	}
	HeroSoulBag(Entity *entity);
};

NS_FM_END
#endif