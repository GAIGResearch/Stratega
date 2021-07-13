#pragma once
#include <random>
#include <Stratega/Agent/Agent.h>
#include <Stratega/Agent/Heuristic/StateHeuristic.h>
#include <Stratega/Agent/Heuristic/AbstractHeuristic.h>

#include <Stratega/Agent/MOPRHEAAgent/MOPRHEAGenome.h>
#include <Stratega/Agent/MOPRHEAAgent/MOPRHEAParameters.h>

namespace SGA
{
	class MOPRHEAAgent : public Agent
	{

	private:
		std::vector<MOPRHEAGenome> pop_;
		std::unique_ptr<StateHeuristic>  heuristic_;
		MOPRHEAParameters params_;

		std::uniform_real<double> doubleDistribution_ = std::uniform_real<double>(0, 1);

	public:
		MOPRHEAAgent(MOPRHEAParameters&& params) :
			Agent{}, params_(std::move(params))
		{
		}

		void init(GameState initialState, const ForwardModel& forwardModel, long timeBudgetMs) override;
		ActionAssignment computeAction(GameState state, const ForwardModel* forwardModel, long timeBudgetMs) override;

	private:
		std::vector<MOPRHEAGenome> shiftPopulation(const ForwardModel& forwardModel, GameState& gameState, std::mt19937& randomGenerator);
		void initializePopulation(const ForwardModel& forwardModel, GameState& gameState, std::mt19937& randomGenerator);

		void rheaLoop(const ForwardModel& forwardModel, GameState& gameState, std::mt19937& randomGenerator);

		std::vector<MOPRHEAGenome> nextGeneration(const ForwardModel& forwardModel, GameState& gameState, std::mt19937& randomGenerator);
		MOPRHEAGenome getNextGenerationIndividual(const ForwardModel& forwardModel, GameState& gameState, std::mt19937& randomGenerator);
		std::vector<MOPRHEAGenome> tournamentSelection();
		void calculateCrowdingDistances(std::vector<MOPRHEAGenome>& population);
		std::vector<std::vector<MOPRHEAGenome*>> getFronts(std::vector<MOPRHEAGenome>& population);
		//void getFronts(std::vector<MOPortfolioRHEAGenome>& population);
	};
}

