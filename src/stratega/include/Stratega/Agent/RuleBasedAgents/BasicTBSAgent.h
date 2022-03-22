#pragma once
#include <Stratega/Agent/Agent.h>
#include <Stratega/Agent/Heuristic/UnitEvaluator.h>
#include <map>
#include <vector>

namespace SGA {
/* Strategy, spawn 4 warriors; 2 as neaby warriors aiming to protect self city, 2 as farAwar warriors attacking enemy city
* maintaining two vectors of units 1) nearbyAttackable; 2) farAwaryAttackable; Each vector is ranked by its distance to self city
* if len(workers) < 2: spaw warkers
* for worker in workers:
*     Go to mine;
* if farAwayAttackable less than 2: 
*     1) if len(nearbyAttackable != 0) move the farer neabyAttackable (end of the vector) into vector of farAwayAttackable; 
*     2) else spawn attackable unit and save into farAwayAttackable;
* if nearbyAttackable less than 2: spawn if can spawn;
* 
* 
*/
class BasicTBSAgent: public Agent {
  public:
	  using Agent::Agent;
      ActionAssignment computeAction(GameState state, const ForwardModel& forwardModel, Timer timer) override;
      void init(GameState initialState, const ForwardModel& forwardModel, Timer timeBudgetMs) override;

  private:
	  bool initialized=false;
	  int self_city_id, enemy_city_id;
      Vector2f self_city, enemy_city;
	  std::vector<int> workers, nearbyAttackable, farAwarAttackable, goldVein;
	  std::vector<Vector2f> goldVeinPosition;
      double enemyAttackerRangeTolerate = 5.0;
      
      BasicTBSAgent();
      std::vector< Entity > filterAttackableUnit();

      bool existAttackableEnemyNearby(double nearestEnemyAttackerDistance);

	  double getProduction(GameState state);
	  double getGold(GameState state);
	  double getScore(GameState state);

	 int filterSpawnWorker(GameState state, std::vector<Action> actionSpace);


};
} // SGA namespace