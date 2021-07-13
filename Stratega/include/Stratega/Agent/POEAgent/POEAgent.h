#pragma once
#include <random>
#include <Stratega/Agent/Agent.h>
#include <Stratega/Agent/Heuristic/AbstractHeuristic.h>

#include <Stratega/Agent/POEAgent/POEGenome.h>
#include <Stratega/Agent/POEAgent/POEParams.h>

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
		POEAgent(POEParams&& params) :
			Agent{}, params_(std::move(params))
		{
		}

		ActionAssignment computeAction(GameState state, const ForwardModel* forwardModel, long timeBudgetMs) override;

		void init(GameState initialState, const ForwardModel& forwardModel, long timeBudgetMs) override;
	
	private:
		std::vector<POEGenome> shiftPopulation(const ForwardModel& forwardModel, GameState& gameState, std::mt19937& randomGenerator);
		void initializePopulation(const ForwardModel& forwardModel, GameState& gameState, std::mt19937& randomGenerator);

		void rheaLoop(const ForwardModel& forwardModel, GameState& gameState, std::mt19937& randomGenerator);

		std::vector<POEGenome> nextGeneration(const ForwardModel& forwardModel, GameState& gameState, std::mt19937& randomGenerator);
		POEGenome getNextGenerationIndividual(const ForwardModel& forwardModel, GameState& gameState, std::mt19937& randomGenerator);
		std::vector<POEGenome> tournamentSelection();
	};
}

