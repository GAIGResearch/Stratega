#include <Agent/TreeSearchAgents/DFSAgent.h>


namespace SGA
{
	void DFSAgent::runTBS(TBSGameCommunicator& gameCommunicator, TBSForwardModel forwardModel)
	{
		while (!gameCommunicator.isGameOver())
		{
			if (gameCommunicator.isMyTurn())
			{
				remainingForwardModelCalls = forwardModelCalls;

				TBSGameState gameState = gameCommunicator.getGameState();
				if (gameState.isGameOver)
					break;
				auto actionSpace = forwardModel.getActions(gameState);
				if (actionSpace->size() == 1)
				{
					gameCommunicator.executeAction(actionSpace->at(0));
				}
				else
				{

					double bestHeuristicValue = -std::numeric_limits<double>::max();
					int bestActionIndex = 0;
					const int playerID = gameState.currentPlayer;

					for (size_t i = 0; i < actionSpace->size(); i++)
					{
						TBSGameState gsCopy(gameState);
						forwardModel.advanceGameState(gsCopy, actionSpace->at(i));
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

					gameCommunicator.executeAction(actionSpace->at(bestActionIndex));
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
			auto actionSpace = forwardModel.getActions(gameState);
			for (Action<Vector2i> action : *actionSpace)
			{
				TBSGameState gsCopy(gameState);
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

	void DFSAgent::applyActionToGameState(const TBSForwardModel& forwardModel, TBSGameState& gameState, const Action<Vector2i>& action)
	{
		remainingForwardModelCalls--;
		const int playerID = gameState.currentPlayer;
		forwardModel.advanceGameState(gameState, action);
		
		while (gameState.currentPlayer != playerID && !gameState.isGameOver)
		{
			if (opponentModel) // use default opponentModel to choose actions until the turn has ended
			{
				auto actionSpace = forwardModel.getActions(gameState);
				auto opAction = opponentModel->getAction(gameState, actionSpace);
				forwardModel.advanceGameState(gameState, opAction);
			}
			else // skip opponent turn
			{
				std::vector<SGA::Action<Vector2i>> endTurnActionSpace;
				forwardModel.generateEndOfTurnActions(gameState, gameState.currentPlayer, endTurnActionSpace);
				forwardModel.advanceGameState(gameState, endTurnActionSpace.at(0));
			}
			remainingForwardModelCalls--;
		}
	}
}
