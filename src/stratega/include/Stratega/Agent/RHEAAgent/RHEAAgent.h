#pragma once
#include <boost/random.hpp>
#include <Stratega/Agent/Agent.h>
#include <Stratega/Agent/Heuristic/StateHeuristic.h>
#include <Stratega/Representation/GameState.h>
#include <Stratega/ForwardModel/ForwardModel.h>

#include <Stratega/Agent/RHEAAgent/RHEAGenome.h>
#include <Stratega/Agent/RHEAAgent/RHEAParameters.h>

namespace SGA
{
	class RHEAAgent : public Agent
	{

	private:
		std::vector<RHEAGenome> pop_;						//Population of individuals for RHEA
		RHEAParameters params_;									//Configuration of RHEA

	public:
		RHEAAgent(const std::string& name, RHEAParameters&& params) :
			Agent{name}, params_(std::move(params))
		{
		}

		/// <summary>
		/// Override from Agent. This function is called at the start of the game only, providing a copy of the initial game state,
		/// a foward model, and a timer that indicates when the game will start.
		/// </summary>
		void init(GameState initialState, const ForwardModel & forwardModel, Timer timer) override;

		/// <summary>
		/// Override from agents. This function is called at every tick of the game (turn in TBS, frame in RTS). The agent must return an
		/// action assignment object that indicates what is to be executed in the game. This function receives a copy of the game state, a 
		/// forward model and a timer that indicates when the action is due.  In TBS games, the game is
		/// blocked waiting for actions for eac agent, and penalties may apply if this time is not respected. In RTS games, 
		/// the game will continue independently of having returned an action, when the timer goes off.
		/// </summary>
		ActionAssignment computeAction(GameState state, const ForwardModel& forwardModel, Timer timer) override;

	private:
		/// <summary>
		/// Shifts all individuals in the population to the left, by eliminating the first action (executed in the previous decision) 
		/// and padding with a random valid action at the end.
		/// </summary>
		std::vector<RHEAGenome> shiftPopulation(const ForwardModel& forwardModel, GameState& gameState, boost::mt19937& randomGenerator);
		
		/// <summary>
		/// Initializes the population of RHEA.
		/// </summary>
		void initializePopulation(const ForwardModel& forwardModel, GameState& gameState, boost::mt19937& randomGenerator);

		/// <summary>
		/// Executes the main iteration of the RHEA algorithm.
		/// </summary>
		void rheaLoop(const ForwardModel& forwardModel, GameState& gameState, boost::mt19937& randomGenerator);

		/// <summary>
		/// Creates a new generation of individuals.
		/// </summary>
		std::vector<RHEAGenome> nextGeneration(const ForwardModel& forwardModel, GameState& gameState, boost::mt19937& randomGenerator);
		
		/// <summary>
		/// Creates a new individual for the next generation
		/// </summary>
		RHEAGenome getNextGenerationIndividual(const ForwardModel& forwardModel, GameState& gameState, boost::mt19937& randomGenerator);

		/// <summary>
		/// Returns a selection of individuals for crossover.
		/// </summary>
		std::vector<RHEAGenome> tournamentSelection();
	};
	
}