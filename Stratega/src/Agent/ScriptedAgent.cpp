#include <random>
#include <Stratega/Agent/ScriptedAgent.h>

namespace SGA
{
	void ScriptedAgent::runTBS(AgentGameCommunicator& gameCommunicator, TBSForwardModel forwardModel)
	{
		while (!gameCommunicator.isGameOver())
		{
			if (gameCommunicator.isMyTurn())
			{
				// Fetch state
				auto state = gameCommunicator.getGameState();
				
				// Generate all available actions
				auto actions = forwardModel.generateActions(state, gameCommunicator.getPlayerID());

				// Return the action selected by the script
				gameCommunicator.executeAction(actionScript_->getAction(state, actions));
			}
		}
	}

	void ScriptedAgent::runRTS(AgentGameCommunicator& gameCommunicator, RTSForwardModel forwardModel)
	{
		auto lastExecution = std::chrono::high_resolution_clock::now();
		while (!gameCommunicator.isGameOver())
		{
			auto now = std::chrono::high_resolution_clock::now();
			std::chrono::duration<double> deltaTime = now - lastExecution;
			if (deltaTime.count() >= 1)
			{
				auto state = gameCommunicator.getGameState();
				auto actions = forwardModel.generateActions(state, gameCommunicator.getPlayerID());
				lastExecution = std::chrono::high_resolution_clock::now();
			}
		}
	}
}