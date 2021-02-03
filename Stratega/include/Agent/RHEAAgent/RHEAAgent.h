#pragma once
#include <random>
#include <Agent/Agent.h>
#include <Agent/Heuristic/StateHeuristic.h>
#include <Representation/TBSGameState.h>
#include <ForwardModel/TBSForwardModel.h>

#include <Agent/RHEAAgent/RHEAGenome.h>
#include <Agent/RHEAAgent/RHEAParams.h>

namespace SGA
{
	class RHEAAgent : public Agent
	{

	private:
		std::vector<RHEAGenome> pop_;
		std::unique_ptr<StateHeuristic>  heuristic_;
		RHEAParams params_;

		std::uniform_real_distribution<double> doubleDistribution_ = std::uniform_real_distribution<double>(0, 1);

	public:
		RHEAAgent() :
			Agent{}, params_(RHEAParams())
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