#include <random>
#include <Stratega/Agent/RandomAgent.h>

namespace SGA
{
	void RandomAgent::runTBS(AgentGameCommunicator& gameCommunicator, TBSForwardModel forwardModel)
	{
		while (!gameCommunicator.isGameOver())
		{
			if (gameCommunicator.isMyTurn())
			{
				// Fetch state
				auto state = gameCommunicator.getGameState();
				// Generate all available actions
				auto actions = forwardModel.generateActions(state, gameCommunicator.getPlayerID());
				// Uniformly sample a action
				std::uniform_int_distribution<size_t> actionDist(0, actions.size() - 1);
				auto actionIndex = actionDist(gameCommunicator.getRNGEngine());
				auto action = actions.at(actionIndex);
				std::cout << "Random " << actionIndex << std::endl;
				// Send action to the game-runner
				gameCommunicator.executeAction(action);
			}
		}
	}

	void RandomAgent::runRTS(AgentGameCommunicator& gameCommunicator, RTSForwardModel forwardModel)
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
				//std::uniform_int_distribution<int> actionDist(0, actions.size() - 1);
				/*gameCommunicator.executeAction(actions.at(actionDist(gameCommunicator.getRNGEngine())));*/
				lastExecution = std::chrono::high_resolution_clock::now();
			}
		}
	}
}