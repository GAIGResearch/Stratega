#include <Stratega/Agent/Heuristic/BasicTBSTechnologyHeuristic.h>


namespace SGA {

	BasicTBSTechnologyHeuristic::BasicTBSTechnologyHeuristic(int playerID, GameState initialState) {
		//std::cout<< "Initialize BasicTBSTechnologyHeuristic" << std::endl;
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
        }
		//initialState.isResearched(playerID, "");
		//std::cout<< "Ends Initialize BasicTBSHeuristic" << std::endl;
		return;
	}

	void BasicTBSTechnologyHeuristic::updateStateInfo(GameState& state, const int playerID) {
		//std::cout<< "Enter updateStateInfo" << std::endl;
		isResearchedMining = state.isResearched(playerID, "Mining");
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
		return;
	}
	/*
	* 1) Check the status of technology, reward for each new technology
	* 2) Defend player's base.
	*/
	double BasicTBSTechnologyHeuristic::evaluateGameState(const ForwardModel& forwardModel, GameState& state, const int playerID) {
		//std::cout<< "Starts evaluateGameState" << std::endl;
		if (!initialized) {
			std::cout<<"[ERROR]: Class Heuristic is not initialized!" << std::endl;
		}
		updateStateInfo(state, playerID);

		double reward = 15.0; // there is a reduction about enermy base health point, initialize 15 avoid negative number.
		if (isResearchedMining) {
			reward += 20.0;
			if (isResearchedDicipline) {
				reward += 20;
			}
		}

		if (isBuiltBarracks) {
			reward += 20.0;
		}

		// have attackable unit.
		if (ableToAttack) {
			reward += 10.0;
		}
		double selfCityHealth = getEntityHealth(state, self_city);
		double enemyCityHealth = getEntityHealth(state, enemy_city);

		// 15% for self base health
		selfCityHealth /= city_max_health;
		reward += selfCityHealth*15.0;

		// 15% for enemy base health
		enemyCityHealth /= city_max_health;
		reward -= enemyCityHealth*15.0;

		//std::cout<< "Ends evaluateGameState" << std::endl;
		return reward/100.0;
	}


	double BasicTBSTechnologyHeuristic::getProduction(GameState state){
		return state.getPlayerParameter(state.getCurrentTBSPlayer(), "Prod");
	}

	double BasicTBSTechnologyHeuristic::getGold(GameState state){
		return state.getPlayerParameter(state.getCurrentTBSPlayer(), "Gold");
	}

	double BasicTBSTechnologyHeuristic::getEntityHealth(GameState state, Vector2f position) {
		auto target_entity = state.getEntityAt(position);
		double paramsValue = target_entity->getParameter("Health");
		if (paramsValue >= 0.0) {
			return paramsValue;
		}
		//std::cout<<"[Error]: passed in invalid parameter name to search"<<std::endl;
	}

	
    
}