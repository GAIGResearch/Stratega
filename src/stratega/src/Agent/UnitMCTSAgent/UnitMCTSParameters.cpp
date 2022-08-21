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
   std::cout << "\tBATCH_SIZE = "           << batch_size <<"\n";
   std::cout << "\tRANDOM_ABSTRACTION = "   << random_abstraction <<"\n";
   if (DO_STATE_ABSTRACTION) {
       std::cout<< "\tABSTRACTION_BATCH = " << absBatch << "\n";
       std::cout<< "\tR_THRESHOLD = "<< R_THRESHOLD << "\n";
       std::cout<< "\tT_THRESHOLD = "<< T_THRESHOLD << "\n";
       std::cout << "\tIS_UNGROUPING = "<< IS_UNGROUPING << "\n";
       std::cout << "\tIS_ACTION_INDEPENDENT = " << IS_ACTION_INDEPENDENT <<"\n";
       if (IS_UNGROUPING) {
           std::cout << "\tIS_PHI_UNGROUPING = "<< IS_PHI_UNGROUPING << "\n";
           std::cout << "\tUNGROUPING_BATCH_THRESHOLD = "<< UNGROUPING_BATCH_THRESHOLD << "\n";
       }
   }
}
}  // namespace SGA