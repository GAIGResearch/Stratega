#include <Stratega/Representation/Entity.h>
#include <Stratega/Representation/Player.h>

namespace SGA
{
	bool Entity::isNeutral() const
	{
		return ownerID == Player::NEUTRAL_PLAYER_ID;
	}

}
