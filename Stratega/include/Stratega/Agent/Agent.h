#pragma once
#include <Stratega/ForwardModel/TBSForwardModel.h>
#include <Stratega/ForwardModel/RTSForwardModel.h>
#include <Stratega/Game/AgentGameCommunicator.h>

namespace SGA
{
	class AgentGameCommunicator;
	
	class Agent
	{
	public:
		Agent() = default;
		virtual ~Agent() = default;
		
		virtual void runTBS(AgentGameCommunicator&, TBSForwardModel)
		{
		}

		virtual void runRTS(AgentGameCommunicator&, RTSForwardModel)
		{
		}
	};
}

