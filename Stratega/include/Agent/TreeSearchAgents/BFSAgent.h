#pragma once
#include <Agent/Agent.h>
#include <Agent/TreeSearchAgents/TreeNode.h>
#include "Agent/AgentParameters.h"

namespace SGA
{
	struct BFSParameters : public AgentParameters
	{
		bool CONTINUE_PREVIOUS_SEARCH = true;
	};

	class BFSAgent : public Agent
	{
		std::unique_ptr<TreeNode> rootNode = nullptr;
		std::list<TreeNode*> openNodes = std::list<TreeNode*>();
		std::list<TreeNode*> knownLeaves = std::list<TreeNode*>();
		int previousActionIndex = -1;
		
		BFSParameters parameters_;
		
	public:
		void runTBS(TBSGameCommunicator& gameCommunicator, TBSForwardModel forwardModel) override;

	private:
		void search(TBSForwardModel& forwardModel, std::list<TreeNode*>& openNodes);
		int getBestActionIdx(TBSForwardModel& forwardModel);
		void fillOpenNodeListWithLeaves();
		void init(TBSForwardModel& forwardModel, TBSGameState& gameState);
	};
}
