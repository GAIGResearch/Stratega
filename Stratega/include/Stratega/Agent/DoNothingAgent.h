#pragma once
#include <Stratega/Agent/Agent.h>

namespace SGA
{
	class DoNothingAgent : public Agent
	{
	public:
		void runTBS(TBSGameCommunicator& gameCommunicator, TBSForwardModel forwardModel) override;
	};
}