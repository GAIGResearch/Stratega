#include <Stratega/Agent/RuleBasedAgents/BasicTBSAgent.h>


namespace SGA {

    void BasicTBSAgent::init(GameState initialState, const ForwardModel& forwardModel, Timer timeBudgetMs) {

		initialized = true; // This function is called by GameRunner
		workers = std::vector<int>();
		nearbyAttackable = std::vector<int>();
		farAwarAttackable = std::vector<int>();
		goldVein = std::vector<int>();
		goldVeinPosition = std::vector<Vector2f>();
		barracks = std::vector<int>();

		bool foundEnemyCity = false, foundSelfCity=false;
		// store the position of self base and enermy base;
        for(const auto& entity : initialState.getEntities()) {

            auto& entityType = initialState.getGameInfo()->getEntityType(entity.getEntityTypeID());
            //std::cout<<entityType.getName() <<std::endl;
            if(entity.getOwnerID() != getPlayerID() ) 
            {
                if(entityType.getName() == "City" && (!foundEnemyCity)) {
                    foundEnemyCity = true; //save some time
                    enemy_city_id = entity.getID();
                    enemy_city.x= entity.x();
                    enemy_city.y = entity.y();

                }
            } else if(entity.getOwnerID() == getPlayerID()){
               if(entityType.getName() == "City" && (!foundSelfCity)) {
                    foundSelfCity = true;
                    self_city_id = entity.getID();
                    self_city.x = entity.x();
                    self_city.y = entity.y();
               }
            } else {

            }
        }

        // make the nearest (to our city) goldvein as the head of vector
        for(const auto& entity : initialState.getEntities()) { 
            auto& entityType = initialState.getGameInfo()->getEntityType(entity.getEntityTypeID());
            if(entityType.getName() == "GoldVein"){
                if (goldVein.size() > 0){
                    double dis_oldGoldVein = initialState.getEntity(goldVein[0])->getPosition().distance(initialState.getEntity(self_city_id)->getPosition());
                    double dis_newGoldVein = entity.getPosition().distance(initialState.getEntity(self_city_id)->getPosition());
                    if(dis_newGoldVein > dis_oldGoldVein ){
                        goldVein.push_back(entity.getID());
                    } else {
                        goldVein.insert(goldVein.begin(), entity.getID());
                    }
                }
                else {
                    goldVein.push_back(entity.getID());
                }
            }
        }
        return;
    }

