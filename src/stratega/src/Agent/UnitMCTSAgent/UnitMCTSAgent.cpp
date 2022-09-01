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
        //parameters_.heuristic = std::make_unique< AimToKingHeuristic >(initialState);
        //parameters_.heuristic = std::make_unique< BasicTBSResourceHeuristic >(parameters_.PLAYER_ID, initialState);
        //parameters_.heuristic = std::make_unique< BasicTBSTechnologyHeuristic >(parameters_.PLAYER_ID, initialState);
        //parameters_.heuristic = std::make_unique< BasicTBSCombatHeuristic >(parameters_.PLAYER_ID, initialState);
        parameters_.heuristic = std::make_unique< TwoKingdomsAbstractionHeuristic >(parameters_.PLAYER_ID, initialState, 1.0, 0.0, 0.0);

        parameters_.abstractionHeuristic = std::make_unique<TwoKingdomsAbstractionHeuristic>(parameters_.PLAYER_ID, initialState,
                                            0.0, 1.0, 0.0); // wCombat, wTechnology, wResource
        //parameters_.abstractionHeuristic = std::make_unique<BasicTBSTechnologyHeuristic>(parameters_.PLAYER_ID, initialState); 

        nD_min_list = std::vector<double> ();
        h_size_list = std::vector<double> ();
        a_size_list = std::vector<double> ();

        isPrintedLossBound = false;

        parameters_.printDetails();
    }

    ActionAssignment UnitMCTSAgent::computeAction(GameState state, const ForwardModel& forwardModel, Timer timer)
    {
        //std::cout<<"Start computeAction\n";
        if(newRound) {
            // record each turn in the game
            newRound = false;
        }
        parameters_.global_nodeID = 0;  // reinitialize the ID for node, witch is incremental as nodes created
        parameters_.REMAINING_FM_CALLS = parameters_.maxFMCalls;
        parameters_.maxDepth = 0;
        const auto processedForwardModel = parameters_.preprocessForwardModel(
            dynamic_cast< const TBSForwardModel& >(forwardModel));

        if(state.getGameType() != GameType::TBS) {
            throw std::runtime_error("MCTSAgent only supports TBS-Games");
        }

        std::vector< Action > actionSpace =  switchUnit(state, forwardModel);

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
                *processedForwardModel, state, unitIndex, unitThisStep, getPlayerID(), 0);
            }

            //std::cout<<"call search\n";
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
            //std::cout<<"after search\n";


            int n_abs_iteration = 0;
            bool stop_abstraction = false;
            bool is_abstraction_eliminated = false;

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
                std::vector< UnitMCTSNode* > deep_layer = depthToNodes[i];
                for(auto node1 : deep_layer) {  // each initial node
                    if(node1->isAbstracted)
                        continue;  // can be adjusted
                    if (node1->children.size() == 0) {
                        continue;
                    }
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
                            if (!parameters_.IS_MULTI_OBJECTIVE) {
                                if( !isTwoNodeApproxmateHomomorphism(
                                    forwardModel,
                                    node1,
                                    absNodes[i][j][k],
                                    parameters_.R_THRESHOLD,
                                    parameters_.T_THRESHOLD)) {
                                match = false;
                                }
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
                if(parameters_.REMAINING_FM_CALLS <= 0 || rootNode->n_search_iteration >= parameters_.maxFMCalls) {
                    rootNode->eliminateAbstraction();

                    ///* // for IJCAI paper
                    if(!stop_abstraction){
                        printBoundStatistics();
                    }
                    //*/
                    break;
                }

                if(tmp_batch_used >= parameters_.absBatch && !stop_abstraction) {

                    ///* // for IJCAI paper
                    if(true){
                        //std::cout<<"Abstraction eliminated!\n";
                        printBoundStatistics(false);
                    }
                    //*/

                    stop_abstraction = true;
                    deleteAbstraction();  // initialize the array empty again,
                    rootNode->eliminateAbstraction();  // make the flag of (has been abstracted) to false

                }
            }

            //std::cout<<"fdnsfi98\n";
            // get and store best action
            auto bestActionIndex = rootNode->mostVisitedAction( parameters_, getRNGEngine() );

            /*if (bestActionIndex == actionSpace.size()-1) { // this action is an endTurn, reinitialize
                //unitThisStep = 0;
                unitNextStep = 0;
            }*/

            //std::cout<<"f249ijv\n";
            auto bestAction = rootNode->getActionSpace(forwardModel, getPlayerID()).at(bestActionIndex);

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
            //

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
            */

            previousActionIndex = (bestAction.getActionFlag() == ActionFlag::EndTickAction)
                                    ? -1
                                    : bestActionIndex;  // std::cout << "*** 9" << std::endl;

            if(n_node == 1)
                previousActionIndex = -1;

            // std::cout << "-> UnitMCTS Action Took: ";
            //state.printActionInfo(bestAction);
            // std::cout << unitActionHash(bestAction) << std::endl;
            // std::cout << " End printActionInfo " << std::endl;
            //rootNode->printTree();

            /*
            for (int i = 0; i < 10; i++) { // depth

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

            if(bestAction.getActionFlag() == ActionFlag::EndTickAction) {
                unitIndexInitialized = false;
                newRound = true;
                step++;
            }

            if (parameters_.DO_STATE_ABSTRACTION) {
                stepInit();  // reinitialize homomorphism
            }
            //std::cout<<"end agent 2\n";
            return ActionAssignment::fromSingleAction(bestAction);
        }
        }

    std::vector< Action > UnitMCTSAgent::switchUnit(GameState state, const ForwardModel& forwardModel) {
            auto units = state.getPlayerEntities(getPlayerID());

        //std::cout<<"start swtichUnit\n";
        // each turn, we get a new entity list, palyer action moves first then entity actions
        if(unitIndexInitialized == false) {
            //std::cout<<"initializing unitIndex\n";
            unitIndex = std::vector<int>();
            for(auto unit : units) {
                unitIndex.push_back(unit.getID());
            }
            unitIndexInitialized = true;
            unitThisStep = -1;
            return forwardModel.generatePlayerActions(state, getPlayerID(), false);
        }

        if (unitThisStep == -1) {
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
                node1->actionToReward[a]);  // not common action. how many common actions in general [9/3]
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

    void UnitMCTSAgent::printBoundStatistics(bool mustPrint) {
        if(isPrintedLossBound)return;
        double nD_min = 1000000.0;
        double h_size = 0;
        double a_size = 0;
        double node_number = 0;
        for (int i = 1; i < 10; i++) { // depth
            //if (absNodes[i].size() == 0) {
            //    break;
            //}

            for (int j = 0; j < absNodes[i].size(); j++) { // abs node
                int nD_tmp = absNodes[i][j][0]->getVisitCount(&absNodeToStatistics);
                if (nD_tmp < nD_min) {
                    nD_min = nD_tmp;
                }
                h_size ++;
                for (int k = 0; k < absNodes[i][j].size(); k++) {
                    if (absNodes[i][j][k]->children.size() > 0) {
                        node_number++;
                        a_size += absNodes[i][j][k]->children.size();
                    }
                    
                }
            }
            //if(node_number !=0.0)
            //    a_size /= node_number;
            //std::cout <<"nD: "<< nD_min  << "\th_size: " << h_size <<"\ta_size: " << a_size << "\n";
        }//end for
        if(node_number !=0.0)
            a_size /= node_number;
        //std::cout <<"nD: "<< nD_min  << "\th_size: " << h_size <<"\ta_size: " << a_size << "\n";
        if(h_size > 0){
            nD_min_list.push_back(nD_min);
            h_size_list.push_back(h_size);
            a_size_list.push_back(a_size);
        }
        if (nD_min_list.size() == nLossBoundStep || mustPrint) {
            isPrintedLossBound = true;
            double avg_nD_min = std::accumulate(nD_min_list.begin(), nD_min_list.end(), 0.0)/ nD_min_list.size();
            double h_size_min = std::accumulate(h_size_list.begin(), h_size_list.end(), 0.0)/ h_size_list.size();
            double a_size_min = std::accumulate(a_size_list.begin(), a_size_list.end(), 0.0)/ a_size_list.size();
            std::cout <<"nD: "<< avg_nD_min  << "\th_size: " << h_size_min <<"\ta_size: " << a_size_min << "\n";
        }
    }
    /* For games there is no player actions
    std::vector< Action > UnitMCTSAgent::switchUnit(GameState state, const ForwardModel& forwardModel) {
        auto units = state.getPlayerEntities(getPlayerID()); 

        // each turn, we get a new entity list
        if(unitIndexInitialized == false) {
           for(auto unit : units) {
              unitIndex.push_back(unit.getID());
           }
           unitIndexInitialized = true;
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
    }*/
}  // namespace SGA