#pragma once
#include <random>
#include <Stratega/Agent/Agent.h>
#include <Stratega/Agent/Heuristic/StateHeuristic.h>
#include <Stratega/Representation/GameState.h>
#include <Stratega/ForwardModel/TBSForwardModel.h>

#include <Stratega/Agent/RHEAAgent/RHEAGenome.h>
#include <Stratega/Agent/RHEAAgent/RHEAParams.h>

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
		std::vector<RHEAGenome> shiftPopulation(TBSForwardModel& forwardModel, GameState& gameState, std::mt19937& randomGenerator);
		void initializePopulation(TBSForwardModel& forwardModel, GameState& gameState, std::mt19937& randomGenerator);

		void rheaLoop(TBSForwardModel& forwardModel, GameState& gameState, std::mt19937& randomGenerator);

		std::vector<RHEAGenome> nextGeneration(TBSForwardModel& forwardModel, GameState& gameState, std::mt19937& randomGenerator);
		RHEAGenome getNextGenerationIndividual(TBSForwardModel& forwardModel, GameState& gameState, std::mt19937& randomGenerator);
		std::vector<RHEAGenome> tournamentSelection();
	};
	
}