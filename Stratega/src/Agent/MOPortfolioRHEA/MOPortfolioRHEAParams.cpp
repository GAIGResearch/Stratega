#include <Agent/MOPortfolioRHEA/MOPortfolioRHEAParams.h>

#include <iostream>


namespace SGA {
	void MOPortfolioRHEAParams::printDetails() const
	{
		std::cout << "PortfolioRHEAParams" << std::endl;
		std::cout << "\tPOP_SIZE = " << POP_SIZE << std::endl;
		std::cout << "\tINDIVIDUAL_LENGTH = " << INDIVIDUAL_LENGTH << std::endl;

		std::cout << "\tMUTATION_RATE = " << MUTATION_RATE << std::endl;
		std::cout << "\tTOURNAMENT_SIZE = " << TOURNAMENT_SIZE << std::endl;
		std::cout << "\tELITISM = " << ELITISM << std::endl;

		std::cout << "\tCONTINUE_SEARCH = " << CONTINUE_SEARCH << std::endl;
		std::cout << "\tMUTATE_BEST = " << MUTATE_BEST << std::endl;
		
		std::cout << "\tMAX_FM_CALLS = " << MAX_FM_CALLS << std::endl;
		std::cout << "\tREMAINING_FM_CALLS = " << REMAINING_FM_CALLS << std::endl;
		std::cout << "\tPLAYER_ID = " << PLAYER_ID << std::endl;

		std::cout << "\t#PORTFOLIOS = " << PORTFOLIO.size() << std::endl;
		std::cout << "\tHEURISTIC1 = " << HEURISTIC1.getName() << std::endl;
		std::cout << "\tHEURISTIC2 = " << HEURISTIC2.getName() << std::endl;
		std::cout << "\tEPSILON = " << EPSILON << std::endl;
	}
}