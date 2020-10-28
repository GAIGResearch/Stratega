#pragma once
#include <Agent/Agent.h>
#include <Agent/TreeSearchAgents/MCTSNode.h>

namespace SGA
{
	class MCTSAgent : public Agent
	{
	public:
		explicit MCTSAgent(MCTSParams&& params)
			: params(std::move(params))
		{
		}
		
		void runTBS(TBSGameCommunicator& gameCommunicator, TBSForwardModel forwardModel) override;

	private:
		MCTSParams params;
		std::unique_ptr<MCTSNode> rootNode = nullptr;
		int previousActionIndex = -1;
		bool continuePreviousSearch = true;
		int playerTurn = -1;
	};
}