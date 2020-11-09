#pragma once
#include <iostream>
#include <set>
#include <random>

namespace SGA {

	class UnitScriptChange {
	public:
		int  tick;
		int  unitID;
		int  targetScript;

		UnitScriptChange(int tick, int unitID, int targetScript) :tick(tick), unitID(unitID), targetScript(targetScript)
		{
		}

		static UnitScriptChange createRandomScriptChange(int horizon, std::set<int> unitIDs, int portfolioSize);

		void mutate(int horizon, std::set<int> unitIDs, int portfolioSize, std::mt19937& randomGenerator);

		void mutateAll(int horizon, std::set<int> unitIDs, int portfolioSize, std::mt19937& randomGenerator);
		
	};
};
