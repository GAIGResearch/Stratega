#pragma once
#include <stratega/Agent/Agent.h>
#include <stratega/Agent/MCTSAgent/MCTSNode.h>
#include <stratega/Agent/Heuristic/AbstractHeuristic.h>
#include <stratega/Agent/ActionScripts/AttackClosestOpponentScript.h>
#include <stratega/Agent/ActionScripts/AttackWeakestOpponentScript.h>

#include "stratega/Agent/MCTSAgent/MCTSParameters.h"

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
		bool continuePreviousSearch = true;
		int playerTurn = -1;
	};
}
