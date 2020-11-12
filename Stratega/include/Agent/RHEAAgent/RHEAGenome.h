#pragma once
#include <ForwardModel/Action.h>
#include <Agent/RHEAAgent/RHEAParams.h>
#include <Representation/TBSGameState.h>
#include <ForwardModel/TBSForwardModel.h>

#include <iostream>
#include <random>
#include <vector>

namespace SGA {

	class RHEAGenome
	{
		
	private:
		std::vector<Action<Vector2i>> actions;
		double value = 0;

	public:
		// creates a random PortfolioGenome
		RHEAGenome(TBSForwardModel& forwardModel, TBSGameState gameState, RHEAParams& params);

		// creates a copy of an existing Portfolio Genome
		RHEAGenome(const RHEAGenome& other) = default;

		std::vector<Action<Vector2i>>& getActions() { return actions; };

		void mutate(TBSForwardModel& forwardModel, TBSGameState gameState, RHEAParams& params, std::mt19937 & randomGenerator);

		double getValue() const { return value; };
		void setValue(const double value) { this->value = value; };

		void shift(TBSForwardModel& forwardModel, TBSGameState gameState, RHEAParams& params);
		void toString() const;
		static RHEAGenome crossover(TBSForwardModel& forwardModel, TBSGameState gameState, RHEAParams& params, std::mt19937 & randomGenerator, RHEAGenome& parent1, RHEAGenome& parent2);

	private:
		RHEAGenome(std::vector<Action<Vector2i>>& actions, double value);
		static void applyActionToGameState(const TBSForwardModel & forwardModel, TBSGameState& gameState, std::vector<SGA::Action<Vector2i>>& actionSpace, const Action<Vector2i>& action, RHEAParams& params);
		
	};
}
