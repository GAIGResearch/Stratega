#pragma once
#include <ForwardModel/AbstractFM/Precondition.h>

namespace SGA
{
	class HasResource : public Precondition
	{
		std::string resource;
	public:
		HasResource(std::string resourceName):
		resource(resourceName)
		{
			
		}
		
		bool isFullfilled(const GameState& state, const std::vector<ActionTarget>& targets) const override
		{
			
		}
	};
}