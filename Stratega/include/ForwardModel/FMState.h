#pragma once
#include <vector>
#include <Representation/TBSGameState.h>

namespace SGA
{
	struct FMState
	{
		TBSGameState& target;
		std::vector<int> deadUnitIDs;

		FMState(TBSGameState& target)
			: target(target), deadUnitIDs()
		{
			
		}
	};

	struct RTSFMState
	{
		RTSGameState& target;
		std::vector<int> deadUnitIDs;

		explicit RTSFMState(RTSGameState& target)
			: target(target), deadUnitIDs()
		{

		}
	};
}
