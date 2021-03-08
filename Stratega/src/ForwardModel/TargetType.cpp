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
			//Avoid player source
			if (sourceActionTarget.getType() == ActionTarget::PlayerReference)
				return true;
				
			//Verify if position is in shape
			auto sourcePosition = sourceActionTarget.getPosition(state);
			auto targetPosition = actionTarget.getPosition(state);
				
			switch (shapeType)
			{
			case ShapeType::Square:			
			{
				return (sourcePosition.x >= targetPosition.x - shapeSize && sourcePosition.x <= targetPosition.x + shapeSize &&
					sourcePosition.y>=targetPosition.y - shapeSize && sourcePosition.y <= targetPosition.y + shapeSize);
			}				
			case ShapeType::Circle:
			{
				return sourcePosition.distance(actionTarget.getPosition(state)) <= shapeSize;
			}				
			default: return true;
			}
		}break;
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
