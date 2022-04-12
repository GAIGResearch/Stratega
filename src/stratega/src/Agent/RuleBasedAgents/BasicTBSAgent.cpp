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
                    int paramID = 0;
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

		// make the nearest goldvein as the head of vector
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

		//std::sort(goldVein.begin(), goldVein.end(), )
		return;
	}

	bool BasicTBSAgent::existAttackableEnemyNearby(double nearestEnemyAttackerDistance){
		if(nearestEnemyAttackerDistance < enemyAttackerRangeTolerate) {
			return true;
		}
		return false;
	}

	ActionAssignment BasicTBSAgent::computeAction(GameState state, const ForwardModel& forwardModel, Timer timer){
		
		// std::cout<<"DEBUG start basicTBSAgent" <<std::endl;
		// get position of all entities
        std::map< int, Vector2f > positions;
        std::set< int > opponentEntites = std::set< int >();
        std::set< int > playerEntities = std::set< int >();
		
        double nearestEnemyAttackerDistance = 1000000.0;
        Entity* nearestEnemyAttacker = nullptr;
        for(auto& entity : state.getEntities()) {
			auto& entityType = state.getGameInfo()->getEntityType(entity.getEntityTypeID());
            if(entity.getOwnerID() != getPlayerID())
            {
                opponentEntites.insert(entity.getID());

                
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
					if( std::find(farAwarAttackable.begin(), 
							      farAwarAttackable.end(), 
					              entity.getID()) == farAwarAttackable.end()) {
						if (farAwarAttackable.size() < 2) {
							farAwarAttackable.push_back(entity.getID());
						}
						else if (std::find(nearbyAttackable.begin(), 
							      nearbyAttackable.end(), 
					              entity.getID()) == nearbyAttackable.end()) {
							nearbyAttackable.push_back(entity.getID());
						}

					}
				}
				
			}
            else { // not owned entity such as goldVein
				//
			}
        }

		// maintain entity vectors, delete dead unit;
		for (int e_id = farAwarAttackable.size()-1 ; e_id >=0; e_id--){
			if(std::find(playerEntities.begin(), playerEntities.end(), farAwarAttackable[e_id]) == playerEntities.end()){
				farAwarAttackable.erase(farAwarAttackable.begin()+e_id);
			}
		}
		for (int e_id = nearbyAttackable.size()-1 ; e_id >=0; e_id--){
			if(std::find(playerEntities.begin(), playerEntities.end(), nearbyAttackable[e_id]) == playerEntities.end()){
				nearbyAttackable.erase(nearbyAttackable.begin()+e_id);
			}
		}
		for (int e_id = workers.size()-1 ; e_id >=0; e_id--){
			if(std::find(playerEntities.begin(), playerEntities.end(), workers[e_id]) == playerEntities.end()){
				workers.erase(workers.begin()+e_id);
			}
		}
		for(int e_id = goldVeinPosition.size()-1 ; false && e_id >=0; e_id--){
			//TODO@if gold resource is exhausted, remove this goldVein from the vectors;
			// for now, it seems not many operation depends on gold, can implement it later
		}
		
		//const std::vector<Action> actionSpace = forwardModel.generateActions(state, getPlayerID());
		
		//Spawn worker
		if( (workers.size()) < 1 && (getProduction(state) > 3)){
			auto actionSpace_tmp = forwardModel.generateUnitActions(state, 
									*(state.getEntity(self_city_id)), getPlayerID(), false);
			if (actionSpace_tmp.size() > 0){
				int a_idx = filterSpawnWorker(state, actionSpace_tmp);

				if (a_idx != -1){

					return ActionAssignment::fromSingleAction(actionSpace_tmp[a_idx]);
				}
			}
		}
		
		// worker go for mining
		for(auto worker_id:workers){
			auto actionSpace_tmp = forwardModel.generateUnitActions(state, *(state.getEntity(worker_id)), 
				                                                    getPlayerID(), false);
			//state.printActionInfo(actionSpace_tmp[0]);
			// if can mine, mine
			int optimal_a_idx = moveAndAct(state, actionSpace_tmp, goldVein[0], "Mine");
			if (optimal_a_idx > 0) {
				return ActionAssignment::fromSingleAction(actionSpace_tmp[optimal_a_idx]);
			}
		}
		
		double prod = getProduction(state), gold = getGold(state);

		auto playerActionSpace = forwardModel.generatePlayerActions(state, getPlayerID(), false);

		// do the research
		if(!(isResearchedMining) && !(prod<researchMiningProduction)){
			for (int a_idx = 0; (!isResearchedMining) && a_idx < playerActionSpace.size(); a_idx++) {
				//state.printActionInfo(actionSpace_tmp[a_idx]);
				if (playerActionSpace[a_idx].getActionType().getName() == "Research") {
					if (playerActionSpace[a_idx].getTargets()[1].getValueString(state) == "Mining") {
						isResearchedMining=true;
						std::cout<<"--> RESEARCH Mining, player: "<< getPlayerID() << std::endl;
						return ActionAssignment::fromSingleAction(playerActionSpace[a_idx]);
					}
				}
			}
		}

		// Research Discipline
		if(!(isResearchedDiscipline) && isResearchedMining && !(prod<researchDiciplineProduction)){
			std::cout<<"try to research discipline" << std::endl;
			for (int a_idx = 0; (!isResearchedDiscipline) && a_idx < playerActionSpace.size(); a_idx++) {
				//state.printActionInfo(actionSpace_tmp[a_idx]);
				if (playerActionSpace[a_idx].getActionType().getName() == "Research") {
					std::cout<<"can research: " << playerActionSpace[a_idx].getTargets()[1].getValueString(state) << std::endl;
					if (playerActionSpace[a_idx].getTargets()[1].getValueString(state) == "Discipline") {
						isResearchedDiscipline=true;
						std::cout<<"--> RESEARCH Dicipline" << std::endl;
						return ActionAssignment::fromSingleAction(playerActionSpace[a_idx]);
					}
				}
			}
		}
		
		// build barracks
		if (isResearchedMining && isResearchedDiscipline && !(isBuiltBarraks) && !(prod<buildBarracksProduction)) { 
			for (int a_idx = 0; a_idx < playerActionSpace.size(); a_idx++) {
				state.printActionInfo(playerActionSpace[a_idx]);
				if (playerActionSpace[a_idx].getActionType().getName() == "Build") {
					if (playerActionSpace[a_idx].getTargets()[1].getValueString(state) == "Barracks") {
						isBuiltBarraks=true;

						std::cout<<"--> BUILD Barracks" << std::endl;
						return ActionAssignment::fromSingleAction(playerActionSpace[a_idx]);
					}
				}
			}
		}

		
		// spawn warriors and merge in farAwaryAttackable
		if (isBuiltBarraks && (farAwarAttackable.size() < 2) && !(gold<spawnWarriorGold) ) {
			// std::cout<<"try spawn warrior"<<std::endl;
			auto actionSpace_tmp = forwardModel.generateUnitActions(state, 
									*(state.getEntity(barracks[0])), getPlayerID(), false);
			if (actionSpace_tmp.size() > 0){
				int a_idx = filterSpawnWarrior(state, actionSpace_tmp);
				if (a_idx != -1){
					std::cout<<"SPAWN WARRIOR"<<std::endl;
					return ActionAssignment::fromSingleAction(actionSpace_tmp[a_idx]);
				}
			}
		}

		if (!(nearestEnemyAttackerDistance > enemyAttackerRangeTolerate)) {
			if (nearbyAttackable.size() > 0) {
				//TODO@send nearbyAttackable unit for this nearby enemy
			}
		}

		
		// send warrior to attack nenemy city
		if (farAwarAttackable.size() > 0) {
			for (auto attackerID: farAwarAttackable){
				auto actionSpace_tmp = forwardModel.generateUnitActions(state, *(state.getEntity(attackerID)), 
					                                                    getPlayerID(), false);
			
				int optimal_a_idx = moveAndAct(state, actionSpace_tmp, enemy_city_id, "Attack");
				if (optimal_a_idx > 0) {
					return ActionAssignment::fromSingleAction(actionSpace_tmp[optimal_a_idx]);
				}
			}
		}
		
		std::cout <<"END THE TURN, prod: " << prod << ", gold: " << gold << ", neaby: "<< nearbyAttackable.size() << ", farAway:"<< farAwarAttackable.size() << std::endl;
		std::cout <<"isresearchedMining: " << isResearchedMining << " isResearchedDiscipline: " << isResearchedDiscipline << std::endl;
		//std::cout <<"END THE TURN" << std::endl;
		state.printBoard();
		//return ActionAssignment::fromSingleAction( Action::createEndAction(state.getCurrentTBSPlayer()));
		return ActionAssignment::fromSingleAction( Action::createEndAction(getPlayerID()));

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
			std::cout<<"[ERROR] filterAction suppors argument actionT in [Spawn, Build]" << std::endl;
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
			std::cout<<"[ERROR] moveAndAct suppors argument actionT in [Attack, Mine]" << std::endl;
		}
		double minimumDistanceToTarget = 1000000.0;
		int optimal_a_idx = -1;
			
		for (int a_idx = 0 ; a_idx < actionSpace.size(); a_idx++) {
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