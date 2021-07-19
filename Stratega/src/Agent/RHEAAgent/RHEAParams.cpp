#include <Stratega/Agent/RHEAAgent/RHEAParams.h>
#include <yaml-cpp/yaml.h>

#include <iostream>


namespace SGA {
	void RHEAParams::printDetails() const
	{
		std::cout << "RHEAParams" << std::endl;
		std::cout << "\tPOP_SIZE = " << POP_SIZE << std::endl;
		std::cout << "\tINDIVIDUAL_LENGTH = " << INDIVIDUAL_LENGTH << std::endl;

		std::cout << "\tMUTATION_RATE = " << MUTATION_RATE << std::endl;
		std::cout << "\tTOURNAMENT_SIZE = " << TOURNAMENT_SIZE << std::endl;
		std::cout << "\tELITISM = " << ELITISM << std::endl;

		std::cout << "\tCONTINUE_SEARCH = " << CONTINUE_SEARCH << std::endl;
		std::cout << "\tMUTATE_BEST = " << MUTATE_BEST << std::endl;
		std::cout << "\tPLAYER_ID = " << PLAYER_ID << std::endl;

		std::cout << "\tHEURISTIC = " << getStateHeuristic()->getName() << std::endl;
		std::cout << "\tEPSILON = " << EPSILON << std::endl;
	}
}


namespace YAML
{
    template<>
    struct convert<SGA::RHEAParams>
    {
        static bool decode(const Node& node, SGA::RHEAParams& rhs)
        {

            rhs.maxFMCalls = node["FmCalls"].as<int>(rhs.maxFMCalls);
            rhs.maxIterations = node["Iterations"].as<int>(rhs.maxIterations);
            if (node["Budget"].as<std::string>("") == "TIME")
                rhs.budgetType = SGA::Budget::TIME;
            else if (node["Budget"].as<std::string>("") == "FMCALLS")
                rhs.budgetType = SGA::Budget::FMCALLS;
            else if (node["Budget"].as<std::string>("") == "ITERATIONS")
                rhs.budgetType = SGA::Budget::ITERATIONS;

            return true;
        }
    };
}
