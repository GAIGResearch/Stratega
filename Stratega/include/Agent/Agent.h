#pragma once
#include <ForwardModel/TBSForwardModel.h>
#include <ForwardModel/RTSForwardModel.h>
#include <Game/RTSGameCommunicator.h>
#include <Game/TBSGameCommunicator.h>

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
		
		virtual void runAbstractTBS(TBSGameCommunicator& gameCommunicator, TBSForwardModel forwardModel)
		{
			
		}

		virtual void runAbstractRTS(RTSGameCommunicator& gameCommunicator, RTSForwardModel forwardModel)
		{

		}
	};
}

