#include <ForwardModel/AbstractFM/Preconditions.h>

namespace  SGA
{
	bool HasResource::isFullfilled(const GameState& state, const std::vector<ActionTarget>& targets) const
	{
		auto targetResource = resourceReference.toValue(state, targets);
		double lowerBound1 = this->lowerBound.toValue(state,targets);

		return targetResource >= lowerBound1;
	}
}
