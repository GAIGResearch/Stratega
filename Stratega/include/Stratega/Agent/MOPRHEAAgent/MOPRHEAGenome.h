#pragma once
#include <random>
#include <vector>

#include <Stratega/ForwardModel/ForwardModel.h>
#include <Stratega/Agent/MOPRHEAAgent/MOPRHEAParameters.h>

namespace SGA {

	class MOPRHEAGenome
	{

	private:
		std::vector<Action> actions;
		std::vector<int> portfolioIndices;

	public:
		double value1 = 0;
		double value2 = 0;
		double crowdingDistance = 0;
		bool dominated = false;

		
		// creates a random PortfolioGenome
		MOPRHEAGenome(const ForwardModel& forwardModel, GameState gameState, MOPRHEAParameters& params);
		
		// creates a copy of an existing Portfolio Genome
		MOPRHEAGenome(const MOPRHEAGenome& other) = default;

		std::vector<Action>& getActions() { return actions; };
		std::vector<int>& getPortfolioVector() { return portfolioIndices; };

		void mutate(const ForwardModel& forwardModel, GameState gameState, MOPRHEAParameters& params, std::mt19937& randomGenerator);

		void shift(const ForwardModel& forwardModel, GameState gameState, MOPRHEAParameters& params);
		void toString() const;
		static MOPRHEAGenome crossover(const ForwardModel& forwardModel, GameState gameState, MOPRHEAParameters& params, std::mt19937& randomGenerator, MOPRHEAGenome& parent1, MOPRHEAGenome& parent2);
		
	private:
		MOPRHEAGenome(std::vector<Action>& actions, std::vector<int>& portfolioIndices, double value1, double value2);
		static void applyActionToGameState(const ForwardModel& forwardModel, GameState& gameState, std::vector<SGA::Action>& actionSpace,  const Action& action, MOPRHEAParameters& params);
		

	};
}
