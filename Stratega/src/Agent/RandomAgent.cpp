#include <random>
#include <Agent/RandomAgent.h>

namespace SGA
{
	void RandomAgent::runTBS(TBSGameCommunicator& gameCommunicator, TBSForwardModel forwardModel)
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
				std::uniform_int_distribution<int> actionDist(0, actions.size() - 1);
				auto actionIndex = actionDist(gameCommunicator.getRNGEngine());
				auto action = actions.at(actionIndex);
				// Send action to the game-runner
				gameCommunicator.executeAction(action);
			}
		}
	}

	void RandomAgent::runRTS(RTSGameCommunicator& gameCommunicator, RTSForwardModel forwardModel)
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
				std::uniform_int_distribution<int> actionDist(0, actions.size() - 1);
				int temp = actionDist(gameCommunicator.getRNGEngine());
				gameCommunicator.executeAction(actions.at(actionDist(gameCommunicator.getRNGEngine())));
				lastExecution = std::chrono::high_resolution_clock::now();
			}
		}
	}
}