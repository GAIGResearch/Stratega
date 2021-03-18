#pragma once
#include <Stratega/Agent/Agent.h>

namespace SGA
{
	class RandomAgent : public Agent
	{
	public:
		void runTBS(AgentGameCommunicator& gameCommunicator, ForwardModel forwardModel) override;
		void runRTS(AgentGameCommunicator& gameCommunicator, ForwardModel forwardModel) override;
	};
}
