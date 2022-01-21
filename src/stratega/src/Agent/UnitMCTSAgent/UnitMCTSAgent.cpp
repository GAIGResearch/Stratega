#include <Stratega/Agent/TreeSearchAgents/Transition.h>
#include <Stratega/Agent/UnitMCTSAgent/UnitMCTSAgent.h>
#include <cassert>
#include <numeric>

namespace SGA {

    ActionAssignment UnitMCTSAgent::computeAction(GameState state, const ForwardModel& forwardModel, long timeBudgetMs)
    {
       if(newRound) {
          // std::cout << "----> start round ----->"<< std::endl; // state.printBoard();
          newRound = false;
       }
       parameters_
          .global_nodeID = 0;  // reinitialize the ID for node, witch is incremental as nodes created
       auto units = state.getPlayerEntities(getPlayerID());  // std::cout << "*** -2" << std::endl;

       /* initialize the order of unit moving, call at the first step store the entities*/
       if(unitIndexInitialized == false) {
          for(auto unit : units) {
             unitIndex.push_back(unit->id);
             // state.printEntityInfo(unit->id);
          }
          unitIndexInitialized = true;
       }

       std::map< int, int > eIDtoUnitArrayIndex =
          {};  // each step, the unit array's index would change because the unit dies

       int tmp_counter1 = 0;
       for(auto u : units) {
          eIDtoUnitArrayIndex.insert(std::pair< int, int >(u->id, tmp_counter1));
          tmp_counter1 += 1;
       }

       // to decide if we need to advance unit for this step. if current unit dies, or have no valid
       // action, move to the next unit
       bool needNextUnit = false;

       std::vector< Action > actionSpace_tmp = {};

       // Condition to move on: 1. actionSpace is empty, 2. dead
       bool unit_alive = eIDtoUnitArrayIndex.count(unitIndex[unitThisStep]);

       if(! unit_alive) {
          needNextUnit = true;
       } else {
          auto e = units[eIDtoUnitArrayIndex[unitIndex[unitThisStep]]];
          auto actionSpace_tmp = forwardModel.generateUnitActions(state, *e, getPlayerID(), false);

          if(actionSpace_tmp.size() == 0) {
             needNextUnit = true;
          } else {
             // std::cout << "[DEBUG57]: actionSpace size " << actionSpace_tmp.size() << std::endl;
          }
       }

       // std::cout << "*** -1" << std::endl;

       // if need to roll, decide which is the next.
       if(needNextUnit) {
          int tmp_unitNextStep = unitThisStep + 1;
          for(; tmp_unitNextStep < unitIndex.size(); tmp_unitNextStep++) {
             if(eIDtoUnitArrayIndex.count(unitIndex[tmp_unitNextStep]))  // if alive
             {
                break;
             }
          }

          // execute EndTurn if there is no valid next unit.
          if(tmp_unitNextStep == unitIndex.size()) {  // every unit moved, end the turn

             step++;  // for debugging

             // std::cout << "[UnitMCTS]: execute EndAction because all units moved" << std::endl;
             Action endAction = Action::createEndAction(getPlayerID());
             unitThisStep = 0;
             unitNextStep = 1;
             newRound = true;

             // state.printBoard();
             return ActionAssignment::fromSingleAction(endAction);
          }

          unitThisStep = tmp_unitNextStep;
       }

       // std::cout << "[UnitMCTS]: unitIndex this step " << unitIndex[unitThisStep] << std::endl;
       // //std::cout << "*** 0" << std::endl;

       parameters_.REMAINING_FM_CALLS = parameters_.MAX_FM_CALLS;

       if(false && parameters_.DO_STATE_ABSTRACTION) {
          std::cout << " ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^ HomoMCTS Starts: " << step
                    << " ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^ " << std::endl;
       } else if(false) {
          std::cout << " ------------------------------ UnitMCTS Starts: " << step
                    << " ------------------------------ " << std::endl;
       }

       if(state.gameType != GameType::TBS) {
          throw std::runtime_error("MCTSAgent only supports TBS-Games");
       }

       // initialize heuristic
       const auto processedForwardModel = parameters_.preprocessForwardModel(
          dynamic_cast< const TBSForwardModel& >(forwardModel));
       if(parameters_.STATE_HEURISTIC == nullptr) {
          parameters_.STATE_HEURISTIC = std::make_unique< AbstractHeuristic >(state);
       }  // std::cout << "*** 1" << std::endl;

       parameters_.maxDepth = 0;  // for debugging

       // generate actions and update unitThisStep
       // const auto actionSpace = forwardModel.generateActions(state, getPlayerID());
       std::vector< Action > actionSpace = forwardModel.generateUnitActions(
          state, *units[eIDtoUnitArrayIndex[unitIndex[unitThisStep]]], getPlayerID(), false);

       // std::cout << "ActionSpace length: " << actionSpace.size() << std::endl;
       // if (actionSpace.size() <= 1) {
       //    std::cout << "CheckCOndition: !(actionSpace.size() <= 1)" << std::endl;
       //}

       // std::cout << "[DEBUG13]: actionSpace size " << actionSpace.size() << std::endl;

       parameters_.PLAYER_ID = getPlayerID();  // used for MCTSNode.

       // if there is just one action and we don't spent the time on continuing our search
       // we just instantly return it
       // todo update condition to an and in case we can compare gameStates, since we currently cannot
       // reuse the tree after an endTurnAction
       // std::cout << actionSpace.size() << std::endl;
       if(actionSpace.size() == 1) {
          // std::cout << "[UnitMCTS]: actionSpace.size() == 1 " << unitIndex[unitThisStep] <<
          // std::endl;
          rootNode = nullptr;
          previousActionIndex = -1;
          step++;
          newRound = true;

          // state.printBoard();
          // state.printActionInfo(actionSpace.at(0));
          return ActionAssignment::fromSingleAction(actionSpace.at(0));
       } else {
          if(parameters_.CONTINUE_PREVIOUS_SEARCH && previousActionIndex != -1) {
             // in case of deterministic games we know which move has been done by us
             // reuse the tree from the previous iteration
             // rootNode = std::move(rootNode->children.at(previousActionIndex));
             int bestChildID = -1;
             for(int i = 0; i < rootNode->children.size(); i++) {
                if(rootNode->children[i]->childIndex == previousActionIndex) {
                   bestChildID = i;
                }
             }

             rootNode = std::move(rootNode->children.at(bestChildID));

             assert(rootNode != nullptr);
             rootNode->parentNode = nullptr;  // release parent
             rootNode->setDepth(0);

          } else  // start a new tree
          {
             rootNode = std::make_unique< UnitMCTSNode >(
                *processedForwardModel, state, unitIndex, unitThisStep, getPlayerID(), 0);

          }  // params.printDetails();  //std::cout << "*** 3" << std::endl;

          // [Homomorphism] do batches
          int tmp_batch_used = 0;
          int tmp_index = 1000;
          if(! parameters_.DO_STATE_ABSTRACTION) {
             rootNode->searchMCTS(
                *processedForwardModel,
                parameters_,
                getRNGEngine(),
                &depthToNodes,
                &absNodeToStatistics);
          }

          // if (parameters_.DO_STATE_ABSTRACTION) {
          //    std::cout << "parameters_.REMAINING_FM_CALLS: "<<  parameters_.REMAINING_FM_CALLS <<
          //    std::endl;
          //}

          int n_abs_iteration = 0;
          bool stop_abstraction = false;
          while(parameters_.DO_STATE_ABSTRACTION) {
             if(parameters_.REMAINING_FM_CALLS <= 0)
                break;

             // std::cout << "[BEFORE SEARCH]: " << parameters_.REMAINING_FM_CALLS <<std::endl;
             rootNode->searchMCTS(
                *processedForwardModel,
                parameters_,
                getRNGEngine(),
                &depthToNodes,
                &absNodeToStatistics);

             if(! stop_abstraction && tmp_batch_used % 2 == 0) {
                deleteAbstraction();  // initialize the array empty again,
                rootNode->eliminateAbstraction();  // make the flag of (has been abstracted) to false
             }

             // do abstraction
             for(int i = parameters_.maxDepth - 1; (! stop_abstraction) && i > 0; i--)  // bottom-up
             {
                std::vector< UnitMCTSNode* > deep_layer = depthToNodes[i];
                for(auto node1 : deep_layer) {  // each initial node
                   if(node1->isAbstracted)
                      continue;  // can be adjusted
                   if(absNodes[i].size() == 0) {  // this depth has no node cluster
                      absNodes[i].push_back(std::vector< UnitMCTSNode* >{node1});

                      // absNodeToStatistics, absNodeHASH -> absNode.value, absNode.visitingCount
                      absNodeToStatistics.insert(std::pair< int, std::vector< double > >(
                         i * tmp_index + absNodes[i].size() - 1,
                         std::vector< double >{node1->value, float(node1->nVisits)}));
                      node1->isAbstracted = true;
                      node1->absNodeID = i * 1000 + absNodes[i].size() - 1;
                      continue;
                   }

                   bool foundExistGroup = false;
                   for(int j = 0; j < absNodes[i].size(); j++)  // each abstract nodes: nodes cluster
                   {
                      bool match = true;
                      for(int k = 0; k < absNodes[i][j].size();
                          k++) {  // compare between new initial nodes to the abstracted Nodes
                         if(! isTwoNodeApproxmateHomomorphism(
                               node1,
                               absNodes[i][j][k],
                               parameters_.R_THRESHOLD,
                               parameters_.T_THRESHOLD)) {
                            match = false;
                         }
                      }
                      if(match) {
                         node1->isAbstracted = true;
                         node1->absNodeID = i * tmp_index + j;
                         absNodes[i][j].push_back(node1);  // add into existing group
                         treeNodetoAbsNode.insert(std::pair< int, int >(node1->nodeID, i * 1000 + j));

                         foundExistGroup = true;
                      }
                   }
                   if(! foundExistGroup) {
                      absNodes[i].push_back(std::vector< UnitMCTSNode* >{node1});
                      absNodeToStatistics.insert(std::pair< int, std::vector< double > >(
                         i * tmp_index + absNodes[i].size() - 1,
                         std::vector< double >{node1->value, float(node1->nVisits)}));
                      // absNodeToStatistics[i * 1000 + absNodes[i].size() - 1] = std::vector<double>{
                      // node1->value, double(node1->nVisits) };
                      node1->isAbstracted = true;
                      node1->absNodeID = i * 1000 + absNodes[i].size() - 1;
                      // std::cout << " create absNode, ID: " << i * tmp_index + absNodes[i].size() - 1
                      // << std::endl;
                   }
                   node1->isAbstracted = true;
                }
                // std::cout << " Depth: " << i << " Initial Number of Nodes: " << deep_layer.size() <<
                // " abstracted node number " << absNodes[i].size() << std::endl;
             }

             n_abs_iteration++;
             tmp_batch_used++;

             // tmp_batch_used >=20 means do maximum 20 times abstraction in a step
             if(parameters_.REMAINING_FM_CALLS <= 0
                || rootNode->n_search_iteration >= parameters_.MAX_FM_CALLS) {
                rootNode->eliminateAbstraction();
                // deleteAbstraction();
                break;
             }

             if(tmp_batch_used >= 0) {
                stop_abstraction = true;
                deleteAbstraction();  // initialize the array empty again,
                rootNode->eliminateAbstraction();  // make the flag of (has been abstracted) to false
             }
          }
          // if (parameters_.DO_STATE_ABSTRACTION) {
          //    std::cout << "[DEBUG 276]: " << tmp_batch_used << "  " << parameters_.REMAINING_FM_CALLS
          //    << std::endl;
          //}

          auto bestActionIndex = rootNode->mostVisitedAction(
             parameters_, getRNGEngine());  // get and store best action

          /*if (bestActionIndex == actionSpace.size()-1) { // this action is an endTurn, reinitialize
              //unitThisStep = 0;
              unitNextStep = 0;
          }*/

          auto bestAction = rootNode->actionSpace.at(bestActionIndex);

          // calculate the branching factor
          std::vector< int > branching_number = {};
          int n_node = 0;
          double mean_braching_factor = 0.0;

          ///*

          rootNode->get_branching_number(&branching_number, &n_node);

          if(branching_number.size() != 0) {
             mean_braching_factor = std::accumulate(
                                       branching_number.begin(), branching_number.end(), 0.0)
                                    / branching_number.size();
          }
          //*/

          if(false && step % 20 == 0) {
             if(parameters_.DO_STATE_ABSTRACTION)
                std::cout << " ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^ HomoMCTS Branching Factor " << step << " "
                          << n_node << " " << mean_braching_factor << " " << parameters_.maxDepth
                          << " ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^ " << std::endl;
             else
                std::cout << " ------------------------------ UnitMCTS Branching Factor " << step << " "
                          << n_node << " " << mean_braching_factor << " " << parameters_.maxDepth
                          << " ------------------------------ " << std::endl;
          }

          if(bestAction.actionTypeFlags == ActionFlag::EndTickAction)
             step++;

          previousActionIndex = (bestAction.actionTypeFlags == ActionFlag::EndTickAction)
                                   ? -1
                                   : bestActionIndex;  // std::cout << "*** 9" << std::endl;

          if(n_node == 1)
             previousActionIndex = -1;

          // std::cout << "-> UnitMCTS Action Took: ";
          // state.printActionInfo(bestAction);
          // std::cout << unitActionHash(bestAction) << std::endl;
          // std::cout << " End printActionInfo " << std::endl;
          // rootNode->printTree();

          /*for (int i = 0; i < 10; i++) { // depth

              std::cout << absNodes[i].size() << " : ";

              for (int j = 0; j < absNodes[i].size(); j++) { // abs node
                  if (j > 0) {
                      std::cout << "    ";
                  }
                  std::cout << absNodes[i][j].size() << " -> " << std::endl;

                  for (int k = 0; k < absNodes[i][j].size(); k++) // original tree nodes
                  {
                      std::cout << " " << absNodes[i][j][k]->value / absNodes[i][j][k]->nVisits << " , "
          << absNodes[i][j][k]->nVisits <<";";
                  }
                  std::cout << "[END]" << std::endl;
              }
              std::cout << std::endl;
          }
          std::cout << " [DEBUG382]: parameters_.REMAINING_FM_CALLS: " << parameters_.REMAINING_FM_CALLS
          << std::endl;
          */

          if(bestAction.actionTypeFlags == ActionFlag::EndTickAction) {
             // std::cout << "----> ends round ----->" << std::endl;
             // state.printBoard();
             newRound = true;
          }

          stepInit();  // reinitialize homomorphism
          return ActionAssignment::fromSingleAction(bestAction);
       }
    }