    ActionAssignment BasicTBSAgent::computeAction(GameState state, const ForwardModel& forwardModel, Timer timer){
        //std::cout<<"Start BasicTBSAgent\n";
        enemy_city_id = -1;
        self_city_id = -1;
        //std::cout<<"Start Rule-based TwoKingdoms Agent\n";
        if (dis(getRNGEngine()) > 0.95) {//87
            //std::cout<<"enter random action\n";
            auto actions = forwardModel.generateActions(state, getPlayerID());
            boost::random::uniform_int_distribution<size_t> actionDist(0, actions.size() - 1);
            auto actionIndex = actionDist(getRNGEngine());
            auto action = actions.at(actionIndex);
            //std::cout<<"select random agent\n";
            return ActionAssignment::fromSingleAction(action);
        }
        // std::cout<<"DEBUG start basicTBSAgent" <<"\n";
        // get position of all entities
        std::map< int, Vector2f > positions;
        //std::set< int > opponentEntites = std::set< int >();
        std::set< int > playerEntities = std::set< int >();

        //std::cout<<"bbbbbbbbbbbbbbbb\n";
        double nearestEnemyAttackerDistance = 1000000.0;
        Entity* nearestEnemyAttacker = nullptr;

        enemy_city_id = -1;
        self_city_id = -1;

        for(auto& entity : state.getEntities()) {
            auto& entityType = state.getGameInfo()->getEntityType(entity.getEntityTypeID());
            if(entity.getOwnerID() != getPlayerID())
            {
                //opponentEntites.insert(entity.getID());

                //if(entityType.getName() == "Warrior" || entityType.getName() == "Archer"
                //   || entityType.getName() == "Catapult") {
                if(entityType.getName() == "City" ) {
                    enemy_city_id = entity.getID();
                    enemy_city.x= entity.x();
                    enemy_city.y = entity.y();
                    //int paramID = 0;
                }
            } else if(entity.getOwnerID() == getPlayerID()) {
                if(entityType.getName() == "City") {
                    self_city_id = entity.getID();
                    self_city.x = entity.x();
                    self_city.y = entity.y();
                }
            }
            else {} // not owned entity such as goldVein
        }
        if (self_city_id == -1) {
            auto actions = forwardModel.generateActions(state, getPlayerID());
            boost::random::uniform_int_distribution<size_t> actionDist(0, actions.size() - 1);
            auto actionIndex = actionDist(getRNGEngine());
            auto action = actions.at(actionIndex);
            return ActionAssignment::fromSingleAction(action);
        }

        for(auto& entity : state.getEntities()) {
            auto& entityType = state.getGameInfo()->getEntityType(entity.getEntityTypeID());
            if(entity.getOwnerID() != getPlayerID())
            {
                //opponentEntites.insert(entity.getID());

                //if(entityType.getName() == "Warrior" || entityType.getName() == "Archer"
                //   || entityType.getName() == "Catapult") {
                if(entityType.getName() == "Warrior" ){
                   double distance = entity.getPosition().distance(self_city);
                   if (distance < nearestEnemyAttackerDistance) {
                      nearestEnemyAttackerDistance = distance;
                      nearestEnemyAttacker = &entity;
                   }
                } 
            } else if(entity.getOwnerID() == getPlayerID()) {
                playerEntities.insert(entity.getID());
                if(entityType.getName() == "Worker" ){
                    if((workers.size()==0) || std::find(workers.begin(), workers.end(), entity.getID())==workers.end()){
                        workers.push_back( entity.getID());
                    }
                }else if(entityType.getName() == "Barracks" ){
                    if((barracks.size()==0) || std::find(barracks.begin(), barracks.end(), entity.getID())==barracks.end()){
                        barracks.push_back( entity.getID());
                    }
                }
                else if (entityType.getName() == "Warrior") {
                    if( std::find(farAwarAttackable.begin(), farAwarAttackable.end(),  entity.getID() ) 
                        == farAwarAttackable.end()) 
                    {
                        if (farAwarAttackable.size() < 2) {
                            farAwarAttackable.push_back(entity.getID());
                        }
                        else if (std::find(nearbyAttackable.begin(), 
                                    nearbyAttackable.end(), 
                                    entity.getID()) == nearbyAttackable.end())
                        {
                            nearbyAttackable.push_back(entity.getID());
                        }

                    }
                }
            }
            else {} // not owned entity such as goldVein
        }

        //std::cout<<"aaaaaaaaaaaaa\n";

        // maintain entity vectors, delete dead unit;
        for (int e_id = farAwarAttackable.size()-1 ; e_id >=0; e_id--){
            if(std::find(playerEntities.begin(), playerEntities.end(), farAwarAttackable[e_id]) == playerEntities.end()){
                farAwarAttackable.erase(farAwarAttackable.begin()+e_id);
            }
        }
        //std::cout<<"000000000000\n";
        for (int e_id = nearbyAttackable.size()-1 ; e_id >=0; e_id--){
            if(std::find(playerEntities.begin(), playerEntities.end(), nearbyAttackable[e_id]) == playerEntities.end()){
                nearbyAttackable.erase(nearbyAttackable.begin()+e_id);
            }
        }
        //std::cout<<"11111111111111111\n";
        for (int e_id = workers.size()-1 ; e_id >=0; e_id--){
            if(std::find(playerEntities.begin(), playerEntities.end(), workers[e_id]) == playerEntities.end()){
                workers.erase(workers.begin()+e_id);
            }
        }
        //std::cout<<"222222222222\n";
        for(int e_id = goldVeinPosition.size()-1 ; false && e_id >=0; e_id--){
            //TODO@if gold resource is exhausted, remove this goldVein from the vectors;
            // for now, it seems not many operation depends on gold, can implement it later
        }
        //std::cout<<"3333333333333333\n";

        //const std::vector<Action> actionSpace = forwardModel.generateActions(state, getPlayerID());

        //Spawn worker
        if( (workers.size()) < 1 && (getProduction(state) > 3) && self_city_id!= -1){
            auto actionSpace_tmp = forwardModel.generateUnitActions(state, 
                                    *(state.getEntity(self_city_id)), getPlayerID(), false);
            if (actionSpace_tmp.size() > 0){
                int a_idx = filterSpawnWorker(state, actionSpace_tmp);

                if (a_idx != -1){
                    //std::cout<<"spawn worker\n";
                    return ActionAssignment::fromSingleAction(actionSpace_tmp[a_idx]);
                }
            }
        }
        //std::cout<<"444444444444\n";

		// worker go for mining
		for(auto worker_id:workers){
			auto actionSpace_tmp = forwardModel.generateUnitActions(state, *(state.getEntity(worker_id)), 
				                                                    getPlayerID(), false);
			//state.printActionInfo(actionSpace_tmp[0]);
			// if can mine, mine
			int optimal_a_idx = moveAndAct(state, actionSpace_tmp, goldVein[0], "Mine");
			if (optimal_a_idx > 0) {
                //std::cout<<"return mining\n";
				return ActionAssignment::fromSingleAction(actionSpace_tmp[optimal_a_idx]);
			}
		}


		double prod = getProduction(state), gold = getGold(state);

		auto playerActionSpace = forwardModel.generatePlayerActions(state, getPlayerID(), false);
        /*
        for (auto a : playerActionSpace) {
            state.printActionInfo(a);
            if (a.getActionFlag() == ActionFlag::AbortContinuousAction) {
                    continue;
                }
            if ( a.getActionType().getName() == "Research") {
                std::cout<<"getname succeeded\n";
            }
            
        }
        */


		// do the research
		if(!(isResearchedMining) && !(prod<researchMiningProduction)){
			for (int a_idx = 0; (!isResearchedMining) && a_idx < playerActionSpace.size(); a_idx++) {
				//state.printActionInfo(actionSpace_tmp[a_idx]);
                if (playerActionSpace[a_idx].getActionFlag() == ActionFlag::AbortContinuousAction) {
                    continue;
                }
				if (playerActionSpace[a_idx].getActionType().getName() == "Research") {
					if (playerActionSpace[a_idx].getTargets()[1].getValueString(state) == "Mining") {
						isResearchedMining=true;
						//std::cout<<"--> RESEARCH Mining, player: "<< getPlayerID() << std::endl;
						return ActionAssignment::fromSingleAction(playerActionSpace[a_idx]);
					}
				}
			}
		}
        //std::cout<<"55555555555\n";

		// Research Discipline
		if(!(isResearchedDiscipline) && isResearchedMining && !(prod < researchDiciplineProduction)){
			//std::cout<<"[TwoKingdom Combat Agent]: try to research discipline\n";
			for (int a_idx = 0; (!isResearchedDiscipline) && a_idx < playerActionSpace.size(); a_idx++) {
				// state.printActionInfo(playerActionSpace[a_idx]);
                if (playerActionSpace[a_idx].getActionFlag() == ActionFlag::AbortContinuousAction) {
                    continue;
                }
				if (playerActionSpace[a_idx].getActionType().getName() == "Research") {
					//std::cout<<"can research: " << playerActionSpace[a_idx].getTargets()[1].getValueString(state) <<"\n";
					if (playerActionSpace[a_idx].getTargets()[1].getValueString(state) == "Discipline") {
						isResearchedDiscipline=true;
						//std::cout<<"--> RESEARCH Dicipline\n";
						return ActionAssignment::fromSingleAction(playerActionSpace[a_idx]);
					}
				}
			}
		}
        //std::cout<<"666666666666\n";

		// build barracks
		if (isResearchedMining && isResearchedDiscipline && !(isBuiltBarraks) && !(prod<buildBarracksProduction)) { 


			for (int a_idx = 0; a_idx < playerActionSpace.size(); a_idx++) {
                if (playerActionSpace[a_idx].getActionFlag() == ActionFlag::AbortContinuousAction) {
                    continue;
                }

				if (playerActionSpace[a_idx].getActionType().getName() == "Build") {
					if (playerActionSpace[a_idx].getTargets()[1].getValueString(state) == "Barracks") {
						isBuiltBarraks=true;

						//std::cout<<"--> BUILD Barracks\n";
						return ActionAssignment::fromSingleAction(playerActionSpace[a_idx]);
					}
				}
			}
		}
        //std::cout<<"7777777777777\n";

		// spawn warriors and merge in farAwaryAttackable
		if (isBuiltBarraks && (farAwarAttackable.size() < 2) && !(gold<spawnWarriorGold) ) {
			// std::cout<<"try spawn warrior"<<std::endl;
			auto actionSpace_tmp = forwardModel.generateUnitActions(state, 
									*(state.getEntity(barracks[0])), getPlayerID(), false);
			if (actionSpace_tmp.size() > 0){
				int a_idx = filterSpawnWarrior(state, actionSpace_tmp);
				if (a_idx != -1){
					//std::cout<<"SPAWN WARRIOR\n";
					return ActionAssignment::fromSingleAction(actionSpace_tmp[a_idx]);
				}
			}
		}
        //std::cout<<"8888888888\n";

		if (!(nearestEnemyAttackerDistance > enemyAttackerRangeTolerate)) {
			if (nearbyAttackable.size() > 0) {
				//TODO@send nearbyAttackable unit for this nearby enemy
			}
		}
        //std::cout<<"999999999999999\n";
		// send warrior to attack nenemy city
		if (farAwarAttackable.size() > 0 && enemy_city_id != -1) {
			for (auto attackerID: farAwarAttackable){
				auto actionSpace_tmp = forwardModel.generateUnitActions(state, *(state.getEntity(attackerID)), 
					                                                    getPlayerID(), false);

				int optimal_a_idx = moveAndAct(state, actionSpace_tmp, enemy_city_id, "Attack");

				if (optimal_a_idx > 0) {
                    //std::cout<< actionSpace_tmp.size() <<" " << optimal_a_idx << "\n";
					return ActionAssignment::fromSingleAction(actionSpace_tmp[optimal_a_idx]);
				}
			}
		}
        //std::cout<<"10101010101010\n";
		
		//std::cout <<"END THE TURN, prod: " << prod << ", gold: " << gold << ", neaby: "<< nearbyAttackable.size() << ", farAway:"<< farAwarAttackable.size() << "\n";
		//std::cout <<"isresearchedMining: " << isResearchedMining << " isResearchedDiscipline: " << isResearchedDiscipline << "\n";
		//std::cout <<"END THE TURN" << "\n";
		//state.printBoard();
		//return ActionAssignment::fromSingleAction( Action::createEndAction(state.getCurrentTBSPlayer()));

		return ActionAssignment::fromSingleAction( Action::createEndAction(getPlayerID()));
    }

