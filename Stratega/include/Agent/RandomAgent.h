#pragma once
#include <Agent/Agent.h>

namespace SGA
{
	class RandomAgent : public Agent
	{
	public:
		void runAbstractTBS(TBSGameCommunicator& gameCommunicator, TBSForwardModel forwardModel) override;
		void runAbstractRTS(RTSGameCommunicator& gameCommunicator, RTSForwardModel forwardModel) override;
	};
}
