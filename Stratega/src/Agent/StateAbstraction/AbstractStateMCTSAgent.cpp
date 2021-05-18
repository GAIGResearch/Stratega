#include <random>
#include <Stratega/Agent/StateAbstraction/AbstractStateMCTSAgent.h>

namespace SGA
{
	ActionAssignment AbstractStateMCTSAgent::computeAction(GameState state, const EntityForwardModel& forwardModel, long timeBudgetMs)
	{
		const auto processedForwardModel = parameters_.preprocessForwardModel(dynamic_cast<const TBSForwardModel&>(forwardModel));

		// initialize the state factory in case it has not been done yet
		if (parameters_.STATE_FACTORY == nullptr)
		{
			parameters_.STATE_FACTORY = std::make_unique<StateFactory>(state);
		}

		// initialize the state heuristic in case it has not been done yet
		if (parameters_.STATE_HEURISTIC == nullptr) 
		{
			parameters_.STATE_HEURISTIC = std::make_unique<AbstractHeuristic>(state);
		}
				
		parameters_.PLAYER_ID = getPlayerID();

		// if there is just one action and we don't spent the time on continuing our search
		// we just instantly return it
		// todo update condition to an and in case we can compare gameStates, since we currently cannot reuse the tree after an endTurnAction
				
		auto actionSpace = processedForwardModel->generateActions(state);
		if (actionSpace.size() == 1 || !parameters_.CONTINUE_PREVIOUS_SEARCH)
		{
			rootNode = nullptr;
			previousActionIndex = -1;
			return ActionAssignment::fromSingleAction(actionSpace.at(0));
		}
		else
		{
			if (parameters_.CONTINUE_PREVIOUS_SEARCH && previousActionIndex != -1)
			{
				// in case of deterministic games we know which move has been done by us
				// reuse the tree from the previous iteration
				rootNode = std::move(rootNode->children.at(previousActionIndex));
				rootNode->parentNode = nullptr;	// release parent
				rootNode->setDepth(0);
			}
			else
			{
				// start a new tree
				auto abstractState = parameters_.STATE_FACTORY->createAbstractState(state);
				auto gameStateCopy(state);
				rootNode = std::make_unique<AbstractMCTSNode>(*processedForwardModel, abstractState, state);
			}


			//params.printDetails();
			parameters_.REMAINING_FM_CALLS = parameters_.MAX_FM_CALLS;
			rootNode->searchMCTS(*processedForwardModel, parameters_, getRNGEngine());

			//rootNode->printTree();
			if (rootNode->children.size() == 0) {
				auto randDist = std::uniform_int_distribution<int>(0, actionSpace.size() - 1);
				int randomIndex = randDist(getRNGEngine());
				return ActionAssignment::fromSingleAction(actionSpace[randomIndex]);
			}

			// get and store best action
			auto bestActionIndex = rootNode->mostVisitedAction(parameters_, getRNGEngine());
			auto bestAction = rootNode->actionSpace.at(bestActionIndex);

			// return best action
			previousActionIndex = (bestAction.actionTypeFlags == ActionFlag::EndTickAction) ? -1 : bestActionIndex;
			return ActionAssignment::fromSingleAction(bestAction);

		}
	}
}