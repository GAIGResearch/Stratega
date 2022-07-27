#include <Stratega/Agent/Heuristic/BasicTBSTechnologyHeuristic.h>


namespace SGA {

	BasicTBSTechnologyHeuristic::BasicTBSTechnologyHeuristic(int playerID, GameState initialState) {
		/*
		* count all gold in the map
		*/
		initialized = true;
        defenceHeuristic = std::make_unique<TwoKingdomsDefenceHeuristic>(playerID, initialState);

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
		if (totalGold < 100.0) {
			std::cout<<"[ERROR]: this game cannot win with technology victory!" << std::endl;
		}
		else {
			totalGold = 100.0;
		}
		
		//std::cout<< "Ends Initialize BasicTBSHeuristic" << std::endl;
		return;
	}

    void BasicTBSTechnologyHeuristic::updateStateInfo(GameState& state, const int playerID) {
        defenceHeuristic->updateStateInfo(state, playerID);

        isResearchedMining = state.isResearched(playerID, "Mining");
        isResearchedDicipline = state.isResearched(playerID, "Discipline");
        isResearchedPotery = state.isResearched(playerID, "Pottery");
        isResearchedMathematics = state.isResearched(playerID, "Mathematics"); 
        isResearchedBronzeWorking = state.isResearched(playerID, "BronzeWorking");
        isResearchedArchery = state.isResearched(playerID, "Archery");
        isResearchedApprenticeship = state.isResearched(playerID, "Apprenticeship");
        isResearchedMetallurgy = state.isResearched(playerID, "Metallurgy");
        isResearchedEngineering = state.isResearched(playerID, "Engineering");;
        auto entities = state.getEntities();

        selfHasWorker = false;
        isBuiltBarracks = false;
        minedGold = 0.0;
        min_dis = maximumBoardDistance;
        double tmp_dis = 0.0;

        bool tmp_isBuiltBarracks=false;
        for (auto entity : entities) {
            auto& entityType = state.getGameInfo()->getEntityType(entity.getEntityTypeID());
            auto entityName = entityType.getName();
            if (entity.getOwnerID() == playerID) {

                if (entityName == "Barracks") {
                    isBuiltBarracks=true;
                } else if (entityName == "Worker") {
                    selfHasWorker = true;
                    tmp_dis = entity.getPosition().distance(goldVeinPosition[currentTargetGoldVeinIdx]);
                    if (tmp_dis < min_dis) {
                        min_dis = tmp_dis;
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
                if (entityName == "GoldVein") {
                    if (playerID == 0) {
                        minedGold += entity.getParameter("PlayerAMined");
                    }
                    else {
                        minedGold += entity.getParameter("PlayerBMined");
                    }
                }
            }

        } // end for

        // target another goldVein for mining
        for (int i = 0; i < goldVeinPosition.size(); i++) {
            auto entity = state.getEntityAt(goldVeinPosition[currentTargetGoldVeinIdx]);
            double remainingGold = entity->getParameter("Gold");
            if (remainingGold != 0.0) {
                currentTargetGoldVeinIdx = i;
                break;
            }
        }

        return;
    }

    /*
    * 1) Check the status of technology, reward for each new technology
    * 2) Defend player's base.
    */
    double BasicTBSTechnologyHeuristic::evaluateGameState(const ForwardModel& forwardModel, GameState& state, const int playerID) {
        return evaluateGameState(forwardModel, state, playerID, false);
    }

    double BasicTBSTechnologyHeuristic::evaluateGameState(const ForwardModel& forwardModel, GameState& state,
        const int playerID, bool isDebug) {
        //std::cout<<"End Technology Heuristic"<<"\n";

        if (!initialized) {
            std::cout<<"[ERROR]: The Heuristic instance is not initialized!" << std::endl;
        }

        if (state.isGameOver()) {
            if (state.getWinnerID() == playerID) {
                return 1.0;
            }
            else {
                return 0.0;
            }
        }

        updateStateInfo(state, playerID);

        // if there is no goldVein in the map, there is no possible of technology victory
        if (goldVeinPosition.size() == 0) {
            return 0.0;
        }

        auto entities = state.getEntities();
        double mineReward = 0.0;

        double r_worker_distance = 0.0;

        if (selfHasWorker) {
            r_worker_distance = (1.0-min_dis/maximumBoardDistance);
            mineReward += R_WORKER_P;
        }
        if (minedGold > totalGold) {
            minedGold = totalGold;
        }

        mineReward += R_DISTANCE_P * r_worker_distance;
        mineReward += R_GOLDPROPORTION_P * (minedGold/totalGold);
        mineReward *= R_MINE_P;

        double reward = 0.0;
        if (isResearchedMining) {
            reward += 10.0;
            if (isResearchedDicipline) {
                reward += 10;
                if (isResearchedBronzeWorking) {
                    reward += 10;
                }
                if (isResearchedArchery) {
                    reward += 10;
                }
            }
            if (isResearchedPotery) {
                reward += 10;
                if (isResearchedApprenticeship) {
                    reward += 10;
                }
                if (isResearchedMetallurgy) {
                    reward += 20;
                }
            }
            if (isResearchedMathematics) {
                reward += 10;
                if (isResearchedEngineering) {
                    reward += 10;
                }
            }
        }

        reward /= 100.0;
        reward *= R_TECH_P;
        if (isDebug) {
            std::cout<<"minedGold/totalGold: "<< minedGold << "/ " <<totalGold << ", mineR: " << mineReward << " tehcR: " << reward;
            std::cout<<" " <<isResearchedBronzeWorking << " " << isResearchedArchery << " " << isResearchedApprenticeship << "-"<< isResearchedMetallurgy << " " << isResearchedEngineering;
            std::cout<<std::endl;
        }
        double w1 = 1.0, w2 = 0.0;
        double defenceR = defenceHeuristic->evaluateGameState(forwardModel, state, playerID, isDebug);

        return w1*(reward+mineReward) + w2*defenceR;
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