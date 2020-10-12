#pragma once
#include <Agent/Agent.h>

#include <Agent/TreeSearchAgents/TreeNode.h>
#include <Agent/Heuristic/LinearSumHeuristic.h>

namespace SGA
{
	class BeamSearchAgent : public Agent
	{
	private:
		int playerBeamWith = 20;
		int playerDepth = 5;
		LinearSumHeuristic heuristic = LinearSumHeuristic();

	public:
		void runTBS(TBSGameCommunicator& gameCommunicator, TBSForwardModel forwardModel) override;
		Action<Vector2i> beamSearch(TBSForwardModel& forwardModel, TreeNode& root);
		std::vector<TreeNode*> Simulate(TBSForwardModel& forwardModel, TreeNode& node);
		static bool sortByValue(const TreeNode* i, const TreeNode* j);

	};
}