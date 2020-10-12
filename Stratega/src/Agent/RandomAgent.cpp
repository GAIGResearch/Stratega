#include <random>
#include <Agent/RandomAgent.h>

namespace SGA
{
	void RandomAgent::runTBS(TBSGameCommunicator& gameCommunicator, TBSForwardModel ForwardModel)
	{
		while (!gameCommunicator.isGameOver())
		{
			if (gameCommunicator.isMyTurn())
			{
				auto state = gameCommunicator.getGameState();
				std::unique_ptr<ActionSpace<Vector2i>> actionSpace = ForwardModel.getActions(state);
				std::uniform_int_distribution<int> actionDist(0, static_cast<int>(actionSpace->count()) - 1);
				gameCommunicator.executeAction(actionSpace->getAction(actionDist(gameCommunicator.getRNGEngine())));
			}
		}
	}

	void RandomAgent::runRTS(RTSGameCommunicator& gameCommunicator, RTSForwardModel forwardModel)
	{
		auto lastExecution = std::chrono::high_resolution_clock::now();
		while(!gameCommunicator.isGameOver())
		{
			auto now = std::chrono::high_resolution_clock::now();
			std::chrono::duration<double> deltaTime = now - lastExecution;
			if(deltaTime.count() >= 1)
			{
				auto state = gameCommunicator.getGameState();
				auto actions = forwardModel.getActions(state, gameCommunicator.getPlayerID());
				std::uniform_int_distribution<int> actionDist(0, actions->count() - 1);
				int temp=actionDist(gameCommunicator.getRNGEngine());
 				gameCommunicator.executeAction(actions->getAction(actionDist(gameCommunicator.getRNGEngine())));
				lastExecution = std::chrono::high_resolution_clock::now();
			}
		}
	}

}