#include <Stratega/Agent/RuleBasedAgents/BasicTBSAgent.h>


namespace SGA {

	void BasicTBSAgent::init(GameState initialState, const ForwardModel& forwardModel, Timer timeBudgetMs) {
		initialized = true; // This function is called by GameRunner
		workers = std::vector<int>();
		nearbyAttackable = std::vector<int>();
		farAwarAttackable = std::vector<int>();
		goldVein = std::vector<int>();
		goldVeinPosition = std::vector<Vector2f>();

		bool foundEnemyCity = false, foundSelfCity=false;
		// store the position of self base and enermy base;
        for(const auto& entity : initialState.getEntities()) {
			
            auto& entityType = initialState.getGameInfo()->getEntityType(entity.getEntityTypeID());
			//std::cout<<entityType.getName() <<std::endl;
            if(entity.getOwnerID()!= initialState.getCurrentTBSPlayer() && (!foundEnemyCity)) 
            {
                if(entityType.getName() == "City") {
					foundEnemyCity = true; //save some time
					enemy_city_id = entity.getID();
                    enemy_city.x= entity.x();
                    enemy_city.y = entity.y();
                    int paramID = 0;
                }
            } else if(entity.getOwnerID() == initialState.getCurrentTBSPlayer()){
               if(entityType.getName() == "City" && (!foundSelfCity)) {
				    foundSelfCity = true;
					self_city_id = entity.getID();
                    self_city.x = entity.x();
                    self_city.y = entity.y();
               }
            } else {

			}
        }
		for(const auto& entity : initialState.getEntities()) { // make the nearest goldvein as the head
			auto& entityType = initialState.getGameInfo()->getEntityType(entity.getEntityTypeID());
			if(entityType.getName() == "GoldVein"){
				if (goldVein.size() > 0){
					double dis1 = initialState.getEntity(goldVein[0])->getPosition().distance(initialState.getEntity(self_city_id)->getPosition());
					double dis2 = entity.getPosition().distance(initialState.getEntity(self_city_id)->getPosition());
					if(!(dis1 > dis2) ){
						goldVein.push_back(entity.getID());
					} else {
                        goldVein.insert(goldVein.begin(), entity.getID());
					}
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
		
		std::cout<<"DEBUG start basicTBSAgent" <<std::endl;
		// get position of all entities
        std::map< int, Vector2f > positions;
        std::set< int > opponentEntites = std::set< int >();
        std::set< int > playerEntities = std::set< int >();
		
        double nearestEnemyAttackerDistance = 1000000.0;
        Entity* nearestEnemyAttacker = nullptr;
        for(auto& entity : state.getEntities()) {
			auto& entityType = state.getGameInfo()->getEntityType(entity.getEntityTypeID());
            if(entity.getOwnerID() != state.getCurrentTBSPlayer())
            {
                opponentEntites.insert(entity.getID());

                
                //if(entityType.getName() == "Warrior" || entityType.getName() == "Archer"
                //   || entityType.getName() == "Catapult") {
				if(entityType.getName() == "Warrior" ){// as a preliminary version to save some computing time
                   double distance = entity.getPosition().distance(self_city);
                   if (distance < nearestEnemyAttackerDistance) {
                      nearestEnemyAttackerDistance = distance;
                      nearestEnemyAttacker = &entity;
                   }
                }
            } else if(entity.getOwnerID() == state.getCurrentTBSPlayer()) {
                playerEntities.insert(entity.getID());
				if(entityType.getName() == "Worker" ){
					if(workers.size() || std::find(workers.begin(), workers.end(), entity.getID())==workers.end()){
						workers.push_back( entity.getID());
					}
				}

            } else { // not owned entity such as goldVein
				//
			}
        }

		// maintain entity vectors;
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
			//if gold resource is exhausted, remove this goldVein from the vectors;
			// TODO@ make it a ranked vector
		}
		
		const std::vector<Action> actionSpace = forwardModel.generateActions(state, getPlayerID());

		if(workers.size() < 2 && getProduction(state) > 3){
			//Spawn worker
			auto actionSpace_tmp = forwardModel.generateUnitActions(state, *(state.getEntity(self_city_id)), false);
			if (actionSpace_tmp.size() > 0){
				int a_idx = filterSpawnWorker(state, actionSpace_tmp);

				if (a_idx != -1){
					return ActionAssignment::fromSingleAction(actionSpace[a_idx]);
				}
			}
		}
		
		for(auto worker_id:workers){
			auto actionSpace_tmp = forwardModel.generateUnitActions(state, *(state.getEntity(worker_id)), false);
			state.printActionInfo(actionSpace_tmp[0]);
			// if can mine, mine
			// if not, head to first mine
		}
		
		// do the research
		auto actionSpace_tmp = forwardModel.generatePlayerActions(state, getPlayerID(), false);

		/*
		// check if there are attackable enemy nearby, if there are, get closest warrior to attack it
        if(existAttackableEnemyNearby(nearestEnemyAttackerDistance)) {
            // for all self attackable units, if movable, move towards the target unit and attack
		    if(existNearbySelfAttackable()){
				for (nearbySelfAttackable) {
					move_and_attack;
				}
			} else if (existFarAwaySelfAttackable()){
				for (farAwaySelfAttackable) {
					move_and_attack;
				}
			} else {
				//spawn attackable
			}
        } else if(numSelfFarAwayWarrior() < 2) {
		    if(existNearbySelfAttackable()){
				//move the farest nearby attackable toward the far
			}
        } else if(numSelfNearbyAttackable() < 2) {
			if (enoughGoldForWarrior()){
				// spawn warrior;
			}
           
        }
		
		// check if there are 2 worker there; if yes, keep them mining
		// else: spaw worker
		*/
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

	int BasicTBSAgent::filterSpawnWorker(GameState state, std::vector<Action> actionSpace){
		// suppose the actionSpace is an eneity action space
		for(int a_idx = 0 ; a_idx < actionSpace.size(); a_idx++ ){
			const ActionType& actionType = actionSpace[a_idx].getActionType();
			std::cout<<actionType.getName() <<std::endl;
			if (actionType.getName() == "Spawn"){
				//std::cout<< "DEBUG 2f0jien  "<< action.getTargets().size() << std::endl;
				if (actionSpace[a_idx].getTargets()[1].getValueString(state) == "Worker" ){
					return a_idx;
				}
			}
		}
		return -1;
	}
}