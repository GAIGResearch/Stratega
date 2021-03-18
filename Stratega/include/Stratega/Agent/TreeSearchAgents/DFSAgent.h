#pragma once
#include <Stratega/Agent/Agent.h>
#include <Stratega/Agent/Heuristic/MinimizeDistanceHeuristic.h>
#include <Stratega/Agent/ActionScripts/BaseActionScript.h>
#include <Stratega/Agent/ActionScripts/RandomActionScript.h>

namespace SGA
{
	class DFSAgent : public Agent
	{

	public:
		MinimizeDistanceHeuristic _stateHeuristic;
		int maxDepth = 3;
		int forwardModelCalls = 2000;
		int remainingForwardModelCalls = forwardModelCalls;
		std::unique_ptr<BaseActionScript> opponentModel = std::make_unique<RandomActionScript>();	// the portfolio the opponent is simulated with, if set to nullptr the opponent's turn will be skipped

		DFSAgent() :
			Agent{},
			_stateHeuristic()
		{
		}
		
		void runTBS(AgentGameCommunicator& gameCommunicator, ForwardModel forwardModel) override;

		double evaluateRollout(ForwardModel& forwardModel, GameState& gameState, int depth, int playerID);
		void applyActionToGameState(const ForwardModel& forwardModel, GameState& gameState, const Action& action);

	};
}
