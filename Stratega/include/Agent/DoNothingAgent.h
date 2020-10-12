#pragma once
#include <Agent/Agent.h>

namespace SGA
{
	class DoNothingAgent : public Agent
	{
	public:
		void runTBS(TBSGameCommunicator& gameCommunicator, TBSForwardModel ForwardModel) override;
	};
}