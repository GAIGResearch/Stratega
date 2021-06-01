#include <Stratega/Agent/TreeSearchAgents/MCTSParameters.h>

#include <iostream>


namespace SGA {
	void MCTSParameters::printDetails() const
	{
		std::cout << "MCTSParameters" << "\n";
		std::cout << "\tK = " << K << "\n";
		std::cout << "\tROLLOUT_LENGTH= " << ROLLOUT_LENGTH << "\n";
		std::cout << "\tROLLOUTS_ENABLED= " << ROLLOUTS_ENABLED << "\n";
		std::cout << "\tFORCE_TURN_END= " << FORCE_TURN_END << "\n";
		std::cout << "\tPRIORITIZE_ROOT= " << PRIORITIZE_ROOT << "\n";
		std::cout << "\tMAX_FM_CALLS= " << MAX_FM_CALLS << "\n";
		std::cout << "\tEPSILON = " << EPSILON << "\n";
		std::cout << "\tPLAYER_ID = " << PLAYER_ID << "\n";
	}
}