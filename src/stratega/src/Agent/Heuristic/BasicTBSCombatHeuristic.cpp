#include <Stratega/Agent/Heuristic/BasicTBSCombatHeuristic.h>


namespace SGA {

    BasicTBSCombatHeuristic::BasicTBSCombatHeuristic(int playerID, GameState initialState) {

        initialized = true;
        defenceHeuristic = std::make_unique<TwoKingdomsDefenceHeuristic>(playerID, initialState);

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
                if (entityType.getName() == "GoldVein") {
                    totalGold += entity.getParameter("Gold");

                    goldVeinPosition.push_back(entity.getPosition());

                    currentTargetGoldVeinIdx = 0;
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
        nGoldVein = goldVeinPosition.size();
		//std::cout<< "Ends Initialize BasicTBSHeuristic" << std::endl;
		return;
	}

	void BasicTBSCombatHeuristic::updateStateInfo(GameState& state, const int playerID) {
        defenceHeuristic->updateStateInfo(state, playerID);

        isResearchedMining = state.isResearched(playerID, "Mining");
        isResearchedDicipline = state.isResearched(playerID, "Discipline");
        auto entities = state.getEntities();

        selfHasWorker = false;
        min_dis = maximumBoardDistance;
        ableToAttack = false;
        enemyHasWarrior = false;
        isBuiltBarracks = false;
        currentTargetGoldVeinIdx = -1;

        for (int i = 0 ; i< nGoldVein; i++) {
            auto gvp = goldVeinPosition[i];
            auto e = state.getEntityAt(gvp);
            if (e->getParameter("Gold") > 0) {
                currentTargetGoldVeinIdx = i;
                break;
            }
        }

        for (auto entity : entities) {
            auto& entityType = state.getGameInfo()->getEntityType(entity.getEntityTypeID());
            auto entityName = entityType.getName();
            if (entity.getOwnerID() == playerID) {
                if (entityType.getName() == "Barracks") {
                    isBuiltBarracks=true;
                } else if (entityType.getName() == "Warrior") {
                    ableToAttack = true;
                    selfHasWarrior = true;
                } else if (entityName == "Worker") {
                    selfHasWorker = true;

                    if (currentTargetGoldVeinIdx != -1) {
                        double tmp_dis = entity.getPosition().distance(goldVeinPosition[currentTargetGoldVeinIdx]);
                        if (tmp_dis < min_dis) {
                            min_dis = tmp_dis;
                        }
                    }

                }
            }
            else {
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
        //std::cout<< "finish update\n";
        return;
    }

    double BasicTBSCombatHeuristic::evaluateGameState(const ForwardModel& forwardModel, GameState& state, const int playerID) {
        return evaluateGameState(forwardModel, state, playerID, false);
    }

    double BasicTBSCombatHeuristic::evaluateGameState(const ForwardModel& forwardModel, GameState& state, const int playerID, bool isDebug) {
        //std::cout<< "Starts updateStateInfo" << "\n";
        //if (!isDebug) {
        //    std::cout<< "start heuristic\n";
        //}

        if (state.isGameOver()) {
			if (state.getWinnerID() == playerID) {
				return 1.0;
			}
			else {
				return 0.0;
			}
		}

        // target another goldVein for mining

        updateStateInfo(state, playerID);

        double enemyCityHealthPenalty = 25.0;
        double reward = enemyCityHealthPenalty; // there is a reduction about enermy base health point, initialize 15 avoid negative final reward.

        // if (!isDebug) {
        //    std::cout<< "start heuristic  2334\n";
        //}
        // 20
        if (isResearchedMining) {
            reward += 10.0;
            if (isResearchedDicipline) {
                reward += 10;
            }
        }

        double playerHasGold = getGold(state);

        // if (!isDebug) {
        //    std::cout<< "start heuristic 43oijfoi\n";
        //}

        // 20
        if (selfHasWorker) {
            reward += 5.0;
            if (isBuiltBarracks) {
                reward += 10.0;
                if (currentTargetGoldVeinIdx != -1) {
                    if (!ableToAttack && playerHasGold < 80.0 ) {
                        reward += (1.0-min_dis/maximumBoardDistance) * 5;
                    }
                }
                
            }
            
        }

        // 20
        if (ableToAttack) {
            reward += 10.0;
        }
        else {
            if (playerHasGold > 80.0) { // a warrior requires 80 gold
                reward += 10.0;
            }
            else {
                reward += 10 * playerHasGold/80.0;
            }
        }

        // if (!isDebug) {
        //    std::cout<< "start fh984t9gj\n";
        //}

        double selfCityHealth = getEntityHealth(state, self_city);

        //if (!isDebug) {
        //    std::cout<< "start 4t98fu98j\n";
        //}

        double enemyCityHealth = getEntityHealth(state, enemy_city);

        if (isDebug) {
            std::cout<<" isResearchedMining: " << isResearchedMining << " isResearchedDicipline: " << isResearchedDicipline << " isBuiltBarracks : "<< isBuiltBarracks<< " ableToAttack : "<< ableToAttack << "\n"
                << " selfCityHP: "<< selfCityHealth << " / "<< city_max_health << " enemyCityHP: "<<enemyCityHealth << " / "<< city_max_health << "\n"
                << " has worker: " << selfHasWorker << " has gold: "<< playerHasGold << " has production: "<< getProduction(state) << "\n";
        }
        //if (!isDebug) {
        //    std::cout<< "start 45860000\n";
        //}

        // 15% for self base health
        selfCityHealth /= city_max_health;
        reward += selfCityHealth*15.0;

        // 15% for enemy base health
        enemyCityHealth /= city_max_health;
        reward -= enemyCityHealth*enemyCityHealthPenalty;

        //std::cout<< "Ends evaluateGameState" << std::endl;
        reward /= 100.0;

        double w1=0.4, w2=0.6;
        auto defenceR = defenceHeuristic->evaluateGameState(forwardModel, state, playerID, false);

        //return w1*reward + w2*defenceR;
        double r = w1*reward + w2*defenceR;
        //std::cout<<r << "\t" << reward << "\t" << defenceR << "\n";
        return r;
    }

    double BasicTBSCombatHeuristic::getProduction(GameState state){
        return state.getPlayerParameter(state.getCurrentTBSPlayer(), "Prod");
    }

    double BasicTBSCombatHeuristic::getGold(GameState state){
        return state.getPlayerParameter(state.getCurrentTBSPlayer(), "Gold");
    }

    double BasicTBSCombatHeuristic::getEntityHealth(GameState state, Vector2f position) {
        auto target_entity = state.getEntityAt(position);
        if (target_entity == nullptr) {
            return 0.0;
        }
        double paramsValue = target_entity->getParameter("Health");
        if (paramsValue >= 0.0) {
            return paramsValue;
        }
        //std::cout<<"[Error]: passed in invalid parameter name to search"<<std::endl;
    }


}