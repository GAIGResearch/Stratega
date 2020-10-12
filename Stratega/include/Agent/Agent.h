#pragma once
#include <ForwardModel/TBSForwardModel.h>
#include <Game/TBSGameCommunicator.h>
#include <Game/RTSGameCommunicator.h>

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
		
		virtual void runTBS(TBSGameCommunicator& gameCommunicator, TBSForwardModel forwardModel)=0;
		virtual void runRTS(RTSGameCommunicator& gameCommunicator, RTSForwardModel forwardModel)
		{	
		}
	};
}

