#pragma once
#include <Agent/Agent.h>
#include <Agent/TreeSearchAgents/TreeNode.h>
#include <Agent/Heuristic/SimpleHeuristic.h>
#include <Agent/Heuristic/LinearSumHeuristic.h>
#include <Agent/Heuristic/StateHeuristic.h>

namespace SGA
{
	class BFSAgent : public Agent
	{
	private:
		int forwardModelCalls = 2000;
		bool continuePreviousSearch = true;
		std::list<TreeNode*> openNodes = std::list<TreeNode*>();
		std::unique_ptr<TreeNode> rootNode = nullptr;
		int previousActionIndex = -1;
		int playerTurn = -1;

	public:
		BFSAgent() :
			Agent{ }
		{

		}
		
		void runTBS(TBSGameCommunicator& gameCommunicator, TBSForwardModel forwardModel) override;
		void search(TBSForwardModel& forwardModel, std::list<TreeNode*>& openNodes) const;
		void fillOpenNodeListWithLeaves();
	};
}