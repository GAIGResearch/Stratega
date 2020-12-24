#include <ForwardModel/Condition.h>
#include <Representation/Entity.h>
namespace  SGA
{
	HasResource::HasResource(const std::vector<FunctionParameter>& parameters) :
		resourceReference(parameters.at(0)),
		lowerBound(parameters.at(1))
	{
	}

	bool HasResource::isFullfilled(const GameState& state, const std::vector<ActionTarget>& targets) const
	{
		auto targetResource = resourceReference.getParameter(state, targets);
		double lowerBound = this->lowerBound.getConstant(state,targets);
		
		return targetResource >= lowerBound;
	}

	SamePlayer::SamePlayer(const std::vector<FunctionParameter>& parameters)
	{
		
	}

	bool SamePlayer::isFullfilled(const GameState& state, const std::vector<ActionTarget>& targets) const
	{
		auto& sourceEntity = targetToEntity(state,targets[0]);
		auto& targetEntity = targetToEntity(state,targets[1]);

		return sourceEntity.owner == targetEntity.owner;
	}


}
