#pragma once
#include <Stratega/Agent/RHEAAgent/RHEAParams.h>
#include <Stratega/Representation/GameState.h>
#include <Stratega/ForwardModel/ForwardModel.h>

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
		RHEAGenome(const ForwardModel& forwardModel, GameState gameState, RHEAParams& params);

		// creates a copy of an existing Portfolio Genome
		RHEAGenome(const RHEAGenome& other) = default;

		std::vector<Action>& getActions() { return actions; };

		void mutate(const ForwardModel& forwardModel, GameState gameState, RHEAParams& params, std::mt19937& randomGenerator);

		[[nodiscard]] double getValue() const { return value; };
		void setValue(double newValue) { this->value = newValue; };


		void shift(const ForwardModel& forwardModel, GameState gameState, RHEAParams& params);
		void toString() const;
		static RHEAGenome crossover(const ForwardModel& forwardModel, GameState gameState, RHEAParams& params, std::mt19937& randomGenerator, RHEAGenome& parent1, RHEAGenome& parent2);

	private:
		RHEAGenome(std::vector<Action>& actions, double value);
		static void applyActionToGameState(const ForwardModel& forwardModel, GameState& gameState, const Action& action, RHEAParams& params);
	};
}
