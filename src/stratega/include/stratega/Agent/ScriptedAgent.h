#pragma once
#include <stratega/Agent/Agent.h>

#include <stratega/Agent/ActionScripts/AttackClosestOpponentScript.h>
#include <stratega/Agent/ActionScripts/AttackWeakestOpponentScript.h>
#include <stratega/Agent/ActionScripts/RunToFriendlyUnitsScript.h>
#include <stratega/Agent/ActionScripts/RunAwayFromOpponentScript.h>

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
