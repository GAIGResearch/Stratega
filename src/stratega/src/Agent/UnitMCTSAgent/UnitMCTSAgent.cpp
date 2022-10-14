#include <Stratega/Agent/UnitMCTSAgent/Transition.h>
#include <Stratega/Agent/UnitMCTSAgent/UnitMCTSAgent.h>
#include <cassert>
#include <numeric>

namespace SGA {
    void UnitMCTSAgent::init(GameState initialState, const ForwardModel& /*forwardModel*/, Timer /*timer*/)
    {
        parameters_.PLAYER_ID = getPlayerID();
        parameters_.OPPONENT_ID = 1- parameters_.PLAYER_ID;
        battle_mode=false;


        if (parameters_.budgetType == Budget::UNDEFINED)
            parameters_.budgetType = Budget::TIME;
        parameters_.opponentModel = std::make_shared<RandomActionScript>();
        //parameters_.heuristic = std::make_unique< AimToKingHeuristic >(initialState);
        //parameters_.heuristic = std::make_unique< BasicTBSResourceHeuristic >(parameters_.PLAYER_ID, initialState);
        //parameters_.heuristic = std::make_unique< BasicTBSTechnologyHeuristic >(parameters_.PLAYER_ID, initialState);
        parameters_.heuristic = std::make_unique< BasicTBSCombatHeuristic >(parameters_.PLAYER_ID, initialState);
        //parameters_.heuristic = std::make_unique< TwoKingdomsAbstractionHeuristic >(parameters_.PLAYER_ID, initialState, 1.0, 0.0, 0.0); // not used in this code

        //parameters_.abstractionHeuristic = std::make_unique<BasicTBSCombatHeuristic>(parameters_.PLAYER_ID, initialState); // wCombat, wTechnology, wResource

        nD_min_list = std::vector<double> ();
        h_size_list = std::vector<double> ();
        a_size_list = std::vector<double> ();

        isPrintedLossBound = false;
        if(is_debug)
            parameters_.printDetails();
    }

    ActionAssignment UnitMCTSAgent::computeAction(GameState state, const ForwardModel& forwardModel, Timer timer)
    {
        if (!battle_mode) {
            int self_warrior_N = 0;
            auto entities = state.getEntities();
            for (auto entity : entities) {
                auto& entityType = state.getGameInfo()->getEntityType(entity.getEntityTypeID());
                auto entityName = entityType.getName();
                if (entity.getOwnerID() == getPlayerID()) {
                    if (entityType.getName() == "Warrior") {
                        self_warrior_N++;
                    }
                    else if (entityType.getName() == "Worker") {
                        auto h = entity.getParameter("Health");
                        /*if (parameters_.DO_STATE_ABSTRACTION) {
                            std::cout<<"health: "<< h<<"\n";
                        }
                        //*/
                    }
                }
                //TODO enter battlemodel at the same time(both player)

            }
            //std::cout<<self_warrior_N<<std::endl;
            if(self_warrior_N>=3){
                if(is_debug)
                    std::cout<<"Player: " << getPlayerID() << " combat mode\n";
                parameters_.heuristic->setBattleMode();
                battle_mode=true;
            }
            
        }

        if(newRound) {
            // [log] record each turn in the game
            newRound = false;
        }
        parameters_.global_nodeID = 0;  // reinitialize the ID for node, witch is incremental as nodes created
        parameters_.REMAINING_FM_CALLS = parameters_.maxFMCalls;
        parameters_.maxDepth = 0;
        parameters_.currentIterations = 0;
        const auto processedForwardModel = parameters_.preprocessForwardModel(
            dynamic_cast< const TBSForwardModel& >(forwardModel));

        if(state.getGameType() != GameType::TBS) {
            throw std::runtime_error("MCTSAgent only supports TBS-Games");
        }

        std::vector< Action > actionSpace = {};
        if (parameters_.HAS_PLAYER_ACTION) {
            actionSpace =  switchPlayerUnit(state, forwardModel);
        }
        else {
            actionSpace =  switchUnit(state, forwardModel);
        }

        //std::cout<<"jfdias9fi\n";

        if(actionSpace.size() == 1) {
            rootNode = nullptr;
            previousActionIndex = -1;
            step++;
            newRound = true;

            Action a = actionSpace.at(0);
            if (a.getActionFlag() == ActionFlag::EndTickAction) {
                unitIndexInitialized = false;
            }
            //std::cout<<"End agent1\n";
            /*
            if (parameters_.DO_STATE_ABSTRACTION) {
                //state.printBoard();
                state.printActionInfo(a);
                //parameters_.abstractionHeuristic->evaluateGameState(forwardModel, state, getPlayerID(), true);
            }
            //*/
            return ActionAssignment::fromSingleAction(a);
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
                *processedForwardModel, state, nullptr, 0, unitIndex, unitThisStep, getPlayerID(), 0, parameters_, getRNGEngine());
            }

