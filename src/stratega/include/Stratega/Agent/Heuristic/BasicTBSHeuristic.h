#pragma once
#include <Stratega/Agent/Heuristic/StateHeuristic.h>

namespace SGA
{
	class BasicTBSHeuristic : public StateHeuristic
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

	public:
		BasicTBSHeuristic(int playerID, GameState initialState);
		//void initialize(int playerID, GameState state);
		void updateStateInfo(GameState& gameState, const int playerID);
		double evaluateGameState( const ForwardModel& forwardModel, GameState& gameState, const int playerID);

		double getProduction(GameState state);
		double getGold(GameState state);

		double getEntityHealth(GameState state, Vector2f position);

		std::string getName() const override { return "BasicTBSHeuristic"; }
	};
}