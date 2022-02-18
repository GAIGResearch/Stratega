#include <Stratega/Agent/MCTSAgent/MCTSAgent.h>
#include <Stratega/Agent/Heuristic/AimToKingHeuristic.h>


namespace SGA
{

    void MCTSAgent::init(GameState initialState, const ForwardModel& /*forwardModel*/, Timer /*timer*/)
    {
        parameters_.PLAYER_ID = getPlayerID();
        // The heuristic is set here because some heuristics have parameters depending on the game state. e.g. num_units
        /*
            if (parameters_.heuristic == nullptr)
                parameters_.heuristic = std::make_unique<AbstractHeuristic>(initialState);
        */
        parameters_.heuristic = std::make_unique<AimToKingHeuristic>(initialState);
        if (parameters_.budgetType == Budget::UNDEFINED)
            parameters_.budgetType = Budget::TIME;
        parameters_.opponentModel = std::make_shared<RandomActionScript>();
    }


    ActionAssignment MCTSAgent::computeAction(GameState state, const ForwardModel& forwardModel, Timer timer)
    {
        //Initialize the budget for this action call.
        parameters_.resetCounters(timer);
        //parameters_.printDetails();

        // generate actions
        const auto actionSpace = forwardModel.generateActions(state, getPlayerID());


        // if there is just one action and we don't spent the time on continuing our search
        // we just instantly return it
        // todo update condition to an and in case we can compare gameStates, since we currently cannot reuse the tree after an endTurnAction
        if (actionSpace.size() == 1 || !parameters_.continuePreviousSearch)
        {
            rootNode = nullptr;
            previousActionIndex = -1;
            return ActionAssignment::fromSingleAction(actionSpace.at(0));
        }
        else
        {
            const auto processedForwardModel = parameters_.preprocessForwardModel(forwardModel);
            if (parameters_.continuePreviousSearch && previousActionIndex != -1
                && rootNode->children.size() > 0)
            {
                // in case of deterministic games we know which move has been done by us
                // reuse the tree from the previous iteration

                rootNode = std::move(rootNode->children.at(static_cast<size_t>(previousActionIndex)));
                rootNode->parentNode = nullptr;	// release parent
                rootNode->setDepth(0);
            }
            else
            {
                // start a new tree
                rootNode = std::make_unique<MCTSNode>(*processedForwardModel, state, getPlayerID());
            }

            //Do search!
            rootNode->searchMCTS(*processedForwardModel, parameters_, getRNGEngine());
            // get and store best action
            auto bestActionIndex = rootNode->mostVisitedAction(parameters_, getRNGEngine());
            auto bestAction = rootNode->getActionSpace(forwardModel, getPlayerID()).at(static_cast<size_t>(bestActionIndex));

            // return best action
            previousActionIndex = (bestAction.getActionFlag() == ActionFlag::EndTickAction) ? -1 : bestActionIndex;
            if (bestAction.getActionFlag() == ActionFlag::EndTickAction) {
                parameters_.step++;
            }
            return ActionAssignment::fromSingleAction(bestAction);
        }
    }

}