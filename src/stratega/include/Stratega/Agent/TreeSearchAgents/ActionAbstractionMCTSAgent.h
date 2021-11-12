#pragma once
#include <Stratega/Agent/Agent.h>
#include <Stratega/Agent/MCTSAgent/MCTSNode.h>
#include <Stratega/Agent/Heuristic/AbstractHeuristic.h>
#include <Stratega/Agent/ActionScripts/AttackClosestOpponentScript.h>
#include <Stratega/Agent/ActionScripts/AttackWeakestOpponentScript.h>

#include "Stratega/Agent/MCTSAgent/MCTSParameters.h"

namespace SGA
{
	
	class ActionAbstractionMCTSAgent : public Agent
	{
	public:
		explicit ActionAbstractionMCTSAgent(const std::string& name, MCTSParameters&& params)
			: Agent(name),parameters_(std::move(params))
		{
		}


		void init(GameState initialState, const ForwardModel& forwardModel, Timer timer) override;
		ActionAssignment computeAction(GameState state, const ForwardModel& forwardModel, Timer timer) override;

	private:
		std::unique_ptr<MCTSNode> rootNode = nullptr;
		int previousActionIndex = -1;
		MCTSParameters parameters_;
		//error: private field 'continuePreviousSearch' is not used [-Werror,-Wunused-private-field]
		//bool continuePreviousSearch = true;
		//int playerTurn = -1;
	};
}
