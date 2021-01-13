#pragma once
#include <Agent/Agent.h>

namespace SGA
{
	class DoNothingAgent : public Agent
	{
	public:
		void runAbstractTBS(TBSGameCommunicator& gameCommunicator, TBSForwardModel forwardModel) override;
	};
}