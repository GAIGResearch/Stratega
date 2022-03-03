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
			double value = heuristic.evaluateGameState(forwardModel, gsCopy, getPlayerID());
			value = noise(value, epsilon, doubleDistribution_(this->getRNGEngine()));
			if (value > bestHeuristicValue)
			{
				bestHeuristicValue = value;
				bestActionIndex = static_cast<int>(i);
			}
		}
		return ActionAssignment::fromSingleAction(actionSpace.at(static_cast<size_t>(bestActionIndex)));
	}

	double OSLAAgent::noise(const double input, const double epsilonValue, const double random)
	{
		return (input + epsilonValue) * (1.0 + epsilonValue * (random - 0.5));
	}
}
