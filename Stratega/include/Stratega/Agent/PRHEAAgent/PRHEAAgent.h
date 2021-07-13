#pragma once
#include <random>
#include <Stratega/Agent/Agent.h>
#include <Stratega/Agent/Heuristic/AbstractHeuristic.h>

#include <Stratega/Agent/PRHEAAgent/PRHEAGenome.h>
#include <Stratega/Agent/PRHEAAgent/PRHEAParameters.h>

namespace SGA
{
	class PRHEAAgent : public Agent
	{

	private:
		std::vector<PRHEAGenome> pop_;
		std::unique_ptr<StateHeuristic>  heuristic_;
		PRHEAParameters params_;

		std::uniform_real<double> doubleDistribution_ = std::uniform_real<double>(0, 1);

	public:
		PRHEAAgent(PRHEAParameters&& params) :
			Agent{}, params_(std::move(params))
		{
		}


		void init(GameState initialState, const ForwardModel& forwardModel, long timeBudgetMs) override;
		ActionAssignment computeAction(GameState state, const ForwardModel* forwardModel, long timeBudgetMs) override;

		
	private:
		std::vector<PRHEAGenome> shiftPopulation(const ForwardModel& forwardModel, GameState& gameState, std::mt19937& randomGenerator);
		void initializePopulation(const ForwardModel& forwardModel, GameState& gameState, std::mt19937& randomGenerator);

		void rheaLoop(const ForwardModel& forwardModel, GameState& gameState, std::mt19937& randomGenerator);

		std::vector<PRHEAGenome> nextGeneration(const ForwardModel& forwardModel, GameState& gameState, std::mt19937& randomGenerator);
		PRHEAGenome getNextGenerationIndividual(const ForwardModel& forwardModel, GameState& gameState, std::mt19937& randomGenerator);
		std::vector<PRHEAGenome> tournamentSelection();
	};
}

