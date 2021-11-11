#include <Stratega/Agent/TreeSearchAgents/DFSAgent.h>


namespace SGA
{
	ActionAssignment DFSAgent::computeAction(GameState state, const ForwardModel& forwardModel, Timer /*timer*/)
	{
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
				applyActionToGameState(forwardModel, gsCopy, actionSpace.at(i), getPlayerID());
				const double value = evaluateRollout(forwardModel, state, 1, getPlayerID());

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


	void DFSAgent::init(GameState /*initialState*/, const ForwardModel& /*forwardModel*/, Timer /*timer*/)
	{
		agentParams.PLAYER_ID = getPlayerID();
	}

	double DFSAgent::evaluateRollout(const ForwardModel& forwardModel, GameState& gameState, int depth, const int newPayerID)
	{
		double bestValue = -std::numeric_limits<double>::max();
		if (depth == maxDepth || gameState.isGameOver() || remainingForwardModelCalls <= 0)
		{
			return _stateHeuristic.evaluateGameState(forwardModel, gameState, newPayerID);
		}
		else
		{
			auto actionSpace = forwardModel.generateActions(gameState, getPlayerID());
			for (const auto& action : actionSpace)
			{
				auto gsCopy(gameState);
				applyActionToGameState(forwardModel, gsCopy, action, newPayerID);

				double value = evaluateRollout(forwardModel, gsCopy, depth + 1, newPayerID);
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

	void DFSAgent::applyActionToGameState(const ForwardModel& forwardModel, GameState& gameState, const Action& action, int newPlayerID)
	{
		//Roll the game state with our action.
		remainingForwardModelCalls -= SGA::roll(gameState, forwardModel, action, newPlayerID, agentParams);

		//Continue rolling the state until the game is over, we run out of budget or this agent can play again. 

		while (!gameState.canPlay(getPlayerID()) && remainingForwardModelCalls > 0 && !gameState.isGameOver())
		{
			//Roll actions for the opponent(s).
			remainingForwardModelCalls -= SGA::rollOppOnly(gameState, forwardModel, agentParams);
		}
	}
}
