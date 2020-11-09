#pragma once
#include <Agent/Agent.h>
#include <Agent/TreeSearchAgents/MCTSNode.h>

#include "MCTSParameters.h"

namespace SGA
{
	
	class MCTSAgent : public Agent
	{
	public:
		explicit MCTSAgent(MCTSParameters&& params)
			: params(std::move(params))
		{
		}
		
		void runTBS(TBSGameCommunicator& gameCommunicator, TBSForwardModel forwardModel) override;

	private:
		std::unique_ptr<MCTSNode> rootNode = nullptr;
		int previousActionIndex = -1;
		MCTSParameters parameters_ = MCTSParameters();
		bool continuePreviousSearch = true;
		int playerTurn = -1;
	};
}
