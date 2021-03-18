#pragma once
#include <Stratega/Agent/Agent.h>

namespace SGA
{
	class OSLAAgent : public Agent
	{
	public:
		void runTBS(AgentGameCommunicator& gameCommunicator, TBSForwardModel forwardModel) override;
		
	private:

	};
}