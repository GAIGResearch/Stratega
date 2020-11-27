#include <ForwardModel/AbstractFM/Precondition.h>

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
}
