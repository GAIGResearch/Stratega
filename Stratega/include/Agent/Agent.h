#pragma once
#include <ForwardModel/TBSAbstractForwardModel.h>
#include <ForwardModel/RTSAbstractForwardModel.h>
#include <Game/AbstractRTSGameCommunicator.h>
#include <Game/AbstractTBSGameCommunicator.h>

namespace SGA
{
	class AbstractTBSGameCommunicator;
	class AbstractRTSGameCommunicator;
	
	class Agent
	{
	public:
		Agent()
		{
			
		}
		virtual ~Agent() = default;
		
		virtual void runAbstractTBS(AbstractTBSGameCommunicator& gameCommunicator, TBSAbstractForwardModel forwardModel)
		{
			
		}

		virtual void runAbstractRTS(AbstractRTSGameCommunicator& gameCommunicator, RTSAbstractForwardModel forwardModel)
		{

		}
	};
}

