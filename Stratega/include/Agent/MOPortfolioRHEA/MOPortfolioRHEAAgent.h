#pragma once
#include <random>
#include <Agent/Agent.h>
#include <Agent/Heuristic/StateHeuristic.h>
#include <Agent/Heuristic/LinearSumHeuristic.h>
#include <Representation/TBSGameState.h>

#include <Agent/MOPortfolioRHEA/MOPortfolioRHEAGenome.h>
#include <Agent/MOPortfolioRHEA/MOPortfolioRHEAParams.h>

namespace SGA
{
	class MOPortfolioRHEAAgent : public Agent
	{

	private:
		std::vector<MOPortfolioRHEAGenome> pop_;
		std::unique_ptr<StateHeuristic>  heuristic_;
		MOPortfolioRHEAParams params_;

		std::uniform_real<double> doubleDistribution_ = std::uniform_real<double>(0, 1);

	public:
		MOPortfolioRHEAAgent() :
			Agent{}, params_(MOPortfolioRHEAParams())
		{
		}
		
		void runTBS(TBSGameCommunicator& gameCommunicator, TBSForwardModel forwardModel) override;

	private:
		std::vector<MOPortfolioRHEAGenome> shiftPopulation(TBSForwardModel& forwardModel, TBSGameState& gameState, std::mt19937& randomGenerator);
		void initializePopulation(TBSForwardModel& forwardModel, TBSGameState& gameState, std::mt19937& randomGenerator);

		void rheaLoop(TBSForwardModel& forwardModel, TBSGameState& gameState, std::mt19937& randomGenerator);

		std::vector<MOPortfolioRHEAGenome> nextGeneration(TBSForwardModel& forwardModel, TBSGameState& gameState, std::mt19937& randomGenerator);
		MOPortfolioRHEAGenome getNextGenerationIndividual(TBSForwardModel& forwardModel, TBSGameState& gameState, std::mt19937& randomGenerator);
		std::vector<MOPortfolioRHEAGenome> tournamentSelection();
		void calculateCrowdingDistances(std::vector<MOPortfolioRHEAGenome>& population);
		std::vector<std::vector<MOPortfolioRHEAGenome*>> getFronts(std::vector<MOPortfolioRHEAGenome>& population);
		//void getFronts(std::vector<MOPortfolioRHEAGenome>& population);
	};
}

