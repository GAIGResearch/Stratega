#pragma once
#include <ForwardModel/Action.h>

#include <random>
#include <vector>

#include <ForwardModel/TBSForwardModel.h>
#include <Agent/PortfolioOnlineEvolution/POEParams.h>

namespace SGA {

	class POEGenome
	{
	private:
		std::vector<std::map<int, BaseActionScript*>> scriptAssignment;
		double value = 0;

	public:
		// creates a random PortfolioGenome
		POEGenome(TBSForwardModel& forwardModel, TBSGameState gameState, POEParams& params);
		
		// creates a copy of an existing Portfolio Genome
		POEGenome(const POEGenome& other) = default;


		void mutate(TBSForwardModel& forwardModel, TBSGameState gameState, POEParams& params, std::mt19937& randomGenerator);

		std::vector<std::map<int, BaseActionScript*>> getScriptAssignment() const { return scriptAssignment; };
		double getValue() const { return value; };
		void setValue(const double value) { this->value = value; };

		void shift(TBSForwardModel& forwardModel, TBSGameState gameState, POEParams& params);
		void toString() const;
		static POEGenome crossover(TBSForwardModel& forwardModel, TBSGameState gameState, POEParams& params, std::mt19937& randomGenerator, POEGenome& parent1, POEGenome& parent2);
		Action<Vector2i> POEGenome::getAction(TBSGameState& gameState, std::vector<Action<Vector2i>>& actionSpace);

	private:
		POEGenome(std::vector<std::map<int, BaseActionScript*>>, TBSForwardModel& forwardModel, TBSGameState& gameState, POEParams& params);
		static void applyActionToGameState(const TBSForwardModel& forwardModel, TBSGameState& gameState, std::vector<Action<Vector2i>>& actionSpace,  const Action<Vector2i>& action, POEParams& params);
		double evaluateGenome(TBSForwardModel& forwardModel, TBSGameState& gameState, POEParams& params);

	};
}
