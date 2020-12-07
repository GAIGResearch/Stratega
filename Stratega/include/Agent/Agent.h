#pragma once
#include <ForwardModel/TBSForwardModel.h>
#include <ForwardModel/RTSForwardModel.h>
#include <ForwardModel/AbstractFM/TBSAbstractForwardModel.h>
#include <Game/TBSGameCommunicator.h>
#include <Game/RTSGameCommunicator.h>
#include <Game/AbstractGameCommunicator.h>
#include <Representation/AbstractGS/TBSGameState2.h>

namespace SGA
{
	class TBSGameCommunicator;
	class RTSGameCommunicator;
	class AbstractGameCommunicator;
	
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
		virtual void runAbstract(AbstractGameCommunicator& gameCommunicator, TBSAbstractForwardModel forwardModel)
		{
			
		}
	};
}

