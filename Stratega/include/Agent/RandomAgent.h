#pragma once
#include <Agent/Agent.h>

namespace SGA
{
	class RandomAgent : public Agent
	{
	public:
		void runAbstractTBS(AbstractTBSGameCommunicator& gameCommunicator, TBSAbstractForwardModel forwardModel) override;
		void runAbstractRTS(AbstractRTSGameCommunicator& gameCommunicator, RTSAbstractForwardModel forwardModel) override;
	};
}
