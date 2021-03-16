#pragma once
#include <Stratega/Agent/RHEAAgent/RHEAParams.h>
#include <Stratega/Representation/GameState.h>
#include <Stratega/ForwardModel/TBSForwardModel.h>

#include <iostream>
#include <random>
#include <vector>

namespace SGA {

	class RHEAGenome
	{
		
	private:
		std::vector<Action> actions;
		double value = 0;

	public:
		// creates a random PortfolioGenome
		RHEAGenome(TBSForwardModel& forwardModel, GameState gameState, RHEAParams& params);

		// creates a copy of an existing Portfolio Genome
		RHEAGenome(const RHEAGenome& other) = default;

		std::vector<Action>& getActions() { return actions; };

		void mutate(TBSForwardModel& forwardModel, GameState gameState, RHEAParams& params, std::mt19937 & randomGenerator);

		double getValue() const { return value; };
		void setValue(const double value) { this->value = value; };

		void shift(TBSForwardModel& forwardModel, GameState gameState, RHEAParams& params);
		void toString() const;
		static RHEAGenome crossover(TBSForwardModel& forwardModel, GameState gameState, RHEAParams& params, std::mt19937 & randomGenerator, RHEAGenome& parent1, RHEAGenome& parent2);

	private:
		RHEAGenome(std::vector<Action>& actions, double value);
		static void applyActionToGameState(const TBSForwardModel& forwardModel, GameState& gameState, std::vector<SGA::Action>& actionSpace, const Action& action, RHEAParams& params);
		
	};
}