    // actual reward, what if using the approximate Q? combination of heuristic score [stage1]
    bool UnitMCTSAgent::isTwoNodeApproxmateHomomorphism(UnitMCTSNode* node1, UnitMCTSNode* node2, double reward_threshold, double transition_threshold)
    {
       auto actionSpace1 = node1->actionSpace;
       auto actionSpace2 = node2->actionSpace;
       double max_reward_difference = 0.0;
       std::map< int, int > commonActions = std::map< int, int >();
       std::vector< int > commonActionsVector = std::vector< int >();

       // reward checking
       for(int i = 0; i < node1->actionHashVector.size(); i++) {
          double diff_this = 0.0;
          int a = node1->actionHashVector[i];
          if(node2->actionHashes.count(a)) {
             commonActions.insert(std::pair< int, int >(a, 1));
             commonActionsVector.push_back(a);
             diff_this = abs(node1->actionToReward[a] - node2->actionToReward[a]);
          } else {
             diff_this = abs(
                node1
                   ->actionToReward[a]);  // not common action. how many common actions in general [9/3]
             // different setting. multiply [0-1]
          }
          if(diff_this > max_reward_difference) {
             max_reward_difference = diff_this;
          }
       }

       for(int i = 0; i < node2->actionHashVector.size(); i++) {
          int a = node2->actionHashVector[i];
          if(! commonActions.count(a))
             continue;
          double diff_this = 0.0;

          if(node1->actionHashes.count(a)) {
             diff_this = abs(node1->actionToReward[a] - node2->actionToReward[a]);
          } else {
             diff_this = abs(node2->actionToReward[a]);
          }
          if(diff_this > max_reward_difference) {
             max_reward_difference = diff_this;
          }
       }
       if(max_reward_difference > reward_threshold) {
          return false;
       }

       /*
       A   1, 2
       B   2, 3
       */
       // for all transition
       double transition_error = 0;
       for(int i = 0; i < node1->nextStateHashVector.size(); i++) {
          int s_prime = node1->nextStateHashVector[i];  // s' next state
          if(! (node2->stateCounter.count(s_prime))) {  // get the same next state
             transition_error += 1.0;
          }
       }

       for(int i = 0; i < node2->nextStateHashVector.size(); i++) {
          int s_prime = node2->nextStateHashVector[i];
          if(! (node1->stateCounter.count(s_prime))) {  // get the same next state
             transition_error += 1.0;
          }
       }

       if(transition_error > transition_threshold) {
          return false;
       }

       // one of the two condition make the merging happen. 9/3
       return true;
    }

}  // namespace SGA