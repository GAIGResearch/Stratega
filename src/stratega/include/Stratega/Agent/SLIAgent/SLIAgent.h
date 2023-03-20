#pragma once
#include <Stratega/Agent/Agent.h>
#include <Stratega/Agent/Heuristic/AbstractHeuristic.h>

#include "SLIParameters.h"

namespace SGA
{

	class SLIAgent : public Agent
	{
	public:
		explicit SLIAgent(const std::string& name, SLIParameters&& params)
			:Agent{ name }, parameters_(std::move(params))
		{
		}

		std::vector<SGA::Action> actionSequence = {};

		/// <summary>
		/// Override from Agent. This function is called at the start of the game only, providing a copy of the initial game state,
		/// a foward model, and a timer that indicates when the game will start.
		/// </summary>
		void init(GameState initialState, const ForwardModel& forwardModel, Timer timer) override;

		/// <summary>
		/// Override from agents. This function is called at every tick of the game (turn in TBS, frame in RTS). The agent must return an
		/// action assignment object that indicates what is to be executed in the game. This function receives a copy of the game state, a 
		/// forward model and a timer that indicates when the action is due.  In TBS games, the game is
		/// blocked waiting for actions for eac agent, and penalties may apply if this time is not respected. In RTS games, 
		/// the game will continue independently of having returned an action, when the timer goes off.
		/// </summary>
		ActionAssignment computeAction(GameState state, const ForwardModel& forwardModel, Timer timer) override;

		std::vector< std::vector<SGA::Action> > generateActionSpace(GameState state, const ForwardModel& forwardModel);
		bool rolloutFinished(GameState& rollerState, int depth, SLIParameters& params);
		double rollOut(GameState gameState, ForwardModel& forwardModel, SLIParameters& params, boost::mt19937& randomGenerator);
		void applyActionToGameState(
			ForwardModel& forwardModel,
			GameState& targetGameState,
			Action& action,
			SLIParameters& params,
			int playerID) const;

	private:
		int previousActionIndex = -1;						//Indicates the index of the action executed in the previous compute call.
		SLIParameters parameters_;							//Configuration of MCTS.
	};
}