            //std::cout<<"call search\n";
            // [Homomorphism] do batches
            int tmp_batch_used = 0;
            int tmp_index = 1000;
            if(!parameters_.DO_STATE_ABSTRACTION){
                rootNode->searchMCTS(
                *processedForwardModel,
                parameters_,
                getRNGEngine(),
                &depthToNodes,
                &absNodeToStatistics);
            }
            int n_abs_iteration = 0;
            bool stop_abstraction = false;
            bool is_abstraction_eliminated = false;
            //compression rate
            /*if (!battle_mode) {
                for (int i = 0; i < 21; i++) {
                    std::cout<<"batch: "<<i<<" "<< "1.0\n";
                }
            }*/

            while(parameters_.DO_STATE_ABSTRACTION) {
                if(parameters_.REMAINING_FM_CALLS <= 0)
                break;

                // std::cout << "[BEFORE SEARCH]: " << parameters_.REMAINING_FM_CALLS <<std::endl;
                //std::cout<<"searchMCTS start\n";
                rootNode->searchMCTS(
                *processedForwardModel,
                parameters_,
                getRNGEngine(),
                &depthToNodes,
                &absNodeToStatistics);
                //std::cout<<"ends searchMCTS\n";

                if(false && ! stop_abstraction && tmp_batch_used % 2 == 0) {
                    deleteAbstraction();  // initialize the array empty again,
                    rootNode->eliminateAbstraction();  // make the flag of (has been abstracted) to false
                }

                // do abstraction
                for(int i = parameters_.maxDepth - 1; (! stop_abstraction) && i > 0; i--)  // bottom-up
                {
                    if (!battle_mode) {
                        break;
                    }
                    std::vector< UnitMCTSNode* > deep_layer = depthToNodes[i];
                    for(auto node1 : deep_layer) {  // each initial node

                        //std::cout<<" isAbstracted: " << node1->isAbstracted << "\n";
                        if(node1->isAbstracted)
                            continue;  // can be adjusted
                        //std::cout<<"2.5\n";
                        if (node1->children.size() == 0) {
                            continue;
                        }
                        // player node not abstracted
                        if(node1->unitName=="Player"||node1->unitName=="City" ||node1->unitName=="Barracks")
                            continue;
                        //std::cout<<"3\n";
                        if(absNodes[i].size() == 0) {  // this depth has no node cluster
                            //std::cout<<"try add new cluster\n";
                            absNodes[i].push_back(std::vector< UnitMCTSNode* >{node1});

                            // absNodeToStatistics, absNodeHASH -> absNode.value, absNode.visitingCount
                            absNodeToStatistics.insert(std::pair< int, std::vector< double > >(
                                i * tmp_index + absNodes[i].size() - 1,
                                std::vector< double >{node1->value, float(node1->nVisits)}));
                            node1->isAbstracted = true;
                            node1->absNodeID = i * 1000 + absNodes[i].size() - 1;
                            //std::cout<<"Ends try add new cluster\n";
                            continue;
                        }
                        //std::cout<<"4\n";
                        
                        if (!parameters_.RANDOM_ABSTRACTION) {
                            bool foundExistGroup = false;
                            if (parameters_.IS_ACTION_INDEPENDENCE) {
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

                            } 
                            // end if(! foundExistGroup)
                            /*for (int j = 0; j < absNodes[i].size(); j++)  // each abstract nodes: nodes cluster
                            {

                                //std::cout<<"5\n";
                                bool match = true;
                                for(int k = 0; k < absNodes[i][j].size();
                                    k++) {  // compare between new initial nodes to the abstracted Nodes
                                    if (!parameters_.IS_MULTI_OBJECTIVE) {
                                        //std::cout<<"isTwoNodeApproxmateHomomorphism\n";
                                        if( !isTwoNodeApproxmateHomomorphism(
                                            forwardModel,
                                            node1,
                                            absNodes[i][j][k],
                                            parameters_.R_THRESHOLD,
                                            parameters_.T_THRESHOLD)) {
                                        match = false;
                                        }
                                        //std::cout<<"Ends isTwoNodeApproxmateHomomorphism\n";
                                    }
                                    else {
                                        if( !isHomomorphismMultiObjective(
                                            forwardModel,
                                            node1,
                                            absNodes[i][j][k])) {
                                        match = false;
                                        }
                                    }

                                } // end for
                                if(match) {
                                    //std::cout<<"matched\n";
                                    node1->isAbstracted = true;
                                    node1->absNodeID = i * tmp_index + j;
                                    absNodes[i][j].push_back(node1);  // add into existing group
                                    treeNodetoAbsNode.insert(std::pair< int, int >(node1->nodeID, i * 1000 + j));

                                    foundExistGroup = true;
                                    //std::cout<<"ends matched\n";
                                }
                                //std::cout<<"6\n";
                            }
                            //std::cout<<"7\n";
                            if(! foundExistGroup) {
                                //std::cout<<"!foundExistGroup\n";
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
                                //std::cout<<"ends !foundExistGroup\n";
                            }*/
                        }
                        else {
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

                // std::cout << " Depth: " << i << " Initial Number of Nodes: " << deep_layer.size() <<
                // " abstracted node number " << absNodes[i].size() << std::endl;
                }

                n_abs_iteration++;
                tmp_batch_used++;
                // analyze compression rate
                //if (tmp_batch_used < 21 && (absNodeToStatistics.size() != 0) && (treeNodetoAbsNode.size() != 0))
                //    std::cout<<"batch: " << tmp_batch_used << " " << float(treeNodetoAbsNode.size()) / absNodeToStatistics.size() << std::endl;


                // run out of budget, delete abstraction
                if(parameters_.REMAINING_FM_CALLS <= 0 || rootNode->n_search_iteration >= parameters_.maxFMCalls) {

                    //rootNode->eliminateAbstraction();
                    rootNode->eliminateAbstraction(&absNodeToStatistics);

                    ///* // for IJCAI paper
                    //if(!stop_abstraction){
                    //    printBoundStatistics();
                    //}
                    //*/
                    break;
                }

                if(tmp_batch_used >= parameters_.absBatch && !stop_abstraction) {

                    ///* // for IJCAI paper
                    //if(true){
                    //    //std::cout<<"Abstraction eliminated!\n";
                    //    printBoundStatistics(false);
                    //}
                    //*/
                    //state.printBoard();
                    //printAbsNodeStatus();
                    //rootNode->printTree();

                    stop_abstraction = true;
                    
                    //rootNode->eliminateAbstraction();  // make the flag of (has been abstracted) to false
                    rootNode->eliminateAbstraction(&absNodeToStatistics);
                    deleteAbstraction();  // initialize the array empty again,
                }
            }

            /*
            if (!parameters_.DO_STATE_ABSTRACTION) {
                std::cout<<parameters_.currentIterations<<std::endl;
                state.printBoard();
            }
            else {
               // rootNode->printTree();
                printAbsNodeStatus();
            }
            //*/
            // get and store best action
            auto bestActionIndex = rootNode->mostVisitedAction( parameters_, getRNGEngine() );
            auto bestAction = rootNode->getActionSpace(forwardModel, getPlayerID()).at(bestActionIndex);


            previousActionIndex = (bestAction.getActionFlag() == ActionFlag::EndTickAction)
                                    ? -1
                                    : bestActionIndex;  // std::cout << "*** 9" << std::endl;


            // std::cout << "-> UnitMCTS Action Took: ";
            //std::cout<< getPlayerID()<< "  ";
            //state.printBoard();
            //state.printActionInfo(bestAction);
            /*
            auto a_space = forwardModel.generateActions(state, getPlayerID());

            for (auto a : a_space) {
                state.printActionInfo(a);
            }
            //*/
            //parameters_.abstractionHeuristic->evaluateGameState(forwardModel, state, getPlayerID(), true);
            //rootNode->printTree();
            /*
            if (parameters_.DO_STATE_ABSTRACTION) {
                state.printBoard();
                state.printActionInfo(bestAction);
                //parameters_.abstractionHeuristic->evaluateGameState(forwardModel, state, getPlayerID(), true);
            }
            //*/

            if(bestAction.getActionFlag() == ActionFlag::EndTickAction) {
                unitIndexInitialized = false;
                newRound = true;
                step++;
            }

            if (parameters_.DO_STATE_ABSTRACTION) {
                stepInit(); // reinitialize homomorphism
            }

            return ActionAssignment::fromSingleAction(bestAction);
        }
    }

