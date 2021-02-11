#pragma once
#include <thread>
#include <Stratega/Game/GameCommunicator.h>
#include <Stratega/Agent/Agent.h>

namespace SGA
{
	class Agent;
	class TBSGameCommunicator;
	
	struct AgentPack
	{
		std::unique_ptr<Agent> agent;
		std::unique_ptr<TBSGameCommunicator> gameCommunicator;
		std::thread agentThread;
	};

}
