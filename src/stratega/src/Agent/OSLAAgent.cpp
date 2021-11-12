#include <Stratega/Agent/OSLAAgent.h>
#include <Stratega/Agent/Heuristic/MinimizeDistanceHeuristic.h>


namespace SGA
{
	ActionAssignment OSLAAgent::computeAction(GameState state, const ForwardModel& forwardModel, Timer /*timer*/)
	{
		std::vector<Action> actionSpace = forwardModel.generateActions(state, getPlayerID());
		MinimizeDistanceHeuristic heuristic;
		double bestHeuristicValue = -std::numeric_limits<double>::max();

		int bestActionIndex = 0;
		for (size_t i = 0; i < actionSpace.size(); i++)
		{
			GameState gsCopy(state);
			forwardModel.advanceGameState(gsCopy, actionSpace.at(i));
			const double value = heuristic.evaluateGameState(forwardModel, gsCopy, getPlayerID());
			if (value > bestHeuristicValue)
			{
				bestHeuristicValue = value;
				bestActionIndex = static_cast<int>(i);
			}
		}

		return ActionAssignment::fromSingleAction(actionSpace.at(static_cast<size_t>(bestActionIndex)));
	}
}
