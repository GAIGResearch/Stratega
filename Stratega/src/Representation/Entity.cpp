#include <Stratega/Representation/Entity.h>
#include <Stratega/Representation/Player.h>
#include <Stratega/Representation/GameInfo.h>

namespace SGA
{
	bool Entity::isNeutral() const
	{
		return ownerID == Player::NEUTRAL_PLAYER_ID;
	}

	double Entity::getEntityParameter(std::string paramName) const
	{
		for (const auto& param : type->parameters)
		{
			if (param.second.name == paramName)
			{
				return parameters[param.second.index];
			}
		}
	}

}
