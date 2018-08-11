#include "FmHeroFavorite.h"
Freeman::HeroFavorite::HeroFavorite(Entity *entity)
	: Component(entity)
{
	m_herofavoriteState.push_back(NoHaveHeroFavorite);
	m_herofavoriteState.push_back(NoHaveHeroFavorite);
	m_herofavoriteState.push_back(NoHaveHeroFavorite);
	m_herofavoriteState.push_back(NoHaveHeroFavorite);
	m_herofavoriteState.push_back(NoHaveHeroFavorite);
	m_herofavoriteState.push_back(NoHaveHeroFavorite);
}
