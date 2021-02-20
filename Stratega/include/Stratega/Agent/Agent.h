#pragma once
#include <Stratega/ForwardModel/TBSForwardModel.h>
#include <Stratega/ForwardModel/RTSForwardModel.h>
#include <Stratega/Game/RTSGameCommunicator.h>
#include <Stratega/Game/TBSGameCommunicator.h>

namespace SGA
{
	class TBSGameCommunicator;
	class RTSGameCommunicator;
	
	class Agent
	{
	public:
		Agent()
		{
			
		}
		virtual ~Agent() = default;
		
		virtual void runTBS(TBSGameCommunicator& gameCommunicator, TBSForwardModel forwardModel)
		{
			
		}

		virtual void runRTS(RTSGameCommunicator& gameCommunicator, RTSForwardModel forwardModel)
		{

		}
	};
}

