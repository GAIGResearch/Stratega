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
			: parameters_(std::move(params))
		{
		}
		
		void runAbstractTBS(AbstractTBSGameCommunicator& gameCommunicator, TBSAbstractForwardModel forwardModel) override;

	private:
		std::unique_ptr<MCTSNode> rootNode = nullptr;
		int previousActionIndex = -1;
		MCTSParameters parameters_;
		bool continuePreviousSearch = true;
		int playerTurn = -1;
	};
}
