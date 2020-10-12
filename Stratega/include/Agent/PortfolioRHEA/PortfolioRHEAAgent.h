#pragma once
#include <random>
#include <Agent/Agent.h>
#include <Agent/Heuristic/StateHeuristic.h>
#include <Agent/Heuristic/LinearSumHeuristic.h>
#include <Representation/TBSGameState.h>

#include <Agent/PortfolioRHEA/PortfolioRHEAGenome.h>
#include <Agent/PortfolioRHEA/PortfolioRHEAParams.h>

namespace SGA
{
	class PortfolioRHEAAgent : public Agent
	{

	private:
		std::vector<PortfolioRHEAGenome> pop_;
		std::unique_ptr<StateHeuristic>  heuristic_;
		PortfolioRHEAParams params_;

		std::uniform_real<double> doubleDistribution_ = std::uniform_real<double>(0, 1);

	public:
		PortfolioRHEAAgent() :
			Agent{}, params_(PortfolioRHEAParams())
		{
		}
		
		void runTBS(TBSGameCommunicator& gameCommunicator, TBSForwardModel forwardModel) override;

	private:
		std::vector<PortfolioRHEAGenome> shiftPopulation(TBSForwardModel& forwardModel, TBSGameState& gameState, std::mt19937& randomGenerator);
		void initializePopulation(TBSForwardModel& forwardModel, TBSGameState& gameState, std::mt19937& randomGenerator);

		void rheaLoop(TBSForwardModel& forwardModel, TBSGameState& gameState, std::mt19937& randomGenerator);

		std::vector<PortfolioRHEAGenome> nextGeneration(TBSForwardModel& forwardModel, TBSGameState& gameState, std::mt19937& randomGenerator);
		PortfolioRHEAGenome getNextGenerationIndividual(TBSForwardModel& forwardModel, TBSGameState& gameState, std::mt19937& randomGenerator);
		std::vector<PortfolioRHEAGenome> tournamentSelection();
	};
}

