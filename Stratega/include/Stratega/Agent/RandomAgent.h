#pragma once
#include <Stratega/Agent/Agent.h>

namespace SGA
{
	class RandomAgent : public Agent
	{
	public:
		void runTBS(AgentGameCommunicator& gameCommunicator, TBSForwardModel forwardModel) override;
		void runRTS(AgentGameCommunicator& gameCommunicator, RTSForwardModel forwardModel) override;
	};
}
