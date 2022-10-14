#include <Stratega/Agent/UnitMCTSAgent/Transition.h>
#include <Stratega/Agent/UnitMCTSAgent/UnitMCTSAgent.h>
#include <cassert>
#include <numeric>

namespace SGA {
    void UnitMCTSAgent::init(GameState initialState, const ForwardModel& /*forwardModel*/, Timer /*timer*/)
    {
        parameters_.PLAYER_ID = getPlayerID();
        parameters_.OPPONENT_ID = 1- parameters_.PLAYER_ID;
        //if (parameters_.heuristic == nullptr)
        //    parameters_.heuristic = std::make_unique<AbstractHeuristic>(initialState);

        if (parameters_.budgetType == Budget::UNDEFINED)
            parameters_.budgetType = Budget::TIME;
        parameters_.opponentModel = std::make_shared<RandomActionScript>();
        if (parameters_.HEURISTIC == "ktk") {
            parameters_.heuristic = std::make_unique< AimToKingHeuristic >(initialState);
        }
        else if (parameters_.HEURISTIC == "pta") {
            parameters_.heuristic = std::make_unique< PushThemAllHeuristic >(getPlayerID(), initialState);
        }
        else {
            std::cout<<"[ERROR] invalid heuristic name parameter\n";
        }
        //parameters_.heuristic = std::make_unique< AimToKingHeuristic >(initialState);
        //parameters_.heuristic = std::make_unique< PushThemAllHeuristic >(getPlayerID(), initialState);
        if(is_debug)
            parameters_.printDetails();
    }

    ActionAssignment UnitMCTSAgent::computeAction(GameState state, const ForwardModel& forwardModel, Timer timer)
    {
       if(newRound) {
          newRound = false;
       }
       parameters_.global_nodeID = 0;  // reinitialize the ID for node, witch is incremental as nodes created
       auto units = state.getPlayerEntities(getPlayerID()); 
       //state.printBoard();
       // initialize the order of unit moving
       if(unitIndexInitialized == false) {
          for(auto unit : units) {
             unitIndex.push_back(unit.getID());
             // state.printEntityInfo(unit.getID());
          }
          unitIndexInitialized = true;
       }

	   // each step, the unit array's index would change because the unit dies
       std::map< int, int > eIDtoUnitArrayIndex = {};  

       int tmp_counter1 = 0;
       for(auto u : units) {
          eIDtoUnitArrayIndex.insert(std::pair< int, int >(u.getID(), tmp_counter1));
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
          auto actionSpace_tmp = forwardModel.generateUnitActions(state, e, getPlayerID(), false);

          if(actionSpace_tmp.size() == 0) {
             needNextUnit = true;
          } else {
          }
       }

	   //std::cout << "Do we need the next unit? " << needNextUnit << std::endl;

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

             Action endAction = Action::createEndAction(getPlayerID());
             unitThisStep = 0;
             unitNextStep = 1;
             newRound = true;

             // state.printBoard();
			 // std::cout<<"execute EndTurn if there is no valid next unit."<<std::endl;
             return ActionAssignment::fromSingleAction(endAction);
          }

