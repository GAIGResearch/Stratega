#include <Stratega/Agent/AgentParameters.h>

namespace SGA {
	
	void AgentParameters::printDetails() const
	{
		std::cout << "AgentParameters" << "\n";
		std::cout << "\tMAX_FM_CALLS= " << maxFMCalls << "\n";
		std::cout << "\tCURRENT_FM_CALLS = " << currentFMCalls << "\n";
		std::cout << "\tPLAYER_ID = " << PLAYER_ID << "\n";
	}
}