    std::vector< Action > UnitMCTSAgent::switchUnit(GameState state, const ForwardModel& forwardModel) {
        auto units = state.getPlayerEntities(getPlayerID());

        //std::cout<<"start swtichUnit\n";
        /* If there are no spawning system, this list can be fixed, but new units required us to update the unit list.
        * each turn, we get a new entity list, palyer action moves first then entity actions
        * how to recognize new turn? check return of computeAction == EndTick
        */

        if(unitIndexInitialized == false) {
            //if (parameters_.DO_STATE_ABSTRACTION) {
            //    state.printBoard();
            //}
            unitIndex = std::vector<int>();
          for(auto unit : units) {
             unitIndex.push_back(unit.getID());
          }
          //if(!parameters_.DO_STATE_ABSTRACTION){
          //    std::cout<<"player ID: "<< getPlayerID()<< "\n";
          //    for (auto i : unitIndex) {
          //        std::cout<<i<<" ";
          //    }
          //    std::cout<<"\n";
          //}
          unitIndexInitialized = true;
        }
        else {
            // newly spawned unit
            for (auto unit : units) {
                if (find(unitIndex.begin(), unitIndex.end(), unit.getID()) == unitIndex.end()) {
                    unitIndex.push_back(unit.getID());
                }
            }
        }

        // store the information of alive units
        std::map< int, int > eIDtoUnitArrayIndex = {};

        // their unit index might change at each step
        int tmp_counter1 = -1;
        for(auto u : units) {
            tmp_counter1++;

            eIDtoUnitArrayIndex.insert(std::pair< int, int >(u.getID(), tmp_counter1));
        }

        // to decide if we need to switch unit at this step.
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

        // if need to switch to a new unit, decide which is the next.
        if(needNextUnit) {
            int tmp_unitNextStep = unitThisStep + 1;
            for(; tmp_unitNextStep < unitIndex.size(); tmp_unitNextStep++) {
                if(eIDtoUnitArrayIndex.count(unitIndex[tmp_unitNextStep]))
                {
                    break;
                }
            }

            // execute EndTurn if there is no valid next unit.
            if(tmp_unitNextStep == unitIndex.size()) {  // every unit moved, end the turn
                step++;  // for debugging

                Action endAction = Action::createEndAction(getPlayerID());
                unitThisStep = 0;
                newRound = true;
                std::vector< Action > actionSpace = {endAction};

                return actionSpace;
            }

            // find an alive unit that `must` has non-empty action spacef
            unitThisStep = tmp_unitNextStep;
        }

        // generate actions and update unitThisStep
        std::vector< Action > actionSpace = forwardModel.generateUnitActions(
                state, units[eIDtoUnitArrayIndex[unitIndex[unitThisStep]]], getPlayerID(), false);
        return actionSpace;
    }

