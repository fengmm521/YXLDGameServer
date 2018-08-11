#ifndef __FMHEROFAVORITE_H__
#define __FMHEROFAVORITE_H__

#include "FmEntity.h"
#include "FmGeometry.h"

NS_FM_BEGIN

class HeroFavoriteData
{
public:
	HeroFavoriteData(int32 favoriteId,int32 heroFavoritePos)
	{
		m_HeroFavoriteID = favoriteId;
		m_HeroFavoritePos = heroFavoritePos;
	}
    ~HeroFavoriteData();
	int32 m_HeroFavoriteID;
	int32 m_HeroFavoritePos;
};

enum HeroFavoriteState
{
	NoHaveHeroFavorite,
	HaveEquipFavorite,
	CanEquipFavorite,
	CannotEquipFavorite,
}; 

class HeroFavorite : public Component
{
private:
	vector<HeroFavoriteData *> m_herofavoriteData;
	vector<HeroFavoriteState> m_herofavoriteState;
	vector<int> m_herofavoriteId;
public:
	vector<HeroFavoriteData *> &GetHeroFavoriteData()
	{
		return m_herofavoriteData;
	}
	vector<HeroFavoriteState> &GetHeroFavoriteState()
	{
		return m_herofavoriteState;
	}
	vector<int> &GetHeroFavoriteId()
	{
		return m_herofavoriteId;
	}
	HeroFavorite(Entity *entity);
};
NS_FM_END
#endif