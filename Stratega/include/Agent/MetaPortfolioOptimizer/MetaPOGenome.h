#pragma once
#include <ForwardModel/Action.h>

#include <random>
#include <vector>

#include <ForwardModel/TBSForwardModel.h>
#include <Agent/MetaPortfolioOptimizer/MetaPOParams.h>
#include <Agent/MetaPortfolioOptimizer/UnitScriptChange.h>


namespace SGA {

	class MetaPOGenome
	{

	private:
		std::vector<UnitScriptChange> scriptUpdates;
		std::map<int, BasePortfolio*> unitScript;
		
		double value = 0;

	public:
		// creates a random PortfolioGenome
		MetaPOGenome(TBSForwardModel& forwardModel, TBSGameState gameState, MetaPOParams& params);
		
		// creates a copy of an existing Portfolio Genome
		MetaPOGenome(const MetaPOGenome& other) = default;

		std::map<int, BasePortfolio*>& getUnitScript() { return unitScript; };

		void mutate(TBSForwardModel& forwardModel, TBSGameState gameState, MetaPOParams& params, std::mt19937& randomGenerator);
		
		double getValue() const { return value; };
		double evaluateGenome(TBSForwardModel& forwardModel, TBSGameState& gameState, MetaPOParams& params);
		void setValue(const double value) { this->value = value; };

		void shift(TBSForwardModel& forwardModel, TBSGameState gameState, MetaPOParams& params, std::set<int>& unitIDs, std::mt19937& randomGenerator);
		void toString() const;
		static MetaPOGenome crossover(TBSForwardModel& forwardModel, TBSGameState gameState, MetaPOParams& params, std::mt19937& randomGenerator, MetaPOGenome& parent1, MetaPOGenome& parent2);

		Action<Vector2i> getAction(TBSGameState& gameState, std::unique_ptr<ActionSpace<Vector2i>>& actionSpace);
		
	private:
		MetaPOGenome(std::map<int, BasePortfolio*>& unitScript, std::vector<UnitScriptChange> scriptUpdates, TBSForwardModel& forwardModel, TBSGameState& gameState, MetaPOParams& params);
		static void applyActionToGameState(const TBSForwardModel& forwardModel, TBSGameState& gameState, std::unique_ptr<ActionSpace<Vector2i>>& actionSpace,  const Action<Vector2i>& action, MetaPOParams& params);

	};
}
