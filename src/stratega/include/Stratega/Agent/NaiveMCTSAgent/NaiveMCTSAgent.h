#pragma once
#include <Stratega/Agent/Agent.h>
#include <Stratega/Agent/NaiveMCTSAgent/NaiveMCTSNode.h>
#include <Stratega/Agent/Heuristic/AbstractHeuristic.h>

#include "NaiveMCTSParameters.h"

namespace SGA
{

	class NaiveMCTSAgent: public Agent
	{
	public:
		explicit NaiveMCTSAgent(const std::string& name, NaiveMCTSParameters&& params)
			:Agent{ name }, parameters_(std::move(params))
		{
		}

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

	private:
		std::unique_ptr<NaiveMCTSNode> rootNode = nullptr;		//Root node of MCTS.
		int previousActionIndex = -1;						//Indicates the index of the action executed in the previous compute call.
		NaiveMCTSParameters parameters_;							//Configuration of MCTS.
	};
}