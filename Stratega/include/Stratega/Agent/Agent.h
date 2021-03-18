#pragma once
#include <Stratega/ForwardModel/TBSForwardModel.h>
#include <Stratega/Game/AgentGameCommunicator.h>

namespace SGA
{
	class AgentGameCommunicator;
	
	class Agent
	{
	public:
		Agent()
		{
			
		}
		virtual ~Agent() = default;
		
		virtual void runTBS(AgentGameCommunicator& gameCommunicator, ForwardModel forwardModel)
		{
			
		}

		virtual void runRTS(AgentGameCommunicator& gameCommunicator, ForwardModel forwardModel)
		{

		}
	};
}

