#pragma once
#include <random>
#include <Agent/Agent.h>
#include <Agent/Heuristic/StateHeuristic.h>
#include <Agent/Heuristic/LinearSumHeuristic.h>
#include <Representation/TBSGameState.h>

#include <Agent/PortfolioOnlineEvolution/POEGenome.h>
#include <Agent/PortfolioOnlineEvolution/POEParams.h>

namespace SGA
{
	class POEAgent : public Agent
	{

	private:
		std::vector<POEGenome> pop_;
		std::unique_ptr<StateHeuristic>  heuristic_;
		POEParams params_;

		std::uniform_real<double> doubleDistribution_ = std::uniform_real<double>(0, 1);

	public:
		POEAgent() :
			Agent{}, params_(POEParams())
		{
		}

		void runTBS(TBSGameCommunicator& gameCommunicator, TBSForwardModel forwardModel) override;

	private:
		std::vector<POEGenome> shiftPopulation(TBSForwardModel& forwardModel, TBSGameState& gameState, std::mt19937& randomGenerator);
		void initializePopulation(TBSForwardModel& forwardModel, TBSGameState& gameState, std::mt19937& randomGenerator);

		void rheaLoop(TBSForwardModel& forwardModel, TBSGameState& gameState, std::mt19937& randomGenerator);

		std::vector<POEGenome> nextGeneration(TBSForwardModel& forwardModel, TBSGameState& gameState, std::mt19937& randomGenerator);
		POEGenome getNextGenerationIndividual(TBSForwardModel& forwardModel, TBSGameState& gameState, std::mt19937& randomGenerator);
		std::vector<POEGenome> tournamentSelection();
	};
}

