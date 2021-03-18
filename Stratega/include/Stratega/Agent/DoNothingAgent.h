#pragma once
#include <Stratega/Agent/Agent.h>

namespace SGA
{
	class DoNothingAgent : public Agent
	{
	public:
		void runTBS(AgentGameCommunicator& gameCommunicator, TBSForwardModel forwardModel) override;
	};
}