#include "FmHeroComponent.h"
#include "FmHero.h"

NS_FM_BEGIN

Hero* HeroComponent::GetHero()
{
	return GetEntity<Hero>();
}



NS_FM_END