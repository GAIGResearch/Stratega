#include <Stratega/Agent/TreeSearchAgents/ActionAbstractionMCTSAgent.h>

namespace SGA
{
    ActionAssignment ActionAbstractionMCTSAgent::computeAction(GameState state, const ForwardModel& forwardModel, Timer timer)
    {
        // generate actions
        const auto actionSpace = forwardModel.generateActions(state, getPlayerID());

        // if there is just one action and we don't spent the time on continuing our search
        // we just instantly return it
        // todo update condition to an and in case we can compare gameStates, since we currently cannot reuse the tree after an endTurnAction
        if (actionSpace.size() == 1)
        {
            rootNode = nullptr;
            previousActionIndex = -1;
            return ActionAssignment::fromSingleAction(actionSpace.at(0));
        }
        else
        {
            const auto processedForwardModel = parameters_.preprocessForwardModel(forwardModel);
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
                rootNode = std::make_unique<MCTSNode>(*processedForwardModel, state, getPlayerID());
            }

            //params.printDetails();
            parameters_.REMAINING_FM_CALLS = parameters_.MAX_FM_CALLS;
            rootNode->searchMCTS(*processedForwardModel, parameters_, getRNGEngine());
            //rootNode->printTree();

            // get and store best action
            auto bestActionIndex = rootNode->mostVisitedAction(parameters_, getRNGEngine());
            auto bestAction = rootNode->getActionSpace(forwardModel, getPlayerID()).at(bestActionIndex);

            // return best action
            previousActionIndex = (bestAction.getActionFlag() == ActionFlag::EndTickAction) ? -1 : bestActionIndex;
            return ActionAssignment::fromSingleAction(bestAction);
        }
    }

    void ActionAbstractionMCTSAgent::init(GameState initialState, const ForwardModel& forwardModel, Timer timer)
    {
        parameters_.PLAYER_ID = getPlayerID();
        if (parameters_.heuristic == nullptr)
        {
            parameters_.heuristic = std::make_unique<AbstractHeuristic>(initialState);
        }

    }

}