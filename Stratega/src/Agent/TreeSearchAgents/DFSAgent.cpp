#include <Stratega/Agent/TreeSearchAgents/DFSAgent.h>


namespace SGA
{
	ActionAssignment DFSAgent::computeAction(GameState state, const EntityForwardModel& forwardModel, long timeBudgetMs)
	{
		if (state.gameType != GameType::TBS)
		{
			throw std::runtime_error("DFSAgent only supports TBS-Games");
		}

		remainingForwardModelCalls = forwardModelCalls;
		auto actionSpace = forwardModel.generateActions(state, getPlayerID());
		if (actionSpace.size() == 1)
		{
			return ActionAssignment::fromSingleAction(actionSpace.front());
		}
		else
		{

			auto bestHeuristicValue = -std::numeric_limits<double>::max();
			size_t bestActionIndex = 0;
			for (size_t i = 0; i < actionSpace.size(); i++)
			{
				auto gsCopy(state);
				forwardModel.advanceGameState(gsCopy, actionSpace.at(i));
				const double value = evaluateRollout(dynamic_cast<const TBSForwardModel&>(forwardModel), state, 1, getPlayerID());
				if (value > bestHeuristicValue)
				{
					bestHeuristicValue = value;
					bestActionIndex = i;
				}

				if (remainingForwardModelCalls <= 0)
					break;
			}
			//std::cout << "DFSAgent Number of FM calls: " << forwardModelCalls << std::endl;

			return ActionAssignment::fromSingleAction(actionSpace.at(bestActionIndex));
		}
	}

	double DFSAgent::evaluateRollout(const TBSForwardModel& forwardModel, GameState& gameState, int depth, const int playerID)
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

	void DFSAgent::applyActionToGameState(const TBSForwardModel& forwardModel, GameState& gameState, const Action& action)
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
