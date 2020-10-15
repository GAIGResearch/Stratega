#include <Agent/MetaPortfolioOptimizer/UnitScriptChange.h>


namespace SGA
{
	template<typename S>
	auto select_random(const S& s, size_t n) {
		auto it = std::begin(s);
		// 'advance' the iterator n times
		std::advance(it, n);
		return it;
	}
	
	UnitScriptChange UnitScriptChange::createRandomScriptChange(int horizon, std::set<int> unitIDs, int portfolioSize)
	{
		const auto r = rand() % unitIDs.size(); // not _really_ random
		const auto selectedUnitID = *select_random(unitIDs, r);
		return UnitScriptChange((rand()%horizon-1)+1, selectedUnitID, rand() % portfolioSize);
	}
	
	void UnitScriptChange::mutateAll(int horizon, std::set<int> unitIDs, int portfolioSize, std::mt19937& randomGenerator)
	{
		const auto r = rand() % unitIDs.size(); // not _really_ random
		const auto selectedUnitID = *select_random(unitIDs, r);
		tick = ((rand() % horizon - 1) + 1);
		unitID = selectedUnitID;
		targetScript = rand() % portfolioSize;
	}


	void UnitScriptChange::mutate(int horizon, std::set<int> unitIDs, int portfolioSize, std::mt19937& randomGenerator)
	{
		const std::uniform_real<double> doubleDistribution_ = std::uniform_real<double>(0, 1);
		const bool randomValue = doubleDistribution_(randomGenerator);

		// mutate time point
		if (randomValue < 0.33)
		{
			tick = (rand() % horizon - 1) + 1;
			return;
		}

		// mutate target unit
		if (randomValue < 0.66)
		{
			const auto r = rand() % unitIDs.size(); // not _really_ random
			unitID = *select_random(unitIDs, r);
			return;
		}

		// mutate target script
		targetScript = rand() % portfolioSize;
	}
}