    bool BasicTBSAgent::existAttackableEnemyNearby(double nearestEnemyAttackerDistance){
        if(nearestEnemyAttackerDistance < enemyAttackerRangeTolerate) {
            return true;
        }
        return false;
    }


	double BasicTBSAgent::getProduction(GameState state){
		return state.getPlayerParameter(getPlayerID(), "Prod");
	}

	double BasicTBSAgent::getGold(GameState state){
		return state.getPlayerParameter(getPlayerID(), "Gold");
	}

	double BasicTBSAgent::getScore(GameState state){
		return state.getPlayerParameter(getPlayerID(), "Score");
	}

	// only for spawn, build
	int BasicTBSAgent::filterAction(GameState state, std::vector<Action> actionSpace, std::string actionT, std::string targetName) {
		if (actionT != "Spawn" && actionT != "Build") {
			std::cout<<"[ERROR] filterAction suppors argument actionT in [Spawn, Build]\n";
		}
		for(int a_idx = 0 ; a_idx < actionSpace.size(); a_idx++ ){
			const ActionType& actionType = actionSpace[a_idx].getActionType();
			
			if (actionType.getName() == actionT){
				//std::cout<< "DEBUG 2f0jien  "<< action.getTargets().size() << std::endl;
				if (actionSpace[a_idx].getTargets()[1].getValueString(state) == targetName ){
					return a_idx;
				}
			}
		}
		return -1;
	}

