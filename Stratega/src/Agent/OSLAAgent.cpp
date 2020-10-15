#include <Agent/OSLAAgent.h>


namespace SGA
{
	void OSLAAgent::runTBS(TBSGameCommunicator& gameCommunicator, TBSForwardModel forwardModel)
	{
		while (!gameCommunicator.isGameOver())
		{
			if (gameCommunicator.isMyTurn())
			{
				TBSGameState gameState = gameCommunicator.getGameState();
				if (gameState.isGameOver)
					break;
				auto actionSpace = forwardModel.getActions(gameState);
				//SimpleHeuristic heuristic = SimpleHeuristic(playerID, gameState.getPlayerNumber());
				LinearSumHeuristic heuristic = LinearSumHeuristic();
				double bestHeuristicValue = -std::numeric_limits<double>::max();
				
				int bestActionIndex = 0;
				const int playerID = gameState.currentPlayer;

				for (int i = 0; i < actionSpace->count(); i++)
				{
					TBSGameState gsCopy(gameState);
					
					forwardModel.advanceGameState(gsCopy, actionSpace->getAction(i));
					const double value = heuristic.evaluateGameState(forwardModel, gsCopy, playerID);
					if (value > bestHeuristicValue)
					{
						bestHeuristicValue = value;
						bestActionIndex = i;
					}
				}

				gameCommunicator.executeAction(actionSpace->getAction(bestActionIndex));
			}
		}
	}
}
