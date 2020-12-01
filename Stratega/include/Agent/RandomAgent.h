#pragma once
#include <Agent/Agent.h>

namespace SGA
{
	class RandomAgent : public Agent
	{
	public:
		void runTBS(TBSGameCommunicator& gameCommunicator, TBSForwardModel ForwardModel) override;
		void runRTS(RTSGameCommunicator& gameCommunicator, RTSForwardModel forwardModel) override;
		void runAbstract(AbstractGameCommunicator& gameCommunicator, ForwardModel<TBSGameState2> forwardModel) override;
	};
}
