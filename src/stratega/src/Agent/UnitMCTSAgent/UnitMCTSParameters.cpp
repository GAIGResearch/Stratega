#include <Stratega/Agent/UnitMCTSAgent/UnitMCTSParameters.h>
#include <iostream>

namespace SGA {
void UnitMCTSParameters::printDetails() const
{
   AgentParameters::printDetails();
   std::cout << "UnitMCTSParameters"
             << "\n";
   std::cout << "\tK = "                    << K << "\n";
   std::cout << "\tROLLOUT_LENGTH= "        << ROLLOUT_LENGTH << "\n";
   std::cout << "\tROLLOUTS_ENABLED= "      << ROLLOUTS_ENABLED << "\n";
   std::cout << "\tPRIORITIZE_ROOT= "       << PRIORITIZE_ROOT << "\n";
   std::cout << "\tMAX_FM_CALLS= "          << maxFMCalls << "\n";
   std::cout << "\tEPSILON = "              << EPSILON << "\n";
   std::cout << "\tCONTINUE_PREVIOUSE_SEARCH = " << CONTINUE_PREVIOUS_SEARCH << "\n";
   std::cout << "\tDO_STATE_ABSTRACTION = " << DO_STATE_ABSTRACTION << "\n";
}
}  // namespace SGA