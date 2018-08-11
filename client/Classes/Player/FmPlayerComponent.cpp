#include "FmPlayerComponent.h"
#include "Player/FmPlayer.h"

NS_FM_BEGIN

Player* PlayerComponent::GetPlayer()
{
	return GetEntity<Player>();
}

NS_FM_END