#pragma once
#include <Agent/Agent.h>

namespace SGA
{
	class RandomAgent : public Agent
	{
	public:
		void runTBS(TBSGameCommunicator& gameCommunicator, TBSForwardModel ForwardModel) override;
		void runRTS(RTSGameCommunicator& gameCommunicator, RTSForwardModel forwardModel) override;
		void runAbstractTBS(AbstractTBSGameCommunicator& gameCommunicator, TBSAbstractForwardModel forwardModel) override;
		void runAbstractRTS(AbstractRTSGameCommunicator& gameCommunicator, RTSAbstractForwardModel forwardModel) override;
	};
}
