#pragma once
#include <Agent/Agent.h>
#include <Agent/Heuristic/LinearSumHeuristic.h>

namespace SGA
{
	class DFSAgent : public Agent
	{

	public:
		LinearSumHeuristic _stateHeuristic;
		int maxDepth = 3;
		int forwardModelCalls = 2000;
		int remainingForwardModelCalls = forwardModelCalls;
		
		DFSAgent() :
			Agent{}, _stateHeuristic(LinearSumHeuristic())
		{

		}
		void runTBS(TBSGameCommunicator& gameCommunicator, TBSForwardModel forwardModel) override;

		double evaluateRollout(TBSForwardModel& forwardModel, TBSGameState& gsCopy, int depth, const int playerID);
		void applyActionToGameState(const TBSForwardModel& forwardModel, TBSGameState& gameState, const Action<Vector2i>& action);

	};
}
