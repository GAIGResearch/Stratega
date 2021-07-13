#pragma once
#include <Stratega/Agent/Agent.h>

#include <Stratega/Agent/ActionScripts/AttackClosestOpponentScript.h>
#include <Stratega/Agent/ActionScripts/AttackWeakestOpponentScript.h>
#include <Stratega/Agent/ActionScripts/RunToFriendlyUnitsScript.h>
#include <Stratega/Agent/ActionScripts/RunAwayFromOpponentScript.h>

namespace SGA
{

	class ScriptedAgent : public Agent
	{
	private:
		std::unique_ptr<BaseActionScript> actionScript_ = std::make_unique<AttackWeakestOpponentScript>();

	public:
		ActionAssignment computeAction(GameState state, const ForwardModel& forwardModel, Timer timer) override;

		//void runTBS(AgentGameCommunicator& gameCommunicator, TBSForwardModel forwardModel) override;
		//void runRTS(AgentGameCommunicator& gameCommunicator, RTSForwardModel forwardModel) override;
	};
}
