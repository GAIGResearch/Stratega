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
		std::unique_ptr<TreeNode> rootNode = nullptr;
		std::list<TreeNode*> openNodes = std::list<TreeNode*>();
		std::list<TreeNode*> knownLeaves = std::list<TreeNode*>();
		int previousActionIndex = -1;
		
		BFSParameters parameters_;
		
	public:
		void runTBS(AgentGameCommunicator& gameCommunicator, ForwardModel forwardModel) override;

	private:
		void search(ForwardModel& forwardModel, std::list<TreeNode*>& openNodes);
		int getBestActionIdx(ForwardModel& forwardModel);
		void fillOpenNodeListWithLeaves();
		void init(ForwardModel& forwardModel, GameState& gameState);
	};
}
