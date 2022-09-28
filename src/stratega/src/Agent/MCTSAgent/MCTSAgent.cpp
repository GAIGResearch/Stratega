#include <Stratega/Agent/MCTSAgent/MCTSAgent.h>
#include <Stratega/Agent/Heuristic/AimToKingHeuristic.h>
#include <Stratega/Agent/Heuristic/BasicTBSHeuristic.h>
#include <Stratega/Agent/Heuristic/PushThemAllHeuristic.h>

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
        //parameters_.heuristic = std::make_unique<AimToKingHeuristic>(initialState);
        //parameters_.heuristic = std::make_unique<PushThemAllHeuristic>(getPlayerID(), initialState);
        if (parameters_.HEURISTIC == "ktk") {
            parameters_.heuristic = std::make_unique< AimToKingHeuristic >(initialState);
        }
        else if (parameters_.HEURISTIC == "pta") {
            parameters_.heuristic = std::make_unique< PushThemAllHeuristic >(getPlayerID(), initialState);
        }
        if (parameters_.budgetType == Budget::UNDEFINED)
            parameters_.budgetType = Budget::TIME;
        parameters_.opponentModel = std::make_shared<RandomActionScript>();
        if(is_debug)
            parameters_.printDetails();
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
        if (actionSpace.size() == 1)
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
            //std::cout<<"a\n";
            rootNode->searchMCTS(*processedForwardModel, parameters_, getRNGEngine());
            //std::cout<<"b\n";
            // get and store best action
            auto bestActionIndex = rootNode->mostVisitedAction(parameters_, getRNGEngine());
            auto bestAction = rootNode->getActionSpace(forwardModel, getPlayerID()).at(static_cast<size_t>(bestActionIndex));

            // return best action
            previousActionIndex = (bestAction.getActionFlag() == ActionFlag::EndTickAction) ? -1 : bestActionIndex;
            if (bestAction.getActionFlag() == ActionFlag::EndTickAction) {
                parameters_.step++;
            }

            //for (auto a : rootNode->actionSpace) {
            //state.printActionInfo(a);
            //}
            //rootNode->printTree();
            //state.printBoard();
            //double score = parameters_.heuristic->evaluateGameState(forwardModel, state, getPlayerID());
            //std::cout<<"score: "<< score<<"\n";
            return ActionAssignment::fromSingleAction(bestAction);
        }
    }

}