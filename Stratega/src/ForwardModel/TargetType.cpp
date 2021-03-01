#include <Stratega/ForwardModel/TargetType.h>
#include <Stratega/ForwardModel/ActionTarget.h>
#include <Stratega/Representation/GameState.h>
#include <Stratega/Representation/EntityType.h>
namespace SGA
{
	bool TargetType::isValid(const GameState& state, const ActionTarget& actionTarget) const
	{
		switch (type)
		{
		case None:
			break;
		case Position:
			break;
		case EntityType:
			break;
		case Entity:
			if (groupEntityTypes.find(actionTarget.getEntityType(state).id) == groupEntityTypes.end())
				return false;
			break;
		case Technology:
			if (technologyTypes.find(actionTarget.getTechnologyID()) == technologyTypes.end())
				return false;
			break;
		case ContinuousAction:
			break;
		}
		return true;
	}

}
