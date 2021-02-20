#include <Stratega/Agent/TreeSearchAgents/DFSAgent.h>


namespace SGA
{
	void DFSAgent::runTBS(TBSGameCommunicator& gameCommunicator, TBSForwardModel forwardModel)
	{
		while (!gameCommunicator.isGameOver())
		{
			if (gameCommunicator.isMyTurn())
			{
				remainingForwardModelCalls = forwardModelCalls;

				auto gameState = gameCommunicator.getGameState();
				if (gameState.isGameOver)
					break;
				auto actionSpace = forwardModel.generateActions(gameState);
				if (actionSpace.size() == 1)
				{
					gameCommunicator.executeAction(actionSpace.at(0));
				}
				else
				{

					double bestHeuristicValue = -std::numeric_limits<double>::max();
					int bestActionIndex = 0;
					const int playerID = gameState.currentPlayer;

					for (size_t i = 0; i < actionSpace.size(); i++)
					{
						auto gsCopy(gameState);
						forwardModel.advanceGameState(gsCopy, actionSpace.at(i));
						const double value = evaluateRollout(forwardModel, gameState, 1, playerID);
						if (value > bestHeuristicValue)
						{
							bestHeuristicValue = value;
							bestActionIndex = i;
						}
						
						if (remainingForwardModelCalls <= 0)
							break;
					}
					//std::cout << "DFSAgent Number of FM calls: " << forwardModelCalls << std::endl;

					gameCommunicator.executeAction(actionSpace.at(bestActionIndex));
				}
			}
		}		
	}

	double DFSAgent::evaluateRollout(TBSForwardModel& forwardModel, TBSGameState& gameState, int depth, const int playerID)
	{
		double bestValue = -std::numeric_limits<double>::max();
		if (depth == maxDepth || gameState.isGameOver)
		{
			return _stateHeuristic.evaluateGameState(forwardModel, gameState, playerID);
		}
		else
		{
			auto actionSpace = forwardModel.generateActions(gameState);
			for (const auto& action : actionSpace)
			{
				auto gsCopy(gameState);
				applyActionToGameState(forwardModel, gameState, action);

				double value = evaluateRollout(forwardModel, gsCopy, depth + 1, playerID);
				if (value > bestValue)
				{
					bestValue = value;
				}
				if (remainingForwardModelCalls <= 0)
					return bestValue;
			}

			return bestValue;
		}
	}

	void DFSAgent::applyActionToGameState(const TBSForwardModel& forwardModel, TBSGameState& gameState, const Action& action)
	{
		remainingForwardModelCalls--;
		const int playerID = gameState.currentPlayer;
		forwardModel.advanceGameState(gameState, action);
		
		while (gameState.currentPlayer != playerID && !gameState.isGameOver)
		{
			if (opponentModel) // use default opponentModel to choose actions until the turn has ended
			{
				auto actionSpace = forwardModel.generateActions(gameState);
				auto opAction = opponentModel->getAction(gameState, actionSpace);
				forwardModel.advanceGameState(gameState, opAction);
			}
			else // skip opponent turn
			{
				forwardModel.advanceGameState(gameState, Action::createEndAction(gameState.currentPlayer));
			}
			remainingForwardModelCalls--;
		}
	}
}
