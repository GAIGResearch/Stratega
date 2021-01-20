#include <Agent/OSLAAgent.h>
#include <Agent/Heuristic/MinimizeDistanceHeuristic.h>

namespace SGA
{
	void OSLAAgent::runAbstractTBS(TBSGameCommunicator& gameCommunicator, TBSForwardModel forwardModel)
	{
		while (!gameCommunicator.isGameOver())
		{
			if (gameCommunicator.isMyTurn())
			{
				auto gameState = gameCommunicator.getGameState();
				if (gameState.isGameOver)
					break;
				auto actionSpace = forwardModel.generateActions(gameState);
				MinimizeDistanceHeuristic heuristic;
				double bestHeuristicValue = -std::numeric_limits<double>::max();
				
				int bestActionIndex = 0;
				const int playerID = gameState.currentPlayer;

				for (int i = 0; i < actionSpace.size(); i++)
				{
					auto gsCopy(gameState);
					
					forwardModel.advanceGameState(gsCopy, actionSpace.at(i));
					const double value = heuristic.evaluateGameState(forwardModel, gsCopy, playerID);
					if (value > bestHeuristicValue)
					{
						bestHeuristicValue = value;
						bestActionIndex = i;
					}
				}

				gameCommunicator.executeAction(actionSpace.at(bestActionIndex));
			}
		}
	}
}
