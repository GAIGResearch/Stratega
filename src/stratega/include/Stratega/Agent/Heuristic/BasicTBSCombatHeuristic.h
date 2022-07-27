#pragma once
#include <Stratega/Agent/Heuristic/StateHeuristic.h>
#include <Stratega/Agent/Heuristic/TwoKingdomsDefenceHeuristic.h>

namespace SGA
{
	class BasicTBSCombatHeuristic : public StateHeuristic
	{
		// encourage greater:
		//   ableGenerateAttackable
		//   selfAttackPower
		//   Resource
		// avoid greater:
		//   enemyAttackPower
		//   

		//enermycityhp: 50%
		//can attack: 30%
		//enough gold for warrior:
		//
	private:
		bool initialized = false, ableToAttack=false, isResearchedMining=false, isResearchedDicipline=false,
			isBuiltBarracks=false;
		int self_city_id=-1, enemy_city_id=-1;
		Vector2f self_city, enemy_city;
		double city_max_health=-1.0;

        bool selfHasWorker = false, enemyHasWarrior = false, selfHasWarrior=false;
        double selfWarriorCityDistance = 100000.0, enemyWarriorCityDistance = 100000.0;
        double min_dis = 10000.0; // the minimum distance between self workers to target gold vein.
        double FULL_HEALTH = 50.0;
        double maximumBoardDistance = 1000000.0;
        double enemyWarriorHealth = 50;

        std::vector<Vector2f> goldVeinPosition={};
        int nGoldVein = -1;
        double totalGold = 0.0;
        int currentTargetGoldVeinIdx = -1;

        std::shared_ptr<TwoKingdomsDefenceHeuristic> defenceHeuristic;

	public:
		BasicTBSCombatHeuristic(int playerID, GameState initialState);
		//void initialize(int playerID, GameState state);
		void updateStateInfo(GameState& gameState, const int playerID);
		double evaluateGameState( const ForwardModel& forwardModel, GameState& gameState, const int playerID);
        double evaluateGameState( const ForwardModel& forwardModel, GameState& gameState, const int playerID, bool isDebug);

		double getProduction(GameState state);
		double getGold(GameState state);

		double getEntityHealth(GameState state, Vector2f position);

		std::string getName() const override { return "TwoKindomsCombatHeurisitc"; }
	};
}