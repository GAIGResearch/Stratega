#pragma once
#include <random>
#include <Agent/Agent.h>
#include <Agent/Heuristic/StateHeuristic.h>
#include <Representation/TBSGameState.h>
#include <ForwardModel/TBSAbstractForwardModel.h>

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

		std::uniform_real<double> doubleDistribution_ = std::uniform_real<double>(0, 1);

	public:
		RHEAAgent() :
			Agent{}, params_(RHEAParams())
		{
		}

		void runAbstractTBS(AbstractTBSGameCommunicator& gameCommunicator, TBSAbstractForwardModel forwardModel) override;

	private:
		std::vector<RHEAGenome> shiftPopulation(TBSAbstractForwardModel& forwardModel, TBSGameState2& gameState, std::mt19937& randomGenerator);
		void initializePopulation(TBSAbstractForwardModel& forwardModel, TBSGameState2& gameState, std::mt19937& randomGenerator);

		void rheaLoop(TBSAbstractForwardModel& forwardModel, TBSGameState2& gameState, std::mt19937& randomGenerator);

		std::vector<RHEAGenome> nextGeneration(TBSAbstractForwardModel& forwardModel, TBSGameState2& gameState, std::mt19937& randomGenerator);
		RHEAGenome getNextGenerationIndividual(TBSAbstractForwardModel& forwardModel, TBSGameState2& gameState, std::mt19937& randomGenerator);
		std::vector<RHEAGenome> tournamentSelection();
	};
	
}