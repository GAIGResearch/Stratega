#include <Stratega/Agent/Heuristic/BasicTBSCombatHeuristic.h>
#include <map>
#include <set>
#include <string>

namespace SGA {

    BasicTBSCombatHeuristic::BasicTBSCombatHeuristic(int playerID, GameState initialState) {

        initialized = true;
        is_battle_mode=false;
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
        double tmp_gold_distance = 10000.0;
        for (int i = 0; i < goldVeinPosition.size(); i++) {
            auto g  = goldVeinPosition[i];
            if (g.distance(self_city) < tmp_gold_distance) {
                tmp_gold_distance = g.distance(self_city);
                currentTargetGoldVeinIdx = i;
            }
        }
        //std::cout<< currentTargetGoldVeinIdx<<"\n";
        //std::cout<<self_city.x << "\t"<<self_city.y<<"\n";

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
        minimum_dis_enemy_City = maximumBoardDistance;
        selfWarriorN = 0.0;
        enemyWarriorN = 0.0;
        totalDis = 0.0;
        double min_gold_dis = 1000.0;
        attackable_N = 0.0;
        attackable_self_city_dis = 0.0;


        for (int i = 0 ; i< nGoldVein; i++) {
            auto gvp = goldVeinPosition[i];
            auto e = state.getEntityAt(gvp);
            if (e->getParameter("Gold") > 0) {
                double tmp_gold_dis = e->getPosition().distance(self_city);
                if (tmp_gold_dis < min_gold_dis) {
                    min_gold_dis=tmp_gold_dis;
                    currentTargetGoldVeinIdx = i;
                }
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
                    selfWarriorN ++;
                    double tmp_dis = entity.getPosition().distance(enemy_city);
                    totalDis += tmp_dis;
                    if (tmp_dis < minimum_dis_enemy_City) {
                        minimum_dis_enemy_City = tmp_dis;
                    }

                    // deal with distance to self city (minimize)
                    attackable_N++;
                    attackable_self_city_dis += entity.getPosition().distance(self_city);
                } else if (entityName == "Worker") {
                    selfHasWorker = true;

                    if (currentTargetGoldVeinIdx != -1) {
                        double tmp_dis = entity.getPosition().distance(goldVeinPosition[currentTargetGoldVeinIdx]);
                        if (tmp_dis < min_dis) {
                            min_dis = tmp_dis;
                        }
                    }

                }
                else if (entityName == "King") {
                    attackable_N++;
                    attackable_self_city_dis += entity.getPosition().distance(self_city);
                }
            }
            else {
                 if (entityName == "Warrior" || entityName == "Archer") {
                    enemyHasWarrior = true;
                    enemyWarriorN ++;
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
        totalDis/= selfWarriorN;
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
				return 2.0;
			}
			else {
				return -2.0;
			}
		}

        // target another goldVein for mining

        //if (selfWarriorN >= 3) {
        if (is_battle_mode){
            double score = 0.0;

            int our_unit_number = 0;
            int king_x = -1, king_y = -1;
            double total_distance = 0.0, mean_distance = 0.0;

            std::map< int, Vector2f > positions;
            std::set< int > opponentEntites = std::set< int >();
            std::set< int > playerEntities = std::set< int >();

            double opponent_king_hp = 400.0;
            for(const auto& entity : state.getEntities()) {
                auto& entityType = state.getGameInfo()->getEntityType(
                    entity.getEntityTypeID());
                if(entityType.getName() == "Barracks"|| entityType.getName() == "Worker" || entityType.getName() == "City") {continue;}

                positions.emplace(
                    entity.getID(), entity.getPosition());  // entity.id, entity.position);

                if(entity.getOwnerID()!= playerID)
                {
                    opponentEntites.insert(entity.getID());

                    // auto& entityType = gameState.gameInfo->getEntityType(entity.typeID);
                    auto& entityType = state.getGameInfo()->getEntityType(
                    entity.getEntityTypeID());
                    if(entityType.getName() == "King") {
                        king_x = entity.x();
                        king_y = entity.y();

                        opponent_king_hp = entity.getParameter("Health");
                    }
                } else { // this is a bug in this game, however, it works somehow (with switching position, this bug should be alright)
                    playerEntities.insert(entity.getID());
                }
                /*else if (entity.getOwnerID() == playerID) {
                    playerEntities.insert(entity.getID());
                }*/
            }

            for(const auto& entity : playerEntities) {
                total_distance += abs(positions[entity].x - king_x)
                                + abs(positions[entity].y - king_y);
            }
            mean_distance = total_distance / playerEntities.size();

            double maximum_distance = 400.0;
            score = 1.0 - mean_distance / maximum_distance;  // set to the mean of distance of our units to opponent
                                                // units, divided by a constant 50 to make it slower
                                                // than the gameOver score

            score += 1.0 - opponent_king_hp / 400.0;

            double Total_N_entities = 5.0;
            // double Total_N_entities = unitNum;
            Total_N_entities *= 4;  // 4 for scaling
            score += playerEntities.size() / Total_N_entities;
            score -= opponentEntites.size() / Total_N_entities;

            return (score + 2.0) / 4.0;
        }
        updateStateInfo(state, playerID);

        double enemyCityHealthPenalty = 15.0+10.0; // 15 for enemy city health and 10 for enemy attacking power
        double reward = 0.0; // there is a reduction about enermy base health point, initialize 15 avoid negative final reward.

        // if (!isDebug) {
        //    std::cout<< "start heuristic  2334\n";
        //}
        // +20
        if (isResearchedMining) {
            reward += 10.0;
            if (isResearchedDicipline) {
                reward += 10;
            }
        }

        double playerHasGold = getGold(state);


        // +30
        if (selfHasWorker) {
            reward += 10.0;
            if (isBuiltBarracks) {
                reward += 10.0;
            }
            if (currentTargetGoldVeinIdx != -1) {
                reward += (1.0-min_dis/maximumBoardDistance) * 10;
            }
            
        }
        else if (selfWarriorN + playerHasGold / 40 >= 3) {
            reward += 30;
        }

        // +20
        if (playerHasGold > 120) {
            reward += 20.0;
        }
        else {
            reward += 18 * playerHasGold/120.0;
        }

        double selfCityHealth = getEntityHealth(state, self_city);
        double enemyCityHealth = getEntityHealth(state, enemy_city);

        if (isDebug) {
            std::cout<<" isResearchedMining: " << isResearchedMining << " isResearchedDicipline: " << isResearchedDicipline << " isBuiltBarracks : "<< isBuiltBarracks<< " ableToAttack : "<< ableToAttack << "\n"
                << " selfCityHP: "<< selfCityHealth << " / "<< city_max_health << " enemyCityHP: "<<enemyCityHealth << " / "<< city_max_health << "\n"
                << " has worker: " << selfHasWorker << " has gold: "<< playerHasGold << " has production: "<< getProduction(state) << "\n"<<" dis enemy city: "<< minimum_dis_enemy_City;
        }

        //+10
        if (selfWarriorN >= 3) {
            reward+= 10;
        }
        else {
             reward+= selfWarriorN/3*10;
        }

        //+20
        double total_distance = (4*maximumBoardDistance-attackable_self_city_dis)/(4*maximumBoardDistance);
        reward += total_distance*20;
        if (isDebug) {
            std::cout<<"total_distance: "<< total_distance <<" , dis reward: "<< total_distance*20.0<<"\n";
        }
       

        // 15% for self base health
        //selfCityHealth /= city_max_health;
        //reward += selfCityHealth*20.0;

        // -15% for enemy base health
        //enemyCityHealth /= city_max_health;
        //reward -= enemyCityHealth*20.0;

        //std::cout<< "Ends evaluateGameState" << std::endl;
        reward /= 100.0;

        double w_attack=1.0, w_defence=0.0;
        //auto defenceR = defenceHeuristic->evaluateGameState(forwardModel, state, playerID, false);

        //return w1*reward + w2*defenceR;
        double r = w_attack*reward;// + w_defence*defenceR;
        //std::cout<<r << "\t" << reward << "\t" << defenceR << "\n";
        if (isDebug) {
            std::cout<<"\treward:" << r<<"\n";
        }
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
    void BasicTBSCombatHeuristic::setBattleMode() {
        is_battle_mode=true;
    }

}