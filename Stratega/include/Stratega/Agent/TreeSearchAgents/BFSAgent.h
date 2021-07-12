#pragma once
#include <Stratega/Agent/Agent.h>
#include <Stratega/Agent/TreeSearchAgents/TreeNode.h>
#include <Stratega/Agent/AgentParameters.h>

namespace SGA
{
	struct BFSParameters : public AgentParameters
	{
		bool CONTINUE_PREVIOUS_SEARCH = true;
	};

	class BFSAgent : public Agent
	{
		using Agent::Agent;

		std::unique_ptr<TreeNode> rootNode = nullptr;
		std::list<TreeNode*> openNodes = std::list<TreeNode*>();
		std::list<TreeNode*> knownLeaves = std::list<TreeNode*>();
		int previousActionIndex = -1;
		
		BFSParameters parameters_;
		
	public:
		ActionAssignment computeAction(GameState state, const ForwardModel* forwardModel, Timer timer) override;

	private:
		void search(TBSForwardModel& forwardModel, std::list<TreeNode*>& nodes);
		int getBestActionIdx(TBSForwardModel& forwardModel);
		void fillOpenNodeListWithLeaves();
		void init(TBSForwardModel& forwardModel, GameState& gameState);
	};
}
