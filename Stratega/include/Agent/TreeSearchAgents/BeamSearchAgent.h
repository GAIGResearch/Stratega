#pragma once
#include <Agent/Agent.h>

#include <Agent/TreeSearchAgents/TreeNode.h>
#include <Agent/Heuristic/LinearSumHeuristic.h>

#include "Agent/Portfolios/AttackClosest.h"

namespace SGA
{
	class BeamSearchAgent : public Agent
	{
	private:
		int playerBeamWith = 20;
		int playerDepth = 5;
		LinearSumHeuristic heuristic = LinearSumHeuristic();
		std::unique_ptr<BasePortfolio> opponentModel = std::make_unique<AttackClosest>();	// the portfolio the opponent is simulated with, if set to nullptr the opponent's turn will be skipped

	public:
		void runTBS(TBSGameCommunicator& gameCommunicator, TBSForwardModel forwardModel) override;
		Action<Vector2i> beamSearch(TBSForwardModel& forwardModel, TreeNode& root);
		std::vector<TreeNode*> Simulate(TBSForwardModel& forwardModel, TreeNode& node, const int playerID);
		static bool sortByValue(const TreeNode* i, const TreeNode* j);

	};
}