          unitThisStep = tmp_unitNextStep;
       }

       parameters_.REMAINING_FM_CALLS = parameters_.maxFMCalls;


       if(state.getGameType() != GameType::TBS) {
          throw std::runtime_error("MCTSAgent only supports TBS-Games");
       }

       // initialize heuristic
       const auto processedForwardModel = parameters_.preprocessForwardModel(
          dynamic_cast< const TBSForwardModel& >(forwardModel));

       parameters_.maxDepth = 0;  // for debugging

       // generate actions and update unitThisStep
       // const auto actionSpace = forwardModel.generateActions(state, getPlayerID());
       std::vector< Action > actionSpace = forwardModel.generateUnitActions(
          state, units[eIDtoUnitArrayIndex[unitIndex[unitThisStep]]], getPlayerID(), false);
       
       // return the only action
       // todo update condition to an and in case we can compare gameStates, since we currently cannot
       // reuse the tree after an endTurnAction
       if(actionSpace.size() == 1) {
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

          }

          // [Homomorphism] do batches
          int tmp_batch_used = 0;
          int tmp_index = 1000;
          if(! parameters_.DO_STATE_ABSTRACTION) {
              //std::cout<<"start search\n";
              rootNode->searchMCTS(
                 *processedForwardModel,
                 parameters_,
                 getRNGEngine(),
                 &depthToNodes,
                 &absNodeToStatistics);
              //std::cout<<"ends search\n";
          }


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

             //for aamas
             if(parameters_.IS_UNGROUPING){
                 if(parameters_.SINGLE_UNGROUPING){
                     for (int i = 1; i < parameters_.maxDepth; i++) { // depth
                        //if (absNodes[i].size() == 0) {
                        //    break;
                        //}

                         std::vector<int> ungrouped_indices = {};
                         for (int j = 0; j < absNodes[i].size(); j++) { // abs node
                             if (absNodes[i].size() > 0){
                                 if(absNodes[i][j][0]->isUngrouped)continue;
                                 int absVisit = absNodes[i][j][0]->getVisitCount(&absNodeToStatistics);
                                 int absSize = absNodes[i][j].size();
                                 double ungrouping_threshold = 0.0;
                                 if (parameters_.IS_PHI_UNGROUPING) {
                                     ungrouping_threshold = absSize*parameters_.batch_size;
                                 }
                                 else {
                                     ungrouping_threshold = parameters_.UNGROUPING_BATCH_THRESHOLD*parameters_.batch_size;
                                 }
                                 if(absVisit - parameters_.R_THRESHOLD*2.0 *absSize* absSize > ungrouping_threshold){
                                     ungrouped_indices.push_back(j);
                                     //std::cout<<"Batch: "<< tmp_batch_used << ", ungrouping abstraction\n";
                                     for (int k = 0 ; k < absSize; k++){
                                         absNodes[i][j][k]->isAbstracted=false;
                                         absNodes[i][j][k]->isUngrouped=true;
                                     }
                                 }
                             }//end if
                         }//end for
                         for (int idx = ungrouped_indices.size()-1; idx>=0 ;idx--) {
                             absNodes[i].erase(absNodes[i].begin()+ungrouped_indices[idx]);
                         }
                     }
                 }
                 else if (parameters_.LAYER_UNGROUPING) {
                     for (int i = 1; i < parameters_.maxDepth; i++) { // depth
                         //if (absNodes[i].size() == 0) {
                         //    break;
                         //}
                         if(absNodes[i].size() == 0)continue;
                         bool toUngroup = true;
                         for (int j = 0; j < absNodes[i].size(); j++) { // abs node
                             if(absNodes[i][j][0]->isUngrouped)continue;
                             int absVisit = absNodes[i][j][0]->getVisitCount(&absNodeToStatistics);
                             int absSize = absNodes[i][j].size();
                             double ungrouping_threshold = 0.0;
                             if (parameters_.IS_PHI_UNGROUPING) {
                                 ungrouping_threshold = parameters_.batch_size/absSize;
                             }
                             else {
                                 ungrouping_threshold = parameters_.UNGROUPING_BATCH_THRESHOLD*parameters_.batch_size;
                                 //ungrouping_threshold = 0.3*parameters_.batch_size;
                             }
                             if(absVisit - absSize* absSize < ungrouping_threshold){
                                 toUngroup=false;
                                 break;
                             }
                         }//end for

                         if (toUngroup) {
                             std::cout<<"[LAYER UNGROUPING] batch: " << n_abs_iteration << " layer: "<< i << " UNGROUPING the subtree\n";
                             /*for (int j = 0; j < absNodes[i].size(); j++) { // abs node
                                 int absSize = absNodes[i][j].size();
                                 for (int k = 0 ; k < absSize; k++){
                                    absNodes[i][j][k]->isAbstracted=false;
                                    absNodes[i][j][k]->isUngrouped=true;
                                 }
                                 absNodes[i].clear();
                                 std::cout<<absNodes[i].size()<<"\n";
                             }*/
                             stop_abstraction = true;
                             deleteAbstraction();  // initialize the array empty again,
                             rootNode->eliminateAbstraction();  // make the flag of (has been abstracted) to false

                         }
                     }
                 }
                 else if (parameters_.SUBTREE_UNGROUPING) {

                     for (int i = 1; i < parameters_.maxDepth; i++) { // depth
                        //if (absNodes[i].size() == 0) {
                        //    break;
                        //}

                         std::vector<int> ungrouped_indices = {};
                         for (int j = 0; j < absNodes[i].size(); j++) { // abs node
                             if (absNodes[i].size() > 0){
                                 if(absNodes[i][j][0]->isUngrouped)continue;
                                 int absVisit = absNodes[i][j][0]->getVisitCount(&absNodeToStatistics);
                                 int absSize = absNodes[i][j].size();
                                 double ungrouping_threshold = 0.0;
                                 if (parameters_.IS_PHI_UNGROUPING) {
                                     ungrouping_threshold = absSize*parameters_.batch_size;
                                 }
                                 else {
                                     ungrouping_threshold = parameters_.UNGROUPING_BATCH_THRESHOLD*parameters_.batch_size;
                                     //ungrouping_threshold = 0.5*parameters_.batch_size;
                                 }
                                 if(absVisit - absSize* absSize > ungrouping_threshold){
                                     ungrouped_indices.push_back(j);
                                     //std::cout<<"Batch: "<< tmp_batch_used << ", ungrouping abstract node, size: " <<absSize <<", visiting: "<<absVisit<<" T: "<<ungrouping_threshold << "\n";
                                     for (int k = 0 ; k < absSize; k++){
                                         absNodes[i][j][k]->eliminateAbstraction();
                                         absNodes[i][j][k]->isUngrouped=true;
                                     }
                                 }
                             }//end if
                         }//end for
                         for (int idx = ungrouped_indices.size()-1; idx>=0 ;idx--) {
                             absNodes[i].erase(absNodes[i].begin()+ungrouped_indices[idx]);
                         }
                    }
                 }// end if
              }
             /*
             std::cout<<"After ungrouping, number of abs Node each depth:\n";
             for (int i = 1; i < parameters_.maxDepth; i++) {
                 int abs_size = absNodes[i].size();
                 std::cout<< "depth: "<< i<< " abs Node: "<< abs_size << "\n";
                 if(abs_size == 0)continue;

                 for (int j = 0; j < abs_size; j++) {
                 std::cout<< absNodes[i][j].size()<< " ";
                 }
                 std::cout<<"\n";
             }//*/

             // do abstraction
             for(int i = parameters_.maxDepth - 1; (! stop_abstraction) && i > 0; i--)  // bottom-up
             {
                 std::vector< UnitMCTSNode* > deep_layer = depthToNodes[i];
                 // try ungroup part of the abstraction

                 for(auto node1 : deep_layer) {  // each initial node
                     // if ungrouped, don't abstract again
                     // 
                     // aamas if one parent is ungrouped, this subtree will not be abstracted
                     if(parameters_.SUBTREE_UNGROUPING){
                         auto tmp_node = node1;
                         while (tmp_node) {
                            if (tmp_node->isUngrouped) {
                                node1->isUngrouped=true;
                                break;
                            }
                            tmp_node = tmp_node->parentNode;
                         }
                     }

                    // aamas, leaf node skip abstraction
                    if(node1->isAbstracted || node1->isUngrouped || node1->children.size() == 0)
                       continue;  // can be adjusted

                    if(absNodes[i].size() == 0) {  // this depth has no node cluster

                        absNodes[i].push_back(std::vector< UnitMCTSNode* >{node1});

                        // absNodeToStatistics, absNodeHASH -> absNode.value, absNode.visitingCount
                        absNodeToStatistics.insert(std::pair< int, std::vector< double > >(
                            i * tmp_index + absNodes[i].size() - 1,
                            std::vector< double >{node1->value, float(node1->nVisits)}));

                        node1->isAbstracted = true;
                        node1->absNodeID = i * 1000 + absNodes[i].size() - 1;

                        treeNodetoAbsNode.insert(std::pair< int, int >(node1->nodeID, i*1000+0)); // added 20221014

                        continue;
                    }

                    if(!(parameters_.random_abstraction)){
                       bool foundExistGroup = false;
                       if (parameters_.IS_ACTION_INDEPENDENT) {
                           for(int j = 0; j < absNodes[i].size(); j++)  // each abstract nodes: nodes cluster
                           {
                              bool match = true;
                              for(int k = 0; k < absNodes[i][j].size(); k++) {  // compare between new ground nodes to the abstract Nodes
                                  if(!isActionIndependentHomomorphism(
                                    forwardModel,
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
                                 //treeNodetoAbsNode.insert(std::pair< int, int >(node1->nodeID, node1->absNodeID));
                                 treeNodetoAbsNode.insert(std::pair< int, int >(node1->nodeID, i*1000+j));

                                 foundExistGroup = true;
                              }//end if (match)
                           }
                       }
                       else {
                           for(int j = 0; j < absNodes[i].size(); j++)  // each abstract nodes: nodes cluster
                           {
                               bool match = false;
                               for(int k = 0; k < absNodes[i][j].size(); k++) {  // compare between new ground nodes to the abstract Nodes

                                   if(isTwoNodeApproxmateHomomorphism(
                                     forwardModel,
                                     node1,
                                     absNodes[i][j][k],
                                     parameters_.R_THRESHOLD,
                                     parameters_.T_THRESHOLD)) {
                                        match = true;
                                        break;
                                     } // end if
                              } // end for
                              if(match) {
                                 node1->isAbstracted = true;
                                 node1->absNodeID = i * tmp_index + j;
                                 absNodes[i][j].push_back(node1);  // add into existing group
                                 treeNodetoAbsNode.insert(std::pair< int, int >(node1->nodeID, node1->absNodeID));

                                 foundExistGroup = true;
                              }//end if (match)
                           }
                       }

                       // not found existing abstract node to add in, create a new one
                       if(! foundExistGroup) {
                          absNodes[i].push_back(std::vector< UnitMCTSNode* >{node1});
                          absNodeToStatistics.insert(std::pair< int, std::vector< double > >(
                             i * tmp_index + absNodes[i].size() - 1,
                             std::vector< double >{node1->value, float(node1->nVisits)}));

                          // absNodeToStatistics[i * 1000 + absNodes[i].size() - 1] = std::vector<double>{
                          // node1->value, double(node1->nVisits) };

                          node1->isAbstracted = true;
                          node1->absNodeID = i * tmp_index + absNodes[i].size() - 1;

                       } // end if(! foundExistGroup) {
                    } else {
                       /*
                        * uniformly add a initial node to abstact node/ create a new abstract node
                        */
                       int nAction = absNodes[i].size();
                       
                       boost::random::uniform_int_distribution<size_t> actionDist(0, nAction); // close interval
                       auto actionIndex = actionDist(getRNGEngine());

                       // create a new abstact node
                       if (actionIndex == nAction) {

                          absNodes[i].push_back(std::vector< UnitMCTSNode* >{node1});

                          // absNodeToStatistics, absNodeHASH -> absNode.value, absNode.visitingCount
                          absNodeToStatistics.insert(std::pair< int, std::vector< double > >(
                             i * tmp_index + absNodes[i].size() - 1,
                             std::vector< double >{node1->value, float(node1->nVisits)}));
                          
                          node1->isAbstracted = true;
                          node1->absNodeID = i * tmp_index + absNodes[i].size() - 1;
                          treeNodetoAbsNode.insert(std::pair<int, int>(node1->nodeID, node1->absNodeID ));

                          continue;
                       }
                       else {

                          //int j = absNodes[i][actionIndex].size()-1;
                          int j = actionIndex;
                          node1->isAbstracted = true;
                          node1->absNodeID = i * tmp_index + j;

                          absNodes[i][actionIndex].push_back(node1);  // add into existing group
                          treeNodetoAbsNode.insert(std::pair< int, int >(node1->nodeID, i * 1000 + j));

                       }
                    }

                    node1->isAbstracted = true;
                 }

             }

             n_abs_iteration++;
             tmp_batch_used++;
             

             /* analyze the compression rate
             if (tmp_batch_used < 21 && (absNodeToStatistics.size() != 0) && (treeNodetoAbsNode.size() != 0))
                    std::cout<<"batch: " << tmp_batch_used << " " << float(treeNodetoAbsNode.size()) / absNodeToStatistics.size() << std::endl;
             //*/

             //aamas
             /*if (tmp_batch_used == 12 || tmp_batch_used == 14) {
                 printAbsNodeStatus();
                 rootNode->printTree();
             }*/

             // tmp_batch_used >=20 means do maximum 20 times abstraction in a step
             if(parameters_.REMAINING_FM_CALLS <= 0 || rootNode->n_search_iteration >= parameters_.maxFMCalls) {
                 //
                 
                 //std::cout<<"End searching, number of abs Node each depth:\n";
                 //rootNode->printTree();
                 rootNode->eliminateAbstraction(&absNodeToStatistics);
                 //deleteAbstraction();
                 //std::cout<<"maximum batch: "<< n_abs_iteration << " \n";

                 break;
             }


             if(!stop_abstraction && tmp_batch_used >= parameters_.absBatch) {
                 //printAbsNodeStatus();
                 //std::cout<<"\n";
                 stop_abstraction = true;
                 rootNode->eliminateAbstraction(&absNodeToStatistics);
                 deleteAbstraction();
             }

          }
          /*
          if (parameters_.DO_STATE_ABSTRACTION) {
              std::cout<<rootNode->n_search_iteration<<"\n";
          }
          //*/
          //std::cout<<"iteration " << rootNode->n_search_iteration<<"\n";
          //std::cout<<n_abs_iteration<<"\n";

          auto bestActionIndex = rootNode->mostVisitedAction( parameters_, getRNGEngine() );  // get and store best action

          /*if (bestActionIndex == actionSpace.size()-1) { // this action is an endTurn, reinitialize
              //unitThisStep = 0;
              unitNextStep = 0;
          }*/

          auto bestAction = rootNode->getActionSpace(forwardModel, getPlayerID()).at(bestActionIndex);

		  /*
		  for (auto a : rootNode->getActionSpace(forwardModel, getPlayerID())) {
		    state.printActionInfo(a);
		  }
		  state.printActionInfo(bestAction);
		  //*/

          // calculate the branching factor
          std::vector< int > branching_number = {};
          int n_node = 0;
          double mean_braching_factor = 0.0;

          /*
          rootNode->get_branching_number(&branching_number, &n_node);

          if(branching_number.size() != 0) {
             mean_braching_factor = std::accumulate(
                                       branching_number.begin(), branching_number.end(), 0.0)
                                    / branching_number.size();
          }
          */

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

          if(bestAction.getActionFlag() == ActionFlag::EndTickAction)
             step++;

          previousActionIndex = (bestAction.getActionFlag() == ActionFlag::EndTickAction)
                                   ? -1
                                   : bestActionIndex;  // std::cout << "*** 9" << std::endl;

          if(n_node == 1)
             previousActionIndex = -1;

          // std::cout << "-> UnitMCTS Action Took: ";
          // state.printActionInfo(bestAction);
          // std::cout << unitActionHash(bestAction) << std::endl;
          // std::cout << " End printActionInfo " << std::endl;
          //state.printBoard();
          //state.printActionInfo(bestAction);
          //auto reward = parameters_.heuristic->evaluateGameState(forwardModel, state, getPlayerID());
          //std::cout<<"reward: "<<reward<<"\n";
          //rootNode->printTree();
          //double score  = parameters_.heuristic->evaluateGameState(forwardModel, state,parameters_.PLAYER_ID );
          //std::cout<<score << "\n";

          /*
          for (int i = 1; i < 10; i++) { // depth
               if (absNodes[i].size() == 0) {
                  std::cout<<"\n";
                  break;
              }

              std::cout << "depth: "<< i << ", N absNode: " << absNodes[i].size() << " : \n";

             
              for (int j = 0; j < absNodes[i].size(); j++) { // abs node
                  if (j > 0) {
                      std::cout << "\t";
                  }
                  std::cout <<absNodes[i][j].size();

                  //for (int k = 0; k < absNodes[i][j].size(); k++) // original tree nodes
                  //{
                  //    std::cout << " " << absNodes[i][j][k]->value / absNodes[i][j][k]->nVisits << " , "
                  //      << absNodes[i][j][k]->nVisits <<";";
                  //}
                  //std::cout << "[END]" << std::endl;
              }
              std::cout << std::endl;
          }
          //std::cout << " [DEBUG382]: parameters_.REMAINING_FM_CALLS: " << parameters_.REMAINING_FM_CALLS
          //<< std::endl;
          //*/

          if(bestAction.getActionFlag() == ActionFlag::EndTickAction) {
             newRound = true;
          }

          if (parameters_.DO_STATE_ABSTRACTION) {
              stepInit();  // reinitialize homomorphism
          }
          //system("pause");
          return ActionAssignment::fromSingleAction(bestAction);
       }
    }

    bool UnitMCTSAgent::isTwoNodeApproxmateHomomorphism(const ForwardModel& forwardModel, UnitMCTSNode* node1, UnitMCTSNode* node2, double reward_threshold, double transition_threshold)
    {
        // node 2 is a ground node from an abstract node
       auto actionSpace1 = node1->getActionSpace(forwardModel, getPlayerID());// ->actionSpace;
       auto actionSpace2 = node2->getActionSpace(forwardModel, getPlayerID());
       double max_reward_difference = 0.0;
       std::map< int, int > commonActions = std::map< int, int >();
       std::map< int, int > actionsUnion = std::map<int, int> ();
       std::vector< int > commonActionsVector = std::vector< int >();

       // reward checking
       for(int i = 0; i < node1->actionHashVector.size(); i++) {
          double diff_this = 0.0;
          int a = node1->actionHashVector[i];
          actionsUnion.insert(std::pair< int, int >(a, 1));
          if(node2->actionHashes.count(a)) {
             commonActions.insert(std::pair< int, int >(a, 1));
             commonActionsVector.push_back(a);
             diff_this = abs(node1->actionToReward[a] - node2->actionToReward[a]);
          } else {
             diff_this = abs(
                node1
                   ->actionToReward[a]);  // not common action.
             // different setting. multiply [0-1]
          }
          if(diff_this > max_reward_difference) {
             max_reward_difference = diff_this;
          }
       }

       for(int i = 0; i < node2->actionHashVector.size(); i++) {
          int a = node2->actionHashVector[i];
          if (!actionsUnion.count(a)) {
              actionsUnion.insert(std::pair<int, int>(a, 1));
          }
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
       // for all transition, in deterministic situation, it could only be 
       // for common action a
       // 0, when T(s1, a, s3) =1.0 and T(s2, a, s3)=1.0
       // 2.0, when T(s1, a, s3) =1.0 and T(s2, a, s3) = 0.0; T(s1, a, s4) =1.0 and T(s2, a, s4) = 0.0;
       // for non-common action a1 for s1, a2 for s2
       // 2.0, T(s1, a1, s3) =1.0, T(s2, a1, s3) = 0.0; T(s1, a2, s4) =0.0, T(s2, a2, s4) = 1.0;
       for (auto a_pair : actionsUnion) {
           int a = a_pair.first;
           double diff_this = 0.0;
           if (node1->actionHashes.count(a) && node2->actionHashes.count(a)) {
               if (node1->actionToNextState[a] == node2->actionToNextState[a]) {
               }
               else {
                   diff_this+=2.0;
               }
           }
           else {
               diff_this+=2.0;
           }
           if(diff_this > transition_threshold)return false;
       }

       return true;
    }

    // actual reward, what if using the approximate Q? combination of heuristic score [stage1]
    bool UnitMCTSAgent::isActionIndependentHomomorphism(const ForwardModel& forwardModel, UnitMCTSNode* node1, UnitMCTSNode* node2, double reward_threshold, double transition_threshold)
    {
       auto actionSpace1 = node1->getActionSpace(forwardModel, getPlayerID());// ->actionSpace;
       auto actionSpace2 = node2->getActionSpace(forwardModel, getPlayerID());
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
       

       
       //A   1, 2
      // B   2, 3
       
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

    void UnitMCTSAgent::printAbsNodeStatus() {
        for (int i = 1; i < parameters_.maxDepth; i++) {
            int abs_size = absNodes[i].size();
            std::cout<< "depth: "<< i<< " abs Node: "<< abs_size << "\n";
            if(abs_size == 0)continue;

            for (int j = 0; j < abs_size; j++) {
            std::cout<< absNodes[i][j].size()<< " ";
            }
            std::cout<<"\n";
        }
    }
}  // namespace SGA