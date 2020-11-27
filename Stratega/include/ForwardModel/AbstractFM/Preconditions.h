#pragma once
#include <ForwardModel/AbstractFM/Precondition.h>

namespace SGA
{
	class HasResource : public Precondition
	{
		FunctionParameter resourceReference;
		FunctionParameter lowerBound;
	public:
		HasResource(FunctionParameter resourceReference, FunctionParameter lowerBound):
			resourceReference(resourceReference),
			lowerBound(lowerBound)
		{
			
		}
		
		bool isFullfilled(const GameState& state, const std::vector<ActionTarget>& targets) const override;
	};
}