	int BasicTBSAgent::filterSpawnWorker(GameState state, std::vector<Action> actionSpace){
		return filterAction(state, actionSpace, "Spawn", "Worker");
	}

	int BasicTBSAgent::filterSpawnWarrior(GameState state, std::vector<Action> actionSpace) {
		return filterAction(state, actionSpace, "Spawn", "Warrior");
	}

	// controll a unit to a target unit and action toward that target
	int BasicTBSAgent::moveAndAct(GameState state, std::vector<Action> actionSpace, int targetEntityID, std::string actionT ) {
		if (actionT != "Attack" && actionT != "Mine") {
			std::cout<<"[ERROR] moveAndAct suppors argument actionT in [Attack, Mine]\n";
		}
		double minimumDistanceToTarget = 1000000.0;
		int optimal_a_idx = -1;
			
		for (int a_idx = 0 ; a_idx < actionSpace.size(); a_idx++) {
            if (actionSpace[a_idx].getActionFlag() == ActionFlag::AbortContinuousAction) {
                    continue;
            }
			std::string a_type_name = actionSpace[a_idx].getActionType().getName();
			if (a_type_name == actionT) {
				return a_idx;
			}
			else if (a_type_name == "Move") { // TODO@Ethan, guide unit go surround the barriers
				//state.printActionInfo(actionSpace[a_idx]);
				auto pos = actionSpace[a_idx].getTargets()[1].getPosition();

				double distance_tmp = pos.distance(state.getEntity(targetEntityID)->getPosition());

				if (distance_tmp < minimumDistanceToTarget) {
					minimumDistanceToTarget = distance_tmp;
					optimal_a_idx = a_idx;
				}
			}
		}
		return optimal_a_idx;
	}
}