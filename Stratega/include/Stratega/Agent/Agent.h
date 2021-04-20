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

		/// <summary>
		/// Is called once at the start of a TBS game and should run until it is over.
		/// The <see cref="SGA::AgentGameCommunicator"/> can be used to get a copy of the current <see cref="SGA::Gamestate"/> and to execute actions.
		/// Additionally, the agent receives the <see cref="SGA::EntityForwardModel"/> for simulating the game.
		/// Depending on which YAML-file was passed to Stratega, the forward model will behave differently.
		/// </summary>
		virtual void runTBS(AgentGameCommunicator&, TBSForwardModel)
		{
		}

		/// <summary>
		/// Is called once at the start of a RTS game and should run until it is over.
		/// The <see cref="SGA::AgentGameCommunicator"/> can be used to get a copy of the current <see cref="SGA::Gamestate"/> and to execute actions.
		/// Additionally, the agent receives the forward model for simulating the game.
		/// Depending on which YAML-file was passed to Stratega, the <see cref="SGA::EntityForwardModel"/> will behave differently.
		/// </summary>
		virtual void runRTS(AgentGameCommunicator&, RTSForwardModel)
		{
		}
	};
}

