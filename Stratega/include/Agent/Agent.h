#pragma once
#include <ForwardModel/TBSForwardModel.h>
#include <ForwardModel/RTSForwardModel.h>
#include <ForwardModel/AbstractFM/TBSAbstractForwardModel.h>
#include <ForwardModel/AbstractFM/RTSAbstractForwardModel.h>
#include <Game/TBSGameCommunicator.h>
#include <Game/RTSGameCommunicator.h>
#include <Game/AbstractGameCommunicator.h>
#include <Representation/AbstractGS/TBSGameState2.h>

namespace SGA
{
	class TBSGameCommunicator;
	class RTSGameCommunicator;
	class AbstractGameCommunicator;
	class AbstractRTSGameCommunicator;
	
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
		virtual void runAbstractTBS(AbstractGameCommunicator& gameCommunicator, TBSAbstractForwardModel forwardModel)
		{
			
		}

		virtual void runAbstractRTS(AbstractRTSGameCommunicator& gameCommunicator, RTSAbstractForwardModel forwardModel)
		{

		}
	};
}

