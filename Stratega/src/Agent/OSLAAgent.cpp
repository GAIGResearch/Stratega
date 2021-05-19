#include <Stratega/Agent/OSLAAgent.h>
#include <Stratega/Agent/Heuristic/MinimizeDistanceHeuristic.h>


namespace SGA
{
	ActionAssignment OSLAAgent::computeAction(GameState state, const EntityForwardModel& forwardModel, long /*timeBudgetMs*/)
	{
		if(state.gameType != GameType::TBS)
		{
			throw std::runtime_error("OSLAAgent only supports TBS-Games");
		}

		/*for (Entity e : state.entities)
		{
			std::cout << e.position.x << ", " << e.position.y << std::endl;
			//std::shared_ptr<std::unordered_map<int, EntityType>> types = state.gameInfo->entityTypes;
			//auto it = types->find(e.typeID);
			//EntityType& et = it->second;
			//std::cout << et.name << std::endl;

			EntityType et = state.gameInfo->getEntityType(e.typeID);
			std::cout << et.name << std::endl;

			for (auto kv : et.parameters)
			{
				Parameter p = kv.second;
				
				std::cout << p.name << "(" << p.id << "): " << p.defaultValue << std::endl;
			}
			
			std::cout << "Hey" << std::endl;
		}
		*/

		auto actionSpace = forwardModel.generateActions(state, getPlayerID());
		MinimizeDistanceHeuristic heuristic;
		double bestHeuristicValue = -std::numeric_limits<double>::max();

		int bestActionIndex = 0;
		for (int i = 0; i < actionSpace.size(); i++)
		{
			auto gsCopy(state);
			forwardModel.advanceGameState(gsCopy, actionSpace.at(i));
			const double value = heuristic.evaluateGameState(dynamic_cast<const TBSForwardModel&>(forwardModel), gsCopy, getPlayerID());
			if (value > bestHeuristicValue)
			{
				bestHeuristicValue = value;
				bestActionIndex = i;
			}
		}

		return ActionAssignment::fromSingleAction(actionSpace.at(bestActionIndex));
	}
}
