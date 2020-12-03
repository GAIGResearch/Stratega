#pragma once
#include <random>
#include <Agent/Agent.h>
#include <Agent/Heuristic/StateHeuristic.h>
#include <Agent/Heuristic/LinearSumHeuristic.h>
#include <Representation/TBSGameState.h>

#include <Agent/RHEAAgent/RHEAGenome.h>
#include <Agent/RHEAAgent/RHEAParams.h>

namespace SGA
{
	class RHEAAgent : public Agent
	{
	public:
		RHEAParams params_;

	private:
		std::vector<RHEAGenome> pop_;
		std::unique_ptr<StateHeuristic>  heuristic_;

		std::uniform_real<double> doubleDistribution_ = std::uniform_real<double>(0, 1);

	public:
		RHEAAgent(RHEAParams&& params) :
			Agent{}, params_(std::move(params))
		{
		}

		void runTBS(TBSGameCommunicator& gameCommunicator, TBSForwardModel forwardModel) override;

	private:
		std::vector<RHEAGenome> shiftPopulation(TBSForwardModel& forwardModel, TBSGameState& gameState, std::mt19937& randomGenerator);
		void initializePopulation(TBSForwardModel& forwardModel, TBSGameState& gameState, std::mt19937& randomGenerator);

		void rheaLoop(TBSForwardModel& forwardModel, TBSGameState& gameState, std::mt19937& randomGenerator);

		std::vector<RHEAGenome> nextGeneration(TBSForwardModel& forwardModel, TBSGameState& gameState, std::mt19937& randomGenerator);
		RHEAGenome getNextGenerationIndividual(TBSForwardModel& forwardModel, TBSGameState& gameState, std::mt19937& randomGenerator);
		std::vector<RHEAGenome> tournamentSelection();
	};
	
}