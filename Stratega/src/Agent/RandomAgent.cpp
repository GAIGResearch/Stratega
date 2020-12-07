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
				auto actionSpace = ForwardModel.generateActions(state);
				std::uniform_int_distribution<int> actionDist(0, static_cast<int>(actionSpace.size()) - 1);
				gameCommunicator.executeAction(actionSpace.at(actionDist(gameCommunicator.getRNGEngine())));
			}
		}
	}

	void RandomAgent::runAbstractTBS(AbstractGameCommunicator& gameCommunicator, TBSAbstractForwardModel ForwardModel)
	{
		while (!gameCommunicator.isGameOver())
		{
			if (gameCommunicator.isMyTurn())
			{
				auto state = gameCommunicator.getGameState();
				auto actionSpace = ForwardModel.generateActions(state, gameCommunicator.getPlayerID());
				std::uniform_int_distribution<int> actionDist(0, static_cast<int>(actionSpace.size()) - 1);
				gameCommunicator.executeAction(actionSpace.at(actionDist(gameCommunicator.getRNGEngine())));
			}
		}
	}

	void RandomAgent::runAbstractRTS(AbstractRTSGameCommunicator& gameCommunicator, RTSAbstractForwardModel forwardModel)
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
				auto actions = forwardModel.generateActions(state, gameCommunicator.getPlayerID());
				std::uniform_int_distribution<int> actionDist(0, actions.size() - 1);
				int temp=actionDist(gameCommunicator.getRNGEngine());
 				gameCommunicator.executeAction(actions.at(actionDist(gameCommunicator.getRNGEngine())));
				lastExecution = std::chrono::high_resolution_clock::now();
			}
		}
	}

}