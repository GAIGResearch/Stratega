#include <Stratega/Agent/Heuristic/TwoKingdomsDefenceHeuristic.h>


namespace SGA {

	TwoKingdomsDefenceHeuristic::TwoKingdomsDefenceHeuristic(int playerID, GameState initialState) {
		/*
		* Record city, goldVein positions
		*/
		initialized = true;
		goldVeinPosition = std::vector<Vector2f>();
		int BOARD_WIDTH = initialState.getBoardWidth();
		maximumBoardDistance = std::sqrt(2*BOARD_WIDTH*BOARD_WIDTH);

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

                goldVeinPosition.push_back(entity.getPosition());
                //std::cout<<"goldVein pos: "<< entity.getPosition().x <<  " " << entity.getPosition().y <<std::endl;
                currentTargetGoldVeinIdx = 0;
            }

        }

        //std::cout<< "Ends Initialize BasicTBSHeuristic" << std::endl;
        return;
    }

    void TwoKingdomsDefenceHeuristic::updateStateInfo(GameState& state, const int playerID) {
        //std::cout<< "Enter updateStateInfo" << std::endl;
        selfWarriorCityDistance = 100000.0;

        isResearchedMining = state.isResearched(playerID, "Mining");
        isResearchedDicipline = state.isResearched(playerID, "Discipline");

        auto entities = state.getEntities();
        selfHasWorker = false;
        isBuiltBarracks = false;

        bool tmp_isBuiltBarracks=false;
        for (auto entity : entities) {
            auto& entityType = state.getGameInfo()->getEntityType(entity.getEntityTypeID());
            auto entityName = entityType.getName();
            if (entity.getOwnerID() == playerID) {
                
                if (entityName == "Barracks") {
                    isBuiltBarracks=true;
                } else if (entityName == "Warrior" || entityName == "Archer") {
                    selfHasWarrior = true;
                    int tmp_selfWarriorCityDistance = entity.getPosition().distance(self_city);
                    if (selfWarriorCityDistance > tmp_selfWarriorCityDistance) {
                        selfWarriorCityDistance = tmp_selfWarriorCityDistance;
                    }
                    if (entityName == "Worker") {
                        selfHasWorker = true;
                    }
                }
            }
            else {
                auto& entityType = state.getGameInfo()->getEntityType(entity.getEntityTypeID());
                if (entityName == "Warrior" || entityName == "Archer") {
                    enemyHasWarrior = true;
                    int tmp_enemyWarriorCityDistance = entity.getPosition().distance(self_city);
                    if (enemyWarriorCityDistance > tmp_enemyWarriorCityDistance) {
                        enemyWarriorCityDistance = tmp_enemyWarriorCityDistance;
                        if (enemyWarriorCityDistance <= 5.0) {
                            enemyWarriorHealth = entity.getParameter("Health");
                        }
                    }
                } 
            }

        }

        return;
    }

    /*
    * 1) Check the status of technology, reward for each new technology
    * 2) Defend player's base.
    */
    double TwoKingdomsDefenceHeuristic::evaluateGameState(const ForwardModel& forwardModel, GameState& state, const int playerID) {
		return evaluateGameState(forwardModel, state, playerID, false);
	}

    double TwoKingdomsDefenceHeuristic::evaluateGameState(const ForwardModel& forwardModel, GameState& state,
        const int playerID, bool isDebug) {
        //std::cout<<"End Technology Heuristic"<<"\n";

        double ret = 50;
        // golden reaches 80? for warrior spawning
        // technology of Discpline developed? for warrior spawning
        // barrack

        if (isResearchedMining) {
            ret += 5.0;
            if (isResearchedDicipline) {
                ret += 10;
            }
        }

        if (isBuiltBarracks) {
            ret += 10.0;
        }

        if (selfHasWorker) {
            ret+=5.0;
        }

        // have attackable unit.
        if (selfHasWarrior) {
            ret += 10.0;
            if (selfWarriorCityDistance <= 5.0) {
                ret += 5.0;
            }
        }
        else { // maximum possible reward is 8 (less than spawn warrior that is 10
            ret += 4*(1.0- min_dis/maximumBoardDistance);

            if (getGold(state) >= 80) {
                ret += 4;
            }
            else {
                ret += 4 * (getGold(state)/80.0);
            }
        }

        if (enemyHasWarrior) {
            ret -= 10;

            if (enemyWarriorCityDistance <= 5.0) {
                ret -= 35 * (enemyWarriorHealth/FULL_HEALTH);
                if (!selfHasWarrior) {
                    ret -= 5;
                }
            }
        }

        if (isDebug) {
            std::cout<< " Mining: " << isResearchedMining << " Discipline: "<< isResearchedDicipline << " barracks: "<< isBuiltBarracks << "\n"
            << " selfHasWorker: " << selfHasWorker << " Gold: " << getGold(state) << " /80\n"
            << " selfHasWarrior: "<<  selfHasWarrior <<" enemyHasWarrior: "<<  enemyHasWarrior <<" enemyWarriorCityDistance: "<<  enemyWarriorCityDistance 
            << " enemy warrior health penalty: "<< 35 * (enemyWarriorHealth / FULL_HEALTH) << "\n"
            << " defence reward: " << ret/100.0 << "\n";
        }

        return ret/100.0;
    }


    double TwoKingdomsDefenceHeuristic::getProduction(GameState state){
        return state.getPlayerParameter(state.getCurrentTBSPlayer(), "Prod");
    }

    double TwoKingdomsDefenceHeuristic::getGold(GameState state){
        return state.getPlayerParameter(state.getCurrentTBSPlayer(), "Gold");
    }

    double TwoKingdomsDefenceHeuristic::getEntityHealth(GameState state, Vector2f position) {
        auto target_entity = state.getEntityAt(position);
        double paramsValue = target_entity->getParameter("Health");
        if (paramsValue >= 0.0) {
            return paramsValue;
        }
        //std::cout<<"[Error]: passed in invalid parameter name to search"<<std::endl;
    }
}