    std::vector< Action > UnitMCTSAgent::switchPlayerUnit(GameState state, const ForwardModel& forwardModel) {
            auto units = state.getPlayerEntities(getPlayerID());

        //std::cout<<"start swtichUnit\n";
        // each turn, we get a new entity list, palyer action moves first then entity actions
        // how to recognize new turn? check return of computeAction == EndTick
        if(unitIndexInitialized == false) {
            //std::cout<<"initializing unitIndex\n";
            unitIndex = std::vector<int>();
            for(auto unit : units) {
                unitIndex.push_back(unit.getID());
            }
            //unitIndex.push_back(-1); // player action
            unitIndexInitialized = true;
            unitThisStep = -1; // player moves first
            unitThisStep = 0;
            return forwardModel.generatePlayerActions(state, getPlayerID(), false);
        }

        if (unitThisStep == -1 || unitIndex[unitThisStep] == -1) {
            auto actionSpace = forwardModel.generatePlayerActions(state, getPlayerID(), false);
            if (actionSpace.size() == 0) {
                unitThisStep = 0;
            }
            else {
                return actionSpace;
            }
        }

        // each step, store the information of alive units
        std::map< int, int > eIDtoUnitArrayIndex = {};

        int tmp_counter1 = 0;
        for(auto u : units) {
            eIDtoUnitArrayIndex.insert(std::pair< int, int >(u.getID(), tmp_counter1));
            tmp_counter1 += 1;
        }

        // to decide if we need to switch unit at this step.
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

        // if need to switch to a new unit, decide which is the next.
        if(needNextUnit) {
            int tmp_unitNextStep = unitThisStep + 1;
            for(; tmp_unitNextStep < unitIndex.size(); tmp_unitNextStep++) {
                if(eIDtoUnitArrayIndex.count(unitIndex[tmp_unitNextStep]))  // if alive
                {
                break;
                }
            }

            // execute EndTurn if there is no valid next unit.
            // might be bad because somtimes you need to mine and get resource and then use player action.
            if(tmp_unitNextStep == unitIndex.size()) {  // every unit moved, end the turn
                step++;  // for debugging

                Action endAction = Action::createEndAction(getPlayerID());
                unitThisStep = 0;
                unitNextStep = 1;
                newRound = true;
                std::vector< Action > actionSpace = {endAction};

                return actionSpace;
            }

            unitThisStep = tmp_unitNextStep;
        }

        // generate actions and update unitThisStep
        std::vector< Action > actionSpace = forwardModel.generateUnitActions(
                state, units[eIDtoUnitArrayIndex[unitIndex[unitThisStep]]], getPlayerID(), false);
        return actionSpace;
    }

