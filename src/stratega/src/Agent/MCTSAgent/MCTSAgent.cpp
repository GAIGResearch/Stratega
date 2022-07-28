#include <Stratega/Agent/MCTSAgent/MCTSAgent.h>
#include <Stratega/Agent/Heuristic/AimToKingHeuristic.h>
#include <Stratega/Agent/Heuristic/BasicTBSHeuristic.h>
#include <Stratega/Agent/Heuristic/BasicTBSCombatHeuristic.h>

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
        //parameters_.heuristic = std::make_unique<BasicTBSHeuristic>(parameters_.PLAYER_ID, initialState);

        //parameters_.heuristic = std::make_unique<BasicTBSResourceHeuristic>(parameters_.PLAYER_ID, initialState);
        parameters_.heuristic = std::make_unique<BasicTBSTechnologyHeuristic>(parameters_.PLAYER_ID, initialState);
        //parameters_.heuristic = std::make_unique<BasicTBSCombatHeuristic>(parameters_.PLAYER_ID, initialState);

        // this one is used to get the score of states for debugging;
        // there is some printing function called inside this function, that's why this extra instance is needed
        //debug_heuristic = std::make_unique<BasicTBSResourceHeuristic>(parameters_.PLAYER_ID, initialState);
        //debug_heuristic = std::make_unique<BasicTBSTechnologyHeuristic>(parameters_.PLAYER_ID, initialState);
        //debug_heuristic = std::make_unique<BasicTBSCombatHeuristic>(parameters_.PLAYER_ID, initialState);

        if (parameters_.budgetType == Budget::UNDEFINED)
            parameters_.budgetType = Budget::TIME;
        parameters_.opponentModel = std::make_shared<RandomActionScript>();

        parameters_.printDetails();
    }


    ActionAssignment MCTSAgent::computeAction(GameState state, const ForwardModel& forwardModel, Timer timer)
    {
        bool DEBUG=false;
        if (DEBUG) {
            std::cout<<"DEBUG start MCTSAgent" <<"\n";
        }
        //Initialize the budget for this action call.
        parameters_.resetCounters(timer);

        // generate actions
        const auto actionSpace = forwardModel.generateActions(state, getPlayerID());

        // if there is just one action and we don't spent the time on continuing our search
        // we just instantly return it
        // todo update condition to an and in case we can compare gameStates, since we currently cannot reuse the tree after an endTurnAction
        if (actionSpace.size() == 1)
        {
            if (DEBUG) {
                std::cout<<"The size of action space is 1" <<"\n";
            }

            rootNode = nullptr;
            previousActionIndex = -1;
            //std::cout<<"Action space size: " << actionSpace.size() <<"  " << std::endl;

            if (actionSpace.at(0).getActionFlag() == ActionFlag::EndTickAction) {
                //state.printBoard();
                //double tmp_r = debug_heuristic->evaluateGameState(forwardModel, state, getPlayerID(), true);
                //std::cout<<"Turn Number: "<< parameters_.step << ", r: "<< tmp_r << "\n";
                parameters_.step++;
            }
            //std::cout<<"ends pint 1 MCTS" <<std::endl;
            return ActionAssignment::fromSingleAction(actionSpace.at(0));
        }
        else
        {
            const auto processedForwardModel = parameters_.preprocessForwardModel(forwardModel);
            if (parameters_.continuePreviousSearch && previousActionIndex != -1
                && rootNode->children.size() > 0)
            {
                if (DEBUG) {
                    std::cout<<"Use the tree from last step" <<"\n";
                }
                // in deterministic games we reuse the tree by root on one level 1 node
                rootNode = std::move(rootNode->children.at(static_cast<size_t>(previousActionIndex)));
                rootNode->parentNode = nullptr;	// release parent
                rootNode->setDepth(0);
            }
            else
            {
                // start a new tree
                rootNode = std::make_unique<MCTSNode>(*processedForwardModel, state, getPlayerID());

            }
            if (DEBUG) {
                std::cout<<"Start Search" <<"\n";
            }

            rootNode->searchMCTS(*processedForwardModel, parameters_, getRNGEngine());

            if (DEBUG) {
                std::cout<<"Ends Search" <<"\n";
            }

            // get and store best action
            auto bestActionIndex = rootNode->mostVisitedAction(parameters_, getRNGEngine());
            auto bestAction = rootNode->getActionSpace(forwardModel, getPlayerID()).at(static_cast<size_t>(bestActionIndex));

            // return best action
            previousActionIndex = (bestAction.getActionFlag() == ActionFlag::EndTickAction) ? -1 : bestActionIndex;
            if (bestAction.getActionFlag() == ActionFlag::EndTickAction) {

                //state.printBoard();
                //double tmp_r = debug_heuristic->evaluateGameState(forwardModel, state, getPlayerID(), true);
                //std::cout<<"mcts cp2, Turn Number: "<< parameters_.step << ", r: "<< tmp_r << std::endl;
                parameters_.step++;
            }

            //rootNode->printTree();
            //state.printActionInfo(bestAction);
            //std::cout<<"End MCTS" << std::endl;
            return ActionAssignment::fromSingleAction(bestAction);
        }
    }

}