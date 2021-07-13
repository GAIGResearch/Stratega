#pragma once
#include <random>
#include <vector>

#include <Stratega/ForwardModel/ForwardModel.h>
#include <Stratega/Agent/POEAgent/POEParams.h>

namespace SGA {

	class POEGenome
	{
	private:
		std::vector<std::map<int, BaseActionScript*>> scriptAssignment;
		double value = 0;

	public:
		// creates a random PortfolioGenome
		POEGenome(const ForwardModel& forwardModel, GameState gameState, POEParams& params);
		
		// creates a copy of an existing Portfolio Genome
		POEGenome(const POEGenome& other) = default;


		void mutate(const ForwardModel& forwardModel, GameState gameState, POEParams& params, std::mt19937& randomGenerator);

		std::vector<std::map<int, BaseActionScript*>> getScriptAssignment() const { return scriptAssignment; };
		double getValue() const { return value; };
		void setValue(const double value) { this->value = value; };

		void shift(const ForwardModel& forwardModel, GameState gameState, POEParams& params);
		void toString() const;
		static POEGenome crossover(const ForwardModel& forwardModel, GameState gameState, POEParams& params, std::mt19937& randomGenerator, POEGenome& parent1, POEGenome& parent2);
		Action getAction(GameState& gameState, std::vector<Action>& actionSpace);

	private:
		POEGenome(std::vector<std::map<int, BaseActionScript*>>, const ForwardModel& forwardModel, GameState& gameState, POEParams& params);
		static void applyActionToGameState(const ForwardModel& forwardModel, GameState& gameState, std::vector<Action>& actionSpace,  const Action& action, POEParams& params);
		double evaluateGenome(const ForwardModel& forwardModel, GameState& gameState, POEParams& params);

	};
}
