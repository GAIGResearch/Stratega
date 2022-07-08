#include <Stratega/Agent/Heuristic/BasicTBSResourceHeuristic.h>


namespace SGA {

	BasicTBSResourceHeuristic::BasicTBSResourceHeuristic(int playerID, GameState initialState) {
		/*
		* count all gold in the map
		*/

		initialized = true;
		
		for(const auto& entity : initialState.getEntities()) {
			bool foundEnemyCity = false, foundSelfCity = false;
			
            auto& entityType = initialState.getGameInfo()->getEntityType(entity.getEntityTypeID());

            if(entity.getOwnerID() != playerID ) 
            {
                if(entityType.getName() == "City" && (!foundEnemyCity)) {
					foundEnemyCity = true; //save some time
					enemy_city_id = entity.getID();
                    enemy_city.x = entity.x();
                    enemy_city.y = entity.y();
                    int paramID = 0;
					city_max_health = getEntityHealth(initialState, enemy_city);
                }
            } else if(entity.getOwnerID() == playerID){
               if(entityType.getName() == "City" && (!foundSelfCity)) {
				    foundSelfCity = true;
					self_city_id = entity.getID();
                    self_city.x = entity.x();
                    self_city.y = entity.y();
               }
            }

			if (entityType.getName() == "GoldVein") {
				totalGold += entity.getParameter("Gold");
			}
        }
		
		//std::cout<< "Ends Initialize BasicTBSHeuristic" << std::endl;
		return;
	}

	void BasicTBSResourceHeuristic::updateStateInfo(GameState& state, const int playerID) {
		//std::cout<< "Enter updateStateInfo" << std::endl;
		/*isResearchedMining = state.isResearched(playerID, "Mining");
		isResearchedDicipline = state.isResearched(playerID, "Discipline");
		auto entities = state.getEntities();

		bool tmp_isBuiltBarracks=false;
		for (auto entity : entities) {
			auto& entityType = state.getGameInfo()->getEntityType(entity.getEntityTypeID());
			if (entityType.getName() == "Barracks") {
				tmp_isBuiltBarracks=true;
			} else if (entityType.getName() == "Warrior") {
				ableToAttack = true;
			}
		}

		if (tmp_isBuiltBarracks) {
			isBuiltBarracks=true;
		}else {
			isBuiltBarracks=false;
		}
		//std::cout<< "Ends updateStateInfo" << std::endl;
		*/
		return;
	}

	double BasicTBSResourceHeuristic::evaluateGameState(const ForwardModel& forwardModel, GameState& state, const int playerID) {
		/*
		* encourage spawning worker: r_existWorker = 0.2
		* encorage mining the gold: r_gold = minedGold/totalGold*0.8
		*/
		
		
		auto entities = state.getEntities();
		double reward = 0.0, minedGold=0.0;

		bool tmp_isWorkerExists=false;
		for (auto entity : entities) {
			auto& entityType = state.getGameInfo()->getEntityType(entity.getEntityTypeID());
			if (entityType.getName() == "Worker") {
				tmp_isWorkerExists=true;
			}
		}
		minedGold = getGold(state);
		

		if (tmp_isWorkerExists) {
			reward += R_WORKER;
		}
		reward += R_GOLDPROPORTION * (minedGold/totalGold);

		return reward;
		
	}


	double BasicTBSResourceHeuristic::getProduction(GameState state){
		return state.getPlayerParameter(state.getCurrentTBSPlayer(), "Prod");
	}

	double BasicTBSResourceHeuristic::getGold(GameState state){
		return state.getPlayerParameter(state.getCurrentTBSPlayer(), "Gold");
	}

	double BasicTBSResourceHeuristic::getEntityHealth(GameState state, Vector2f position) {
		auto target_entity = state.getEntityAt(position);
		double paramsValue = target_entity->getParameter("Health");
		if (paramsValue >= 0.0) {
			return paramsValue;
		}
		//std::cout<<"[Error]: passed in invalid parameter name to search"<<std::endl;
	}
}