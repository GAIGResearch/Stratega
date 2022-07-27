#include <Stratega/Agent/Heuristic/BasicTBSResourceHeuristic.h>


namespace SGA {

    BasicTBSResourceHeuristic::BasicTBSResourceHeuristic(int playerID, GameState initialState) {
        /*
        * count all gold in the map
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

    void BasicTBSResourceHeuristic::updateStateInfo(GameState& state, const int playerID) {
        isResearchedMining = state.isResearched(playerID, "Mining");
        isResearchedDicipline = state.isResearched(playerID, "Dicipline");

        auto entities = state.getEntities();
        selfHasWorker = false;
        isBuiltBarracks = false;
        selfHasWarrior = false, enemyHasWarrior = false;
        enemyWarriorCityDistance = maximumBoardDistance;


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

	double BasicTBSResourceHeuristic::evaluateGameState(const ForwardModel& forwardModel, GameState& state, const int playerID) {
		/*
		* proportion of spawning worker reward: r_existWorker = 0.2
		* proportion of mining the gold: r_gold = minedGold/totalGold*0.8
		*/
		return evaluateGameState(forwardModel, state, playerID, false);
	}

	double BasicTBSResourceHeuristic::evaluateGameState(const ForwardModel& forwardModel, GameState& state, 
		const int playerID, bool isDebug) {
		/*
		* proportion of spawning worker reward: r_existWorker = 0.2
		* proportion of mining the gold: r_gold = minedGold/totalGold*0.8
		*/

		// if there is no goldVein in the map
		if (goldVeinPosition.size() == 0) {
			return 0.0;
		}

		// if game over
		if (state.isGameOver()) {
			if (state.getWinnerID() == playerID) {
				return 1.0;
			}
			else {
				return 0.0;
			}
		}

		// target another goldVein for mining
		for (int i = 0; i < goldVeinPosition.size(); i++) {
			auto entity = state.getEntityAt(goldVeinPosition[currentTargetGoldVeinIdx]);
			double remainingGold = entity->getParameter("Gold");
			if (remainingGold != 0.0) {
				currentTargetGoldVeinIdx = i;
				break;
			}
		}
		
		auto entities = state.getEntities();
		double reward = 0.0, minedGold=0.0;
		std::vector<double> workerDistance = std::vector<double>();
		double totalWorkerDistance = 0.0;
		double r_worker_distance = 0.0;

		bool tmp_isWorkerExists=false;
		double min_dis = maximumBoardDistance;
		double tmp_dis = 0.0;
		for (auto entity : entities) {
			auto& entityType = state.getGameInfo()->getEntityType(entity.getEntityTypeID());
			if(entity.getOwnerID() == playerID){
				
				if (entityType.getName() == "Worker") {
					tmp_isWorkerExists=true;
					tmp_dis = entity.getPosition().distance(goldVeinPosition[currentTargetGoldVeinIdx]);
					if (tmp_dis < min_dis) {
						min_dis = tmp_dis;
					}
					//workerDistance.push_back();
				}
			}
			if (entityType.getName() == "GoldVein") {
				if (playerID == 0) {
					minedGold += entity.getParameter("PlayerAMined");
				}
				else {
					minedGold += entity.getParameter("PlayerBMined");
				}
			}
		}


		if (tmp_isWorkerExists) {
			r_worker_distance = (1.0-min_dis/maximumBoardDistance);
			reward += R_WORKER_P;
		}

		reward += R_DISTANCE_P * r_worker_distance;
		reward += R_GOLDPROPORTION_P * (minedGold/totalGold);

		if (isDebug) {
			std::cout<< "r_worker_distance: "<< r_worker_distance << ", mined/total " << minedGold/totalGold << std::endl;
		}
		return reward;
	}

    double BasicTBSResourceHeuristic::defenceReward(const ForwardModel& forwardModel, GameState& state, const int playerID, bool isDebug) {
        double ret = 50;

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