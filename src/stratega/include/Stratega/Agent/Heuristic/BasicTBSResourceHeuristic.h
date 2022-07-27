#pragma once
#include <Stratega/Agent/Heuristic/StateHeuristic.h>

namespace SGA
{
	class BasicTBSResourceHeuristic : public StateHeuristic
	{
		/*Rewards given when
		* 1) worker exists
		* 2) mined gold increases
		*/
	private:
		bool initialized = false, ableToAttack=false, isResearchedMining=false, isResearchedDicipline=false,
			isBuiltBarracks=false;

		const double R_WORKER_P = 0.1, R_DISTANCE_P=0.2, R_GOLDPROPORTION_P=0.7;
		double maximumBoardDistance = 1000000.0;
		double totalGold = 0.0;

		int self_city_id=-1, enemy_city_id=-1;
		Vector2f self_city, enemy_city;
		double city_max_health=-1.0;

		std::vector<Vector2f> goldVeinPosition;
		int currentTargetGoldVeinIdx=-1;

        // defence
        bool selfHasWorker=false, selfHasWarrior=false, enemyHasWarrior=false;
        double selfWarriorCityDistance=1000, min_dis=1000, enemyWarriorCityDistance=1000;
        double enemyWarriorHealth=50, FULL_HEALTH=50;

	public:
		BasicTBSResourceHeuristic(int playerID, GameState initialState);
		void updateStateInfo(GameState& gameState, const int playerID);
		double evaluateGameState( const ForwardModel& forwardModel, GameState& gameState, const int playerID);
		double evaluateGameState(const ForwardModel& forwardModel, GameState& state, const int playerID, bool isDebug=false);

        double defenceReward(const ForwardModel& forwardModel, GameState& state, const int playerID, bool isDebug);

		double getProduction(GameState state);
		double getGold(GameState state);
		double getEntityHealth(GameState state, Vector2f position);

		std::string getName() const override { return "TwoKingdomsResourceHeuristic"; }
	};
}