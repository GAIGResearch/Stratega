#pragma once
#include <Stratega/Agent/Agent.h>
#include <Stratega/Agent/TreeSearchAgents/MCTSNode.h>

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


		void init(GameState initialState, const ForwardModel& forwardModel, long timeBudgetMs) override;

		ActionAssignment computeAction(GameState state, const ForwardModel& forwardModel, long timeBudgetMs) override;

	private:
		std::unique_ptr<MCTSNode> rootNode = nullptr;
		int previousActionIndex = -1;
		MCTSParameters parameters_;
		bool continuePreviousSearch = true;
		int playerTurn = -1;
	};
}
