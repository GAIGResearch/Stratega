#include <Representation/Entity.h>
#include <Representation/Player.h>

namespace SGA
{
	bool Entity::isNeutral() const
	{
		return ownerID == Player::NEUTRAL_PLAYER_ID;
	}

}
