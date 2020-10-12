#pragma once
#include <Agent/Agent.h>
#include <Agent/TreeSearchAgents/MCTSNode.h>

namespace SGA
{
	class MCTSAgent : public Agent
	{
	private:
		std::unique_ptr<MCTSNode> rootNode = nullptr;
		int previousActionIndex = -1;
		bool continuePreviousSearch = true;
		int playerTurn = -1;
	public:
		void runTBS(TBSGameCommunicator& gameCommunicator, TBSForwardModel forwardModel) override;
	};
}