        // actual reward, what if using the approximate Q? combination of heuristic score [stage1]
    bool UnitMCTSAgent::isTwoNodeApproxmateHomomorphism(const ForwardModel& forwardModel, UnitMCTSNode* node1, UnitMCTSNode* node2, 
            double reward_threshold, double transition_threshold){

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
        std::cout<<"\n";
    }

    bool UnitMCTSAgent::isHomomorphismMultiObjective(const ForwardModel& forwardModel, UnitMCTSNode* node1, UnitMCTSNode* node2){

        auto actionSpace1 = node1->getActionSpace(forwardModel, getPlayerID());// ->actionSpace;
        auto actionSpace2 = node2->getActionSpace(forwardModel, getPlayerID());
        double max_technology_reward_difference = 0.0, max_combat_reward_difference = 0.0, max_resource_reward_difference =0.0;
        std::map< int, int > commonActions = std::map< int, int >();
        std::vector< int > commonActionsVector = std::vector< int >();

        // reward checking
        for(int i = 0; i < node1->actionHashVector.size(); i++) {
            //double diff_this = 0.0;
            double diff_technology = 0.0;
            double diff_resource = 0.0;
            double diff_combat = 0.0;

            int a = node1->actionHashVector[i];
            if(node2->actionHashes.count(a)) {
                commonActions.insert(std::pair< int, int >(a, 1));
                commonActionsVector.push_back(a);
                //diff_this = abs(node1->actionToReward[a] - node2->actionToReward[a]);
                diff_technology = abs(node1->actionToTechnologyReward[a] - node2->actionToTechnologyReward[a]);
                diff_resource = abs(node1->actionToResourceReward[a] - node2->actionToResourceReward[a]);
                diff_combat = abs(node1->actionToCombatReward[a] - node2->actionToCombatReward[a]);
            } 
            // node2 has no same action
            else {
                //diff_this = abs(node1->actionToReward[a]);  // not common action. how many common actions in general [9/3]
                // different setting. multiply [0-1]
                diff_technology = abs(node1->actionToTechnologyReward[a]);
                diff_resource = abs(node1->actionToResourceReward[a]);
                diff_combat = abs(node1->actionToCombatReward[a]);
            }
            if(diff_technology > max_technology_reward_difference) {
                max_technology_reward_difference = diff_technology;
            }
            if(diff_resource > max_resource_reward_difference) {
                max_resource_reward_difference = diff_resource;
            }
            if(diff_combat > max_combat_reward_difference) {
                max_combat_reward_difference = diff_combat;
            }
        }

        for(int i = 0; i < node2->actionHashVector.size(); i++) {
            int a = node2->actionHashVector[i];
            if(! commonActions.count(a))
                continue;

            double diff_technology = 0.0;
            double diff_resource = 0.0;
            double diff_combat = 0.0;

            if(node1->actionHashes.count(a)) {
                //diff_this = abs(node1->actionToReward[a] - node2->actionToReward[a]);
                diff_technology = abs(node1->actionToTechnologyReward[a] - node2->actionToTechnologyReward[a]);
                diff_resource = abs(node1->actionToResourceReward[a] - node2->actionToResourceReward[a]);
                diff_combat = abs(node1->actionToCombatReward[a] - node2->actionToCombatReward[a]);
            } else {
                //diff_this = abs(node2->actionToReward[a]);
                diff_technology = abs(node2->actionToTechnologyReward[a]);
                diff_resource = abs(node2->actionToResourceReward[a]);
                diff_combat = abs(node2->actionToCombatReward[a]);
            }

            if(diff_technology > max_technology_reward_difference) {
                max_technology_reward_difference = diff_technology;
            }
            if(diff_resource > max_resource_reward_difference) {
                max_resource_reward_difference = diff_resource;
            }
            if(diff_combat > max_combat_reward_difference) {
                max_combat_reward_difference = diff_combat;
            }
        }
        //std::cout<< "tech_diff: "<< max_technology_reward_difference << ", combat_diff : "<< max_combat_reward_difference<< "\n";
        if(max_technology_reward_difference > parameters_.TECHNOLOGY_R_THRESHOLD || 
            //max_resource_reward_difference > parameters_.RESOURCE_R_THRESHOLD &&
            max_combat_reward_difference > parameters_.COMBAT_R_THRESHOLD) {
            
            /*if (max_technology_reward_difference > parameters_.TECHNOLOGY_R_THRESHOLD && max_combat_reward_difference > parameters_.COMBAT_R_THRESHOLD) {
                std::cout<<"Rejected by both, tech_diff: "<< max_technology_reward_difference << ", combat_diff: "<< max_combat_reward_difference<< "\n";
            }
            else if(max_technology_reward_difference > parameters_.TECHNOLOGY_R_THRESHOLD) {
                std::cout<<"Rejected by TECHNOLOGY_R_THRESHOLD, tech_diff: " << max_technology_reward_difference << ", combat_diff: "<< max_combat_reward_difference<< "\n";
            } else if(max_combat_reward_difference > parameters_.COMBAT_R_THRESHOLD) {
                std::cout<<"Rejected by COMBAT_R_THRESHOLD, tech_diff: " << max_technology_reward_difference << ", combat_diff: "<< max_combat_reward_difference<< "\n";
            }*/
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
        //std::cout<< "transition_error: "<<transition_error<<"\n";
        if(transition_error > parameters_.T_THRESHOLD) {
            //std::cout<<"Rejected by transition! err: "<< transition_error<< "\n";
            return false;
        }

        // one of the two condition make the merging happen. 9/3
        return true;
    }

}  // namespace SGA