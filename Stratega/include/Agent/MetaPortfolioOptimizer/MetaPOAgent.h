#pragma once
#include <random>
#include <Agent/Agent.h>
#include <Agent/Heuristic/StateHeuristic.h>
#include <Agent/Heuristic/LinearSumHeuristic.h>
#include <Representation/TBSGameState.h>

#include <Agent/MetaPortfolioOptimizer/MetaPOGenome.h>
#include <Agent/MetaPortfolioOptimizer/MetaPOParams.h>

namespace SGA
{
	class MetaPOAgent : public Agent
	{

	private:
		std::vector<MetaPOGenome> pop_;
		std::unique_ptr<StateHeuristic>  heuristic_;
		MetaPOParams params_;
		int lastKnownTick = -1;

		std::uniform_real<double> doubleDistribution_ = std::uniform_real<double>(0, 1);

	public:
		MetaPOAgent() :
			Agent{}, params_(MetaPOParams())
		{
		}
		
		void runTBS(TBSGameCommunicator& gameCommunicator, TBSForwardModel forwardModel) override;

	private:
		std::vector<MetaPOGenome> shiftPopulation(TBSForwardModel& forwardModel, TBSGameState& gameState, std::mt19937& randomGenerator);
		void initializePopulation(TBSForwardModel& forwardModel, TBSGameState& gameState, std::mt19937& randomGenerator);

		void rheaLoop(TBSForwardModel& forwardModel, TBSGameState& gameState, std::mt19937& randomGenerator);

		std::vector<MetaPOGenome> nextGeneration(TBSForwardModel& forwardModel, TBSGameState& gameState, std::mt19937& randomGenerator);
		MetaPOGenome getNextGenerationIndividual(TBSForwardModel& forwardModel, TBSGameState& gameState, std::mt19937& randomGenerator);
		std::vector<MetaPOGenome> tournamentSelection();
	};
}

