#include <ForwardModel/AbstractFM/Precondition.h>
#include <Representation/AbstractGS/Entity.h>
namespace  SGA
{
	HasResource::HasResource(FunctionParameter resourceReference, FunctionParameter lowerBound) :
		resourceReference(resourceReference),
		lowerBound(lowerBound)
	{

	}

	bool HasResource::isFullfilled(const GameState& state, const std::vector<ActionTarget>& targets) const
	{
		auto targetResource = resourceReference.getParameter(state, targets);
		double lowerBound = this->lowerBound.getConstant(state,targets);
		
		return targetResource >= lowerBound;
	}

	SamePlayer::SamePlayer()
	{
		
	}

	bool SamePlayer::isFullfilled(const GameState& state, const std::vector<ActionTarget>& targets) const
	{
		auto& sourceEntity = targetToEntity(state,targets[0]);
		auto& targetEntity = targetToEntity(state,targets[1]);

		return sourceEntity.owner == targetEntity.owner;
	}


}
