#pragma once
#include <Agent/Agent.h>
#include <Agent/Heuristic/MinimizeDistanceHeuristic.h>
#include <Agent/ActionScripts/BaseActionScript.h>
#include <Agent/ActionScripts/RandomActionScript.h>

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
		
		void runTBS(TBSGameCommunicator& gameCommunicator, TBSForwardModel forwardModel) override;

		double evaluateRollout(TBSForwardModel& forwardModel, TBSGameState& gameState, int depth, int playerID);
		void applyActionToGameState(const TBSForwardModel& forwardModel, TBSGameState& gameState, const Action& action);

	};
}
