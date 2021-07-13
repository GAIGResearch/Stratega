#pragma once
#include <random>
#include <vector>

#include <Stratega/Agent/PRHEAAgent/PRHEAParameters.h>

namespace SGA {

	class PRHEAGenome
	{

	private:
		std::vector<Action> actions;
		std::vector<int> portfolioIndices;
		double value = 0;

	public:
		// creates a random PortfolioGenome
		PRHEAGenome(const ForwardModel& forwardModel, GameState gameState, PRHEAParameters& params);
		
		// creates a copy of an existing Portfolio Genome
		PRHEAGenome(const PRHEAGenome& other) = default;

		std::vector<Action>& getActions() { return actions; };
		std::vector<int>& getPortfolioVector() { return portfolioIndices; };

		void mutate(const ForwardModel& forwardModel, GameState gameState, PRHEAParameters& params, std::mt19937& randomGenerator);
		
		double getValue() const { return value; };
		void setValue(const double value) { this->value = value; };

		void shift(const ForwardModel& forwardModel, GameState gameState, PRHEAParameters& params);
		void toString() const;
		static PRHEAGenome crossover(const ForwardModel& forwardModel, GameState gameState, PRHEAParameters& params, std::mt19937& randomGenerator, PRHEAGenome& parent1, PRHEAGenome& parent2);
		
	private:
		PRHEAGenome(std::vector<Action>& actions, std::vector<int>& portfolioIndices, double value);
		static void applyActionToGameState(const ForwardModel& forwardModel, GameState& gameState, std::vector<SGA::Action>& actionSpace,  const Action& action, PRHEAParameters& params);
		

	};
}
