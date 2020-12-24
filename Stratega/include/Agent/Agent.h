#pragma once
#include <ForwardModel/TBSForwardModel.h>
#include <ForwardModel/RTSForwardModel.h>
#include <ForwardModel/AbstractFM/TBSAbstractForwardModel.h>
#include <ForwardModel/AbstractFM/RTSAbstractForwardModel.h>
#include <Game/TBSGameCommunicator.h>
#include <Game/RTSGameCommunicator.h>
#include <Game/AbstractTBSGameCommunicator.h>
#include <Game/AbstractRTSGameCommunicator.h>
#include <Representation/AbstractGS/TBSGameState2.h>

namespace SGA
{
	class TBSGameCommunicator;
	class RTSGameCommunicator;
	class AbstractTBSGameCommunicator;
	class AbstractRTSGameCommunicator;
	
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
		virtual void runAbstractTBS(AbstractTBSGameCommunicator& gameCommunicator, TBSAbstractForwardModel forwardModel)
		{
			
		}

		virtual void runAbstractRTS(AbstractRTSGameCommunicator& gameCommunicator, RTSAbstractForwardModel forwardModel)
		{

		}
	};
}

