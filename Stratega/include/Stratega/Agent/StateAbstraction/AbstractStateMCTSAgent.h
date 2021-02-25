#pragma once
#include <Stratega/Agent/Agent.h>
#include <Stratega/Agent/StateAbstraction/StateFactory.h>
#include <Stratega/Agent/StateAbstraction/AbstractMCTSNode.h>
#include <Stratega/Agent/StateAbstraction/AbstractMCTSParameters.h>
#include <Stratega/Agent/Heuristic/AbstractHeuristic.h>


namespace SGA
{
	class AbstractStateMCTSAgent : public Agent
	{
	private:
		std::unique_ptr<AbstractMCTSNode> rootNode = nullptr;
		int previousActionIndex = -1;
		AbstractMCTSParameters parameters_;
		bool continuePreviousSearch = true;
		int playerTurn = -1;
	
	public:
		void runTBS(TBSGameCommunicator& gameCommunicator, TBSForwardModel forwardModel) override;
		void runRTS(RTSGameCommunicator& gameCommunicator, RTSForwardModel forwardModel) override;
	};
}
