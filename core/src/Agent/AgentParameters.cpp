#include <Stratega/Agent/AgentParameters.h>

namespace SGA {
	
	void AgentParameters::printDetails() const
	{
		std::cout << "AgentParameters" << "\n";
		std::cout << "\tMAX_FM_CALLS= " << MAX_FM_CALLS << "\n";
		std::cout << "\tREMAINING_FM_CALLS = " << REMAINING_FM_CALLS << "\n";
		std::cout << "\tPLAYER_ID = " << PLAYER_ID << "\n";
		std::cout << "\tOPPONENT_MODEL = " << PLAYER_ID << "\n";
	}
}