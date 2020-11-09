#pragma once
#include <ForwardModel/Action.h>

#include <random>
#include <vector>

#include <ForwardModel/TBSForwardModel.h>
#include <Agent/MOPortfolioRHEA/MOPortfolioRHEAParams.h>

namespace SGA {

	class MOPortfolioRHEAGenome
	{

	private:
		std::vector<Action<Vector2i>> actions;
		std::vector<int> portfolioIndices;

	public:
		double value1 = 0;
		double value2 = 0;
		double crowdingDistance = 0;
		bool dominated = false;

		
		// creates a random PortfolioGenome
		MOPortfolioRHEAGenome(TBSForwardModel& forwardModel, TBSGameState gameState, MOPortfolioRHEAParams& params);
		
		// creates a copy of an existing Portfolio Genome
		MOPortfolioRHEAGenome(const MOPortfolioRHEAGenome& other) = default;

		std::vector<Action<Vector2i>>& getActions() { return actions; };
		std::vector<int>& getPortfolioVector() { return portfolioIndices; };

		void mutate(TBSForwardModel& forwardModel, TBSGameState gameState, MOPortfolioRHEAParams& params, std::mt19937& randomGenerator);

		void shift(TBSForwardModel& forwardModel, TBSGameState gameState, MOPortfolioRHEAParams& params);
		void toString() const;
		static MOPortfolioRHEAGenome crossover(TBSForwardModel& forwardModel, TBSGameState gameState, MOPortfolioRHEAParams& params, std::mt19937& randomGenerator, MOPortfolioRHEAGenome& parent1, MOPortfolioRHEAGenome& parent2);
		
	private:
		MOPortfolioRHEAGenome(std::vector<Action<Vector2i>>& actions, std::vector<int>& portfolioIndices, double value1, double value2);
		static void applyActionToGameState(const TBSForwardModel& forwardModel, TBSGameState& gameState, std::unique_ptr<ActionSpace<Vector2i>>& actionSpace,  const Action<Vector2i>& action, MOPortfolioRHEAParams& params);
		

	};
}
