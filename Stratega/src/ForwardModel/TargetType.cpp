#include <Stratega/ForwardModel/TargetType.h>
#include <Stratega/ForwardModel/ActionTarget.h>
#include <Stratega/Representation/GameState.h>
#include <Stratega/Representation/EntityType.h>
namespace SGA
{
	bool TargetType::isValid(const GameState& state, const ActionTarget& actionTarget, const ActionTarget& sourceActionTarget) const
	{
		switch (type)
		{
		case None:
			break;
		case Position:
		{
			//Get target position
			auto targetPosition = actionTarget.getPosition(state);

			//Avoid player source
			if (sourceActionTarget.getType() == ActionTarget::PlayerReference)
			{
				if (!samplingMethod->validatePosition(state, targetPosition))
					return false;
			}
			else
			{
				//Check sampling methods validation
				//Verify if position is in shape
				auto sourcePosition = sourceActionTarget.getPosition(state);

				if (!samplingMethod->validatePosition(state, sourcePosition, targetPosition))
					return false;
			}
			
	
		}break;
		case EntityType:
			break;
		case Entity:
		{
			if (sourceActionTarget.getType() == ActionTarget::PlayerReference)
			{
				return true;
			}
			else
			{
				//Check if entity type
				if (groupEntityTypes.find(actionTarget.getEntityType(state).getID()) == groupEntityTypes.end())
					return false;

				//Verify if position is in shape
				auto sourcePosition = sourceActionTarget.getPosition(state);
				auto targetPosition = actionTarget.getPosition(state);

				if (!samplingMethod->validatePosition(state, sourcePosition, targetPosition))
					return false;
			}
		}			
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
